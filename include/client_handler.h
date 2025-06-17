#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FILENAMES                                                              \
  (const char *[4]){"/", "/index.html", "/about.html", "/contact.html"}
#define PAGES_DIR "pages"

typedef enum res_type { BAD_REQ, FILE_NOT_FOUND, SERVER_ERR, OK } res_type;

typedef struct {
  char *method;
  char *path;
} request;

int handle_client(int client_fd, char *buf);
int parse_request(char *buf, request *req);
int sanitize_request(request *req, char *real_path);
char *generate_header(res_type type, long file_size);
int generate_response(int client_fd, char *path);

#endif
