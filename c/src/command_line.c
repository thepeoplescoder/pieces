#include <string.h>
#include <stdlib.h>
#include "command_line.h"

const char *command_line___OUT_OF_MEMORY     = "Out of memory to evaluate argument\n";
const char *command_line___NO_MORE_ARGUMENTS = "No more arguments\n";
const char *command_line___INVALID_ARGUMENT  = "Invalid argument: %s\n";

#define OUT_OF_MEMORY     command_line___OUT_OF_MEMORY
#define NO_MORE_ARGUMENTS command_line___NO_MORE_ARGUMENTS
#define INVALID_ARGUMENT  command_line___INVALID_ARGUMENT

const char *command_line___unshift_next_valid_arg(
    int argc,
    char **argv,
    int *index,
    command_line___transformer transform_next_arg,
    command_line___checker is_valid_arg)
{
    const char *result;
    char *transformed_arg;
    char *arg;

    if (*index >= argc)
    {
        return NO_MORE_ARGUMENTS;
    }

    transformed_arg = arg = argv[*index];

    /* if we're transforming, we're going to need a copy of ourselves */
    if (transform_next_arg)
    {
        transformed_arg = strdup(arg);
        if (!transformed_arg)
        {
            return OUT_OF_MEMORY;
        }
        (*transform_next_arg)(transformed_arg);
    }

    if (is_valid_arg && !(*is_valid_arg)(transformed_arg))
    {
        result = INVALID_ARGUMENT;
    }
    else
    {
        result = arg;
        (*index)++;
    }

    /* if we did a transformation, we have to free the memory previously allocated */
    if (transform_next_arg)
    {
        free(transformed_arg);
    }

    return result;
}