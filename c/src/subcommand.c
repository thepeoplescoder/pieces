#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include "subcommand.h"
#include "command_line.h"
#include "command_line_syntax.h"
#include "zstrings.h"

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

#define INDENT "   "

#define NO_MORE_ARGUMENTS      command_line___NO_MORE_ARGUMENTS
#define INVALID_ARGUMENT       command_line___INVALID_ARGUMENT
#define unshift_next_arg       command_line___unshift_next_arg
#define unshift_next_valid_arg command_line___unshift_next_valid_arg

#define is_help_arg        command_line_syntax___is_help_arg
#define is_merge_arg       command_line_syntax___is_merge_arg
#define is_interleaved_arg command_line_syntax___is_interleaved_arg
#define HELP_ARGS          command_line_syntax___HELP_ARGS
#define MERGE_ARGS         command_line_syntax___MERGE_ARGS
#define SPLIT_ARGS         command_line_syntax___SPLIT_ARGS
#define INTERLEAVED_ARGS   command_line_syntax___INTERLEAVED_ARGS

#define join_zstrings zstrings___join_zstrings
#define zstring_array zstrings___zstring_array

#define static_array_length(a) (sizeof(a) / sizeof((a)[0]))

typedef int (*split_merge_handler)(const char *f1, const char *f2, const char *f3, bool interleaved);

static int display_help(int argc, char **argv, int *index, const char *current_arg);
static int split_files_get_arguments(int argc, char **argv, int *index, const char *current_arg);
static int merge_files_get_arguments(int argc, char **argv, int *index, const char *current_arg);
static int split_files(const char *infilename, const char *outfilename1, const char *outfilename2, bool is_interleaved);
static int merge_files(const char *infilename1, const char *infilename2, const char *outfilename, bool is_interleaved);
static int handle_split_or_merge(int argc, char **argv, int *index, const char *current_arg, const char **error_messages, split_merge_handler handler);
static bool get_arguments_helper(int argc, char **argv, int *index, const char *current_arg, const char **filenames, const char **error_messages, bool *is_interleaved);

static bool get_next_three_filenames(int argc, char **argv, int *index, const char **filenames, const char **error_messages);

static long file_length(FILE *fp);
static void file_close(FILE *fp);

#define __abs(x) (((x) >= 0) ? (x) : (-(x)))

subcommand___subcommand subcommand___pick_subcommand(const char *arg)
{
    if      (is_help_arg(arg) || arg == NO_MORE_ARGUMENTS) { return display_help;              }
    else if (is_merge_arg(arg))                            { return merge_files_get_arguments; }
    else                                                   { return split_files_get_arguments; }
}

static int display_help(int argc, char **argv, int *index, const char *current_arg)
{
    char *arg_executable_without_path;
    char format_string[256];

    char *help_args_string        = NULL;
    char *split_args_string       = NULL;
    char *merge_args_string       = NULL;
    char *interleaved_args_string = NULL;

    const char *ARG_SEP = ", ";

    int result = 0;

    help_args_string        = join_zstrings(ARG_SEP, static_array_length(HELP_ARGS),        (zstring_array)HELP_ARGS);
    split_args_string       = join_zstrings(ARG_SEP, static_array_length(SPLIT_ARGS),       (zstring_array)SPLIT_ARGS);
    merge_args_string       = join_zstrings(ARG_SEP, static_array_length(MERGE_ARGS),       (zstring_array)MERGE_ARGS);
    interleaved_args_string = join_zstrings(ARG_SEP, static_array_length(INTERLEAVED_ARGS), (zstring_array)INTERLEAVED_ARGS);

    if (help_args_string && split_args_string && merge_args_string && interleaved_args_string)
    {
        arg_executable_without_path = strrchr(argv[0], PATH_SEPARATOR) + 1;

        printf("Usage:\n");
        printf("%s%s [--help | -?]\n", INDENT, arg_executable_without_path);
        printf("%s%s {--split | -s} infile  outfile1 outfile2 [--interleaved | -i]\n", INDENT, arg_executable_without_path);
        printf("%s%s {--merge | -m} infile1 infile2  outfile  [--interleaved | -i]\n", INDENT, arg_executable_without_path);
        putchar('\n');

        sprintf(format_string, "%%s%%s%%-%ds%%s\n", 30);    // potentially unsafe, but this string should never surpass 256 chars.

        printf(format_string, INDENT, INDENT, help_args_string,        "Displays this help section.");
        printf(format_string, INDENT, INDENT, split_args_string,       "Splits infile into outfile1 and outfile2.");
        printf(format_string, INDENT, INDENT, merge_args_string,       "Merges infile1 and infile2 into outfile.");
        printf(format_string, INDENT, INDENT, interleaved_args_string, "Split or merge interleaved.");
        printf(format_string, INDENT, INDENT, "",                      "If omitted, splitting/merging is");
        printf(format_string, INDENT, INDENT, "",                      "done in consecutive chunks.");
        putchar('\n');

        printf("%sArguments in [] are optional, arguments in {} are required.\n\n", INDENT);
    }
    else
    {
        fprintf(stderr, "We ran out of memory creating some strings.  This shouldn't happen.\n");
        result = 1;
    }

    free(help_args_string);
    free(split_args_string);
    free(merge_args_string);
    free(interleaved_args_string);

    return result;
}

static int split_files_get_arguments(int argc, char **argv, int *index, const char *current_arg)
{
    const char *error_messages[] = {
        "Input file name not provided.",
        "Output file name #1 not provided.",
        "Output file name #2 not provided."
    };
    return handle_split_or_merge(argc, argv, index, current_arg, error_messages, split_files);
}

static int merge_files_get_arguments(int argc, char **argv, int *index, const char *current_arg)
{
    const char *error_messages[] = {
        "Input file name #1 not provided.",
        "Input file name #2 not provided.",
        "Output file name not provided."
    };
    return handle_split_or_merge(argc, argv, index, current_arg, error_messages, merge_files);
}

static int split_files(const char *infilename, const char *outfilename1, const char *outfilename2, bool is_interleaved)
{
    FILE *fin = NULL;
    FILE *fout[] = {NULL, NULL};
    const char *outfilename[] = {outfilename1, outfilename2};
    int result;
    int i;
    int ch;
    long offset;
    long midpoint;
    long length;
    long starts[2];
    long ends[2];

    fin = fopen(infilename, "rb");
    if (!fin)
    {
        fprintf(stderr, "Could not open %s for reading.", infilename);
        return 1;
    }

    for (i = 0; i < 2; i++)
    {
        fout[i] = fopen(outfilename[i], "wb");
        if (!fout[i])
        {
            fprintf(stderr, "Could not open %s for writing.", outfilename[i]);
            file_close(fin);
            file_close(fout[0]);
            return 1;
        }
    }

    result = 0;

    if (is_interleaved)
    {
        i = 0;
        while ( (ch = getc(fin)) != EOF )
        {
            if (putc(ch, fout[i]) == EOF)
            {
                perror("Error");
                result = 1;
                break;
            }
            i = !i;
        }
    }
    else
    {
        length = file_length(fin);

        ends[1]   = length;
        starts[1] = (length / 2) + (length % 2);    // we want the second half to be shorter.

        ends[0]   = starts[1];
        starts[0] = 0;

        for (offset = 0, i = 0; i < 2 && 0 <= offset && offset < length; i++)
        {
            for (offset = starts[i]; offset < ends[i]; offset++)
            {
                if ( (ch = getc(fin)) == EOF || putc(ch, fout[i]) == EOF )
                {
                    perror("Error");
                    offset = length;
                    result = 1;
                }
            }
        }
    }

    fclose(fin);
    fclose(fout[0]);
    fclose(fout[1]);

    return result;
}

static int merge_files(const char *infilename1, const char *infilename2, const char *outfilename, bool is_interleaved)
{
    FILE *fin[] = {NULL, NULL};
    FILE *fout = NULL;
    const char *infilename[] = {infilename1, infilename2};
    int result;
    int i;
    int ch;
    long offset, len[2];

    for (i = 0; i < 2; i++)
    {
        fin[i] = fopen(infilename[i], "rb");
        if (!fin[i])
        {
            fprintf(stderr, "Could not open %s for reading.\n", infilename[i]);
            file_close(fin[0]);
            return 1;
        }
    }

    fout = fopen(outfilename, "wb");
    if (!fout)
    {
        fprintf(stderr, "Could not open %s for writing.\n", outfilename);
        fclose(fin[0]);
        fclose(fin[1]);
        return 1;
    }

    len[0] = file_length(fin[0]);
    len[1] = file_length(fin[1]);

    if (len[1] > len[0])
    {
        fprintf(stderr, "First file must be equal in size or larger than the first file.");
        return 1;
    }

    if (len[0] - len[1] > 1)
    {
        fprintf(stderr, "Files can only differ in size by one byte.");
        return 1;
    }

    result = 0;

    if (is_interleaved)
    {
        i = 0;
        while (true)
        {
            ch = getc(fin[i]);
            if (ch == EOF)
            {
                if (i)
                {
                    perror("Error");
                    result = 1;
                }
                break;
            }
            putc(ch, fout);
            i = !i;
        }
    }
    else
    {
        for (i = 0; i < 2; i++)
        {
            for (offset = 0; offset < len[i]; offset++)
            {
                if ( (ch = getc(fin[i])) == EOF || putc(ch, fout) == EOF )
                {
                    perror("Error");
                    result = 1;
                    i = 2;
                    break;
                }
            }
        }
    }

    fclose(fin[0]);
    fclose(fin[1]);
    fclose(fout);

    return result;
}

static int handle_split_or_merge(int argc, char **argv, int *index, const char *current_arg, const char **error_messages, split_merge_handler handler)
{
    const char *filenames[] = {NULL, NULL, NULL};
    bool is_interleaved;

    if (!get_arguments_helper(argc, argv, index, current_arg, filenames, error_messages, &is_interleaved))
    {
        return 1;
    }

    return (*handler)(filenames[0], filenames[1], filenames[2], is_interleaved);
}

static bool get_arguments_helper(int argc, char **argv, int *index, const char *current_arg, const char **filenames, const char **error_messages, bool *is_interleaved)
{
    if (!get_next_three_filenames(argc, argv, index, filenames, error_messages))
    {
        return false;
    }

    current_arg = unshift_next_valid_arg(argc, argv, index, NULL, is_interleaved_arg);
    if (current_arg == INVALID_ARGUMENT)
    {
        fprintf(stderr, "Invalid argument: %s\n", argv[*index]);
        return false;
    }

    if (!is_interleaved) { return false; }
    *is_interleaved = (current_arg != NO_MORE_ARGUMENTS);

    return true;
}

static bool get_next_three_filenames(int argc, char **argv, int *index, const char **filenames, const char **error_messages)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        filenames[i] = unshift_next_arg(argc, argv, index);
        if (filenames[i] == NO_MORE_ARGUMENTS)
        {
            fprintf(stderr, "%s\n", error_messages[i]);
            return false;
        }
    }

    return true;
}

static long file_length(FILE *fp)
{
    long current_pos, length;
    if (!fp) { return 0L; }

    current_pos = ftell(fp);

    fseek(fp, 0, SEEK_END);
    length = ftell(fp);

    fseek(fp, current_pos, SEEK_SET);

    return length;
}

static void file_close(FILE *fp)
{
    if (fp)
    {
        fclose(fp);
    }
}