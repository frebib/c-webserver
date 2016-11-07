#include <malloc.h>

#include "response.h"
#include "http_header.h"

int send_status_line(FILE* stream, int response_code) {
  // Send appropriate status line for response code
  size_t len = status_line_len(response_code);
  char buffer[len];
  status_line(response_code, buffer, len);
  int ret = fputs(buffer, stream);
  if (ret < 0)
    return ret;
  ret = send_crlf(stream);
  if (ret < 0)
    return ret;

  return 0;
}

int send_head(FILE* stream, http_header_t* header) {
  while (header != NULL) {
    fputs(header->name, stream);
    fputs(": ", stream);
    fputs(header->value, stream);
    send_crlf(stream);

    header = header->next;
  }
  send_crlf(stream);

  // Flush the stream to ensure headers get sent first
  fflush(stream);
  return 0;
}

int send_crlf(FILE* stream) {
  return fputs("\r\n", stream);
}
