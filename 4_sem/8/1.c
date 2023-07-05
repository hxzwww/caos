#include <error.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
  const char* hostname = argv[1];
  const char* script_path = argv[2];

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

  int file = open(argv[3], O_RDONLY);
  int file_size = lseek(file, 0, SEEK_END);
  lseek(file, 0, SEEK_SET);
  char file_buf[file_size];
  file_buf[file_size] = '\0';
  read(file, file_buf, file_size);

  char request[1024];
  sprintf(request,
           "POST %s HTTP/1.1\r\n"
           "Host: %s\r\n"
           "Content-Length: %d\r\nConnection: "
           "close\r\n\r\n%s\r\n\r\n",
           script_path, hostname, file_size, file_buf);

  write(server_sock, request, strlen(request));

  const size_t response_size = 1024;
  char response_buff[response_size];
  FILE* sock_stream = fdopen(server_sock, /*mode=*/"r");

  while (fgets(response_buff, response_size, sock_stream)) {
    if (strcmp(response_buff, "\r\n") == 0) {
      break;
    }
  }
  while (fgets(response_buff, response_size, sock_stream)) {
    printf("%s", response_buff);
  }

  fclose(sock_stream);
  close(server_sock);
  return 0;
}
