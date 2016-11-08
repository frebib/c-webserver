#ifndef _HTTP_REQUEST
#define _HTTP_REQUEST

#include "http.h"
#include "http_header.h"

/**
 * Retrieves a HTTP request from a
 * stream and responds accordingly
 * @param request   Request object to populate
 * @param client_fd Client stream file descriptor
 */
int read_req(struct http_req* request, FILE* stream);

int read_head(FILE* stream, http_header_t** ptr);

void free_req(struct http_req* request);

#endif
