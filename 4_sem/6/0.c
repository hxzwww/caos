#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define EXIT_IF(condition, message, cleanup)                                   \
    {                                                                          \
        if (condition) {                                                       \
            cleanup;                                                           \
            error(EXIT_FAILURE, errno, message);                               \
        }                                                                      \
    }

void close_socket(int socket) {
  shutdown(socket, SHUT_RDWR);
  close(socket);
}

int create_udp_socket() {
  int udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  EXIT_IF(-1 == udp_socket, "could not create UDP socket", /*cleanup=*/{});
  return udp_socket;
}

struct sockaddr_in server_addr(const char *server_ip_str, size_t server_port) {
  struct in_addr server_ip_addr;
  EXIT_IF(
          0 == inet_aton(server_ip_str, &server_ip_addr),
          "invalid server IP address",
          {});
  in_port_t ns_server_port = htons(server_port);
  struct sockaddr_in server_addr = {
          .sin_family = AF_INET,
          .sin_addr = server_ip_addr,
          .sin_port = ns_server_port};

  return server_addr;
}

void send_number(int udp_socket, const struct sockaddr_in *addr, int number) {
  number = htonl(number);
  ssize_t send_status = sendto(
          udp_socket,
          &number,
          sizeof(number),
          /*flags=*/0,
          (const struct sockaddr *) addr,
          sizeof(struct sockaddr_in));
  EXIT_IF(-1 == send_status, "could not send message to the server", {
    close_socket(udp_socket);
  });
}

int recv_number(int udp_socket) {
  int number;
  ssize_t recv_status = recvfrom(
          udp_socket,
          &number,
          sizeof(number),
          /*flags=*/0,
          /*address=*/NULL,
          /*address_len=*/0);
  EXIT_IF(-1 == recv_status, "could not receive message from the server", {
    close_socket(udp_socket);
  });

  number = ntohl(number);
  return number;
}

int main(int argc, char *argv[]) {
  const char *server_ip_str = "127.0.0.1";
  size_t server_port = atoi(argv[1]);

  struct sockaddr_in addr = server_addr(server_ip_str, server_port);
  int udp_socket = create_udp_socket();

  int number;
  while (scanf("%d", &number) != EOF) {
    send_number(udp_socket, &addr, number);

    int response = recv_number(udp_socket);
    printf("%d\n", response);
  }

  close_socket(udp_socket);
}
