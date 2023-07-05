#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {
  char *cmd1 = argv[1];
  char *cmd2 = argv[2];

  int pipefd[2];
  pipe(pipefd);

  pid_t pid1 = fork();
  if (pid1 == 0) {
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);
    close(pipefd[0]);
    execlp(cmd1, cmd1, NULL);
    exit(1);
  }

  pid_t pid2 = fork();
  if (pid2 == 0) {
    dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[0]);
    close(pipefd[1]);
    execlp(cmd2, cmd2, NULL);
    exit(1);
  }

  close(pipefd[0]);
  close(pipefd[1]);
  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);

  return 0;
}