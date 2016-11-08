//
// Created by frebib on 08/11/16.
//

#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <zconf.h>
#include <stdbool.h>

#include "transport.h"

int bindSocket(int port) {
  // Format the port as a string
  char port_chr[6];
  sprintf(port_chr, "%d", port);

  int sock, yes = 1;
  struct addrinfo hints, * info;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;     // use IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  int ret = getaddrinfo(NULL, port_chr, &hints, &info);
  if (ret != 0) {
    fprintf(stderr, "getaddrinfo: %s", gai_strerror(ret));
    freeaddrinfo(info);
    return -1;
  }

  // Open a TCP stream socket
  if ((sock = socket(info->ai_family, info->ai_socktype, info->ai_protocol)) == -1) {
    fprintf(stderr, "Failed to open socket: %s\n", strerror(errno));
    freeaddrinfo(info);
    return -1;
  }
  // Ensure SO_REUSEADDR is set
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    fprintf(stderr, "Failed to set socket option SO_REUSEADDR: %s\n", strerror(errno));
    freeaddrinfo(info);
    return -1;
  }

  // Bind the socket
  if (bind(sock, info->ai_addr, info->ai_addrlen) == -1) {
    fprintf(stderr, "Failed to bind socket: %s\n", strerror(errno));
    if (errno == EACCES) {
      fprintf(stderr, "You should run this program as root to bind the port %d or use a port >= 1024\n", port);
    }
    freeaddrinfo(info);
    return -1;
  }
  freeaddrinfo(info);

  return sock;
}

void ssl_init() {
  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();
}

SSL_CTX* ssl_create_ctx(const char* cert_path, const char* key_path) {
  const SSL_METHOD* method;
  SSL_CTX* ctx;

  // Auto-negotiate SSL method
  method = SSLv23_server_method();

  ctx = SSL_CTX_new(method);
  if (!ctx) {
    fprintf(stderr, "Unable to create SSL context: ");
    ERR_print_errors_fp(stderr);
    return NULL;
  }

  SSL_CTX_set_ecdh_auto(ctx, 1);

  // Load key and certificate files
  if (SSL_CTX_use_certificate_file(ctx, cert_path, SSL_FILETYPE_PEM) < 0) {
    ERR_print_errors_fp(stderr);
    return NULL;
  }
  if (SSL_CTX_use_PrivateKey_file(ctx, key_path, SSL_FILETYPE_PEM) < 0) {
    ERR_print_errors_fp(stderr);
    return NULL;
  }

  return ctx;
}

ssize_t read_sock(http_sock_t* stream, void* buf, size_t count) {
  switch (stream->http_sock_type) {
    case HTTP_SOCK_RAW:
      return read(stream->fd, buf, count);

    case HTTP_SOCK_TLS:
      return SSL_read(stream->ssl_conn, buf, (int) count);

    default:
      return -1;
  }
}

int write_sock(http_sock_t* stream, void* buf, size_t count) {
  switch (stream->http_sock_type) {
    case HTTP_SOCK_RAW:
      return (int) write(stream->fd, buf, count);

    case HTTP_SOCK_TLS:
      return SSL_write(stream->ssl_conn, buf, (int) count);

    default:
      return -1;
  }
}

int sputs(char* buffer, http_sock_t* stream) {
  return write_sock(stream, buffer, strlen(buffer));
}

int sreadc(http_sock_t* stream) {
  char c;
  ssize_t ret = read_sock(stream, &c, 1);
  return (int) (ret >= 0 ? c : ret);
}

int sclose(http_sock_t* stream) {
  switch (stream->http_sock_type) {
    case HTTP_SOCK_TLS:
      SSL_free(stream->ssl_conn);
      break;
  }
  return close(stream->fd);
}

ssize_t read_line(char** buf, size_t* buf_len, http_sock_t* stream) {
  return read_delim(buf, buf_len, '\n', stream);
}

#define BODY_CHUNK 32

ssize_t read_delim(char** buf, size_t* buf_len, int delim, http_sock_t* stream) {
  ssize_t buf_read = 0;

  if (*buf == NULL) {
    *buf_len = BODY_CHUNK;
    *buf = malloc(*buf_len);
  }

  while (true) {
    int c = sreadc(stream);

    if (c == EOF || c == delim) {
      (*buf)[buf_read++] = (char) c;
      (*buf)[buf_read] = '\0';
      return buf_read;
    } else {
      // If buffer is full
      if (buf_read >= *buf_len - 1) {
        // Double size of the buffer
        *buf_len <<= 2;
        // Reallocate it
        *buf = realloc(*buf, *buf_len);
      }

      (*buf)[buf_read++] = (char) c;
    }
  }
}
