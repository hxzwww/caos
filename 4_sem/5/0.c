#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>

void close_socket(int fd) {
  shutdown(fd, SHUT_RDWR);
  close(fd);
}

int connect_to_server(const char *ip_address, int port_number) {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct in_addr server_ip;
  inet_aton(ip_address, &server_ip);
  struct sockaddr_in server_addr = {
          .sin_family = AF_INET,
          .sin_port = htons(port_number),
          .sin_addr = server_ip
  };
  if (0 != connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
    close_socket(socket_fd);
    error(EXIT_FAILURE, errno, "Could not connect to %s:%d", ip_address, port_number);
  }
  return socket_fd;
}

int main(int argc, char **argv) {
  const char *ip_address = argv[1];
  int port = atoi(argv[2]);
  int socket_fd = connect_to_server(ip_address, port);
  int read;
  while (EOF != scanf("%d", &read)) {
    send(socket_fd, &read, sizeof(read), 0);
    int received;
    if (0 == recv(socket_fd, &received, sizeof(received), 0)) {
      break;
    }
    printf("%d\n", received);
  }
  close_socket(socket_fd);
  return 0;
}