#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// Test function parameters
#define TESTFUNC_PARAMS const uint8_t *, size_t, uint8_t, size_t *
#define TESTFUNC_PARAMS_NAMED const uint8_t *s, size_t slen, uint8_t c, size_t *ccnt
#define TESTFUNC_PARAMS_USE s2, s2len, s1[1], ccnt

// List of implementations
static size_t libc_strchr(TESTFUNC_PARAMS_NAMED);
static size_t naive_strchr(TESTFUNC_PARAMS_NAMED);
static size_t rig_strchr(TESTFUNC_PARAMS_NAMED);

const struct s_bench_implementations {
	const uint8_t *name;
	size_t (*func)(TESTFUNC_PARAMS);
} bench_implementations[] = {
	{ "libc_strchr", libc_strchr },
	{ "naive_strchr", naive_strchr },
	{ "rig_strchr", rig_strchr },
};

#include "commonbench.h"

// This actually resolves to GCC's optimized strchr()
// I have found no way to tell GCC to not use its built-in in this case
static size_t libc_strchr(TESTFUNC_PARAMS_NAMED) {
	const uint8_t *sp = strchr(s, c);

	if (sp == NULL) {
		return (SIZE_MAX);
	}

	return (size_t)(sp - s);
}

static size_t naive_strchr(TESTFUNC_PARAMS_NAMED) {
	const uint8_t *sp = s;

	while (slen) {
		INC_CCNT;
		if (*s == c) {
			return (size_t)(s - sp);
		}
		slen--;
		s++;
	}

	return (SIZE_MAX);
}

// Define general purpose bit masks according to size_t size
#define RIG_LOWBITSMASK  ((size_t)(-1) / 0xFF)
#define RIG_HIGHBITSMASK (((size_t)(-1) / 0xFF) * 0x80)

// Non-zero if there is a NULL-byte in X (X is size_t)
#define RIG_FINDNULL(X) (((X) - RIG_LOWBITSMASK) & ~(X) & RIG_HIGHBITSMASK)

static size_t rig_strchr(TESTFUNC_PARAMS_NAMED) {
	const uint8_t *sp = s;

	while ((slen) && (MISALIGNED_SIZET(s))) {
		INC_CCNT;
		if (*s == c) {
			return (size_t)(s - sp);
		}
		slen--;
		s++;
	}

	if (slen >= sizeof(size_t)) {
		size_t cbuf = c;

		for (size_t i = 8; i < (sizeof(size_t) * 8); i *= 2) {
			cbuf |= (cbuf << i);
		}

		const size_t *as = (const size_t *)s;
		size_t asxcbuf;

		while (slen >= sizeof(size_t)) {
			INC_CCNT;
			asxcbuf = *as ^ cbuf;
			if (RIG_FINDNULL(asxcbuf)) {
				break;
			}
			slen -= sizeof(size_t);
			as++;
		}

		s = (const uint8_t *)as;
	}

	while (slen) {
		INC_CCNT;
		if (*s == c) {
			return (size_t)(s - sp);
		}
		slen--;
		s++;
	}

	return (SIZE_MAX);
}
