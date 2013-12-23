#include <rig.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define INIT_LOOPS   1000000
#define RDLOCK_LOOPS 1000000
#define WRLOCK_LOOPS 1000000

#define REC(X) for (size_t n = 0; n < 10; n++) { X; }

int main(void) {
	pthread_rwlock_t plock;
	RIG_RWLOCK rlock;
	RIG_MRWLOCK mrlock;

	struct timespec s_time, e_time;

	printf("Pthreads RWLock Init/Destroy Benchmark\n");

	if (clock_gettime(CLOCK_REALTIME, &s_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	for (int i = 0; i < INIT_LOOPS; i++) {
		pthread_rwlock_init(&plock, NULL);
		pthread_rwlock_destroy(&plock);
	}

	if (clock_gettime(CLOCK_REALTIME, &e_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	printf("\ttime elapsed: %.12f\n", (double)(e_time.tv_sec - s_time.tv_sec) + ((double)(e_time.tv_nsec - s_time.tv_nsec) / 1000000000.0));

	printf("Rig RWLock Init/Destroy Benchmark\n");

	if (clock_gettime(CLOCK_REALTIME, &s_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	for (int i = 0; i < INIT_LOOPS; i++) {
		rlock = rig_rwlock_init();
		rig_rwlock_destroy(&rlock);
	}

	if (clock_gettime(CLOCK_REALTIME, &e_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	printf("\ttime elapsed: %.12f\n", (double)(e_time.tv_sec - s_time.tv_sec) + ((double)(e_time.tv_nsec - s_time.tv_nsec) / 1000000000.0));

	printf("Rig Mini RWLock Init/Destroy Benchmark\n");

	if (clock_gettime(CLOCK_REALTIME, &s_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	for (int i = 0; i < INIT_LOOPS; i++) {
		mrlock = rig_mrwlock_init(0);
		rig_mrwlock_destroy(&mrlock);
	}

	if (clock_gettime(CLOCK_REALTIME, &e_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	printf("\ttime elapsed: %.12f\n", (double)(e_time.tv_sec - s_time.tv_sec) + ((double)(e_time.tv_nsec - s_time.tv_nsec) / 1000000000.0));

	printf("\n");

	printf("Pthreads RWLock RDLock/Unlock Benchmark\n");

	pthread_rwlock_init(&plock, NULL);

	if (clock_gettime(CLOCK_REALTIME, &s_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	for (int i = 0; i < RDLOCK_LOOPS; i++) {
		REC(pthread_rwlock_rdlock(&plock));
		REC(pthread_rwlock_unlock(&plock));
	}

	if (clock_gettime(CLOCK_REALTIME, &e_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	pthread_rwlock_destroy(&plock);

	printf("\ttime elapsed: %.12f\n", (double)(e_time.tv_sec - s_time.tv_sec) + ((double)(e_time.tv_nsec - s_time.tv_nsec) / 1000000000.0));

	printf("Rig RWLock RDLock/Unlock Benchmark\n");

	rlock = rig_rwlock_init();

	if (clock_gettime(CLOCK_REALTIME, &s_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	for (int i = 0; i < RDLOCK_LOOPS; i++) {
		REC(rig_rwlock_rdlock(rlock));
		REC(rig_rwlock_unlock(rlock));
	}

	if (clock_gettime(CLOCK_REALTIME, &e_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	rig_rwlock_destroy(&rlock);

	printf("\ttime elapsed: %.12f\n", (double)(e_time.tv_sec - s_time.tv_sec) + ((double)(e_time.tv_nsec - s_time.tv_nsec) / 1000000000.0));

	printf("Rig Mini RWLock RDLock/Unlock Benchmark\n");

	mrlock = rig_mrwlock_init(RIG_MRWLOCK_RECURSIVE);

	if (clock_gettime(CLOCK_REALTIME, &s_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	for (int i = 0; i < RDLOCK_LOOPS; i++) {
		REC(rig_mrwlock_rdlock(mrlock));
		REC(rig_mrwlock_unlock(mrlock));
	}

	if (clock_gettime(CLOCK_REALTIME, &e_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	rig_mrwlock_destroy(&mrlock);

	printf("\ttime elapsed: %.12f\n", (double)(e_time.tv_sec - s_time.tv_sec) + ((double)(e_time.tv_nsec - s_time.tv_nsec) / 1000000000.0));

	printf("\n");

	printf("Pthreads RWLock WRLock/Unlock Benchmark\n");

	pthread_rwlock_init(&plock, NULL);

	if (clock_gettime(CLOCK_REALTIME, &s_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	for (int i = 0; i < WRLOCK_LOOPS; i++) {
		pthread_rwlock_wrlock(&plock);
		pthread_rwlock_unlock(&plock);
	}

	if (clock_gettime(CLOCK_REALTIME, &e_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	pthread_rwlock_destroy(&plock);

	printf("\ttime elapsed: %.12f\n", (double)(e_time.tv_sec - s_time.tv_sec) + ((double)(e_time.tv_nsec - s_time.tv_nsec) / 1000000000.0));

	printf("Rig RWLock WRLock/Unlock Benchmark\n");

	rlock = rig_rwlock_init();

	if (clock_gettime(CLOCK_REALTIME, &s_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	for (int i = 0; i < WRLOCK_LOOPS; i++) {
		rig_rwlock_wrlock(rlock);
		rig_rwlock_unlock(rlock);
	}

	if (clock_gettime(CLOCK_REALTIME, &e_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	rig_rwlock_destroy(&rlock);

	printf("\ttime elapsed: %.12f\n", (double)(e_time.tv_sec - s_time.tv_sec) + ((double)(e_time.tv_nsec - s_time.tv_nsec) / 1000000000.0));

	printf("Rig Mini RWLock WRLock/Unlock Benchmark\n");

	mrlock = rig_mrwlock_init(0);

	if (clock_gettime(CLOCK_REALTIME, &s_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	for (int i = 0; i < WRLOCK_LOOPS; i++) {
		rig_mrwlock_wrlock(mrlock);
		rig_mrwlock_unlock(mrlock);
	}

	if (clock_gettime(CLOCK_REALTIME, &e_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	rig_mrwlock_destroy(&mrlock);

	printf("\ttime elapsed: %.12f\n", (double)(e_time.tv_sec - s_time.tv_sec) + ((double)(e_time.tv_nsec - s_time.tv_nsec) / 1000000000.0));

	return (EXIT_SUCCESS);
}
