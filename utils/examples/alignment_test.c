#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define CACHELINE_SIZE 64

#define ATTR_ALIGNED(x) __attribute__ ((__aligned__ (x)))
#define CACHELINE_ALIGNED ATTR_ALIGNED(CACHELINE_SIZE)
#define CACHELINE_ALONE(t, v) t v CACHELINE_ALIGNED; uint8_t PAD_##v[CACHELINE_SIZE - sizeof(t)]

struct aligned16 {
	size_t mem1 ATTR_ALIGNED(16);
	size_t mem2;
	size_t mem3;
};

struct alignedcache {
	size_t mem1 CACHELINE_ALIGNED;
	size_t mem2;
	size_t mem3;
};

struct alignedcachealone {
	CACHELINE_ALONE(size_t, mem1);
	size_t mem2;
	size_t mem3;
};

int main(void) {
	// On the stack
	struct aligned16 a16;
	struct alignedcache ac;
	struct alignedcachealone aca;

	printf("Address of a16.mem1: %zu - %zX - ALIGNED 16: %i\n",
		(size_t)&a16.mem1, (size_t)&a16.mem1, ((size_t)&a16.mem1 & (16 - 1)) == 0);
	printf("Size of a16: %zu\n", sizeof(a16));
	printf("Diff between mem1 and mem2: %zu\n", (size_t)&a16.mem2 - (size_t)&a16.mem1);
	printf("Diff between mem2 and mem3: %zu\n", (size_t)&a16.mem3 - (size_t)&a16.mem2);

	printf("Address of ac.mem1: %zu - %zX - ALIGNED CL: %i\n",
		(size_t)&ac.mem1, (size_t)&ac.mem1, ((size_t)&ac.mem1 & (CACHELINE_SIZE - 1)) == 0);
	printf("Size of ac: %zu\n", sizeof(ac));
	printf("Diff between mem1 and mem2: %zu\n", (size_t)&ac.mem2 - (size_t)&ac.mem1);
	printf("Diff between mem2 and mem3: %zu\n", (size_t)&ac.mem3 - (size_t)&ac.mem2);

	printf("Address of aca.mem1: %zu - %zX - ALIGNED CL: %i\n",
		(size_t)&aca.mem1, (size_t)&aca.mem1, ((size_t)&aca.mem1 & (CACHELINE_SIZE - 1)) == 0);
	printf("Size of aca: %zu\n", sizeof(aca));
	printf("Diff between mem1 and mem2: %zu\n", (size_t)&aca.mem2 - (size_t)&aca.mem1);
	printf("Diff between mem2 and mem3: %zu\n", (size_t)&aca.mem3 - (size_t)&aca.mem2);

	// On the heap
	struct aligned16 *pa16;
	if (posix_memalign((void **)&pa16, 16, sizeof(*pa16)))
		exit(EXIT_FAILURE);

	struct alignedcache *pac;
	if (posix_memalign((void **)&pac, CACHELINE_SIZE, sizeof(*pac)))
		exit(EXIT_FAILURE);

	struct alignedcachealone *paca;
	if (posix_memalign((void **)&paca, CACHELINE_SIZE, sizeof(*paca)))
		exit(EXIT_FAILURE);

	printf("Address of pa16.mem1: %zu - %zX - ALIGNED 16: %i\n",
		(size_t)&pa16->mem1, (size_t)&pa16->mem1, ((size_t)&pa16->mem1 & (16 - 1)) == 0);
	printf("Size of *pa16: %zu\n", sizeof(*pa16));
	printf("Diff between mem1 and mem2: %zu\n", (size_t)&pa16->mem2 - (size_t)&pa16->mem1);
	printf("Diff between mem2 and mem3: %zu\n", (size_t)&pa16->mem3 - (size_t)&pa16->mem2);

	printf("Address of pac.mem1: %zu - %zX - ALIGNED CL: %i\n",
		(size_t)&pac->mem1, (size_t)&pac->mem1, ((size_t)&pac->mem1 & (CACHELINE_SIZE - 1)) == 0);
	printf("Size of *pac: %zu\n", sizeof(*pac));
	printf("Diff between mem1 and mem2: %zu\n", (size_t)&pac->mem2 - (size_t)&pac->mem1);
	printf("Diff between mem2 and mem3: %zu\n", (size_t)&pac->mem3 - (size_t)&pac->mem2);

	printf("Address of paca.mem1: %zu - %zX - ALIGNED CL: %i\n",
		(size_t)&paca->mem1, (size_t)&paca->mem1, ((size_t)&paca->mem1 & (CACHELINE_SIZE - 1)) == 0);
	printf("Size of *paca: %zu\n", sizeof(*paca));
	printf("Diff between mem1 and mem2: %zu\n", (size_t)&paca->mem2 - (size_t)&paca->mem1);
	printf("Diff between mem2 and mem3: %zu\n", (size_t)&paca->mem3 - (size_t)&paca->mem2);

	free(pa16);
	free(pac);
	free(paca);

	return (EXIT_SUCCESS);
}
