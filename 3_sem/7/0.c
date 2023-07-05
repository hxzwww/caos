#include <asm/unistd_64.h>

extern long syscall(long number, ...);

int _start() {
    char HelloWorld[] = "Hello, World!\n";
    syscall(__NR_write, 1, HelloWorld, sizeof(HelloWorld) - 1);
    syscall(__NR_exit, 0);
}
