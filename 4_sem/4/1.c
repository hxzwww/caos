#include <signal.h>
#include <stdio.h>
#include <unistd.h>


volatile sig_atomic_t sigint_count = 0;
volatile sig_atomic_t sigterm_count = 0;
volatile sig_atomic_t num = 0;

void sigusr1_handler() {
  ++num;
  printf("%d\n", num);
  fflush(stdout);
}

void sigusr2_handler() {
  num = -num;
  printf("%d\n", num);
  fflush(stdout);
}

void sigint_handler() {
  ++sigint_count;
}

void sigterm_handler() {
  ++sigterm_count;
}

int main() {
  sigset_t mask;
  sigemptyset(&mask);
  sigprocmask(SIG_SETMASK, &mask, NULL);

  struct sigaction sigint_action = {.sa_handler = sigint_handler, .sa_flags=SA_RESTART};
  sigaction(SIGINT, &sigint_action, NULL);
  struct sigaction sigterm_action = {.sa_handler = sigterm_handler, .sa_flags=SA_RESTART};
  sigaction(SIGTERM, &sigterm_action, NULL);
  struct sigaction sigusr1_action = {.sa_handler = sigusr1_handler, .sa_flags=SA_RESTART};
  sigaction(SIGUSR1, &sigusr1_action, NULL);
  struct sigaction sigusr2_action = {.sa_handler = sigusr2_handler, .sa_flags=SA_RESTART};
  sigaction(SIGUSR2, &sigusr2_action, NULL);

  printf("%d\n", getpid());
  fflush(stdout);
  scanf("%d", &num);

  while (!sigterm_count && !sigint_count) {
    pause();
  }

  return 0;
}