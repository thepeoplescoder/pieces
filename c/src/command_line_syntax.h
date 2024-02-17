#pragma once

#include <stdbool.h>
#include <string.h>

bool command_line_syntax___is_first_task      (const char * const arg);
bool command_line_syntax___is_help_arg        (const char * const arg);
bool command_line_syntax___is_split_arg       (const char * const arg);
bool command_line_syntax___is_merge_arg       (const char * const arg);
bool command_line_syntax___is_interleaved_arg (const char * const arg);

extern const char * const command_line_syntax___HELP_ARGS[3];
extern const char * const command_line_syntax___SPLIT_ARGS[3];
extern const char * const command_line_syntax___MERGE_ARGS[3];
extern const char * const command_line_syntax___INTERLEAVED_ARGS[2];