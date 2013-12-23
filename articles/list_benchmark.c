#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "list_llist_lockfree_smr.c"

#ifndef THRS
	#define THRS 4
#endif

void *thr_function(void *dts);

void *thr_function(void *dts) {
	uintptr_t got = 0;

	for (size_t i = 0; i < 10000; i++) {
		list_insert(dts, &i);
	}

	for (size_t i = 0; i < 5000; i++) {
		list_get(dts, &got);
	}

	for (size_t i = 0; i < 5000; i++) {
		list_look(dts, &got);
	}

	for (size_t i = 1; i < 1000; i++) {
		list_insert(dts, &i);
	}

	for (size_t i = 500; i < 1500; i++) {
		list_contains(dts, &i);
	}

	for (size_t i = 9000; i < 11000; i++) {
		list_contains(dts, &i);
	}

	for (size_t i = 1000; i > 0; i--) {
		list_remove(dts, &i);
	}

	for (size_t i = 0; i < 10000; i++) {
		list_remove(dts, &i);
	}

	return (NULL);
}

int main(void) {
	pthread_t thr[THRS];
	struct timespec s_time, e_time;

	void *dts = list_init(0, 1);

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

	list_destroy((LIST *)&dts);

	return (0);
}
