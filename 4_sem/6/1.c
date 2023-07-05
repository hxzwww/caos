#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdbool.h>

#define EXIT_IF(condition, message, cleanup)                                   \
    {                                                                          \
        if (condition) {                                                       \
            cleanup;                                                           \
            error(EXIT_FAILURE, errno, message);                               \
        }                                                                      \
    }

#define ICMP_LEN 32

volatile sig_atomic_t end = 0;

void alarm_handler(int var) {
  end = 1;
}

void close_socket(int socket) {
  shutdown(socket, SHUT_RDWR);
  close(socket);
}

int create_udp_socket() {
  int udp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  EXIT_IF(-1 == udp_socket, "could not create UDP socket", {});
  return udp_socket;
}

struct sockaddr_in server_addr(const char *server_ip_str) {
  struct in_addr server_ip_addr;
  EXIT_IF(
          0 == inet_aton(server_ip_str, &server_ip_addr),
          "invalid server IP address",
          {});
  struct sockaddr_in server_addr = {
          .sin_family = AF_INET,
          .sin_addr = server_ip_addr,
          .sin_port = htons(80)};
  return server_addr;
}

uint16_t checksum(uint16_t *buffer, int len) {
  uint16_t result = 0;
  uint16_t *token = buffer;
  while (len > 1) {
    result += *token++;
    len -= 2;
  }
  if (len == 1) {
    result += *(unsigned char *) token;
  }
  result = (result >> 16) + (result & 0xFFFF);
  result += (result >> 16);
  return ~result;
}

void fill_datagram(char *buffer, uint16_t request) {
  struct icmp *icmp = (struct icmp *) buffer;
  icmp->icmp_type = ICMP_ECHO;
  icmp->icmp_code = 0;
  icmp->icmp_cksum = 0;
  icmp->icmp_id = 64;
  icmp->icmp_seq = request;
  icmp->icmp_cksum = checksum((uint16_t *) icmp, ICMP_LEN);
}

int result(char *buffer, int len) {
  struct ip *head = (struct ip *) buffer;
  int head_len = head->ip_hl * 4;
  len -= head_len;
  struct icmp *icmp = (struct icmp *) (buffer + head_len);
  if (len < 8) {
    return 0;
  }
  if (icmp->icmp_type == ICMP_ECHOREPLY && icmp->icmp_id == 64) {
    uint16_t checksum_ = icmp->icmp_cksum;
    icmp->icmp_cksum = 0;
    return (checksum_ == checksum((uint16_t *) icmp, len));
  }
  return 0;
}

bool ping(int udp_socket, struct sockaddr_in addr, int request) {
  const int buf_size = 1024;
  char buffer[buf_size];
  memset(buffer, 0, buf_size);

  fill_datagram(buffer, request);
  ssize_t send_status = sendto(
          udp_socket,
          buffer,
          ICMP_LEN,
          0,
          (struct sockaddr *) &addr,
          sizeof addr);
  if (send_status == -1) {
    return 0;
  }

  ssize_t recv_status = recvfrom(
          udp_socket,
          buffer,
          buf_size,
          0,
          NULL,
          NULL);
  if (recv_status == -1) {
    return 0;
  }

  return result(buffer, (int)recv_status);
}


int main(int argc, char *argv[]) {
  const char *server_ip_str = argv[1];
  const size_t timeout = atoi(argv[2]);
  const size_t interval = atoi(argv[3]);

  size_t cnt = 0;
  int request = 0;

  signal(SIGALRM, alarm_handler);
  alarm(timeout);

  struct sockaddr_in addr = server_addr(server_ip_str);
  int udp_socket = create_udp_socket();

  while (!end) {
    ++request;
    cnt += ping(udp_socket, addr, request);
    usleep(interval);
  }

  close_socket(udp_socket);
  printf("%ld", cnt);
}
