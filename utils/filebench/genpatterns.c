#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void) {
	#define NCOUNT 1000
	uint8_t rando[30];
	int fd = open("/dev/urandom", O_RDONLY);

	printf("size_t ncount = %d;\n", NCOUNT);

	printf("uint8_t *needle[%d] = {\n", NCOUNT);
	for (size_t y = 0, plen = 10; y < NCOUNT; y++) {
		printf("\t\"");
		if (read(fd, rando, plen) < 0) {
			return (EXIT_FAILURE);
		}
		for (size_t x = 0; x < plen; x++) {
			printf("\\x%02X", rando[x]);
		}
		printf("\",\n");
		plen++;
		if (plen == 30) plen = 10;
	}
	printf("};\n");

	printf("size_t nlen[%d] = {\n", NCOUNT);
	for (size_t y = 0, plen = 10; y < NCOUNT; y++) {
		printf("\t");
		printf("%zu,\n", plen);
		plen++;
		if (plen == 30) plen = 10;
	}
	printf("};\n");

	return (EXIT_SUCCESS);
}
