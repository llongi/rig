#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

static __thread int thi = 10;

void *thr_func(void *arg);

void *thr_func(void *arg) {
	thi = rand() % 1000;

	printf("thi is in thread: %i\n", thi);

	return (NULL);
}

int main(void) {
	printf("thi is in main: %i\n", thi);

	pthread_t threads[NUM_THREADS];

	for (size_t i = 0; i < NUM_THREADS; i++)
		pthread_create(&threads[i], NULL, &thr_func, NULL);

	for (size_t i = 0; i < NUM_THREADS; i++)
		pthread_join(threads[i], NULL);

	printf("thi is in main: %i\n", thi);

	return (EXIT_SUCCESS);
}
