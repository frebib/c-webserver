#ifndef _HTTP_RESPONSE
#define _HTTP_RESPONSE

#include <stdio.h>

#include "http.h"
#include "http_header.h"
#include "transport.h"

int send_err_resp(http_sock_t* stream, int response_code, http_header_t* headers);

int send_status_line(http_sock_t* stream, int response_code);

int send_head(http_sock_t* stream, http_header_t* header);

int send_crlf(http_sock_t* stream);

#endif