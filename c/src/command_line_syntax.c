#include <stdbool.h>
#include "command_line_syntax.h"

#define __strcmp strcmp

const char * const command_line_syntax___HELP_ARGS[2]        = {"--help",        "-?"};
const char * const command_line_syntax___SPLIT_ARGS[2]       = {"--split",       "-s"};
const char * const command_line_syntax___MERGE_ARGS[2]       = {"--merge",       "-m"};
const char * const command_line_syntax___INTERLEAVED_ARGS[2] = {"--interleaved", "-i"};

#define HELP_ARGS        command_line_syntax___HELP_ARGS
#define SPLIT_ARGS       command_line_syntax___SPLIT_ARGS
#define MERGE_ARGS       command_line_syntax___MERGE_ARGS
#define INTERLEAVED_ARGS command_line_syntax___INTERLEAVED_ARGS

#define is_help_arg  command_line_syntax___is_help_arg
#define is_split_arg command_line_syntax___is_split_arg
#define is_merge_arg command_line_syntax___is_merge_arg

bool command_line_syntax___is_first_task(const char * const arg)
{
    return is_help_arg(arg) || is_split_arg(arg) || is_merge_arg(arg);
}

bool command_line_syntax___is_help_arg(const char * const arg)
{
    return !__strcmp(arg, HELP_ARGS[0])  || !__strcmp(arg, HELP_ARGS[1]);
}

bool command_line_syntax___is_split_arg(const char * const arg)
{
    return !__strcmp(arg, SPLIT_ARGS[0]) || !__strcmp(arg, SPLIT_ARGS[1]);
}

bool command_line_syntax___is_merge_arg(const char * const arg)
{
    return !__strcmp(arg, MERGE_ARGS[0]) || !__strcmp(arg, MERGE_ARGS[1]);
}

bool command_line_syntax___is_interleaved_arg(const char * const arg)
{
    return !__strcmp(arg, INTERLEAVED_ARGS[0]) || !__strcmp(arg, INTERLEAVED_ARGS[1]);
}