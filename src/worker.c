#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <magic.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#include "worker.h"
#include "response.h"
#include "request.h"

void handle(http_sock_t* stream) {
  // Get default headers, these are always sent
  http_header_t* headers = default_headers();

  // Parse client request
  struct http_req* request = malloc(sizeof(struct http_req));
  memset(request, 0, sizeof(struct http_req));
  int error = read_req(request, stream);
  // read_req may throw an HTTP code as an error
  if (error != 0) {
    if (error < 100) {
      error = HTTP_BAD_REQUEST;
    }

    send_err_resp(stream, error, headers);
    free_head(headers);
    return;
  }

  char* filepath = request->path;
  if (*filepath == '/') {
    filepath += 1;
  }

  if (access(filepath, F_OK) == 0) {
    // file exists
    if (access(filepath, R_OK) == 0) {
      // file is readable
      send_status_line(stream, HTTP_OK);

      // Use libmagic (so amazing) to determine file mimetype
      magic_t magic_cookie = magic_open(MAGIC_MIME);
      if (magic_cookie == NULL) {
        fprintf(stderr, "Error initialing magic cookie for libmagic MIME type lookup\n");
      } else {
        // Attempt to load the magic database
        if (magic_load(magic_cookie, NULL) != 0) {
          printf("Cannot load magic database: %s\n", magic_error(magic_cookie));
          magic_close(magic_cookie);
          exit(1);
        }

        // Determine the mimetype for the found file
        const char* mime = magic_file(magic_cookie, filepath);

        // Construct content-type header
        http_header_t* cont_typ = malloc(sizeof(http_header_t));
        cont_typ->name = strdup("Content-Type");
        cont_typ->value = strdup(mime);

        cont_typ->next = headers;
        headers = cont_typ;

        // Clean-up
        magic_close(magic_cookie);
      }

      // Open file and seek to end to determine length
      struct stat file_stat;
      int file_fd = open(filepath, O_RDONLY);
      fstat(file_fd, &file_stat);

      // Construct Content-Length header
      http_header_t* cont_len = cont_len_head(file_stat.st_size);
      cont_len->next = headers;
      headers = cont_len;

      // Send headers
      send_head(stream, headers);
      free_head(headers);

      if (request->method != HTTP_HEAD) {
        // Cheat and use sendfile() to copy the file
        sendfile(stream, file_fd, 0, (size_t) file_stat.st_size);
      }

      // Close sent file
      close(file_fd);

    } else {
      // file exists but is not readable
      send_err_resp(stream, HTTP_FORBIDDEN, headers);
      free_head(headers);
    }
  } else {
    // file doesn't exist
    send_err_resp(stream, HTTP_NOT_FOUND, headers);
    free_head(headers);

    // TODO: Send default 'Error page'
  }

  free_req(request);
  fclose(stream);
}
