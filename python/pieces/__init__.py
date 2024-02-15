from . import subcommand
from .command_line import CommandLineDeque
from .command_line import INVALID_ARGUMENT
from .command_line_syntax import is_help_arg
from .command_line_syntax import is_split_arg
from .command_line_syntax import is_merge_arg

def main() -> int:
    """Entry point."""
    argd = CommandLineDeque()

    first_task: str | None

    first_task, _ = argd.unshift_and_transform_next_valid_arg(
        next_arg_as=lambda x: x.lower(),

        display_help=is_help_arg,
        split_file_get_arguments=is_split_arg,
        merge_files_get_arguments=is_merge_arg)

    if first_task is None:
        first_task = "display_help"

    elif first_task is INVALID_ARGUMENT:
        first_task = "invalid_argument"

    return getattr(subcommand, first_task)(argd)