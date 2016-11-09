#include "http.h"
#include "transport.h"

void init_handle(http_sock_t* stream, int fd, SSL_CTX* ctx);

void cleanup_handle(http_sock_t* stream);

void handle(http_sock_t* stream);
