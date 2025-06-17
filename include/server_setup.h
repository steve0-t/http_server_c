#ifndef SERVER_SETUP_H
#define SERVER_SETUP_H

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BACKLOG 20
#define PORT "8080"
#define DOMAIN "localhost"

int setup_server();

#endif
