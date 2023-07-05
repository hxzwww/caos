#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int N = atoi(argv[1]);
    int status;
    for (int i = 0; i < N; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            printf("%d", i + 1);
            printf(i + 1 != N ? " " : "\n");
            exit(0);
        } else {
        	  wait(NULL);
        }
    }
    return 0;
}

