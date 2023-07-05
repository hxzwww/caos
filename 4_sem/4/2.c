#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


volatile sig_atomic_t filenum = -1;

void handler(int signum) {
  filenum = signum - SIGRTMIN;
}


int main(int argc, char **argv) {
  sigset_t mask;
  sigfillset(&mask);
  sigprocmask(SIG_SETMASK, &mask, NULL);
  for (int signum = SIGRTMIN; signum <= SIGRTMAX; ++signum) {
    sigdelset(&mask, signum);
  }
  sigprocmask(SIG_SETMASK, &mask, NULL);
  struct sigaction signum_action = {.sa_handler = handler, .sa_flags=SA_RESTART};
  for (int signum = SIGRTMIN; signum <= SIGRTMAX; ++signum) {
    sigaction(signum, &signum_action, NULL);
  }

  while (filenum != 0) {
    if (filenum == -1) {
      continue;
    }
    FILE *file = fopen(argv[filenum], "r");
    unsigned long bufsize = 4096;
    char *line = (char *) malloc(bufsize * sizeof(char));
    getline(&line, &bufsize, file);
    printf("%s", line);
    fflush(stdout);
    fclose(file);
    filenum = -1;
  }

  return 0;
}