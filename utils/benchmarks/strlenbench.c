#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// Test function parameters
#define TESTFUNC_PARAMS const uint8_t *
#define TESTFUNC_PARAMS_NAMED const uint8_t *s
#define TESTFUNC_PARAMS_USE s2

// List of implementations
static size_t libc_strlen(TESTFUNC_PARAMS_NAMED);
static size_t naive_strlen(TESTFUNC_PARAMS_NAMED);
static size_t cp_strlen(TESTFUNC_PARAMS_NAMED);
static size_t rig_strlen(TESTFUNC_PARAMS_NAMED);
static size_t kjs_strlen_utf8(TESTFUNC_PARAMS_NAMED);
static size_t gp_strlen_utf8(TESTFUNC_PARAMS_NAMED);
static size_t cp_strlen_utf8(TESTFUNC_PARAMS_NAMED);
static size_t rig_strlen_utf8(TESTFUNC_PARAMS_NAMED);

const struct s_bench_implementations {
	const uint8_t *name;
	size_t (*func)(TESTFUNC_PARAMS);
} bench_implementations[] = {
	{ "libc_strlen", libc_strlen },
	{ "naive_strlen", naive_strlen },
	{ "cp_strlen", cp_strlen },
	{ "rig_strlen", rig_strlen },
	{ "kjs_strlen_utf8", kjs_strlen_utf8 },
	{ "gp_strlen_utf8", gp_strlen_utf8 },
	{ "cp_strlen_utf8", cp_strlen_utf8 },
	{ "rig_strlen_utf8", rig_strlen_utf8 },
};

#include "commonbench.h"

#define RIG_LOWBITSMASK  ((size_t)(-1) / 0xFF)
#define RIG_HIGHBITSMASK (((size_t)(-1) / 0xFF) * 0x80)

static size_t libc_strlen(TESTFUNC_PARAMS_NAMED) {
	return (strlen(s));
}

static size_t naive_strlen(TESTFUNC_PARAMS_NAMED) {
	size_t len = 0;

	while (*s++) {
		len++;
	}

	return (len);
}


static size_t cp_strlen(TESTFUNC_PARAMS_NAMED) {
	const uint8_t *_s = s;
	size_t u;

	/* Handle any initial misaligned bytes. */
	for (; (uintptr_t)(s) & (sizeof(size_t) - 1); s++) {
		/* Exit if we hit a zero byte. */
		if (*s == '\0')
			goto done;
	}

	/* Handle complete blocks. */
	for (; ; s += sizeof(size_t)) {
		/* Prefetch 256 bytes ahead. */
		__builtin_prefetch(&s[256], 0, 0);

		/* Grab 4 or 8 bytes of UTF-8 data. */
		u = *(const size_t *)(s);

		/* Exit the loop if there are any zero bytes. */
		if ((u - RIG_LOWBITSMASK) & (~u) & RIG_HIGHBITSMASK)
			break;
	}

	/* Take care of any left-over bytes. */
	for (; ; s++) {
		/* Exit if we hit a zero byte. */
		if (*s == '\0')
			break;
	}

done:
	return (size_t)(s - _s);
}

static size_t kjs_strlen_utf8(TESTFUNC_PARAMS_NAMED) {
	size_t i = 0, j = 0;

	while (s[i]) {
		if ((s[i] & 0xC0) != 0x80)
			j++;
		i++;
	}

	return (j);
}

// NOTE: this is broken for the "random 1k bytes" test benchmark
static size_t gp_strlen_utf8(TESTFUNC_PARAMS_NAMED) {
	const int8_t *_s = (const int8_t *)s;
	size_t i = 0;
	size_t iBefore = 0;
	size_t count = 0;

	while (_s[i] > 0) {
ascii:
		i++;
	}

	count += i - iBefore;

	while (_s[i]) {
		if (_s[i] > 0) {
			iBefore = i;
			goto ascii;
		} else {
			switch (0xF0 & _s[i]) {
			case 0xE0:
				i += 3;
				break;
			case 0xF0:
				i += 4;
				break;
			default:
				i += 2;
				break;
			}
		}

		count++;
	}

	return (count);
}

static size_t cp_strlen_utf8(TESTFUNC_PARAMS_NAMED) {
	const uint8_t *_s = s;
	size_t count = 0;
	size_t u;
	uint8_t b;

	/* Handle any initial misaligned bytes. */
	for (; (uintptr_t)(s) & (sizeof(size_t) - 1); s++) {
		b = *s;

		/* Exit if we hit a zero byte. */
		if (b == '\0')
			goto done;

		/* Is this byte NOT the first byte of a character? */
		count += (size_t)((b >> 7) & ((~b) >> 6));
	}

	/* Handle complete blocks. */
	for (; ; s += sizeof(size_t)) {
		/* Prefetch 256 bytes ahead. */
		__builtin_prefetch(&s[256], 0, 0);

		/* Grab 4 or 8 bytes of UTF-8 data. */
		u = *(const size_t *)(s);

		/* Exit the loop if there are any zero bytes. */
		if ((u - RIG_LOWBITSMASK) & (~u) & RIG_HIGHBITSMASK)
			break;

		/* Count bytes which are NOT the first byte of a character. */
		u = ((u & RIG_HIGHBITSMASK) >> 7) & ((~u) >> 6);
		count += (u * RIG_LOWBITSMASK) >> ((sizeof(size_t) - 1) * 8);
	}

	/* Take care of any left-over bytes. */
	for (; ; s++) {
		b = *s;

		/* Exit if we hit a zero byte. */
		if (b == '\0')
			break;

		/* Is this byte NOT the first byte of a character? */
		count += (size_t)((b >> 7) & ((~b) >> 6));
	}

done:
	return ((size_t)(s - _s) - count);
}

// Non-zero if there is a NULL-byte in X (X is size_t)
#define RIG_FINDNULL(X) (((X) - RIG_LOWBITSMASK) & ~(X) & RIG_HIGHBITSMASK)

static size_t rig_strlen(const uint8_t *str) {
	size_t range = 0;
	bool utf8range = 0;
	bool utf8result = 0;
	size_t len = 0;
	size_t utf8_neglen = 0;

	while ((*str) && (MISALIGNED_SIZET(str)) && ((range == 0) || ((utf8range) ? ((len - utf8_neglen) < range) : (len < range)))) {
		len++;
		utf8_neglen += (size_t)((*str >> 6) == 0x02);
		str++;
	}

	if ((*str) && ((range == 0) || ((utf8range) ? ((len + sizeof(size_t) - 1 - utf8_neglen) < range) : ((len + sizeof(size_t) - 1) < range)))) {
		const size_t *astr = (const size_t *)str;

		__builtin_prefetch(&astr[32], 0, 0);

		while ((!RIG_FINDNULL(*astr)) && ((range == 0) || ((utf8range) ? ((len + sizeof(size_t) - 1 - utf8_neglen) < range) : ((len + sizeof(size_t) - 1) < range)))) {
			len += sizeof(size_t);
			utf8_neglen += ((((((*astr) >> 1) & (~(*astr))) >> 6) & RIG_LOWBITSMASK) * RIG_LOWBITSMASK) >> ((sizeof(size_t) - 1) * 8);
			astr++;
			__builtin_prefetch(&astr[32], 0, 0);
		}

		str = (const uint8_t *)astr;
	}

	while ((*str) && ((range == 0) || ((utf8range) ? ((len - utf8_neglen) < range) : (len < range)))) {
		len++;
		utf8_neglen += (size_t)((*str >> 6) == 0x02);
		str++;
	}

	if (utf8result) {
		return (len - utf8_neglen);
	}

	return (len);
}

static size_t rig_strlen_utf8(const uint8_t *str) {
	size_t range = 0;
	bool utf8range = 0;
	bool utf8result = 1;
	size_t len = 0;
	size_t utf8_neglen = 0;

	while ((*str) && (MISALIGNED_SIZET(str)) && ((range == 0) || ((utf8range) ? ((len - utf8_neglen) < range) : (len < range)))) {
		len++;
		utf8_neglen += (size_t)((*str >> 6) == 0x02);
		str++;
	}

	if ((*str) && ((range == 0) || ((utf8range) ? ((len + sizeof(size_t) - 1 - utf8_neglen) < range) : ((len + sizeof(size_t) - 1) < range)))) {
		const size_t *astr = (const size_t *)str;

		__builtin_prefetch(&astr[32], 0, 0);

		while ((!RIG_FINDNULL(*astr)) && ((range == 0) || ((utf8range) ? ((len + sizeof(size_t) - 1 - utf8_neglen) < range) : ((len + sizeof(size_t) - 1) < range)))) {
			len += sizeof(size_t);
			utf8_neglen += ((((((*astr) >> 1) & (~(*astr))) >> 6) & RIG_LOWBITSMASK) * RIG_LOWBITSMASK) >> ((sizeof(size_t) - 1) * 8);
			astr++;
			__builtin_prefetch(&astr[32], 0, 0);
		}

		str = (const uint8_t *)astr;
	}

	while ((*str) && ((range == 0) || ((utf8range) ? ((len - utf8_neglen) < range) : (len < range)))) {
		len++;
		utf8_neglen += (size_t)((*str >> 6) == 0x02);
		str++;
	}

	if (utf8result) {
		return (len - utf8_neglen);
	}

	return (len);
}
