import functools
import io
import os

from typing import TypeAlias
from collections.abc import Callable
from typing import BinaryIO

from .command_line import CommandLineDeque
from .command_line import INVALID_ARGUMENT
from .command_line_syntax import HELP_ARGS
from .command_line_syntax import SPLIT_ARGS
from .command_line_syntax import MERGE_ARGS
from .command_line_syntax import INTERLEAVED_ARGS
from .command_line_syntax import is_interleaved_arg

SplitMergeHandler: TypeAlias = Callable[[tuple[str, str, str], bool], int]

def display_help(argd: CommandLineDeque) -> int:
    """The help page."""
    INDENT:      str = ' ' * 3
    SCRIPT_NAME: str = __package__

    LB,  RB  = '[', ']'
    LCB, RCB = '{', '}'

    sep: str = " | "

    print(f"Usage:")
    print(f"{INDENT}{SCRIPT_NAME} {LB }{sep.join(HELP_ARGS) }{RB }")
    print(f"{INDENT}{SCRIPT_NAME} {LCB}{sep.join(SPLIT_ARGS)}{RCB} infile  outfile1 outfile2 {LB}{sep.join(INTERLEAVED_ARGS)}{RB}")
    print(f"{INDENT}{SCRIPT_NAME} {LCB}{sep.join(MERGE_ARGS)}{RCB} infile1 infile2  outfile  {LB}{sep.join(INTERLEAVED_ARGS)}{RB}")
    print()

    sep = ", "

    print(f"{INDENT}{INDENT}{sep.join(HELP_ARGS)       :<30}Displays this help section.")
    print(f"{INDENT}{INDENT}{sep.join(SPLIT_ARGS)      :<30}Splits infile into outfile1 and outfile2.")
    print(f"{INDENT}{INDENT}{sep.join(MERGE_ARGS)      :<30}Merges infile1 and infile2 into outfile.")
    print(f"{INDENT}{INDENT}{sep.join(INTERLEAVED_ARGS):<30}Split or merge interleaved.")
    print(f"{INDENT}{INDENT}{''                        :<30}If omitted, splitting/merging is")
    print(f"{INDENT}{INDENT}{''                        :<30}done in consecutive chunks.")
    print()

    print(f"{INDENT}Arguments in {LB}{RB} are optional, arguments in {LCB}{RCB} are required.\n")

    return 0

def merge(argd: CommandLineDeque) -> int:
    """Merges the files together."""
    return handle_merge_or_split(argd, handler=merge_files, filename_errors=[
        "Input file name #1 not provided.",
        "Input file name #2 not provided.",
        "Output file name not provided.",
    ])

def split(argd: CommandLineDeque) -> int:
    """Splits a file into two parts"""
    return handle_merge_or_split(argd, handler=split_file, filename_errors=[
        "Input file name not provided.",
        "Output file name #1 not provided.",
        "Output file name #2 not provided.",
    ])

def handle_merge_or_split(argd: CommandLineDeque, *, handler: SplitMergeHandler, filename_errors: list[str]) -> int:
    filenames = argd.unshift_next_args(count=3)
    try:
        index = filenames.index(None)
        print(filename_errors[index])
        return 1
    except ValueError:
        pass

    filenames: tuple[str, str, str]

    is_interleaved = argd.unshift_next_valid_arg(is_valid_arg=is_interleaved_arg)
    if is_interleaved is INVALID_ARGUMENT:
        print(f"Invalid argument: {argd[0]}")
        return 1

    is_interleaved = is_interleaved is not None

    if not callable(handler):
        raise TypeError("handler is not callable")

    return handler(filenames, is_interleaved)

def merge_files(filenames: tuple[str, str, str], is_interleaved: bool) -> int:
    files: list[BinaryIO] | None = open_files(filenames, "rb", "rb", "wb")

    if files is None:
        return 1

    infile: list[BinaryIO] = files[:2]
    outfile:      BinaryIO = files[2]

    with infile[0]:
        with infile[1]:
            with outfile:
                infile_length = [file_length(f) for f in infile]

                if infile_length[1] > infile_length[0]:
                    print("First file must be equal in size or larger than the first file.")
                    return 1

                if infile_length[0] - infile_length[1] > 1:
                    print("Files can only differ in size by one byte.")
                    return 1

                i = 0

                if is_interleaved:
                    def read() -> bytes:
                        nonlocal i
                        b = infile[i].read(1)
                        i = int(not i)
                        return b
                else:
                    def read() -> bytes:
                        nonlocal i
                        b = infile[int(i >= infile_length[0])].read(1)
                        i += 1
                        return b

                bytes_in = iter(read, b'')
                for b in bytes_in:
                    outfile.write(b)

    return 0

def split_file(filenames: tuple[str, str, str], is_interleaved: bool) -> int:
    files: list[BinaryIO] | None = open_files(filenames, "rb", "wb", "wb")

    if files is None:
        return 1

    infile:        BinaryIO = files[0]
    outfile: list[BinaryIO] = files[1:]

    with infile:
        with outfile[0]:
            with outfile[1]:
                infile_length = file_length(infile)
                midpoint = infile_length - (infile_length // 2)

                i = 0

                if is_interleaved:
                    def write(b: bytes) -> None:
                        nonlocal i
                        outfile[i].write(b)
                        i = int(not i)
                else:
                    def write(b: bytes) -> None:
                        nonlocal i
                        outfile[int(i >= midpoint)].write(b)
                        i += 1

                bytes_in = iter(functools.partial(infile.read, 1), b'')
                for b in bytes_in:
                    write(b)

    return 0

def open_files(filenames: tuple[str, str, str], *modes: str) -> list[BinaryIO] | None:
    if len(filenames) > len(modes):
        raise ValueError("There must be a file mode for every file name provided.")

    error_strings: dict[str, str] = {
        "rb": "Can't open {0} for reading.",
        "wb": "Can't open {0} for writing.",
    }

    files: list[BinaryIO] = []

    for filename, mode in zip(filenames, modes):
        if mode not in error_strings:
            print(f"Invalid mode: {mode}\n")
            return None
        try:
            files.append(open(filename, mode))
        except OSError:
            for f in files:
                f.close()
            print(error_strings[mode].format(filename))
            return None

    return files

def file_length(f: io.IOBase) -> int:
    if f is None:
        return 0
    pos = f.tell()
    f.seek(0, os.SEEK_END)
    length = f.tell()
    f.seek(pos, os.SEEK_SET)
    return length

def invalid_argument(argd: CommandLineDeque) -> int:
    print(f"Invalid argument: {argd[0]}")
    return 1