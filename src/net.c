#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>

#include "net.h"

int bindSocket(int port) {
  // Format the port as a string
  char port_chr[6];
  sprintf(port_chr, "%d", port);

  int sock, yes = 1;
  struct addrinfo hints, *info;
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
