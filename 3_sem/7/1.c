#include <asm/unistd_64.h>

extern long syscall(long number, ...);

int _start() {
  char char_[1024];

  while (syscall(__NR_read, 0, char_, 1)) {
    syscall(__NR_write, 1, char_, 1);
  }

  syscall(__NR_exit, 0);
}
