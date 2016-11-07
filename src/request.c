#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <signal.h>
#include <pcre.h>
#include <string.h>
#include <stdbool.h>

#include "request.h"

void req_timeout(int signal) {
  // TODO: Cleanup and reply on timeout
  fprintf(stderr, "Timeout!\n");
}

void read_req(struct http_req* request, FILE* stream) {
  // Handle timeout after 60 seconds
  signal(SIGALRM, req_timeout);
  alarm(60);

  // Parse request method
  char* buf = NULL;
  size_t buf_size;
  ssize_t read = getdelim(&buf, &buf_size, ' ', stream);
  // Remove trailing space
  if (buf[read - 1] == ' ')
    buf[read - 1] = 0;

  // Ensure the method is a valid one
  if ((request->method = request_type(buf)) == -1) {
    // TODO: Send HTTP_NOT_IMPLEMENTED
    fprintf(stderr, "Invalid method: %s\n", buf);
    free(buf);
    return;
  }

  // Cleanup
  free(buf);
  buf = NULL;
  buf_size = 0;

  // Get request path
  read = getdelim(&buf, &buf_size, ' ', stream);
  // Remove trailing space
  if (buf[read - 1] == ' ')
    buf[read - 1] = 0;
  request->path = buf;
  buf = NULL;
  buf_size = 0;

  // Read HTTP Version
  read = getdelim(&buf, &buf_size, '\n', stream);
  int ret = parse_http_ver(buf, (size_t) read, request);
  if (ret != 0) {
    switch (ret) {
      case PCRE_ERROR_NOMATCH:
        // TODO: Send Invalid HTTP Version
        return;
      default:
        // TODO: Send HTTP_INTERN_SRV_ERR
        return;
    }
  }
  free(buf);

  // Read headers from request
  request->headers = read_head(stream);

  // Read body if Content-Length header is present
  struct http_header* len_head = find_header(request->headers, "content-length");
  if (len_head != NULL) {
    size_t cont_len = strtoul(len_head->value, NULL, 10);
    char* body = malloc((size_t) cont_len);
    fread(body, 1, cont_len, stream);

    request->body = body;
    request->body_len = cont_len;
  }

  // Prevent timeout, we're done now.
  alarm(0);
}

struct http_header* read_head(FILE* stream) {
  struct http_header* head = NULL, *curr = NULL;

  while (true) {
    char* line = NULL;
    size_t line_size;
    ssize_t read = getline(&line, &line_size, stream);
    if (line[read - 2] == '\r')
      line[read - 2] = '\0';
    else if (line[read - 1] == '\n')
      line[read - 1] = '\0';

    // Return if hit an empty line
    if (*line == '\0') {
      free(line);
      return head;
    }

    char* colon = strchr(line, ':');
    colon[0] = '\0';

    char* value = colon + 1;
    while (*value == ' ' || *value == '\t') {
      value++;
    }

    struct http_header* prev = curr;
    curr = malloc(sizeof(struct http_header));
    if (head == NULL)
      head = curr;
    if (prev != NULL)
      prev->next = curr;
    curr->name = line;
    curr->value = value;
    curr->next = NULL;
  }
}

void free_req(struct http_req* request) {
  free(request->body);
  free(request->path);
  free_head(request->headers);
  free(request);
}

void free_head(struct http_header* headers) {
  while(headers != NULL) {
    struct http_header* next = headers->next;
    free(headers->name);
    free(headers);
    headers = next;
  }
}
