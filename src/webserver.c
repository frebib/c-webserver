#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>

#include "net.h"
#include "worker.h"

int PORT = 8088;

int serve(int sock) {
  while (true) {
    struct sockaddr_storage cl_addr;
    socklen_t cl_addr_size = sizeof cl_addr;
    int client_fd;

    // Accept incoming client connections
    if ((client_fd = accept(sock, (struct sockaddr*) &cl_addr, &cl_addr_size)) != -1) {

      int pid = fork();
      if (pid == -1) {
        fprintf(stderr, "Failed to fork() on client connection: %s\n", strerror(errno));
        return EXIT_FAILURE;

        // Child process
      } else if (pid == 0) {
        // Close server socket
        close(sock);

        handle(client_fd);

        // Terminate child process
        return EXIT_SUCCESS;

      } else {
        // Parent server process
        close(client_fd);

        // Clear the client address
        memset(&cl_addr, '\0', cl_addr_size);

        // Loop back around and accept another connection
      }
    } else {
      fprintf(stderr, "Failed to accept connections: %s\n", strerror(errno));
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {

  // TODO: Take daemon flag as a command line argument
  bool daemon = true;

  // Open a socket and bind to it (for your life)
  int sock;
  if ((sock = bindSocket(PORT)) == -1) {
    return EXIT_FAILURE;
  }
  // Listen for incoming connections
  if (listen(sock, 5) == -1) {
    fprintf(stderr, "Failed to listen on socket: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }
  printf("Server listening on port %d\n", PORT);

  int pid = daemon ? fork() : 0;
  if (pid == -1) {
    fprintf(stderr, "Failed to fork() creating daemon: %s\n", strerror(errno));
    return EXIT_FAILURE;
  } else if (pid == 0) {
    // Child Process
    setsid();

    // Don't wait for the child to terminate
    // This prevents zombie children processes
    struct sigaction sigchld_action = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_NOCLDWAIT
    };
    sigaction(SIGCHLD, &sigchld_action, NULL);

    return serve(sock);

  } else {
    // Parent process

    // Daemon complete, exit now!
    return EXIT_SUCCESS;
  }
}
