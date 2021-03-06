#ifndef _HTTP_HEADER
#define _HTTP_HEADER

#define SERVER_VER  "0.1"
#define SERVER_NAME "webserver"
#define SERVER_NAME_VER SERVER_NAME "/" SERVER_VER

typedef struct http_header {
    char* name;
    char* value;
    struct http_header* next;
} http_header_t;

void free_head(http_header_t* headers);

http_header_t* find_header(http_header_t* headers, char* key);

http_header_t* default_headers();

http_header_t* cont_len_head(long len);

int date_head(http_header_t* header);

int serv_head(http_header_t* header);

int conn_close_head(http_header_t* header);

#endif //WEBSERVER_HTTP_HEADER_H
