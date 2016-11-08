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

int read_req(struct http_req* request, FILE* stream) {
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
    fprintf(stderr, "Invalid method: %s\n", buf);
    free(buf);
    return HTTP_NOT_IMPLEMENTED;
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
        return HTTP_BAD_REQUEST;
      default:
        // TODO: Send HTTP_INTERN_SRV_ERR
        return HTTP_INTERN_SRV_ERR;
    }
  }
  free(buf);

  // Read headers from request
  if (read_head(stream, &request->headers) != 0) {
    return HTTP_BAD_REQUEST;
  }

  // Read body if Content-Length header is present
  http_header_t* len_head = find_header(request->headers, "content-length");
  if (len_head != NULL) {
    size_t cont_len = strtoul(len_head->value, NULL, 10);
    char* body = malloc((size_t) cont_len);
    fread(body, 1, cont_len, stream);

    request->body = body;
    request->body_len = cont_len;
  }

  // Prevent timeout, we're done now.
  alarm(0);

  return 0;
}

int read_head(FILE* stream, http_header_t** ptr) {
  http_header_t* head = NULL, *curr = NULL;

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
      *ptr = head;
      return 0;
    }

    char* colon = strchr(line, ':');
    if (colon != NULL) {
      colon[0] = '\0';
    } else {
      // Header line contains no colon
      return HTTP_BAD_REQUEST;
    }

    // malloc() a buffer for header name
    size_t len = strlen(line) + 1;
    char* name = malloc(len);
    if (name == NULL) {
      free(line);
      free_head(head);
      return -1;
    }
    strncpy(name, line, len);

    // malloc() a buffer for header value
    char* val_tmp = colon + 1;
    while (*val_tmp == ' ' || *val_tmp == '\t') {
      val_tmp++;
    }
    len = strlen(val_tmp) + 1;
    char* value = malloc(len);
    if (value == NULL) {
      free(line);
      free_head(head);
      return -1;
    }
    strncpy(value, val_tmp, len);

    // Dispose of temporary line buffer
    free(line);

    http_header_t* prev = curr;
    curr = malloc(sizeof(http_header_t));
    if (head == NULL)
      head = curr;
    if (prev != NULL)
      prev->next = curr;
    curr->name = name;
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
