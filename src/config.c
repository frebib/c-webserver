#include <stdlib.h>
#include <jansson.h>
#include <string.h>
#include <libgen.h>

#include "config.h"


void default_args(cmd_flags_t* config) {
  memset(config, '\0', sizeof(cmd_flags_t));
  config->daemon = false;
  config->conf_file = NULL;
}

int parse_argv(cmd_flags_t* config, int argc, char** argv) {
  if (config == NULL) {
    return -1;
  }

  int c;
  while ((c = getopt(argc, argv, "hdc:")) != -1) {
    switch (c) {
      case 'h':
        printf("Usage: %s -c <config file> [-d] [-h]\n\nOptions:\n", basename(*argv));
        printf("    -h      \tShow this help message, then exit\n");
        printf("    -d      \tRun the server in daemon mode\n");
        printf("    -c file \tSpecify a server config file\n");
        exit(0);
      case 'd':
        config->daemon = true;
        break;
      case 'c':
        config->conf_file = optarg;
        break;
      default:
        if (optopt == 'c')
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        return -1;
    }
  }

  return 0;
}


http_config_t* load_config(char* path) {
  json_error_t error;
  json_t* conf = json_load_file(path, 0, &error);

  json_t* srv_arr = NULL;
  if (json_is_array(conf))
    srv_arr = conf;
  else if (json_is_object(conf))
    srv_arr = json_object_get(conf, "servers");
  else
    return NULL;

  if (srv_arr == NULL) {
    fprintf(stderr, "Configuration file contains no valid servers.\n");
    return NULL;
  }

  http_config_t* config = calloc(1, sizeof(http_config_t));
  http_server_t* srv_conf = NULL, *temp;

  int i;
  json_t* srv;
  json_array_foreach(srv_arr, i, srv) {
    temp = srv_conf;
    srv_conf = calloc(1, sizeof(http_server_t));

    srv_conf->next = temp;
    srv_conf->port = (int) json_integer_value(json_object_get(srv, "port"));
    srv_conf->use_ssl = json_boolean_value(json_object_get(srv, "use_ssl"));
    srv_conf->webroot = strdup(json_string_value(json_object_get(srv, "webroot")));
    srv_conf->ssl_key = strdup(json_string_value(json_object_get(srv, "ssl_key")));
    srv_conf->ssl_cert = strdup(json_string_value(json_object_get(srv, "ssl_cert")));
  }

  config->servers = srv_conf;
  config->user = strdup(json_string_value(json_object_get(conf, "user")));
  config->group = strdup(json_string_value(json_object_get(conf, "group")));

  json_delete(conf);

  return config;
}

void free_config(http_config_t* config) {
  http_server_t* srv = config->servers, *tmp;
  while(srv != NULL) {
    tmp = srv->next;
    free(srv->webroot);
    free(srv->ssl_key);
    free(srv->ssl_cert);
    free(srv);
    srv = tmp;
  }
  free(config);
}
