#include <rig.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

void *dts_init(size_t capacity);
void dts_destroy(void **dts);
void *thr_function(void *dts);

int main(int argc, char **argv) {
	if (argc > 1) {
		printf("Too many arguments passed. None are supported!\n");
		return (EXIT_FAILURE);
	}

	printf("'%s': Executing %i benchmark runs on %s version %s ...\n",
		argv[0], NUM_BENCH_RUNS, RIG_NAME_STRING, RIG_VERSION_STRING);
	printf("\n");
	printf("%5s | %14s | %14s\n", "run", "run time", "time sum");

	struct timespec stime, etime, tt_stime, tt_etime;
	double mt, sd, tt;
	double t, sum = 0, sum2 = 0;

	RIG_THREAD thr = rig_thread_init(0, NUM_THREADS);

	if (clock_gettime(CLOCK_REALTIME, &tt_stime)) {
		fprintf(stderr, "clock_gettime failed");
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < NUM_BENCH_RUNS; i++) {
		void *dts = dts_init(0);

		if (clock_gettime(CLOCK_REALTIME, &stime)) {
			fprintf(stderr, "clock_gettime failed");
			exit(EXIT_FAILURE);
		}

		rig_thread_start(thr, &thr_function, dts);

		rig_thread_join(thr, NULL);

		if (clock_gettime(CLOCK_REALTIME, &etime)) {
			fprintf(stderr, "clock_gettime failed");
			exit(EXIT_FAILURE);
		}

		dts_destroy(&dts);

		rig_smr_hp_mem_scan_full();

		// Compute the time taken and add it to the sums
		t = (double)(etime.tv_sec - stime.tv_sec) +
			((double)(etime.tv_nsec - stime.tv_nsec) / 1000000000.0);
		sum += t;
		sum2 += t * t;

		// Print the times for this particular run
		printf("%02zu/%02i | %.12f | %.12f\n", i + 1, NUM_BENCH_RUNS, t, sum);
	}

	if (clock_gettime(CLOCK_REALTIME, &tt_etime)) {
		fprintf(stderr, "clock_gettime failed");
		exit(EXIT_FAILURE);
	}

	rig_thread_destroy(&thr);

	// Compute statistics and return result
	mt = sum / NUM_BENCH_RUNS;
	sd = sqrt(sum2 - (sum * sum / NUM_BENCH_RUNS));
	tt = (double)(tt_etime.tv_sec - tt_stime.tv_sec) +
		((double)(tt_etime.tv_nsec - tt_stime.tv_nsec) / 1000000000.0);

	printf("\n");
	printf("%14s +/- %14s | %14s\n", "mean time", "std dev", "total time");
	printf("%.12f +/- %.12f | %.12f\n", mt, sd, tt);

	return (EXIT_SUCCESS);
}
