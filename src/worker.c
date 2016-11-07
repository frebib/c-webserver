#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <tkPort.h>
#include <magic.h>
#include <sys/sendfile.h>

#include "worker.h"
#include "response.h"
#include "request.h"

void handle(int fd) {
  FILE* stream = fdopen(fd, "w+");

  // Parse client request
  struct http_req* request = malloc(sizeof(struct http_req));
  memset(request, 0, sizeof(struct http_req));
  read_req(request, stream);

  char* filepath = request->path;
  if (*filepath == '/') {
    filepath += 1;
  }

  // Get default headers, these are always sent
  http_header_t* headers = default_headers();

  if (access(filepath, F_OK) == 0) {
    // file exists
    if (access(filepath, R_OK) == 0) {
      // file is readable
      send_status_line(stream, HTTP_OK);

      // TODO: Determine mimetype and send header
      
      // Open file and seek to end to determine length
      struct stat file_stat;
      int file_fd = open(filepath, O_RDONLY);
      fstat(file_fd, &file_stat);

      // Construct Content-Length header
      http_header_t* cont_len = malloc(sizeof(http_header_t));
      cont_len->name = strdup("Content-Length");
      cont_len->value = malloc(20);
      snprintf(cont_len->value, 20, "%li", file_stat.st_size);

      cont_len->next = headers;
      headers = cont_len;

      // Send headers
      send_head(stream, headers);
      free_head(headers);

      // Cheat and use sendfile() to copy the file
      sendfile(fd, file_fd, 0, (size_t) file_stat.st_size);

      // Close sent file
      close(file_fd);

    } else {
      // file exists but is not readable
      send_status_line(stream, HTTP_FORBIDDEN);

      // Send headers
      send_head(stream, headers);
      free_head(headers);

      // TODO: Send default 'Error page'
    }
  } else {
    // file doesn't exist
    send_status_line(stream, HTTP_NOT_FOUND);

    // Send headers
    send_head(stream, headers);
    free_head(headers);

    // TODO: Send default 'Error page'
  }

  free_req(request);
  fclose(stream);
}
