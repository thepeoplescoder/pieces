#include <string.h>
#include <stdlib.h>
#include "zstrings.h"

#define join_zstrings_buffer_create zstrings___join_zstrings_buffer_create
#define join_zstrings_length        zstrings___join_zstrings_length
#define join_zstrings_to_buffer     zstrings___join_zstrings_to_buffer

char *zstrings___join_zstrings(const char *join_string, size_t array_len, zstrings___zstring_array string_array)
{
    char *buffer;

    if (array_len < 1)
    {
        return strdup("");
    }

    buffer = join_zstrings_buffer_create(join_string, array_len, string_array);

    return join_zstrings_to_buffer(buffer, join_string, array_len, string_array);
}

char *zstrings___join_zstrings_buffer_create(const char *join_string, size_t array_len, zstrings___zstring_array string_array)
{
    return (char *)calloc(join_zstrings_length(join_string, array_len, string_array) + 1, sizeof(char));
}

size_t zstrings___join_zstrings_length(const char *join_string, size_t array_len, zstrings___zstring_array string_array)
{
    size_t len, index;

    if (array_len < 1)
    {
        return 0;
    }

    if (join_string == NULL)
    {
        join_string = "";
    }

    len = 0;

    for (index = 0; index < array_len; index++)
    {
        len += strlen(string_array[index]);
    }

    len += (array_len - 1) * strlen(join_string);

    return len;
}

char *zstrings___join_zstrings_to_buffer(char *buffer, const char *join_string, size_t array_len, zstrings___zstring_array string_array)
{
    size_t index;
    char *ptr = buffer;

    if (join_string == NULL)
    {
        join_string = "";
    }

    if (array_len > 0 && buffer)
    {
        strcpy(buffer, string_array[0]);
        ptr = strchr(buffer, '\0');

        for (index = 1; index < array_len; index++)
        {
            strcpy(ptr, join_string);
            ptr = strchr(ptr, '\0');
            strcpy(ptr, string_array[index]);
            ptr = strchr(ptr, '\0');
        }
    }

    return buffer;
}