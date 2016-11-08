#ifndef _HTTP_SSL
#define _HTTP_SSL

#include <openssl/ssl.h>

#define HTTP_SOCK_RAW 0
#define HTTP_SOCK_TLS 1

typedef struct http_sock {
    int http_sock_type;
    int fd;
    FILE* file;
    union {
      // SOCK_HTTP
        // - Nothing special required
      // SOCK_HTTPS_TLS
        SSL* ssl_conn;
    };
} http_sock_t;


/**
 * Opens a socket and binds to all available interfaces
 * @param port Port to open and bind socket on
 * @return file descriptor of socket or -1 on error
 */
int bindSocket(int port);

void ssl_init();

SSL_CTX* ssl_create_ctx(const char* cert_path, const char* key_path);

ssize_t read_sock(http_sock_t* stream, void* buf, size_t count);

int write_sock(http_sock_t* stream, void* buf, size_t count);

int sputs(char* buffer, http_sock_t* stream);

int sreadc(http_sock_t* stream);

int sclose(http_sock_t* stream);

ssize_t ssendfile(http_sock_t* stream, int fd_out, size_t count);

ssize_t read_line(char** buf, size_t* buf_len, http_sock_t* stream);

ssize_t read_delim(char** lineptr, size_t* buf_len, int delim, http_sock_t* stream);

#endif
