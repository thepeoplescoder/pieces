#include <stdbool.h>
#include <string.h>
#include "command_line_syntax.h"

#define __strcmp strcmp     // in case i want to use a different string comparison function

const char * const command_line_syntax___HELP_ARGS[3]        = {"help", "--help", "-?"};
const char * const command_line_syntax___SPLIT_ARGS[3]       = {"split", "--split", "-s"};
const char * const command_line_syntax___MERGE_ARGS[3]       = {"merge", "--merge", "-m"};
const char * const command_line_syntax___INTERLEAVED_ARGS[2] = {"--interleaved", "-i"};

#define HELP_ARGS        command_line_syntax___HELP_ARGS
#define SPLIT_ARGS       command_line_syntax___SPLIT_ARGS
#define MERGE_ARGS       command_line_syntax___MERGE_ARGS
#define INTERLEAVED_ARGS command_line_syntax___INTERLEAVED_ARGS

#define is_help_arg  command_line_syntax___is_help_arg
#define is_split_arg command_line_syntax___is_split_arg
#define is_merge_arg command_line_syntax___is_merge_arg

#define static_array_length(a) (sizeof(a) / sizeof((a)[0]))

#define __is_arg_helper_macro(x) \
    const char * const *ptr = (x);                    \
    int index, len          = static_array_length(x); \
    for (index = 0; index < len; index++)             \
    {                                                 \
        if (!__strcmp(arg, ptr[index]))               \
        {                                             \
            return true;                              \
        }                                             \
    }                                                 \
    return false

bool command_line_syntax___is_first_task(const char * const arg)
{
    return is_help_arg(arg) || is_split_arg(arg) || is_merge_arg(arg);
}

bool command_line_syntax___is_help_arg(const char * const arg)
{
    __is_arg_helper_macro(HELP_ARGS);
}

bool command_line_syntax___is_split_arg(const char * const arg)
{
    __is_arg_helper_macro(SPLIT_ARGS);
}

bool command_line_syntax___is_merge_arg(const char * const arg)
{
    __is_arg_helper_macro(MERGE_ARGS);
}

bool command_line_syntax___is_interleaved_arg(const char * const arg)
{
    __is_arg_helper_macro(INTERLEAVED_ARGS);
}