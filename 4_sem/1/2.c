#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int is_ok(int value) {
    return 0 != value && value <= 100;
}

void *f1(void *arg) {
    int *fd = (int *) arg;
    int value = 1;
    while (is_ok(value)) {
        read(fd[0], &value, sizeof(int));
        if (is_ok(value)) {
            value -= 3;
            printf("%d ", value);
            write(fd[0], &value, sizeof(int));
        }
    }
    return NULL;
}

void *f2(void *arg) {
    int *fd = (int *) arg;
    int value = 1;
    while (is_ok(value)) {
        read(fd[1], &value, sizeof(int));
        if (is_ok(value)) {
            value += 5;
            printf("%d ", value);
            write(fd[1], &value, sizeof(int));
        }
    }
    return NULL;
}


void socketing(int value) {
    int fd[2];
    socketpair(PF_LOCAL, SOCK_STREAM, 0, fd);
    pthread_t thread1;
    pthread_t thread2;
    pthread_create(&thread1, NULL, &f1, fd);
    pthread_create(&thread2, NULL, &f2, fd);
    write(fd[1], &value, sizeof(int));
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
}


int main(int argc, char *argv[]) {
    socketing(atoi(argv[1]));
}
