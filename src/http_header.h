#ifndef _HTTP_HEADER
#define _HTTP_HEADER

struct http_header {
    char* name;
    char* value;
    struct http_header* next;
};

void free_head(struct http_header* headers);

struct http_header* find_header(struct http_header* headers, char* key);

#endif //WEBSERVER_HTTP_HEADER_H
