#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/epoll.h>

volatile sig_atomic_t term = 0;

void sigterm_handler(int _) {
  term = 1;
}

void close_socket(int socket) {
  shutdown(socket, SHUT_RDWR);
  close(socket);
}

int unlock_io(int fd) {
  int flags = fcntl(fd, F_GETFL);
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int register_read_interest(int epoll_fd, int fd) {
  struct epoll_event ready_for_reading = {
          .events = EPOLLIN,
          .data.fd = fd
  };
  return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ready_for_reading);
}

int init_epoll(int socket_fd) {
  unlock_io(socket_fd);
  int epoll_fd = epoll_create1(0);
  if (-1 == epoll_fd) {
    error(
            EXIT_FAILURE,
            errno,
            "could not create epoll instance");
  }
  register_read_interest(epoll_fd, socket_fd);
  return epoll_fd;
}


int create_server(const in_addr_t server_ip, const in_port_t server_port) {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server_addr = {
          .sin_family = AF_INET,
          .sin_addr.s_addr = server_ip,
          .sin_port = server_port
  };
  if (bind(socket_fd, (const struct sockaddr *) &server_addr,
           sizeof(struct sockaddr_in)) == -1) {
    close(socket_fd);
    error(
            EXIT_FAILURE,
            errno,
            "Could not bind"
    );
  }
  if (listen(socket_fd, SOMAXCONN) == -1) {
    close(socket_fd);
    error(
            EXIT_FAILURE,
            errno,
            "Could not start to listen"
    );
  }
  return socket_fd;
}

void upper(char *buffer, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    buffer[i] = toupper(buffer[i]);
  }
}

void process_fd(int fd_to_read, int socket_fd, int epoll_fd) {
  if (fd_to_read == socket_fd) {
    int client_fd = 0;
    while ((client_fd = accept(socket_fd, NULL, NULL)) != -1) {
      unlock_io(client_fd);
      register_read_interest(epoll_fd, client_fd);
    }
    return;
  }

  const int buf_size = 4096;
  char buffer[buf_size];
  memset(buffer, 0, buf_size);

  int bytes = 0;

  if ((bytes = read(fd_to_read, buffer, buf_size)) <= 0) {
    close_socket(fd_to_read);
    return;
  }

  upper(buffer, bytes);

  if (write(fd_to_read, buffer, bytes) <= 0) {
    close_socket(fd_to_read);
    return;
  }
}

int main(int argc, char **argv) {
  struct sigaction sigterm_action = {.sa_handler = sigterm_handler};
  sigaction(SIGTERM, &sigterm_action, NULL);

  const in_addr_t server_ip = inet_addr("127.0.0.1");
  const in_port_t server_port = htons(strtol(argv[1], NULL, 10));

  int socket_fd = create_server(server_ip, server_port);
  int epoll_fd = init_epoll(socket_fd);

  struct epoll_event events[SOMAXCONN];

  while (!term) {
    int read_ready_count = epoll_wait(epoll_fd, events, SOMAXCONN, -1);
    if (read_ready_count == -1) {
      break;
    }
    for (size_t i = 0; i < read_ready_count; ++i) {
      process_fd(events[i].data.fd, socket_fd, epoll_fd);
    }
  }
  close(epoll_fd);
  close(socket_fd);
  return 0;
}