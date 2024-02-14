import sys

from collections import deque

from . import subcommands

from .command_line import CommandLineDeque

def main() -> int:
    """Entry point."""
    argd = CommandLineDeque()

    first_task: str | None = argd.unshift_and_transform_next_valid_arg(
        next_arg_as=lambda x: x.lower(),

        display_help=["--help", "-?"],
        split_files=["--split", "-s"],
        merge_files=["--merge", "-m"])
    
    return getattr(subcommands, first_task or "invalid_argument")(argd)#