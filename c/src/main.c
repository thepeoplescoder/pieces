#include <stdio.h>
#include <stdlib.h>
#include "command_line.h"
#include "command_line_syntax.h"
#include "subcommand.h"

#define OUT_OF_MEMORY          command_line___OUT_OF_MEMORY
#define INVALID_ARGUMENT       command_line___INVALID_ARGUMENT
#define unshift_next_valid_arg command_line___unshift_next_valid_arg

#define is_first_task          command_line_syntax___is_first_task
#define HELP_ARGS              command_line_syntax___HELP_ARGS

#define pick_subcommand        subcommand___pick_subcommand

int main(int argc, char **argv)
{
    int index = 1;
    const char *current_arg;

    current_arg = unshift_next_valid_arg(argc, argv, &index, NULL, is_first_task);

    if (current_arg == OUT_OF_MEMORY)
    {
        fprintf(stderr, "Out of memory to evaluate argument\n");
        return 1;
    }
    else if (current_arg == INVALID_ARGUMENT)
    {
        fprintf(stderr, "Invalid argument: %s\n", argv[index]);
        return 1;
    }

    return pick_subcommand(current_arg)(argc, argv, &index, current_arg);
}