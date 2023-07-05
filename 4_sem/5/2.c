#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <netdb.h>
#include <sys/wait.h>

volatile sig_atomic_t st_si_flag = 0;

void st_si_handler(int sig) {
  st_si_flag = 1;
}

int create_server(const char *server_ipa_str, int server_port) {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct in_addr server_ipa;
  inet_aton(server_ipa_str, &server_ipa);
  in_port_t lit_end_server_port = htons(server_port);
  struct sockaddr_in server_addr = {
          .sin_family = AF_INET,
          .sin_addr = server_ipa,
          .sin_port = lit_end_server_port
  };
  if (0 != bind(socket_fd, (const struct sockaddr *) (&server_addr), sizeof(server_addr))) {
    close(socket_fd);
    error(
            EXIT_FAILURE,
            errno,
            "Could not bind to %s:%d",
            server_ipa_str,
            server_port
    );
  }
  return socket_fd;
}

int accept_client(int socket_fd, const char *server_ipa_str, int server_port) {
  struct sockaddr_in client_addr;
  socklen_t addr_len = sizeof(client_addr);
  int client_fd = accept(socket_fd, (struct sockaddr *) (&client_addr), &addr_len);
  if (-1 == client_fd) {
    close(socket_fd);
    if (errno == EINTR) {
      exit(0);
    }
    error(
            EXIT_FAILURE,
            errno,
            "Could not accept client on %s:%d",
            server_ipa_str,
            server_port
    );
  }
  return client_fd;
}

size_t recv_client_msg(int client_fd, int socket_fd, char *buffer, size_t buffer_len) {
  int recv_status = recv(client_fd, buffer, buffer_len, 0);
  if (recv_status < 0) {
    if (errno == EINTR) {
      return recv_client_msg(client_fd, socket_fd, buffer, buffer_len);
    }
    close(client_fd);
    close(socket_fd);
    error(
            EXIT_FAILURE,
            errno,
            "Could not receive message from the client"
    );
  } else if (recv_status == 0) {
    close(client_fd);
    close(socket_fd);
    error(
            EXIT_FAILURE,
            0,
            "Empty message from the client"
    );
  }
  return recv_status;
}

void send_msg_to_client(int client_fd, int socket_fd, const char *msg, size_t msg_len) {
  if (-1 == send(client_fd, msg, msg_len, 0)) {
    if (errno == EINTR) {
      send_msg_to_client(client_fd, socket_fd, msg, msg_len);
      return;
    }
    close(client_fd);
    close(socket_fd);
    error(
            EXIT_FAILURE,
            errno,
            "Could not send message to the client"
    );
  }
}

void process_client(int socket_fd, const char *server_ipa_str, int server_port, char *dir_path) {
  int client_fd = accept_client(socket_fd, server_ipa_str, server_port);
  char buffer[16384];
  recv_client_msg(client_fd, socket_fd, buffer, sizeof(buffer));
  char file_name[1024];
  sscanf(buffer, "GET %s HTTP/1.1\r\n", file_name);
  char abs_file_path[1024];
  strcpy(abs_file_path, dir_path);
  strcat(abs_file_path, file_name);
  char response[16384];
  response[0] = '\0';
  bool file_exists = access(abs_file_path, F_OK) == 0;
  bool file_is_readable = access(abs_file_path, R_OK) == 0;
  bool file_is_exec = access(abs_file_path, X_OK) == 0;
  if (!file_exists) {
    strcat(response, "HTTP/1.1 404 Not Found\r\n");
  } else if (!file_is_readable) {
    strcat(response, "HTTP/1.1 403 Forbidden\r\n");
  } else {
    strcat(response, "HTTP/1.1 200 OK\r\n");
  }
  if (file_is_exec) {
    write(client_fd, response, strlen(response));
    pid_t pid = fork();
    if (pid == 0) {
      dup2(client_fd, STDOUT_FILENO);
      close(client_fd);
      execl(abs_file_path, abs_file_path, NULL);
    }
    waitpid(pid, 0, 0);
  } else {
    size_t content_len;
    if (!file_exists || !file_is_readable) {
      content_len = 0;
    } else {
      struct stat file_st;
      stat(abs_file_path, &file_st);
      content_len = file_st.st_size;
    }
    char content_len_response[1024];
    sprintf(content_len_response, "Content-Length: %ld\r\n", content_len);
    strcat(response, content_len_response);
    strcat(response, "\r\n");
    if (file_exists && file_is_readable) {
      int fd = open(abs_file_path, O_RDONLY);
      char *file_contents = (char *) mmap(0, content_len, PROT_READ, MAP_PRIVATE, fd, 0);
      strncat(response, file_contents, content_len);
      munmap(file_contents, content_len);
    }
    send_msg_to_client(client_fd, socket_fd, response, strlen(response));
  }
  close(client_fd);
}

int main(int argc, char **argv) {
  sigset_t mask;
  sigfillset(&mask);
  sigdelset(&mask, SIGINT);
  sigdelset(&mask, SIGTERM);
  sigprocmask(SIG_SETMASK, &mask, NULL);
  struct sigaction st_si_action = {.sa_handler = st_si_handler};
  sigaction(SIGTERM, &st_si_action, NULL);
  sigaction(SIGINT, &st_si_action, NULL);
  char server_ipa_str[1024];
  strcpy(server_ipa_str, inet_ntoa(*((struct in_addr **) gethostbyname("localhost")->h_addr_list)[0]));
  int server_port = atoi(argv[1]);
  const char *initial_dir_path = argv[2];
  char dir_path[1024];
  strcpy(dir_path, initial_dir_path);
  if (dir_path[strlen(dir_path) - 1] != '/') {
    strcat(dir_path, "/");
  }
  int socket_fd = create_server(server_ipa_str, server_port);
  if (-1 == listen(socket_fd, SOMAXCONN)) {
    close(socket_fd);
    error(
            EXIT_FAILURE,
            errno,
            "Could not start to listen on %s:%d",
            server_ipa_str,
            server_port
    );
  }
  while (st_si_flag != 1) {
    process_client(socket_fd, server_ipa_str, server_port, dir_path);
  }
  close(socket_fd);
  return 0;
}
