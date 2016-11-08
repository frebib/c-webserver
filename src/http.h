#ifndef _HTTP
#define _HTTP

#define HTTP_VERSION "HTTP/1.1"

#define HTTP_OPTIONS  0
#define HTTP_GET      1
#define HTTP_HEAD     2
#define HTTP_POST     3
#define HTTP_PUT      4
#define HTTP_DELETE   5

// 2XX Codes
#define HTTP_OK               200
#define HTTP_CREATED          201
#define HTTP_ACCEPTED         202
#define HTTP_NO_CONTENT       204
#define HTTP_PARTIAL_CONTENT  206

// 3XX Codes
#define HTTP_MULT_CHOICES     300
#define HTTP_MOVED_PERM       301
#define HTTP_FOUND            302
#define HTTP_SEE_OTHER        303
#define HTTP_NOT_MODIFIED     304

// 4XX Codes
#define HTTP_BAD_REQUEST      400
#define HTTP_UNAUTHORIZED     401
#define HTTP_FORBIDDEN        403
#define HTTP_NOT_FOUND        404
#define HTTP_METHOD_NOT_ALLOWED 405
#define HTTP_NOT_ACCEPTABLE   406
#define HTTP_REQ_TIMEOUT      408
#define HTTP_IM_A_TEAPOT      418

// 5XX Codes
#define HTTP_INTERN_SRV_ERR   500
#define HTTP_NOT_IMPLEMENTED  501
#define HTTP_BAD_GATEWAY      502
#define HTTP_SERVICE_UNAVAIL  503
#define HTTP_GATEWAY_TIMEOUT  504
#define HTTP_VERSION_UNSUPP   505


struct http_req {
    int http_ver_major;
    int http_ver_minor;
    int method;
    char* path;
    struct http_header* headers;
    size_t body_len;
    char* body;
};


int request_type(char* type);

const char* status(int status_code);

int status_line(int status_code, char* buffer, size_t buf_size);

size_t status_line_len(int status_code);

int parse_http_ver(char* version_str, size_t version_str_len, struct http_req* request);

int error_page(char** buffer, int response_code);

#endif
