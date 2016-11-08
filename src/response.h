#ifndef _HTTP_RESPONSE
#define _HTTP_RESPONSE

#include <stdio.h>

#include "http.h"
#include "http_header.h"

int send_err_resp(FILE *stream, int response_code, http_header_t *headers);

int send_status_line(FILE* stream, int response_code);

int send_head(FILE* stream, http_header_t* header);

int send_crlf(FILE* stream);

#endif