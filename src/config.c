#include <stdlib.h>
#include <jansson.h>
#include <string.h>

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
  while ((c = getopt(argc, argv, "dc:")) != -1) {
    switch (c) {
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
