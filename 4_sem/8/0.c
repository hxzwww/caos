#include <error.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char* argv[]) {
  const char* hostname = argv[1];
  const char* filepath = argv[2];

  const struct addrinfo addr_hints = {
          .ai_family = AF_INET,
          .ai_socktype = SOCK_STREAM
  };
  struct addrinfo* res_list;
  if (getaddrinfo(hostname, /*service=*/"http", &addr_hints, &res_list) != 0) {
    fprintf(stderr, "Could not obtain server address");
    return EXIT_FAILURE;
  }
  const struct addrinfo* first_res = res_list;

  int server_sock = socket(AF_INET, SOCK_STREAM, /*protocol=*/0);
  int connect_status =
          connect(server_sock, first_res->ai_addr, first_res->ai_addrlen);
  freeaddrinfo(res_list);
  if (connect_status == -1) {
    error(EXIT_FAILURE, errno, "Could not connect to the server");
  }

  const size_t request_size = 1024;
  char* request_buff = (char*) calloc(request_size, 1);
  const char* request_fmt =
          "GET %s HTTP/1.1\r\n"
          "Host: %s\r\n"
          "Connection: close\r\n\r\n";
  ssize_t resp_size = snprintf(request_buff, request_size, request_fmt, filepath, hostname);
  ssize_t
          send_status = send(server_sock, request_buff, resp_size, /*flags=*/0);
  free(request_buff);
  if (send_status == -1) {
    error(EXIT_FAILURE, errno, "Could not send request to the server");
  }

  const size_t response_size = 1024;
  char* response_buff = (char*) calloc(response_size, 1);
  FILE* sock_stream = fdopen(server_sock, /*mode=*/"r");
  bool headers_received = false;
  while (fgets(response_buff, response_size, sock_stream) != NULL) {
    if (strcmp(response_buff, "\n") == 0
        || strcmp(response_buff, "\r\n") == 0) {
      headers_received = true;
      continue;
    }

    if (headers_received) {
      printf("%s", response_buff);
    }
  }
  free(response_buff);

  fclose(sock_stream);
}
