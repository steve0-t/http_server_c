#include "../include/sig_handler.h"

void sigchld_handler() {
  int status = errno;
  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;
  errno = status;
}

int setup_sig_handler() {
  struct sigaction sa;
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    return -1;
  }
  return 0;
}
