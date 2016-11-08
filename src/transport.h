#ifndef _HTTP_SSL
#define _HTTP_SSL

#include <openssl/ssl.h>

#define HTTP_SOCK_RAW 0
#define HTTP_SOCK_TLS 1

typedef struct http_sock {
    int http_sock_type;
    int fd;
    union {
      // SOCK_HTTP
        // - Nothing special required
      // SOCK_HTTPS_TLS
        SSL* ssl_conn;
    };
} http_sock_t;

#endif
