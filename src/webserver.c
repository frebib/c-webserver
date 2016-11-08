#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <pwd.h>
#include <openssl/ssl.h>
#include <grp.h>

#include "worker.h"
#include "config.h"

int serve(int sock, http_server_t* config) {
  while (true) {
    struct sockaddr_storage cl_addr;
    socklen_t cl_addr_size = sizeof cl_addr;
    int client_fd;

    int sock_type = config->use_ssl ? HTTP_SOCK_TLS : HTTP_SOCK_RAW;

    SSL_CTX* ctx;
    if (sock_type == HTTP_SOCK_TLS) {
      ssl_init();
      ctx = ssl_create_ctx(config->ssl_cert, config->ssl_key);
    }

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

        http_sock_t connection = {
            .http_sock_type = sock_type,
            .fd             = client_fd,
            .file           = fdopen(client_fd, "w+")
        };

        switch (sock_type) {
          case HTTP_SOCK_TLS:
            connection.ssl_conn = SSL_new(ctx);
            SSL_set_fd(connection.ssl_conn, client_fd);
            SSL_accept(connection.ssl_conn);
            break;
        }

        handle(&connection);

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

  cmd_flags_t flags;
  default_args(&flags);

  if (parse_argv(&flags, argc, argv) != 0) {
    fprintf(stderr, "Error parsing command-line arguments\n");
    return EXIT_FAILURE;
  }

  if (flags.conf_file == NULL) {
    printf("No config file specified. Specify one with -c <file>\n");
    return EXIT_FAILURE;
  }

  http_config_t* config = load_config(flags.conf_file);
  // TODO: Free config at some point

  // Open a socket and bind to it (for your life)
  int sock;
  if ((sock = bindSocket(config->servers->port)) == -1) {
    return EXIT_FAILURE;
  }
  // Listen for incoming connections
  if (listen(sock, 5) == -1) {
    fprintf(stderr, "Failed to listen on socket: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  // Retrieve user info
  errno = 0;
  struct passwd* user = getpwnam(config->user);
  if (user == NULL) {
    fprintf(stderr, "Failed to get user information for '%s':\n%s\n", config->user, strerror(errno));
    return EXIT_FAILURE;
  }
  // Give up higher privilege
  int ret_uid = setuid(user->pw_uid);
  if (ret_uid != 0)
    fprintf(stderr, "Failed to setuid(%d): %s\n", user->pw_uid, strerror(errno));
  struct group* grp = getgrnam(config->group);
  int ret_gid = setgid(grp->gr_gid);
  if (ret_gid != 0)
    fprintf(stderr, "Failed to setgid(%d): %s\n", grp->gr_gid, strerror(errno));

  if (ret_uid != 0 || ret_gid != 0) {
    struct passwd* passwd = getpwuid(getuid());
    fprintf(stderr, "WARNING: Running as %s:%s\n", passwd->pw_name, getgrgid(passwd->pw_gid)->gr_name);
  }

  printf("Server listening on port %d\n", config->servers->port);

  bool daemon = flags.daemon;
  if (daemon) {
    printf("Running in daemon mode. The process will now slide into the shadows\n");
  }

  // Take 'daemon' flag from commandline arguments
  int pid = daemon ? fork() : 0;
  if (pid == -1) {
    fprintf(stderr, "Failed to fork() creating daemon: %s\n", strerror(errno));
    return EXIT_FAILURE;
  } else if (pid == 0) {
    // Child Process

    // A strong independent process who don't need no parent
    setsid();

    // Don't wait for the child to terminate
    // This prevents zombie children processes
    struct sigaction sigchld_action = {
        .sa_handler = SIG_DFL,
        .sa_flags   = SA_NOCLDWAIT
    };
    sigaction(SIGCHLD, &sigchld_action, NULL);

    return serve(sock, config->servers);

  } else {
    // Parent process

    // Daemon complete, exit now!
    return EXIT_SUCCESS;
  }
}
