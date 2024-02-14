import sys

from .command_line import CommandLineDeque

def merge_files(
        argd: CommandLineDeque
        ) -> int:
    """Merges the files together."""
    print("Merging the files...")
    return 0

def split_files(
        argd: CommandLineDeque
        ) -> int:
    """Splits a file into two parts"""
    print("Splitting files...")
    required_tasks: set[str] = {"input", "output"}
    return 0

def display_help(
        argd:CommandLineDeque
        ) -> int:
    """The help page."""
    script_name = sys.argv[0]
    print("This is the help page.")
    return 0

def invalid_argument(argd: CommandLineDeque) -> int:
    print(f"Invalid argument: {argd[0]}")
    return 1
