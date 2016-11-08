#include <malloc.h>
#include <errno.h>
#include <string.h>

#include "response.h"

int send_err_resp(http_sock_t* stream, int response_code, http_header_t* headers) {
  errno = 0;
  char* error_resp;
  if (error_page(&error_resp, response_code) < 0) {
    fprintf(stderr, "Failed to generate error page: %s\n", strerror(errno));
    return -1;
  }

  send_status_line(stream, response_code);

  // Construct Content-Length header
  http_header_t* cont_len = cont_len_head(strlen(error_resp));
  cont_len->next = headers;
  headers = cont_len;

  // Send headers
  send_head(stream, headers);
  sputs(error_resp, stream);
  free(error_resp);

  return 0;
}

int send_status_line(http_sock_t* stream, int response_code) {
  // Send appropriate status line for response code
  size_t len = status_line_len(response_code);
  char buffer[len];
  status_line(response_code, buffer, len);
  int ret = write_sock(stream, buffer, sizeof buffer);
  if (ret < 0)
    return ret;
  ret = send_crlf(stream);
  if (ret < 0)
    return ret;

  return 0;
}

int send_head(http_sock_t* stream, http_header_t* header) {
  while (header != NULL) {
    sputs(header->name, stream);
    sputs(": ", stream);
    sputs(header->value, stream);
    send_crlf(stream);

    header = header->next;
  }
  send_crlf(stream);

  return 0;
}

int send_crlf(http_sock_t* stream) {
  return write_sock(stream, "\r\n", 2);
}
