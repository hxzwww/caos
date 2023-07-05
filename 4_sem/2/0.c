#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct args {
    int i;
    int k;
    int n;
    double *arr;
    pthread_mutex_t *mutex;
};


void *routine(void *arg) {
    struct args *args_ = (struct args *) arg;
    int i = args_->i;
    int k = args_->k;
    int n = args_->n;
    double *arr = args_->arr;
    pthread_mutex_t *mutex = args_->mutex;
    int left = (i == 0 ? k - 1 : i - 1);
    int right = (i == k - 1 ? 0 : i + 1);
    for (int j = 0; j < n; ++j) {
        pthread_mutex_lock(mutex);
        arr[i] += 1;
        arr[left] += 0.99;
        arr[right] += 1.01;
        pthread_mutex_unlock(mutex);
    }
    return NULL;
}


int main(int argc, char *argv[]) {
    int n = atoi(argv[1]);
    int k = atoi(argv[2]);
    double arr[k];
    memset(arr, 0, sizeof(double) * k);
    pthread_t *thread = (pthread_t *) malloc(k * sizeof(pthread_t));
    pthread_mutex_t *mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);
    for (int i = 0; i < k; ++i) {
        struct args *args_ = (struct args *) malloc(sizeof(struct args));
        args_->i = i;
        args_->k = k;
        args_->n = n;
        args_->arr = arr;
        args_->mutex = mutex;
        pthread_create(thread + i, NULL, &routine, args_);
    }
    for (int i = 0; i < k; ++i) {
        pthread_join(thread[i], NULL);
    }
    pthread_mutex_destroy(mutex);
    for (int i = 0; i < k; ++i) {
        printf("%.10g ", arr[i]);
    }
}
