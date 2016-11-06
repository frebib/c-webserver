#include <stdlib.h>

#include "net.h"

int PORT = 8088;

int main(int argc, char* argv[]) {
  int sock = bindSocket(PORT);
  if (sock == -1) {
    return EXIT_FAILURE;
  }


  return EXIT_SUCCESS;
}
