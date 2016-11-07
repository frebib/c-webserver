#include <malloc.h>
#include <string.h>
#include <time.h>

#include "http_header.h"

void free_head(struct http_header* headers) {
  while (headers != NULL) {
    struct http_header* next = headers->next;
    free(headers->name);
    free(headers->value);
    free(headers);
    headers = next;
  }
}

struct http_header* find_header(struct http_header* headers, char* key) {
  while (headers != NULL) {
    if (strcasecmp(headers->name, key) == 0) {
      return headers;
    }
    headers = headers->next;
  }
  return NULL;
}
