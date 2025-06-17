#ifndef SIG_HANDLER_H
#define SIG_HANDLER_H

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>

void sigchld_handler();
int setup_sig_handler();

#endif
