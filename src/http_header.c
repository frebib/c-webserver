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

http_header_t* default_headers() {
  http_header_t* headers = malloc(sizeof(http_header_t));
  headers->next               = malloc(sizeof(http_header_t));
  headers->next->next         = malloc(sizeof(http_header_t));
  headers->next->next->next   = NULL;

  date_head(headers);
  serv_head(headers->next);
  conn_close_head(headers->next->next);

  return headers;
}

http_header_t* cont_len_head(long len) {
  http_header_t* cont_len = malloc(sizeof(http_header_t));
  cont_len->name = strdup("Content-Length");
  cont_len->value = malloc(20);
  snprintf(cont_len->value, 20, "%li", len);

  return cont_len;
}

int date_head(http_header_t* header) {
  if (header == NULL)
    return -1;

  size_t len = 128;
  char* buf = malloc(len);
  time_t now = time(0);
  struct tm time = *gmtime(&now);
  strftime(buf, len, "%a, %d %b %Y %H:%M:%S %Z", &time);

  header->name = strdup("Date");
  header->value = buf;

  return 0;
}

int serv_head(http_header_t* header) {
  if (header == NULL)
    return -1;

  header->name = strdup("Server");
  header->value = strdup(SERVER_NAME_VER);

  return 0;
}

int conn_close_head(http_header_t* header) {
  if (header == NULL)
    return -1;

  header->name = strdup("Connection");
  header->value = strdup("close");

  return 0;
}
