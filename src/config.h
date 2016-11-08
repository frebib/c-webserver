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

// Configuration file

typedef struct http_server {
    char** host;
    size_t host_len;

    int port;
    char* webroot;
    bool use_ssl;
    char* ssl_key;
    char* ssl_cert;

    struct http_server* next;
} http_server_t;

typedef struct http_config {
    bool daemon;
    char* user;
    char* group;
    http_server_t* servers;
} http_config_t;

http_config_t* load_config(char* path);

void free_config(http_config_t* config);

#endif //WEBSERVER_CONFIG_H
