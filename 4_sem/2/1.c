#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef struct {
  int64_t a;
  int64_t b;
  int64_t n;
  bool generated;
  int64_t *value;
  pthread_cond_t *cond;
  pthread_mutex_t *mutex;
} args;


void *routine(void *arg) {
  args *args_ = (args *) arg;
  int64_t a = args_->a;
  int64_t b = args_->b;
  int64_t n = args_->n;
  pthread_mutex_t *mutex = args_->mutex;

  int64_t cnt = 0;

  pthread_mutex_lock(mutex);
  for (int64_t i = a; i <= b && cnt < n; ++i) {

    int32_t is_prime = 1;

    for (int64_t j = 2; j * j <= i; ++j) {
      if (i % j == 0) {
        is_prime = 0;
        break;
      }
    }

    if (is_prime) {
      ++cnt;
      *args_->value = i;
      args_->generated = true;
      pthread_cond_signal(args_->cond);
      while (args_->generated) {
        pthread_cond_wait(args_->cond, mutex);
      }
    }
  }
  pthread_mutex_unlock(mutex);
  return NULL;
}


int main(int argc, char *argv[]) {
  int64_t a = atoi(argv[1]);
  int64_t b = atoi(argv[2]);
  int64_t n = atoi(argv[3]);

  pthread_cond_t cond;
  pthread_cond_init(&cond, NULL);

  pthread_mutex_t mutex;
  pthread_mutex_init(&mutex, NULL);

  int64_t value = 0;

  args args_ = {
          .a = a,
          .b = b,
          .n = n,
          .generated = false,
          .value = &value,
          .cond = &cond,
          .mutex = &mutex
  };

  pthread_t thread;
  pthread_create(&thread, NULL, routine, &args_);

  pthread_mutex_lock(&mutex);
  for (int i = 0; i < n; ++i) {
    while (!args_.generated) {
      pthread_cond_wait(&cond, &mutex);
    }
    printf("%ld ", value);
    args_.generated = false;
    pthread_cond_signal(&cond);
  }
  pthread_mutex_unlock(&mutex);

  pthread_join(thread, NULL);

  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&mutex);

  return 0;
}