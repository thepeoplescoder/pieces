#pragma once

#include <stdio.h>

typedef char * const *zstrings___zstring_array;

char   *zstrings___join_zstrings                         (const char *join_string, size_t array_len, zstrings___zstring_array string_array);
char   *zstrings___join_zstrings_buffer_create           (const char *join_string, size_t array_len, zstrings___zstring_array string_array);
size_t  zstrings___join_zstrings_length                  (const char *join_string, size_t array_len, zstrings___zstring_array string_array);
char   *zstrings___join_zstrings_to_buffer (char *buffer, const char *join_string, size_t array_len, zstrings___zstring_array string_array);