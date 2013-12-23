#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <rig.h>

#include "str_ops.c"
#include "str_ops_search.c"
#include "patterns.h"

int main(void) {
	struct timespec s_time, e_time;
	int fd = open("urandom_1g", O_RDONLY);

	if (clock_gettime(CLOCK_REALTIME, &s_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	STR_OPS_SEARCH_BIG3MULTIPLE_MULTIBYTE_PREP pb3mm = str_ops_search_big3multiple_multibyte_prep(needle, nlen, ncount);

	if (clock_gettime(CLOCK_REALTIME, &e_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	printf("\nPREPROCESSING STATS\n");
	printf("number of patterns: %zu\n", ncount);
	printf("nminlen value: %zu\n", pb3mm->nminlen);
	printf("minskip value: %zu\n", pb3mm->minskip);
	printf("time elapsed: %.12f\n", (double)(e_time.tv_sec - s_time.tv_sec) + ((double)(e_time.tv_nsec - s_time.tv_nsec) / 1000000000.0));

	uint8_t haystack[1024 * 1024];
	ssize_t hlen = 0;
	size_t count = 0;

	if (clock_gettime(CLOCK_REALTIME, &s_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	while((hlen = read(fd, haystack, 1024 * 1024)) > 0) {
		count += str_ops_search_big3multiple_multibyte(haystack, (size_t)hlen, pb3mm, MODE_COUNT, NULL);
	}

	if (clock_gettime(CLOCK_REALTIME, &e_time)) {
		fprintf(stderr, "clock_gettime failed");
		exit(1);
	}

	free(pb3mm);
	close(fd);

	printf("\nSEARCH STATS\n");
	printf("total match count: %zu\n", count);
	double tt = (double)(e_time.tv_sec - s_time.tv_sec) + ((double)(e_time.tv_nsec - s_time.tv_nsec) / 1000000000.0);
	printf("time elapsed: %.12f\n", tt);
	printf("throughput: ~ %.2f Gbit/s\n\n", (8.0 / tt));

	return (0);
}
