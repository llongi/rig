#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// Test function parameters
#define TESTFUNC_PARAMS const uint8_t *, size_t, const uint8_t *, size_t, size_t *
#define TESTFUNC_PARAMS_NAMED const uint8_t *needle, size_t nlen, const uint8_t *haystack, size_t hlen, size_t *ccnt
#define TESTFUNC_PARAMS_USE s1, s1len, s2, s2len, ccnt

// List of implementations
static size_t libc_strstr(TESTFUNC_PARAMS_NAMED);
static size_t naive_strstr(TESTFUNC_PARAMS_NAMED);
static size_t notson_strstr(TESTFUNC_PARAMS_NAMED);
static size_t bmh_strstr(TESTFUNC_PARAMS_NAMED);
static size_t sunday_strstr(TESTFUNC_PARAMS_NAMED);
static size_t py_strstr(TESTFUNC_PARAMS_NAMED);
static size_t rig_strstr(TESTFUNC_PARAMS_NAMED);
static size_t rig_strstr_skipn(TESTFUNC_PARAMS_NAMED);
static size_t rig_strstr_exact(TESTFUNC_PARAMS_NAMED);
static size_t rig_strstr_exact_skipn(TESTFUNC_PARAMS_NAMED);
static size_t rig_strstr_skipn_qgrams(TESTFUNC_PARAMS_NAMED);
static size_t rig_strstr_skipn_qgrams_exact(TESTFUNC_PARAMS_NAMED);

const struct s_bench_implementations {
	const uint8_t *name;
	size_t (*func)(TESTFUNC_PARAMS);
} bench_implementations[] = {
	{ "libc_strstr", libc_strstr },
	{ "naive_strstr", naive_strstr },
	{ "notson_strstr", notson_strstr },
	{ "bmh_strstr", bmh_strstr },
	{ "sunday_strstr", sunday_strstr },
	{ "py_strstr", py_strstr },
	{ "rig_strstr", rig_strstr },
	{ "rig_strstr_skipn", rig_strstr_skipn },
	{ "rig_strstr_exact", rig_strstr_exact },
	{ "rig_strstr_exact_skipn", rig_strstr_exact_skipn },
	{ "rig_strstr_skipn_qgrams", rig_strstr_skipn_qgrams },
	{ "rig_strstr_skipn_qgrams_exact", rig_strstr_skipn_qgrams_exact },
};

#include "commonbench.h"

// here for comparison against the others
static size_t libc_strstr(TESTFUNC_PARAMS_NAMED) {
	const uint8_t *hp = strstr(haystack, needle);

	if (hp == NULL) {
		return (SIZE_MAX);
	}

	return (size_t)(hp - haystack);
}

// Memory usage: 2 * size_t (counters) = 16 b
// O(hlen * nlen) worstcase, expected around 2 * nlen
static size_t naive_strstr(TESTFUNC_PARAMS_NAMED) {
	// Searching
	for (size_t j = 0, w = hlen - nlen; j <= w; j++) {
		if (internal_str_cmp(needle, haystack + j, nlen, ccnt) == 0) {
			return (j);
		}
	}

	return (SIZE_MAX);
}

// Memory usage: 2 * size_t (counters), 2 * uint8_t = 18 b
// O(hlen * nlen) worstcase, expected around linear
static size_t notson_strstr(TESTFUNC_PARAMS_NAMED) {
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
		if (needle[1] != haystack[j + 1]) {
			j += k;
		}
		else {
			INC_CCNT;
			if (needle[0] == haystack[j] && internal_str_cmp(needle + 2, haystack + j + 2, nlen - 2, ccnt) == 0) {
				return (j);
			}

			j += l;
		}
	}

	return (SIZE_MAX);
}

// Memory usage: 2 * size_t (counters), 256 * size_t (table) = 2064 b
// O(hlen * nlen) worstcase, expected sublinear, O(hlen/nlen) minimum
static size_t bmh_strstr(TESTFUNC_PARAMS_NAMED) {
	size_t bmBCTable[UINT8_MAX + 1];

	// Preprocessing
	for (uint16_t i = 0; i < UINT8_MAX + 1; i++) {
		bmBCTable[i] = nlen;
	}

	for (size_t i = 0; i < nlen - 1; i++) {
		bmBCTable[needle[i]] = nlen - 1 - i;
	}

	// Searching
	for (size_t j = 0, w = hlen - nlen; j <= w; j += bmBCTable[haystack[j + nlen - 1]]) {
		INC_CCNT;
		if (haystack[j + nlen - 1] == needle[nlen - 1] && internal_str_cmp(needle, haystack + j, nlen - 1, ccnt) == 0) {
			return (j);
		}
	}

	return (SIZE_MAX);
}

// Memory usage: 2 * size_t (counters), 256 * size_t (table) = 2064 b
// O(hlen * nlen) worstcase, expected sublinear, O(hlen/nlen) minimum
static size_t sunday_strstr(TESTFUNC_PARAMS_NAMED) {
	size_t bmBCTable[UINT8_MAX + 1];

	// Preprocessing
	for (uint16_t i = 0; i < UINT8_MAX + 1; i++) {
		bmBCTable[i] = nlen + 1;
	}

	for (size_t i = 0; i < nlen; i++) {
		bmBCTable[needle[i]] = nlen - i;
	}

	// Searching
	for (size_t j = 0, w = hlen - nlen; j <= w; j += bmBCTable[haystack[j + nlen]]) {
		INC_CCNT;
		if (haystack[j + nlen - 1] == needle[nlen - 1] &&  internal_str_cmp(needle, haystack + j, nlen - 1, ccnt) == 0) {
			return (j);
		}
	}

	return (SIZE_MAX);
}

// Memory usage: 3 * size_t (counters), 3 * size_t (data) = 48 b
// O(hlen * nlen) worstcase, expected sublinear, O(hlen/nlen) minimum
static size_t py_strstr(TESTFUNC_PARAMS_NAMED) {
	size_t mask, skip, mlast;
	size_t i, j, w = hlen - nlen;

	mlast = nlen - 1;

	/* create compressed boyer-moore delta 1 table */
	skip = mlast - 1;
	/* process pattern[:-1] */
	for (mask = i = 0; i < mlast; i++) {
		mask |= ((size_t)1 << (needle[i] & 0x1F));
		if (needle[i] == needle[mlast]) {
			skip = mlast - i - 1;
		}
	}
	/* process pattern[-1] outside the loop */
	mask |= ((size_t)1 << (needle[mlast] & 0x1F));

	for (i = 0; i <= w; i++) {
		/* note: using mlast in the skip path slows things down on x86 */
		INC_CCNT;
		if (haystack[i + nlen - 1] == needle[nlen - 1]) {
			/* candidate match */
			for (j = 0; j < mlast; j++) {
				INC_CCNT;
				if (haystack[i + j] != needle[j]) {
					break;
				}
			}
			if (j == mlast) {
				/* got a match! */
				return (i);
			}
			/* miss: check if next character is part of pattern */
			if (!(mask & ((size_t)1 << (haystack[i + nlen] & 0x1F)))) {
				i = i + nlen;
			}
			else {
				i = i + skip;
			}
		}
		else {
			/* skip: check if next character is part of pattern */
			if (!(mask & ((size_t)1 << (haystack[i + nlen] & 0x1F)))) {
				i = i + nlen;
			}
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

// Memory usage: 2 * size_t (counters), 2 * size_t (data) = 32 b
// O(hlen * nlen) worstcase, expected sublinear, O(hlen/nlen) minimum
static size_t rig_strstr(TESTFUNC_PARAMS_NAMED) {
	size_t i, skip, mask = 0, w = hlen - nlen;

	/* create compressed boyer-moore delta 1 table */
	skip = nlen - 1;
	/* process pattern[:-2] */
	for (i = 0; i < nlen - 1; i++) {
		mask |= ((size_t)1 << (needle[i] & STRSTRMASK));
		if (needle[i] == needle[nlen - 1]) {
			skip = nlen - 2 - i;
		}
	}
	/* process pattern[-1] outside the loop */
	mask |= ((size_t)1 << (needle[nlen - 1] & STRSTRMASK));

	for (i = 0; i <= w; i++) {
		INC_CCNT;
		if (haystack[i + nlen - 1] == needle[nlen - 1]) {
			/* candidate match */
			if (internal_str_cmp(haystack + i, needle, nlen - 1, ccnt) == 0) {
				/* got a match! */
				return (i);
			}
			/* miss: check if next character is part of pattern */
			if (!(mask & ((size_t)1 << (haystack[i + nlen] & STRSTRMASK)))) {
				i += nlen;
			}
			else {
				i += skip;
			}
		}
		else {
			/* skip: check if next character is part of pattern */
			if (!(mask & ((size_t)1 << (haystack[i + nlen] & STRSTRMASK)))) {
				i += nlen;
			}
		}
	}

	return (SIZE_MAX);
}

// Memory usage: 2 * size_t (counters), 2 * size_t (data) = 32 b
// O(hlen * nlen) worstcase, expected sublinear, O(hlen/nlen) minimum
static size_t rig_strstr_skipn(TESTFUNC_PARAMS_NAMED) {
	size_t i, skip, mask = 0, w = hlen - nlen;

	/* create compressed boyer-moore delta 1 table */
	skip = nlen - 1;
	/* process pattern[:-2] */
	for (i = 0; i < nlen - 1; i++) {
		mask |= ((size_t)1 << (needle[i] & STRSTRMASK));
		if (needle[i] == needle[nlen - 1]) {
			skip = nlen - 2 - i;
		}
	}
	/* process pattern[-1] outside the loop */
	mask |= ((size_t)1 << (needle[nlen - 1] & STRSTRMASK));

	for (i = 0; i <= w; i++) {
		INC_CCNT;
		if (haystack[i + nlen - 1] == needle[nlen - 1]) {
			/* candidate match */
			if (internal_str_cmp(haystack + i, needle, nlen - 1, ccnt) == 0) {
				/* got a match! */
				return (i);
			}
			/* miss: check if next character is part of pattern */
			if (!(mask & ((size_t)1 << (haystack[i + nlen] & STRSTRMASK)))) {
				i += nlen;
			}
			else {
				i += skip;
			}
		}
		else {
			/* skip: check if next character is part of pattern */
			if (!(mask & ((size_t)1 << (haystack[i + nlen] & STRSTRMASK)))) {
				i += nlen;
			}
			else if (!(mask & ((size_t)1 << (haystack[i + nlen - 1] & STRSTRMASK)))) {
				i += nlen - 1;
			}
		}
	}

	return (SIZE_MAX);
}

// Memory usage: 2 * size_t (counters), 1 * size_t (data), 8 * uint32_t (table) = 56 b
// O(hlen * nlen) worstcase, expected sublinear, O(hlen/nlen) minimum
static size_t rig_strstr_exact(TESTFUNC_PARAMS_NAMED) {
	size_t i, skip, w = hlen - nlen;
	uint32_t masks[8] = { 0 };

	/* create compressed boyer-moore delta 1 table */
	skip = nlen - 1;
	/* process pattern[:-2] */
	for (i = 0; i < nlen - 1; i++) {
		masks[(needle[i] & 0xE0) >> 5] |= ((uint32_t)1 << (needle[i] & 0x1F));
		if (needle[i] == needle[nlen - 1]) {
			skip = nlen - 2 - i;
		}
	}
	/* process pattern[-1] outside the loop */
	masks[(needle[nlen - 1] & 0xE0) >> 5] |= ((uint32_t)1 << (needle[nlen - 1] & 0x1F));

	for (i = 0; i <= w; i++) {
		INC_CCNT;
		if (haystack[i + nlen - 1] == needle[nlen - 1]) {
			/* candidate match */
			if (internal_str_cmp(haystack + i, needle, nlen - 1, ccnt) == 0) {
				/* got a match! */
				return (i);
			}
			/* miss: check if next character is part of pattern */
			if (!(masks[(haystack[i + nlen] & 0xE0) >> 5] & ((uint32_t)1 << (haystack[i + nlen] & 0x1F)))) {
				i += nlen;
			}
			else {
				i += skip;
			}
		}
		else {
			/* skip: check if next character is part of pattern */
			if (!(masks[(haystack[i + nlen] & 0xE0) >> 5] & ((uint32_t)1 << (haystack[i + nlen] & 0x1F)))) {
				i += nlen;
			}
		}
	}

	return (SIZE_MAX);
}

// Memory usage: 2 * size_t (counters), 1 * size_t (data), 8 * uint32_t (table) = 56 b
// O(hlen * nlen) worstcase, expected sublinear, O(hlen/nlen) minimum
static size_t rig_strstr_exact_skipn(TESTFUNC_PARAMS_NAMED) {
	size_t i, skip, w = hlen - nlen;
	uint32_t masks[8] = { 0 };

	/* create compressed boyer-moore delta 1 table */
	skip = nlen - 1;
	/* process pattern[:-2] */
	for (i = 0; i < nlen - 1; i++) {
		masks[(needle[i] & 0xE0) >> 5] |= ((uint32_t)1 << (needle[i] & 0x1F));
		if (needle[i] == needle[nlen - 1]) {
			skip = nlen - 2 - i;
		}
	}
	/* process pattern[-1] outside the loop */
	masks[(needle[nlen - 1] & 0xE0) >> 5] |= ((uint32_t)1 << (needle[nlen - 1] & 0x1F));

	for (i = 0; i <= w; i++) {
		INC_CCNT;
		if (haystack[i + nlen - 1] == needle[nlen - 1]) {
			/* candidate match */
			if (internal_str_cmp(haystack + i, needle, nlen - 1, ccnt) == 0) {
				/* got a match! */
				return (i);
			}
			/* miss: check if next character is part of pattern */
			if (!(masks[(haystack[i + nlen] & 0xE0) >> 5] & ((uint32_t)1 << (haystack[i + nlen] & 0x1F)))) {
				i += nlen;
			}
			else {
				i += skip;
			}
		}
		else {
			/* skip: check if next character is part of pattern */
			if (!(masks[(haystack[i + nlen] & 0xE0) >> 5] & ((uint32_t)1 << (haystack[i + nlen] & 0x1F)))) {
				i += nlen;
			}
			else if (!(masks[(haystack[i + nlen - 1] & 0xE0) >> 5] & ((uint32_t)1 << (haystack[i + nlen - 1] & 0x1F)))) {
				i += nlen - 1;
			}
		}
	}

	return (SIZE_MAX);
}

static size_t rig_strstr_skipn_qgrams(TESTFUNC_PARAMS_NAMED) {
	size_t i, skip, w = hlen - nlen;
	size_t masks[sizeof(size_t) * 8] = { 0 };

	/* create compressed boyer-moore delta 1 table */
	skip = nlen - 2;
	/* process pattern[:-3] */
	for (i = 0; i < nlen - 2; i++) {
		masks[(needle[i] & STRSTRMASK)] |= ((size_t)1 << (needle[i + 1] & STRSTRMASK));
		if (needle[i] == needle[nlen - 2] && needle[i + 1] == needle[nlen - 1]) {
			skip = nlen - 3 - i;
		}
	}
	/* process pattern[-2,-1] outside the loop */
	masks[(needle[nlen - 2] & STRSTRMASK)] |= ((size_t)1 << (needle[nlen - 1] & STRSTRMASK));

	for (i = 0; i <= w; i++) {
		INC_CCNT;
		if (haystack[i + nlen - 2] == needle[nlen - 2] && haystack[i + nlen - 1] == needle[nlen - 1]) {
			INC_CCNT;
			/* candidate match */
			if (internal_str_cmp(haystack + i, needle, nlen - 2, ccnt) == 0) {
				/* got a match! */
				return (i);
			}
			/* miss: check if next q-gram is part of pattern */
			if (!(masks[(haystack[i + nlen - 1] & STRSTRMASK)] & ((size_t)1 << (haystack[i + nlen] & STRSTRMASK)))) {
				i += nlen - 1;
			}
			else {
				i += skip;
			}
		}
		else {
			/* skip: check if next q-gram is part of pattern */
			if (!(masks[(haystack[i + nlen - 1] & STRSTRMASK)] & ((size_t)1 << (haystack[i + nlen] & STRSTRMASK)))) {
				i += nlen - 1;
			}
			else if (!(masks[(haystack[i + nlen - 2] & STRSTRMASK)] & ((size_t)1 << (haystack[i + nlen - 1] & STRSTRMASK)))) {
				i += nlen - 2;
			}
		}
	}

	return (SIZE_MAX);
}

static size_t rig_strstr_skipn_qgrams_exact(TESTFUNC_PARAMS_NAMED) {
	size_t i, skip, w = hlen - nlen;
	uint32_t masks[2048] = { 0 };

	/* create compressed boyer-moore delta 1 table */
	skip = nlen - 2;
	/* process pattern[:-3] */
	for (i = 0; i < nlen - 2; i++) {
		masks[(needle[i] << 3) | ((needle[i + 1] & 0xE0) >> 5)] |= ((uint32_t)1 << (needle[i + 1] & 0x1F));
		if (needle[i] == needle[nlen - 2] && needle[i + 1] == needle[nlen - 1]) {
			skip = nlen - 3 - i;
		}
	}
	/* process pattern[-2,-1] outside the loop */
	masks[(needle[nlen - 2] << 3) | ((needle[nlen - 1] & 0xE0) >> 5)] |= ((uint32_t)1 << (needle[nlen - 1] & 0x1F));

	for (i = 0; i <= w; i++) {
		INC_CCNT;
		if (haystack[i + nlen - 2] == needle[nlen - 2] && haystack[i + nlen - 1] == needle[nlen - 1]) {
			INC_CCNT;
			/* candidate match */
			if (internal_str_cmp(haystack + i, needle, nlen - 2, ccnt) == 0) {
				/* got a match! */
				return (i);
			}
			/* miss: check if next q-gram is part of pattern */
			if (!(masks[(haystack[i + nlen - 1] << 3) | ((haystack[i + nlen] & 0xE0) >> 5)] & ((uint32_t)1 << (haystack[i + nlen] & 0x1F)))) {
				i += nlen - 1;
			}
			else {
				i += skip;
			}
		}
		else {
			/* skip: check if next q-gram is part of pattern */
			if (!(masks[(haystack[i + nlen - 1] << 3) | ((haystack[i + nlen] & 0xE0) >> 5)] & ((uint32_t)1 << (haystack[i + nlen] & 0x1F)))) {
				i += nlen - 1;
			}
			else if (!(masks[(haystack[i + nlen - 2] << 3) | ((haystack[i + nlen - 1] & 0xE0) >> 5)] & ((uint32_t)1 << (haystack[i + nlen - 1] & 0x1F)))) {
				i += nlen - 2;
			}
		}
	}

	return (SIZE_MAX);
}
