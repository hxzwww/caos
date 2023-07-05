#include <pthread.h>
#include <stdio.h>

void* function(void* arg) {
	int elem;
	if (scanf("%d", &elem) != EOF) {
		pthread_t next;
		pthread_create(&next, NULL, &function, NULL);
		pthread_join(next, NULL);
		printf("%d ", elem);
	}
	return NULL;
}

int main() {
	pthread_t root;
	pthread_create(&root, NULL, &function, NULL);
	pthread_join(root, NULL);
}
