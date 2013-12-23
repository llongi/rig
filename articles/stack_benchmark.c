#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "stack_llist_lockfree_smr.c"

#ifndef THRS
	#define THRS 4
#endif

void *thr_function(void *dts);

void *thr_function(void *dts) {
	uintptr_t got = 0;

	for (size_t rep = 0; rep < 10; rep++) {

	for (size_t i = 0; i < 100000; i++) {
		stack_push(dts, &i);
	}

	for (size_t i = 0; i < 50000; i++) {
		stack_pop(dts, &got);
	}

	for (size_t i = 0; i < 100000; i++) {
		stack_push(dts, &i);
	}

	for (size_t i = 0; i < 50000; i++) {
		stack_look(dts, &got);
	}

	for (size_t i = 0; i < 10000; i++) {
		stack_push(dts, &i);
	}

	for (size_t i = 0; i < 160000; i++) {
		stack_pop(dts, &got);
	}

	}

	return (NULL);
}

int main(void) {
	pthread_t thr[THRS];
	struct timespec s_time, e_time;

	void *dts = stack_init(0);

	if (clock_gettime(CLOCK_REALTIME, &s_time)) {
		fprintf(stderr, "clock_gettime s_time failed");
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < THRS; i++) {
		pthread_create(&thr[i], NULL, &thr_function, dts);
	}

	for (size_t i = 0; i < THRS; i++) {
		pthread_join(thr[i], NULL);
	}

	if (clock_gettime(CLOCK_REALTIME, &e_time)) {
		fprintf(stderr, "clock_gettime e_time failed");
		exit(EXIT_FAILURE);
	}

	printf("time elapsed: %.12f\n", (double)(e_time.tv_sec - s_time.tv_sec) + ((double)(e_time.tv_nsec - s_time.tv_nsec) / 1000000000.0));

	stack_destroy((STACK *)&dts);

	return (0);
}
