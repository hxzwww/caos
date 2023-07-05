#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

void* function(void* arg) {
	int* sum = (int*)arg;
	int next;
	while (scanf("%d", &next) != EOF) {
		*sum += next;
	}
	return NULL;
}

int main(int argc, char* argv[]) {
	int count = atoi(argv[1]);
	pthread_t* workers = (pthread_t*)malloc(count * sizeof(pthread_t));
	pthread_attr_t attr;
	pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    pthread_attr_setguardsize(&attr, 0);
    int* part_sum = (int*)malloc(count * sizeof(int));
    for (int i = 0; i < count; ++i) {
        pthread_create(&workers[i], &attr, &function, &part_sum[i]);
    }
    pthread_attr_destroy(&attr);
    int sum = 0;
    for (int i = 0; i < count; ++i) {
        pthread_join(workers[i], NULL);
        sum += part_sum[i];
    }
    free(workers);
    free(part_sum);
    printf("%d", sum);
}
