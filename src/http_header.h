#ifndef _HTTP_HEADER
#define _HTTP_HEADER

typedef struct http_header {
    char* name;
    char* value;
    struct http_header* next;
} http_header_t;

void free_head(http_header_t* headers);

http_header_t* find_header(http_header_t* headers, char* key);


#endif //WEBSERVER_HTTP_HEADER_H
