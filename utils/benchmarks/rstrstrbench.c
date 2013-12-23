#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// Test function parameters
#define TESTFUNC_PARAMS const uint8_t *, size_t, const uint8_t *, size_t, size_t *
#define TESTFUNC_PARAMS_NAMED const uint8_t *needle, size_t nlen, const uint8_t *haystack, size_t hlen, size_t *ccnt
#define TESTFUNC_PARAMS_USE s1, s1len, s2, s2len, ccnt

// List of implementations
static size_t naive_rstrstr(TESTFUNC_PARAMS_NAMED);
static size_t notson_rstrstr(TESTFUNC_PARAMS_NAMED);
static size_t rig_rstrstr(TESTFUNC_PARAMS_NAMED);
static size_t rig_rstrstr_skipn(TESTFUNC_PARAMS_NAMED);

const struct s_bench_implementations {
	const uint8_t *name;
	size_t (*func)(TESTFUNC_PARAMS);
} bench_implementations[] = {
	{ "naive_rstrstr", naive_rstrstr },
	{ "notson_rstrstr", notson_rstrstr },
	{ "rig_rstrstr", rig_rstrstr },
	{ "rig_rstrstr_skipn", rig_rstrstr_skipn },
};

#define REVERSE_BIGSTR
#include "commonbench.h"

static size_t naive_rstrstr(TESTFUNC_PARAMS_NAMED) {
	// Searching
	for (size_t j = 0, w = hlen - nlen; j <= w; j++) {
		if (internal_str_cmp(needle, haystack + w - j, nlen, ccnt) == 0) {
			return (w - j);
		}
	}

	return (SIZE_MAX);
}

static size_t notson_rstrstr(TESTFUNC_PARAMS_NAMED) {
	uint8_t k, l;

	// Preprocessing
	if (needle[0] == needle[1]) {
		k = 2;
		l = 1;
	}
	else {
		k = 1;
		l = 2;
	}

	// Searching
	for (size_t j = 0, w = hlen - nlen; j <= w;) {
		INC_CCNT;
		if (needle[0] != haystack[w - j]) {
			j += k;
		}
		else {
			if (internal_str_cmp(needle + 1, haystack + w - j + 1, nlen - 1, ccnt) == 0) {
				return (w - j);
			}

			j += l;
		}
	}

	return (SIZE_MAX);
}

#if SIZE_MAX == 65535UL
	#define STRSTRMASK 0x0F
#elif SIZE_MAX == 4294967295UL
	#define STRSTRMASK 0x1F
#elif SIZE_MAX == 18446744073709551615UL
	#define STRSTRMASK 0x3F
#else
	#error size_t is not a 16, 32 or 64 bit type.
#endif

static size_t rig_rstrstr(TESTFUNC_PARAMS_NAMED) {
	size_t i, skip, mask = 0, w = hlen - nlen;

	/* create compressed boyer-moore delta 1 table */
	skip = nlen - 1;
	/* process pattern[1:] */
	for (i = nlen - 1; i > 0; i--) {
		mask |= ((size_t)1 << (needle[i] & STRSTRMASK));
		if (needle[i] == needle[0]) {
			skip = i - 1;
		}
	}
	/* process pattern[0] outside the loop */
	mask |= ((size_t)1 << (needle[0] & STRSTRMASK));

	for (i = 0; i <= w; i++) {
		INC_CCNT;
		if (haystack[w - i] == needle[0]) {
			/* candidate match */
			if (internal_str_cmp(haystack + w - i + 1, needle + 1, nlen - 1, ccnt) == 0) {
				/* got a match! */
				return (w - i);
			}
			/* miss: check if previous character is part of pattern */
			if (i != w && !(mask & ((size_t)1 << (haystack[w - i - 1] & STRSTRMASK)))) {
				i += nlen;
			}
			else {
				i += skip;
			}
		}
		else {
			/* skip: check if previous character is part of pattern */
			if (i != w && !(mask & ((size_t)1 << (haystack[w - i - 1] & STRSTRMASK)))) {
				i += nlen;
			}
		}
	}

	return (SIZE_MAX);
}

static size_t rig_rstrstr_skipn(TESTFUNC_PARAMS_NAMED) {
	size_t i, skip, mask = 0, w = hlen - nlen;

	/* create compressed boyer-moore delta 1 table */
	skip = nlen - 1;
	/* process pattern[1:] */
	for (i = nlen - 1; i > 0; i--) {
		mask |= ((size_t)1 << (needle[i] & STRSTRMASK));
		if (needle[i] == needle[0]) {
			skip = i - 1;
		}
	}
	/* process pattern[0] outside the loop */
	mask |= ((size_t)1 << (needle[0] & STRSTRMASK));

	for (i = 0; i <= w; i++) {
		INC_CCNT;
		if (haystack[w - i] == needle[0]) {
			/* candidate match */
			if (internal_str_cmp(haystack + w - i + 1, needle + 1, nlen - 1, ccnt) == 0) {
				/* got a match! */
				return (w - i);
			}
			/* miss: check if previous character is part of pattern */
			if (i != w && !(mask & ((size_t)1 << (haystack[w - i - 1] & STRSTRMASK)))) {
				i += nlen;
			}
			else {
				i += skip;
			}
		}
		else {
			/* skip: check if previous character is part of pattern */
			if (i != w && !(mask & ((size_t)1 << (haystack[w - i - 1] & STRSTRMASK)))) {
				i += nlen;
			}
			else if (!(mask & ((size_t)1 << (haystack[w - i] & STRSTRMASK)))) {
				i += nlen - 1;
			}
		}
	}

	return (SIZE_MAX);
}
