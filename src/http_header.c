#include <malloc.h>
#include <string.h>
#include <time.h>

#include "http_header.h"

void free_head(http_header_t* headers) {
  while (headers != NULL) {
    http_header_t* next = headers->next;
    free(headers->name);
    free(headers->value);
    free(headers);
    headers = next;
  }
}

http_header_t* find_header(http_header_t* headers, char* key) {
  while (headers != NULL) {
    if (strcasecmp(headers->name, key) == 0) {
      return headers;
    }
    headers = headers->next;
  }
  return NULL;
}
