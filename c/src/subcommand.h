#pragma once

typedef int (*subcommand___subcommand)(int argc, char **argv, int *index, const char *arg);

subcommand___subcommand subcommand___pick_subcommand(const char *arg);