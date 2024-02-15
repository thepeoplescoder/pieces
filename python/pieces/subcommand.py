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

def merge_files_get_arguments(argd: CommandLineDeque) -> int:
    """Merges the files together."""
    filename_errors = [
        "Input file name #1 not provided.",
        "Input file name #2 not provided.",
        "Output file name not provided.",
    ]
    return handle_merge_or_split(argd, filename_errors, merge_files)

def split_file_get_arguments(argd: CommandLineDeque) -> int:
    """Splits a file into two parts"""
    filename_errors = [
        "Input file name not provided.",
        "Output file name #1 not provided.",
        "Output file name #2 not provided.",
    ]
    return handle_merge_or_split(argd, filename_errors, split_file)

def handle_merge_or_split(argd: CommandLineDeque, filename_errors: list[str], handler: SplitMergeHandler) -> int:
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

    return 0

def split_file(filenames: tuple[str, str, str], is_interleaved: bool) -> int:
    files: list[BinaryIO] | None = open_files(filenames, "rb", "wb", "wb")

    if files is None:
        return 1

    infile:        BinaryIO = files[0]
    outfile: list[BinaryIO] = files[1:]

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
        try:
            if mode not in error_strings:
                print(f"Invalid mode: {mode}\n")
                return None

            files.append(open(filename, mode))
        except OSError:
            print(error_strings[mode].format(filename))
            return None

    return files

def invalid_argument(argd: CommandLineDeque) -> int:
    print(f"Invalid argument: {argd[0]}")
    return 1