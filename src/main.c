#include "../include/main.h"
#include <stdio.h>
#include <sys/socket.h>

int main() {
  int server_fd;
  char buf[1024];
  memset(buf, 0, sizeof(buf));

  if ((server_fd = setup_server()) == -1) {
    fprintf(stderr, "server: failed to setup server\n");
    exit(1);
  }
  if (setup_sig_handler() == -1) {
    fprintf(stderr, "server: failed to setup signal handler\n");
    exit(1);
  }
  printf("server: waiting for connections...\n");

  int client_fd;
  struct sockaddr_storage client_addr;
  socklen_t client_len;

  while (1) {
    client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd == -1) {
      if (errno == EINTR)
        continue;
      close(server_fd);
      perror("accept");
      exit(1);
    }

    fprintf(stdout, "server: new connection from %s\n",
            inet_ntoa(((struct sockaddr_in *)&client_addr)->sin_addr));

    if (!fork()) {
      close(server_fd);
      if (read(client_fd, buf, sizeof(buf)) == -1)
        perror("read");
      if (handle_client(client_fd, buf) == -1) {
        perror("handle_client");
        fflush(stdout);
        exit(1);
      }
      close(client_fd);
      fprintf(stdout, "server: client closed\n\n");
      fflush(stdout);
      exit(0);
    }
    close(client_fd);
  }

  return EXIT_SUCCESS;
}
