#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {
  char *cmd = argv[1];
  char *in = argv[2];

  int infd = open(in, O_RDONLY);

  int pipefd[2];
  pipe(pipefd);

  pid_t pid = fork();
  if (pid == 0) {
    dup2(infd, STDIN_FILENO);
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[0]);
    close(pipefd[1]);
    execlp(cmd, cmd, NULL);
    exit(1);
  }

  close(pipefd[1]);

  size_t total_count = 0;
  size_t count;
  char buffer[4096];
  while ((count = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
    total_count += count;
  }

  printf("%ld", total_count);

  wait(NULL);

  return 0;
}