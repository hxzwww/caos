#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

int unlock_io(int fd) {
  int curr_fd_flags = fcntl(fd, F_GETFL);
  return fcntl(fd, F_SETFL, curr_fd_flags | O_NONBLOCK);
}

int register_read_interest(int epoll_fd, int fd) {
  epoll_data_t epoll_data = {.fd = fd};
  struct epoll_event ready_for_reading = {
          .events = EPOLLIN, .data = epoll_data};

  return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ready_for_reading);
}

void handle_epoll_event(
        struct epoll_event ready_for_reading,
        size_t *total_bytes_read,
        size_t *fds_fully_read) {
  assert((ready_for_reading.events & EPOLLIN) != 0);

  int fd = ready_for_reading.data.fd;

  const size_t BUFF_SIZE = 4096;
  char buff[BUFF_SIZE];
  ssize_t bytes_read;
  while ((bytes_read = read(fd, buff, sizeof(buff))) > 0) {
    *total_bytes_read += bytes_read;
  }

  if (0 == bytes_read) {
    close(fd);
    *fds_fully_read += 1;
  } else {
    assert(errno == EWOULDBLOCK);
  }
}

int init_epoll(size_t fd_count, int in_fds[fd_count]) {
  for (size_t i = 0; i < fd_count; ++i) {
    if (-1 == unlock_io(in_fds[i])) {
      error(EXIT_FAILURE, errno, "could not unlock IO for FD");
    }
  }

  int epoll_fd = epoll_create1(/*flags=*/0);
  if (-1 == epoll_fd) {
    error(EXIT_FAILURE, errno, "could not create epoll instance");
  }

  for (size_t i = 0; i < fd_count; ++i) {
    if (-1 == register_read_interest(epoll_fd, in_fds[i])) {
      close(epoll_fd);
      error(EXIT_FAILURE, errno, "could not add FD to the interest list");
    }
  }

  return epoll_fd;
}

extern size_t read_data_and_count(size_t fd_count, int in_fds[fd_count]) {
  int epoll_fd = init_epoll(fd_count, in_fds);

  const size_t MAX_PENDING_EVENTS = 65536;
  struct epoll_event pending_events[MAX_PENDING_EVENTS];

  size_t fds_fully_read = 0;
  size_t total_bytes_read = 0;
  while (fds_fully_read < fd_count) {
    int pending_events_count = epoll_wait(
            epoll_fd,
            pending_events,
            MAX_PENDING_EVENTS,
            /*timeout=*/-1);

    for (size_t i = 0; i < pending_events_count; ++i) {
      handle_epoll_event(
              pending_events[i], &total_bytes_read, &fds_fully_read);
    }
  }

  close(epoll_fd);

  return total_bytes_read;
}
