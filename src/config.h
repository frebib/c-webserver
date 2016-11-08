#ifndef WEBSERVER_CONFIG_H
#define WEBSERVER_CONFIG_H

#include <stdbool.h>
#include <getopt.h>
#include <ctype.h>

// Command-line arguments

typedef struct cmd_flags {
    bool daemon;
    char* conf_file;
} cmd_flags_t;

void default_args(cmd_flags_t* config);

int parse_argv(cmd_flags_t* config, int argc, char** argv);

#endif //WEBSERVER_CONFIG_H
