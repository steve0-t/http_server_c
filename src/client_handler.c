#include "../include/client_handler.h"

int sanitize_request(request *req, char *real_path) {
  if (!req || strlen(req->method) == 0 || strlen(req->path) == 0)
    return -1;
  if (!req->method || strcmp(req->method, "GET") != 0)
    return -1;

  for (int i = 0; i < 4; i++) {
    if (strcmp(req->path, FILENAMES[i]) == 0) {
      strcpy(real_path, FILENAMES[i]);
      return 0;
    }
  }
  return -1;
}

int parse_request(char *buf, request *req) {
  if (!buf || !req || !strlen(buf))
    return -1;
  req->method = strtok(buf, " ");
  req->path = strtok(NULL, " ");

  if (!req->method || !req->path)
    return -1;

  // printf("method: %s\n", req->method);
  // printf("path: %s\n", req->path);
  return 0;
}

char *generate_header(res_type type, long file_size) {
  char *header = (char *)malloc(256);
  switch (type) {
  case BAD_REQ: {
    sprintf(header, "HTTP/1.1 400 Bad Request\r\n"
                    "CONTENT-TYPE: text/plain\r\n"
                    "CONTENT-LENGTH: 15\r\n"
                    "CONNECTION: close\r\n"
                    "\r\n"
                    "Bad Request");
    break;
  }
  case FILE_NOT_FOUND: {
    sprintf(header, "HTTP/1.1 404 File Not Found\r\n"
                    "CONTENT-TYPE: text/plain\r\n"
                    "CONTENT-LENGTH: 15\r\n"
                    "CONNECTION: close\r\n"
                    "\r\n"
                    "File Not Found");
    break;
  }
  case SERVER_ERR: {
    sprintf(header, "HTTP/1.1 500 Internal Server Error\r\n"
                    "CONTENT-TYPE: text/plain\r\n"
                    "CONTENT-LENGTH: 21\r\n"
                    "CONNECTION: close\r\n"
                    "\r\n"
                    "Internal Server Error");
    break;
  }
  case OK: {
    sprintf(header,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %ld\r\n"
            "Connection: close\r\n"
            "\r\n",
            file_size);
    break;
  }
  }
  return header;
}

int generate_response(int client_fd, char *path) {
  FILE *fp = fopen(path, "r");
  if (!fp) {
    char *err_msg = generate_header(FILE_NOT_FOUND, 0);
    if (write(client_fd, err_msg, strlen(err_msg)) == -1)
      perror("write");
    return -1;
  }

  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  rewind(fp);

  char *file_content = malloc(file_size + 1);
  fread(file_content, 1, file_size, fp);
  file_content[file_size] = '\0';
  fclose(fp);

  char *header = generate_header(OK, file_size);
  if (write(client_fd, header, strlen(header)) == -1)
    perror("write");
  if (write(client_fd, file_content, file_size) == -1)
    perror("write");

  free(file_content);
  return 0;
}

int handle_client(int client_fd, char *buf) {
  request req = {0};
  parse_request(buf, &req);

  if (strcmp(req.path, "/favicon.ico") == 0) {
    char *err_msg = generate_header(OK, 0);
    if (write(client_fd, err_msg, strlen(err_msg)) == -1)
      perror("write");
    fprintf(stdout, "server: favicon.ico requested\n");
    return 1;
  }

  char real_path[256] = {0};
  fprintf(stdout, "server: real_path: %s\n", real_path);
  if (sanitize_request(&req, real_path) == -1) {
    char *err_msg = generate_header(BAD_REQ, 0);
    if (write(client_fd, err_msg, strlen(err_msg)) == -1)
      perror("write");
    return -1;
  }

  char *path;
  if (asprintf(&path, "%s%s", PAGES_DIR,
               (strcmp(real_path, "/") == 0) ? "/index.html" : real_path) ==
      -1) {
    char *err_msg = generate_header(SERVER_ERR, 0);
    if (write(client_fd, err_msg, strlen(err_msg)) == -1)
      perror("write");
    return -1;
  }
  fprintf(stdout, "server: path: %s\n", path);

  if (generate_response(client_fd, path) == -1) {
    perror("generate_response");
    return -1;
  }
  free(path);
  return 0;
}
