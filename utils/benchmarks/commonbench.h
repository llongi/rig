#include <time.h>
#include <math.h>
#include <stdio.h>

#define REPEAT_NR 10000

#define MISALIGNED_SIZET(X) ((size_t)(X) & (sizeof(size_t) - 1))
#define ALIGNED_SIZET(X) (!MISALIGNED_SIZET(X))

#define INC_CCNT (*ccnt)++

static inline int32_t internal_str_cmp(const uint8_t *s1, const uint8_t *s2, size_t len, size_t *ccnt);
static size_t testfunc(size_t func(TESTFUNC_PARAMS),
		const uint8_t *s1, size_t s1len, const uint8_t *s2, size_t s2len, double *mu, double *sd, double *tt, size_t *ccnt);

static inline int32_t internal_str_cmp(const uint8_t *s1, const uint8_t *s2, size_t len, size_t *ccnt) {
	// Nothing to compare, so let's return directly
	if (len == 0 || s1 == s2) {
		return (0);
	}

	// Try aligning first, it could be that both strings are misaligned by the same amount
	INC_CCNT;
	while ((len) && (*s1 == *s2) && (MISALIGNED_SIZET(s1))) {
		len--;
		if (len) {
			INC_CCNT;
			s1++;
			s2++;
		}
	}

	// If s1 and s2 are aligned, and we need to compare something that is
	// at least equal or bigger to word size, we can speed up things a bit
	if ((len >= sizeof(size_t)) && (*s1 == *s2) && (ALIGNED_SIZET(s2))) {
		// If s1 and s2 are word-aligned, compare them one word at a time
		const size_t *a1 = (const size_t *)s1;
		const size_t *a2 = (const size_t *)s2;

		INC_CCNT;
		while ((len >= sizeof(size_t)) && (*a1 == *a2)) {
			len -= sizeof(size_t);
			if (len) {
				INC_CCNT;
				a1++;
				a2++;
			}
		}

		// Either difference detected or not enough bytes left, so compare byte-wise
		s1 = (const uint8_t *)a1;
		s2 = (const uint8_t *)a2;
	}

	// Normal byte-wise compare
	INC_CCNT;
	while ((len) && (*s1 == *s2)) {
		len--;
		if (len) {
			INC_CCNT;
			s1++;
			s2++;
		}
	}

	return (*s1 - *s2);
}

static size_t testfunc(size_t func(TESTFUNC_PARAMS),
		const uint8_t *s1, size_t s1len, const uint8_t *s2, size_t s2len, double *mu, double *sd, double *tt, size_t *ccnt) {
	struct timespec stime, etime, tt_stime, tt_etime;
	double t, sum = 0, sum2 = 0;
	size_t res;

	if (clock_gettime(CLOCK_REALTIME, &tt_stime)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	for (int i = 0; i < REPEAT_NR; i++) {
		if (clock_gettime(CLOCK_REALTIME, &stime)) {
			fprintf(stderr, "clock_gettime failed");
			exit(1);
		}

		// Do the test
		*ccnt = 0;
		res = func(TESTFUNC_PARAMS_USE);

		if (clock_gettime(CLOCK_REALTIME, &etime)) {
			fprintf(stderr, "clock_gettime failed");
			exit(1);
		}

		// Compute the time taken and add it to the sums
		t = (double)(etime.tv_sec - stime.tv_sec) +
		    ((double)(etime.tv_nsec - stime.tv_nsec) / 1000000000.0);
		sum += t;
		sum2 += t * t;
	}

	if (clock_gettime(CLOCK_REALTIME, &tt_etime)) {
			fprintf(stderr, "clock_gettime failed");
			exit(1);
		}

	// Compute statistics and return result
	*mu = sum / REPEAT_NR;
	*sd = sqrt(sum2 - (sum * sum / REPEAT_NR));
	*tt = (double)(tt_etime.tv_sec - tt_stime.tv_sec) +
		((double)(tt_etime.tv_nsec - tt_stime.tv_nsec) / 1000000000.0);

	return (res);
}

// Test strings
#include "stringdb.h"

static double *tts_ptr;

static int tts_comparer(const void *a, const void *b) {
	size_t ax = (*((size_t *)a));
	size_t bx = (*((size_t *)b));

	int ret;

	if (tts_ptr[ax] > tts_ptr[bx]) {
		ret = 1;
	}
	else if (tts_ptr[ax] < tts_ptr[bx]) {
		ret = -1;
	}
	else {
		ret = 0;
	}

	return (ret);
}

int main(int argc, char **argv) {
	// Check cmd-line
	bool SORT_OUTPUT = false;

	if (argc > 2) {
		printf("Too many arguments passed. Only -sort is supported!\n");
		return (EXIT_FAILURE);
	}

	if (argc == 2) {
		if (!strcmp(argv[1], "-sort")) {
			SORT_OUTPUT = true;
		}
		else {
			printf("Garbage argument passed. Only -sort is supported!\n");
			return (EXIT_FAILURE);
		}
	}

	const size_t strs_to_test = sizeof(bench_testdata) / sizeof(bench_testdata[0]);
	const size_t imps_to_test = sizeof(bench_implementations) / sizeof(bench_implementations[0]);
	double imps_mus[imps_to_test], imps_sds[imps_to_test], imps_tts[imps_to_test];
	size_t i, j, res, ref_res, ccnt;
	double mu, sd, tt;
	size_t s1len, s2len, bslen;
	uint8_t *bigstr;

	tts_ptr = imps_tts;

	// Init accumulators
	for (i = 0; i < imps_to_test; i++) {
		imps_mus[i] = 0, imps_sds[i] = 0, imps_tts[i] = 0;
	}

	// Apply all functions to each string
	for (i = 0; i < strs_to_test; i++) {
		// Generate big string (s2 * s2rep)
		if (bench_testdata[i].addend) {
			s1len = strlen(bench_testdata[i].s1);
		}
		else {
			s1len = 0;
		}
		s2len = strlen(bench_testdata[i].s2);
		bigstr = malloc(bench_testdata[i].s2rep * s2len + s1len + 1);
#ifdef REVERSE_BIGSTR
		if (bench_testdata[i].addend) {
			memcpy(bigstr, bench_testdata[i].s1, s1len);
		}
		for (j = 0; j < bench_testdata[i].s2rep; j++) {
			memcpy(bigstr + s1len + j * s2len, bench_testdata[i].s2, s2len);
		}
#else
		for (j = 0; j < bench_testdata[i].s2rep; j++) {
			memcpy(bigstr + j * s2len, bench_testdata[i].s2, s2len);
		}
		if (bench_testdata[i].addend) {
			memcpy(bigstr + bench_testdata[i].s2rep * s2len, bench_testdata[i].s1, s1len);
		}
#endif
		bslen = bench_testdata[i].s2rep * s2len + s1len;
		bigstr[bslen] = '\0';

		// Do the benchmarks
		printf("Executing %i runs on \"%.40s\" (rep %zu) (str len: %zu) with pattern \"%.10s\" (pat len: %zu):\n", REPEAT_NR, bench_testdata[i].s2, bench_testdata[i].s2rep, bslen, bench_testdata[i].s1, strlen(bench_testdata[i].s1));

		for (j = 0, ref_res = 0; j < imps_to_test; j++) {
			res = testfunc(bench_implementations[j].func, bench_testdata[i].s1, strlen(bench_testdata[i].s1), bigstr, bslen, &mu, &sd, &tt, &ccnt);
			if (j == 0) {
				ref_res = res;
			}
			if (ref_res == res) {
				printf("%32s = %12zu: %.12f +/- %.12f | %.12f | %12zu\n", bench_implementations[j].name, res, mu, sd, tt, ccnt);
			}
			else {
				printf("\x1B[1;31mFAIL FAIL FAIL\x1B[0m - %15s = %12zu: %.12f +/- %.12f | %.12f | %12zu\n", bench_implementations[j].name, res, mu, sd, tt, ccnt);
			}
			imps_mus[j] += mu, imps_sds[j] += sd, imps_tts[j] += tt;
		}

		printf("\n");

		// Free allocated memory
		free(bigstr);
	}

	size_t map[imps_to_test];

	for (i = 0; i < imps_to_test; i++) {
		map[i] = i;
	}

	if (SORT_OUTPUT) {
		qsort(&map, imps_to_test, sizeof(size_t), &tts_comparer);
	}

	// Print mean time and standard deviation across all strings for an implementation
	printf("Final mean of the mean times and standard deviations across all %zu strings, plus total execution time:\n", strs_to_test);

	for (i = 0; i < imps_to_test; i++) {
		printf("%32s = %.12f +/- %.12f | %.12f\n", bench_implementations[map[i]].name, imps_mus[map[i]] / (double)strs_to_test, imps_sds[map[i]] / (double)strs_to_test, imps_tts[map[i]]);
	}

	return (EXIT_SUCCESS);
}
