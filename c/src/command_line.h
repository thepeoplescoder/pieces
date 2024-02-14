#pragma once

#include <stdbool.h>

typedef char *(*command_line___transformer)(char * str);
typedef bool (*command_line___checker)(const char * const arg);

const char *command_line___unshift_next_valid_arg(int argc, char **argv, int *index, command_line___transformer transform_next_arg, command_line___checker is_valid_arg);

#define command_line___unshift_next_arg(argc, argv, index) command_line___unshift_next_valid_arg((argc), (argv), (index), NULL, NULL)

extern const char *command_line___OUT_OF_MEMORY;
extern const char *command_line___NO_MORE_ARGUMENTS;
extern const char *command_line___INVALID_ARGUMENT;