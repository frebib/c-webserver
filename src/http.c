#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <pcre.h>
#include <time.h>

#include "http.h"

int request_type(char* type) {
  if (strcasecmp("GET", type) == 0)
    return HTTP_GET;
  else if (strcasecmp("POST", type) == 0)
    return HTTP_POST;
  else if (strcasecmp("OPTIONS", type) == 0)
    return HTTP_OPTIONS;
  else if (strcasecmp("HEAD", type) == 0)
    return HTTP_HEAD;
  else if (strcasecmp("PUT", type) == 0)
    return HTTP_PUT;
  else if (strcasecmp("DELETE", type) == 0)
    return HTTP_DELETE;
  else
    return -1;
}

const char* status(int status_code) {
  switch (status_code) {
    // 2XX Codes
    case HTTP_OK:
      return "OK";
    case HTTP_CREATED:
      return "Created";
    case HTTP_ACCEPTED:
      return "Accepted";
    case HTTP_NO_CONTENT:
      return "No Content";
    case HTTP_PARTIAL_CONTENT:
      return "Partial Content";

      // 3XX Codes
    case HTTP_MULT_CHOICES:
      return "Multiple Choices";
    case HTTP_MOVED_PERM:
      return "Moved Permanently";
    case HTTP_FOUND:
      return "Found";
    case HTTP_SEE_OTHER:
      return "See Other";
    case HTTP_NOT_MODIFIED:
      return "Not Modified";

      // 4XX Codes
    case HTTP_BAD_REQUEST:
      return "Bad Request";
    case HTTP_UNAUTHORIZED:
      return "Unauthorized";
    case HTTP_FORBIDDEN:
      return "Forbidden";
    case HTTP_NOT_FOUND:
      return "Not Found";
    case HTTP_METHOD_NOT_ALLOWED:
      return "Method Not Allowed";
    case HTTP_NOT_ACCEPTABLE:
      return "Not Acceptable";
    case HTTP_REQ_TIMEOUT:
      return "Request Time-out";
    case HTTP_IM_A_TEAPOT:
      return "I'm a Teapot";

      // 5XX Codes
    case HTTP_INTERN_SRV_ERR:
      return "Internal Server Error";
    case HTTP_NOT_IMPLEMENTED:
      return "Not Implemented";
    case HTTP_BAD_GATEWAY:
      return "Bad Gateway";
    case HTTP_SERVICE_UNAVAIL:
      return "Service Unavailable";
    case HTTP_GATEWAY_TIMEOUT:
      return "Gateway Timeout";
    case HTTP_VERSION_UNSUPP:
      return "HTTP Version Not Supported";

    default:
      return NULL;
  }
}

int status_line(int status_code, char* buffer, size_t buf_size) {
  const char* status_str = status(status_code);
  if (status_str == NULL) {
    return -1;
  }
  return snprintf(buffer, buf_size, "%s %d %s", HTTP_VERSION, status_code, status_str);
}

size_t status_line_len(int status_code) {
  // "{{version}} {{code}} {{status}}\0"
  return strlen(HTTP_VERSION) + 1 + 3 + 1 + strlen(status(status_code)) + 1;
}

int parse_http_ver(char* version_str, size_t version_str_len, struct http_req* request) {
  // Use regex to parse major and minor HTTP versions
  int len = (int) version_str_len;
  int substr_vec[len];
  int error_offs = 0;
  const char* error = NULL;
  pcre* http_regex = pcre_compile("HTTP/(\\d+)\\.(\\d+)", 0, &error, &error_offs, NULL);
  if (http_regex == NULL) {
    fprintf(stderr, "ERROR: Could not compile regex: %s\n", error);
    return -100;
  }
  pcre_extra* extra = pcre_study(http_regex, 0, &error);
  int ret = pcre_exec(http_regex, extra, version_str, len, 0, 0, substr_vec, len);

  if (ret < 0) {
    return ret;
  } else {
    const char* substr = NULL;
    pcre_get_substring(version_str, substr_vec, ret, 1, &substr);
    request->http_ver_major = atoi(substr);
    pcre_free_substring(substr);

    pcre_get_substring(version_str, substr_vec, ret, 2, &substr);
    request->http_ver_minor = atoi(substr);
    pcre_free_substring(substr);
  }
  pcre_free_study(extra);
  pcre_free(http_regex);

  return 0;
}
