#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// Test function parameters
#define TESTFUNC_PARAMS const uint8_t *, size_t, uint8_t, size_t *
#define TESTFUNC_PARAMS_NAMED const uint8_t *s, size_t slen, uint8_t c, size_t *ccnt
#define TESTFUNC_PARAMS_USE s2, s2len, s1[1], ccnt

// List of implementations
static size_t libc_rstrchr(TESTFUNC_PARAMS_NAMED);
static size_t naive_rstrchr(TESTFUNC_PARAMS_NAMED);
static size_t rig_rstrchr(TESTFUNC_PARAMS_NAMED);

const struct s_bench_implementations {
	const uint8_t *name;
	size_t (*func)(TESTFUNC_PARAMS);
} bench_implementations[] = {
	{ "libc_rstrchr", libc_rstrchr },
	{ "naive_rstrchr", naive_rstrchr },
	{ "rig_rstrchr", rig_rstrchr },
};

#define REVERSE_BIGSTR
#include "commonbench.h"

static size_t libc_rstrchr(TESTFUNC_PARAMS_NAMED) {
	const uint8_t *sp = strrchr(s, c);

	if (sp == NULL) {
		return (SIZE_MAX);
	}

	return (size_t)(sp - s);
}

static size_t naive_rstrchr(TESTFUNC_PARAMS_NAMED) {
	const uint8_t *sp = s;
	s += slen - 1;

	while (slen) {
		INC_CCNT;
		if (*s == c) {
			return (size_t)(s - sp);
		}
		slen--;
		s--;
	}

	return (SIZE_MAX);
}

// Define general purpose bit masks according to size_t size
#define RIG_LOWBITSMASK  ((size_t)(-1) / 0xFF)
#define RIG_HIGHBITSMASK (((size_t)(-1) / 0xFF) * 0x80)

// Non-zero if there is a NULL-byte in X (X is size_t)
#define RIG_FINDNULL(X) (((X) - RIG_LOWBITSMASK) & ~(X) & RIG_HIGHBITSMASK)

static size_t rig_rstrchr(TESTFUNC_PARAMS_NAMED) {
	const uint8_t *sp = s;
	s += slen - 1;

	while ((slen) && (MISALIGNED_SIZET(s))) {
		INC_CCNT;
		if (*s == c) {
			return (size_t)(s - sp);
		}
		slen--;
		s--;
	}

	if (slen >= sizeof(size_t)) {
		// Need to check the aligned char too
		INC_CCNT;
		if (*s == c) {
			return (size_t)(s - sp);
		}

		size_t cbuf = c;

		for (size_t i = 8; i < (sizeof(size_t) * 8); i *= 2) {
			cbuf |= (cbuf << i);
		}

		const size_t *as = (const size_t *)s;
		size_t asxcbuf;

		as--;

		while (slen >= sizeof(size_t)) {
			INC_CCNT;
			asxcbuf = *as ^ cbuf;
			if (RIG_FINDNULL(asxcbuf)) {
				break;
			}
			slen -= sizeof(size_t);
			as--;
		}

		as++;

		s = (const uint8_t *)as;
	}

	while (slen) {
		INC_CCNT;
		if (*s == c) {
			return (size_t)(s - sp);
		}
		slen--;
		s--;
	}

	return (SIZE_MAX);
}
