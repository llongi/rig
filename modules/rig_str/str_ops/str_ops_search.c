/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: str_ops_search.c 905 2011-03-10 07:38:20Z llongi $
 */

#ifndef STR_OPS_SEARCH_C
#define STR_OPS_SEARCH_C 1

#include "str_ops.c"

typedef struct str_ops_search_multiple_byte_prep *STR_OPS_SEARCH_MULTIPLE_BYTE_PREP;
typedef struct str_ops_search_multibyte_prep *STR_OPS_SEARCH_MULTIBYTE_PREP;
typedef struct str_ops_search_multibyte_reverse_prep *STR_OPS_SEARCH_MULTIBYTE_REVERSE_PREP;
typedef struct str_ops_search_multiple_multibyte_prep *STR_OPS_SEARCH_MULTIPLE_MULTIBYTE_PREP;
typedef struct str_ops_search_big3multiple_multibyte_prep *STR_OPS_SEARCH_BIG3MULTIPLE_MULTIBYTE_PREP;

static inline ssize_t str_ops_search_byte(const uint8_t *s, size_t slen, const uint8_t c, uint8_t mode, RIG_LIST allres);

static inline ssize_t str_ops_search_byte_reverse(const uint8_t *s, size_t slen, const uint8_t c, uint8_t mode, RIG_LIST allres);

static inline void str_ops_search_multiple_byte_prep(STR_OPS_SEARCH_MULTIPLE_BYTE_PREP mbp, const uint8_t *c, size_t clen);
static inline ssize_t str_ops_search_multiple_byte(STR_OPS_SEARCH_MULTIPLE_BYTE_PREP mbp, const uint8_t *s, size_t slen, uint8_t mode, RIG_LIST allres);

static inline void str_ops_search_multibyte_prep(STR_OPS_SEARCH_MULTIBYTE_PREP mp, const uint8_t *needle, size_t nlen);
static inline ssize_t str_ops_search_multibyte(STR_OPS_SEARCH_MULTIBYTE_PREP mp, const uint8_t *haystack, size_t hlen, uint8_t mode, RIG_LIST allres);

static inline void str_ops_search_multibyte_reverse_prep(STR_OPS_SEARCH_MULTIBYTE_REVERSE_PREP mrp, const uint8_t *needle, size_t nlen);
static inline ssize_t str_ops_search_multibyte_reverse(STR_OPS_SEARCH_MULTIBYTE_REVERSE_PREP mrp, const uint8_t *haystack, size_t hlen, uint8_t mode, RIG_LIST allres);

static inline void str_ops_search_multiple_multibyte_prep(STR_OPS_SEARCH_MULTIPLE_MULTIBYTE_PREP mmp, uint8_t *needle[], size_t nlen[], size_t ncount);
static inline ssize_t str_ops_search_multiple_multibyte(STR_OPS_SEARCH_MULTIPLE_MULTIBYTE_PREP mmp, const uint8_t *haystack, size_t hlen, uint8_t mode, RIG_LIST allres);

static inline void str_ops_search_big3multiple_multibyte_prep(STR_OPS_SEARCH_BIG3MULTIPLE_MULTIBYTE_PREP b3mmp, uint8_t *needle[], size_t nlen[], size_t ncount);
static inline ssize_t str_ops_search_big3multiple_multibyte(STR_OPS_SEARCH_BIG3MULTIPLE_MULTIBYTE_PREP b3mmp, const uint8_t *haystack, size_t hlen, uint8_t mode, RIG_LIST allres);

// String search masks
#if SIZE_MAX == 65535UL
	#define STRSTRMASK 0x0F
	#define STRSTRSHIFT 4
#elif SIZE_MAX == 4294967295UL
	#define STRSTRMASK 0x1F
	#define STRSTRSHIFT 5
#elif SIZE_MAX == 18446744073709551615UL
	#define STRSTRMASK 0x3F
	#define STRSTRSHIFT 6
#else
	#error size_t is not a 16, 32 or 64 bit type.
#endif

// String search modes
#define MODE_SEARCH		0x01
#define MODE_COUNT		0x02
#define MODE_SEARCH_ALL	0x03

// String search return macros
#define COUNT_RETURN(cntvar, retvar, listvar) \
	if (mode == MODE_SEARCH) { \
		return ((ssize_t)(retvar)); \
	} \
	else if (mode == MODE_COUNT) { \
		cntvar++; \
	} \
	else { \
		cntvar++; \
		rig_list_add(listvar, &(size_t) { ((size_t)(retvar)) }); \
	}

#define COUNT_FRETURN(cntvar) \
	if (mode == MODE_COUNT || mode == MODE_SEARCH_ALL) { \
		return ((ssize_t)(cntvar)); \
	}

#define COUNT_LIST_INIT(listvar) \
	if (mode == MODE_SEARCH_ALL && listvar == NULL) { \
		ERRET(EINVAL, -1); \
	}

/*
 * All search functions here support a maximum string length of SSIZE_MAX, meaning the string can at most occupy
 * SSIZE_MAX + 1 bytes, where the "+ 1" is to accommodate the final \0.
 * While this doesn't happen for the byte search functions, the multibyte search functions can access haystack[hlen],
 * meaning strings passed to them need to be properly NUL-terminated (haystack[hlen] must exist and be \0).
 * Where this is the case, it is properly NOTEd.
 */


static inline ssize_t str_ops_search_byte(const uint8_t *s, size_t slen, const uint8_t c, uint8_t mode, RIG_LIST allres) {
	const uint8_t *sp = s;
	size_t count = 0;

	COUNT_LIST_INIT(allres);

	while ((slen) && (MISALIGNED_SIZET(s))) {
		if (*s == c) {
			COUNT_RETURN(count, s - sp, allres);
		}
		slen--;
		s++;
	}

	if (slen >= sizeof(size_t)) {
		size_t cbuf = c;

		for (size_t i = 8; i < (sizeof(size_t) * 8); i <<= 1) {
			cbuf |= (cbuf << i);
		}

		const size_t *as = (const size_t *)s;
		size_t asxcbuf;

		while (slen >= sizeof(size_t)) {
			asxcbuf = *as ^ cbuf;
			if (RIG_FINDNULL(asxcbuf)) {
				if (mode == MODE_SEARCH) {
					break;
				}
				else if (mode == MODE_COUNT) {
					count += RIG_COUNTNULL(asxcbuf);
				}
				else {
					count += RIG_COUNTNULL(asxcbuf);
					s = (const uint8_t *)as;
					if (*s   == c) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
					if (*++s == c) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
					if (sizeof(size_t) > 2) {
						if (*++s == c) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
						if (*++s == c) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
						if (sizeof(size_t) > 4) {
							if (*++s == c) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
							if (*++s == c) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
							if (*++s == c) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
							if (*++s == c) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
						}
					}
				}
			}
			slen -= sizeof(size_t);
			as++;
		}

		s = (const uint8_t *)as;
	}

	while (slen) {
		if (*s == c) {
			COUNT_RETURN(count, s - sp, allres);
		}
		slen--;
		s++;
	}

	COUNT_FRETURN(count);

	return (-1); // NOT FOUND
}

static inline ssize_t str_ops_search_byte_reverse(const uint8_t *s, size_t slen, const uint8_t c, uint8_t mode, RIG_LIST allres) {
	const uint8_t *sp = s;
	s += slen - 1;
	size_t count = 0;

	COUNT_LIST_INIT(allres);

	while ((slen) && (MISALIGNED_SIZET(s))) {
		if (*s == c) {
			COUNT_RETURN(count, s - sp, allres);
		}
		slen--;
		s--;
	}

	if (slen >= sizeof(size_t)) {
		// Need to check the aligned char too
		if (*s == c) {
			COUNT_RETURN(count, s - sp, allres);
		}

		size_t cbuf = c;

		for (size_t i = 8; i < (sizeof(size_t) * 8); i <<= 1) {
			cbuf |= (cbuf << i);
		}

		const size_t *as = (const size_t *)s;
		size_t asxcbuf;

		as--;

		while (slen >= sizeof(size_t)) {
			asxcbuf = *as ^ cbuf;
			if (RIG_FINDNULL(asxcbuf)) {
				if (mode == MODE_SEARCH) {
					break;
				}
				else if (mode == MODE_COUNT) {
					count += RIG_COUNTNULL(asxcbuf);
				}
				else {
					count += RIG_COUNTNULL(asxcbuf);
					s = (const uint8_t *)(as + 1);
					if (*--s == c) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
					if (*--s == c) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
					if (sizeof(size_t) > 2) {
						if (*--s == c) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
						if (*--s == c) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
						if (sizeof(size_t) > 4) {
							if (*--s == c) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
							if (*--s == c) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
							if (*--s == c) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
							if (*--s == c) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
						}
					}
				}
			}
			slen -= sizeof(size_t);
			as--;
		}

		as++;

		s = (const uint8_t *)as;
	}

	while (slen) {
		if (*s == c) {
			COUNT_RETURN(count, s - sp, allres);
		}
		slen--;
		s--;
	}

	COUNT_FRETURN(count);

	return (-1); // NOT FOUND
}

struct str_ops_search_multiple_byte_prep {
	const uint8_t *c;
	size_t clen;
	size_t cbufs[];
};

static inline void str_ops_search_multiple_byte_prep(STR_OPS_SEARCH_MULTIPLE_BYTE_PREP mbp, const uint8_t *c, size_t clen) {
	mbp->c = c;
	mbp->clen = clen;

	for (size_t i = 0, j; i < clen; i++) {
		mbp->cbufs[i] = c[i];

		for (j = 8; j < (sizeof(size_t) * 8); j <<= 1) {
			mbp->cbufs[i] |= (mbp->cbufs[i] << j);
		}
	}
}

static inline ssize_t str_ops_search_multiple_byte(STR_OPS_SEARCH_MULTIPLE_BYTE_PREP mbp, const uint8_t *s, size_t slen, uint8_t mode, RIG_LIST allres) {
	const uint8_t *sp = s;
	size_t i;
	size_t count = 0;

	COUNT_LIST_INIT(allres);

	while ((slen) && (MISALIGNED_SIZET(s))) {
		for (i = 0; i < mbp->clen; i++) {
			if (*s == mbp->c[i]) {
				COUNT_RETURN(count, s - sp, allres);
			}
		}
		slen--;
		s++;
	}

	if (slen >= sizeof(size_t)) {
		const size_t *as = (const size_t *)s;
		size_t asxcbuf;

		while (slen >= sizeof(size_t)) {
			for (i = 0; i < mbp->clen; i++) {
				asxcbuf = *as ^ mbp->cbufs[i];
				if (RIG_FINDNULL(asxcbuf)) {
					if (mode == MODE_SEARCH) {
						goto breakout;
					}
					else if (mode == MODE_COUNT) {
						count += RIG_COUNTNULL(asxcbuf);
					}
					else {
						count += RIG_COUNTNULL(asxcbuf);
						s = (const uint8_t *)as;
						if (*s   == mbp->c[i]) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
						if (*++s == mbp->c[i]) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
						if (sizeof(size_t) > 2) {
							if (*++s == mbp->c[i]) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
							if (*++s == mbp->c[i]) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
							if (sizeof(size_t) > 4) {
								if (*++s == mbp->c[i]) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
								if (*++s == mbp->c[i]) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
								if (*++s == mbp->c[i]) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
								if (*++s == mbp->c[i]) rig_list_add(allres, &(size_t) { ((size_t)(s - sp)) });
							}
						}
					}
				}
			}
			slen -= sizeof(size_t);
			as++;
		}

		breakout:
		s = (const uint8_t *)as;
	}

	while (slen) {
		for (i = 0; i < mbp->clen; i++) {
			if (*s == mbp->c[i]) {
				COUNT_RETURN(count, s - sp, allres);
			}
		}
		slen--;
		s++;
	}

	COUNT_FRETURN(count);

	return (-1); // NOT FOUND
}

struct str_ops_search_multibyte_prep {
	const uint8_t *needle;
	size_t nlen;
	size_t mask;
	size_t skip;
};

static inline void str_ops_search_multibyte_prep(STR_OPS_SEARCH_MULTIBYTE_PREP mp, const uint8_t *needle, size_t nlen) {
	mp->needle = needle;
	mp->nlen = nlen;

	/* create compressed boyer-moore delta 1 table */
	mp->mask = 0;
	mp->skip = nlen - 1;
	/* process pattern[:-2] */
	for (size_t i = 0; i < nlen - 1; i++) {
		mp->mask |= ((size_t)1 << (needle[i] & STRSTRMASK));
		if (needle[i] == needle[nlen - 1]) {
			mp->skip = nlen - 2 - i;
		}
	}
	/* process pattern[-1] outside the loop */
	mp->mask |= ((size_t)1 << (needle[nlen - 1] & STRSTRMASK));
}

static inline ssize_t str_ops_search_multibyte(STR_OPS_SEARCH_MULTIBYTE_PREP mp, const uint8_t *haystack, size_t hlen, uint8_t mode, RIG_LIST allres) {
	size_t count = 0;

	COUNT_LIST_INIT(allres);

	for (size_t i = 0, nlen = mp->nlen, w = hlen - nlen; i <= w; i++) {
		if (haystack[i + nlen - 1] == mp->needle[nlen - 1]) {
			/* candidate match */
			if (str_ops_cmp(haystack + i, mp->needle, nlen - 1) == 0) {
				/* got a match! */
				COUNT_RETURN(count, i, allres);
			}
			/* miss: check if next character is part of pattern */
			if (!(mp->mask & ((size_t)1 << (haystack[i + nlen] & STRSTRMASK)))) { // NOTE: may access haystack[hlen]
				i += nlen;
			}
			else {
				i += mp->skip;
			}
		}
		else {
			/* skip: check if next character is part of pattern */
			if (!(mp->mask & ((size_t)1 << (haystack[i + nlen] & STRSTRMASK)))) { // NOTE: may access haystack[hlen]
				i += nlen;
			}
			else if (!(mp->mask & ((size_t)1 << (haystack[i + nlen - 1] & STRSTRMASK)))) {
				i += nlen - 1;
			}
		}
	}

	COUNT_FRETURN(count);

	return (-1); // NOT FOUND
}

struct str_ops_search_multibyte_reverse_prep {
	const uint8_t *needle;
	size_t nlen;
	size_t mask;
	size_t skip;
};

static inline void str_ops_search_multibyte_reverse_prep(STR_OPS_SEARCH_MULTIBYTE_REVERSE_PREP mrp, const uint8_t *needle, size_t nlen) {
	mrp->needle = needle;
	mrp->nlen = nlen;

	/* create compressed boyer-moore delta 1 table */
	mrp->mask = 0;
	mrp->skip = nlen - 1;
	/* process pattern[1:] */
	for (size_t i = nlen - 1; i > 0; i--) {
		mrp->mask |= ((size_t)1 << (needle[i] & STRSTRMASK));
		if (needle[i] == needle[0]) {
			mrp->skip = i - 1;
		}
	}
	/* process pattern[0] outside the loop */
	mrp->mask |= ((size_t)1 << (needle[0] & STRSTRMASK));
}

static inline ssize_t str_ops_search_multibyte_reverse(STR_OPS_SEARCH_MULTIBYTE_REVERSE_PREP mrp, const uint8_t *haystack, size_t hlen, uint8_t mode, RIG_LIST allres) {
	size_t count = 0;

	COUNT_LIST_INIT(allres);

	for (size_t i = 0, nlen = mrp->nlen, w = hlen - nlen; i <= w; i++) {
		if (haystack[w - i] == mrp->needle[0]) {
			/* candidate match */
			if (str_ops_cmp(haystack + w - i + 1, mrp->needle + 1, nlen - 1) == 0) {
				/* got a match! */
				COUNT_RETURN(count, w - i, allres);
			}
			/* miss: check if previous character is part of pattern */
			if (i != w && !(mrp->mask & ((size_t)1 << (haystack[w - i - 1] & STRSTRMASK)))) {
				i += nlen;
			}
			else {
				i += mrp->skip;
			}
		}
		else {
			/* skip: check if previous character is part of pattern */
			if (i != w && !(mrp->mask & ((size_t)1 << (haystack[w - i - 1] & STRSTRMASK)))) {
				i += nlen;
			}
			else if (!(mrp->mask & ((size_t)1 << (haystack[w - i] & STRSTRMASK)))) {
				i += nlen - 1;
			}
		}
	}

	COUNT_FRETURN(count);

	return (-1); // NOT FOUND
}

struct str_ops_search_multiple_multibyte_prep {
	uint8_t **needle;
	size_t *nlen;
	size_t nminlen;
	size_t minskip;
	size_t suffixes[256][256 / (sizeof(size_t) * 8)];
	size_t allpairs[256][256 / (sizeof(size_t) * 8)];
	size_t TLB[65536 + 1]; // + 1 because we access the 65536th element later
};

static inline void str_ops_search_multiple_multibyte_prep(STR_OPS_SEARCH_MULTIPLE_MULTIBYTE_PREP mmp, uint8_t *needle[], size_t nlen[], size_t ncount) {
	str_ops_set((uint8_t *)mmp, 0, sizeof(*mmp));

	mmp->needle = needle;
	mmp->nlen = nlen;

	/* search shortest pattern */
	size_t nminlen = nlen[0];
	for (size_t i = 1; i < ncount; i++) {
		nminlen = (nlen[i] < nminlen) ? (nlen[i]) : (nminlen);
	}
	mmp->nminlen = nminlen;

	/* sort strings and corresponding lengths by descending suffix */
	str_ops_lscsort(needle, nlen, ncount, nminlen, 2);

	/* process patterns */
	size_t minskip = nminlen - 2;
	for (size_t i = 0, j = 0, act_suff = 0, old_suff = 1, skip; i < ncount; i++) {
		/* initialize translation buffer */
		act_suff = (size_t)((needle[i][nminlen - 2] << 8) | (needle[i][nminlen - 1]));
		if (act_suff >= old_suff) {
			for (j = old_suff; j <= act_suff; j++) {
				mmp->TLB[j] = i;
			}
			old_suff = act_suff + 1;
		}

		/* complete translation buffer initialization on last iteration */
		if (i == (ncount - 1)) {
			for (j = old_suff; j <= 65536; j++) {
				mmp->TLB[j] = ncount;
			}
		}

		/* process all pairs */
		for (j = 0, skip = nminlen - 2; j < nminlen - 2; j++) {
			mmp->allpairs[(needle[i][j])][(needle[i][j + 1] >> STRSTRSHIFT)] |= ((size_t)1 << (needle[i][j + 1] & STRSTRMASK));

			/* set skip value */
			if (needle[i][j] == needle[i][nminlen - 2] && needle[i][j + 1] == needle[i][nminlen - 1]) {
				skip = nminlen - 3 - j;
			}
		}
		for (j = nminlen - 2; j < nlen[i] - 1; j++) {
			mmp->allpairs[(needle[i][j])][(needle[i][j + 1] >> STRSTRSHIFT)] |= ((size_t)1 << (needle[i][j + 1] & STRSTRMASK));
		}

		/* set minskip value to minimal skip */
		minskip = (skip < minskip) ? (skip) : (minskip);

		/* process suffix */
		mmp->suffixes[(needle[i][nminlen - 2])][(needle[i][nminlen - 1] >> STRSTRSHIFT)] |= ((size_t)1 << (needle[i][nminlen - 1] & STRSTRMASK));
	}
	mmp->minskip = minskip;
}

static inline ssize_t str_ops_search_multiple_multibyte(STR_OPS_SEARCH_MULTIPLE_MULTIBYTE_PREP mmp, const uint8_t *haystack, size_t hlen, uint8_t mode, RIG_LIST allres) {
	size_t count = 0;

	COUNT_LIST_INIT(allres);

	for (size_t i = 0, nminlen = mmp->nminlen, w = hlen - nminlen, suff = 0, nidx, nidx_end; i <= w; i++) {
		if ((mmp->suffixes[(haystack[i + nminlen - 2])][(haystack[i + nminlen - 1] >> STRSTRSHIFT)] & ((size_t)1 << (haystack[i + nminlen - 1] & STRSTRMASK)))) {
			suff = (size_t)((haystack[i + nminlen - 2] << 8) | (haystack[i + nminlen - 1]));
			for (nidx = mmp->TLB[suff], nidx_end = mmp->TLB[suff + 1]; nidx < nidx_end; nidx++) {
				/* candidate matches */
				if (haystack[i] == mmp->needle[nidx][0] && haystack[i + 1] == mmp->needle[nidx][1] &&
					str_ops_cmp(haystack + i + 2, mmp->needle[nidx] + 2, mmp->nlen[nidx] - 2) == 0) {
					/* got a match! */
					COUNT_RETURN(count, i, allres);
				}
			}
			/* miss: check if next q-gram is part of pattern */
			if (!(mmp->allpairs[(haystack[i + nminlen - 1])][(haystack[i + nminlen] >> STRSTRSHIFT)] & ((size_t)1 << (haystack[i + nminlen] & STRSTRMASK)))) { // NOTE: may access haystack[hlen]
				i += nminlen - 1;
			}
			else {
				i += mmp->minskip;
			}
		}
		else {
			/* skip: check if next q-gram is part of pattern */
			if (!(mmp->allpairs[(haystack[i + nminlen - 1])][(haystack[i + nminlen] >> STRSTRSHIFT)] & ((size_t)1 << (haystack[i + nminlen] & STRSTRMASK)))) { // NOTE: may access haystack[hlen]
				i += nminlen - 1;
			}
			else if (!(mmp->allpairs[(haystack[i + nminlen - 2])][(haystack[i + nminlen - 1] >> STRSTRSHIFT)] & ((size_t)1 << (haystack[i + nminlen - 1] & STRSTRMASK)))) {
				i += nminlen - 2;
			}
		}
	}

	COUNT_FRETURN(count);

	return (-1); // NOT FOUND
}

struct str_ops_search_big3multiple_multibyte_prep {
	uint8_t **needle;
	size_t *nlen;
	size_t nminlen;
	size_t minskip;
	size_t suffixes[128][256][256 / (sizeof(size_t) * 8)];
	size_t allpairs[128][256][256 / (sizeof(size_t) * 8)];
	size_t TLB[65536 + 1]; // + 1 because we access the 65536th element later
};
static inline void str_ops_search_big3multiple_multibyte_prep(STR_OPS_SEARCH_BIG3MULTIPLE_MULTIBYTE_PREP b3mmp, uint8_t *needle[], size_t nlen[], size_t ncount) {
	str_ops_set((uint8_t *)b3mmp, 0, sizeof(*b3mmp));

	b3mmp->needle = needle;
	b3mmp->nlen = nlen;

	/* search shortest pattern */
	size_t nminlen = nlen[0];
	for (size_t i = 1; i < ncount; i++) {
		nminlen = (nlen[i] < nminlen) ? (nlen[i]) : (nminlen);
	}
	b3mmp->nminlen = nminlen;

	/* sort strings and corresponding lengths by descending suffix */
	str_ops_lscsort(needle, nlen, ncount, nminlen, 2);

	/* process patterns */
	size_t minskip = nminlen - 3;
	for (size_t i = 0, j = 0, act_suff = 0, old_suff = 1, skip; i < ncount; i++) {
		/* initialize translation buffer */
		act_suff = (size_t)((needle[i][nminlen - 2] << 8) | (needle[i][nminlen - 1]));
		if (act_suff >= old_suff) {
			for (j = old_suff; j <= act_suff; j++) {
				b3mmp->TLB[j] = i;
			}
			old_suff = act_suff + 1;
		}

		/* complete translation buffer initialization on last iteration */
		if (i == (ncount - 1)) {
			for (j = old_suff; j <= 65536; j++) {
				b3mmp->TLB[j] = ncount;
			}
		}

		/* process all pairs */
		for (j = 0, skip = nminlen - 3; j < nminlen - 3; j++) {
			b3mmp->allpairs[(needle[i][j]) & 0x7F][(needle[i][j + 1])][(needle[i][j + 2] >> STRSTRSHIFT)] |= ((size_t)1 << (needle[i][j + 2] & STRSTRMASK));

			/* set skip value */
			if (needle[i][j] == needle[i][nminlen - 3] && needle[i][j + 1] == needle[i][nminlen - 2] && needle[i][j + 2] == needle[i][nminlen - 1]) {
				skip = nminlen - 4 - j;
			}
		}
		for (j = nminlen - 3; j < nlen[i] - 2; j++) {
			b3mmp->allpairs[(needle[i][j]) & 0x7F][(needle[i][j + 1])][(needle[i][j + 2] >> STRSTRSHIFT)] |= ((size_t)1 << (needle[i][j + 2] & STRSTRMASK));
		}

		/* set minskip value to minimal skip */
		minskip = (skip < minskip) ? (skip) : (minskip);

		/* process suffix */
		b3mmp->suffixes[(needle[i][nminlen - 3] & 0x7F)][(needle[i][nminlen - 2])][(needle[i][nminlen - 1] >> STRSTRSHIFT)] |= ((size_t)1 << (needle[i][nminlen - 1] & STRSTRMASK));
	}
	b3mmp->minskip = minskip;
}

static inline ssize_t str_ops_search_big3multiple_multibyte(STR_OPS_SEARCH_BIG3MULTIPLE_MULTIBYTE_PREP b3mmp, const uint8_t *haystack, size_t hlen, uint8_t mode, RIG_LIST allres) {
	size_t count = 0;

	COUNT_LIST_INIT(allres);

	for (size_t i = 0, nminlen = b3mmp->nminlen, w = hlen - nminlen, suff = 0, nidx, nidx_end; i <= w; i++) {
		if ((b3mmp->suffixes[(haystack[i + nminlen - 3] & 0x7F)][(haystack[i + nminlen - 2])][(haystack[i + nminlen - 1] >> STRSTRSHIFT)] & ((size_t)1 << (haystack[i + nminlen - 1] & STRSTRMASK)))) {
			suff = (size_t)((haystack[i + nminlen - 2] << 8) | (haystack[i + nminlen - 1]));
			for (nidx = b3mmp->TLB[suff], nidx_end = b3mmp->TLB[suff + 1]; nidx < nidx_end; nidx++) {
				/* candidate matches */
				if (haystack[i] == b3mmp->needle[nidx][0] && haystack[i + 1] == b3mmp->needle[nidx][1] &&
					str_ops_cmp(haystack + i + 2, b3mmp->needle[nidx] + 2, b3mmp->nlen[nidx] - 2) == 0) {
					/* got a match! */
					COUNT_RETURN(count, i, allres);
				}
			}
			/* miss: check if next q-gram is part of pattern */
			if (!(b3mmp->allpairs[(haystack[i + nminlen - 2] & 0x7F)][(haystack[i + nminlen - 1])][(haystack[i + nminlen] >> STRSTRSHIFT)] & ((size_t)1 << (haystack[i + nminlen] & STRSTRMASK)))) { // NOTE: may access haystack[hlen]
				i += nminlen - 2;
			}
			else {
				i += b3mmp->minskip;
			}
		}
		else {
			/* skip: check if next q-gram is part of pattern */
			if (!(b3mmp->allpairs[(haystack[i + nminlen - 2] & 0x7F)][(haystack[i + nminlen - 1])][(haystack[i + nminlen] >> STRSTRSHIFT)] & ((size_t)1 << (haystack[i + nminlen] & STRSTRMASK)))) { // NOTE: may access haystack[hlen]
				i += nminlen - 2;
			}
			else if (!(b3mmp->allpairs[(haystack[i + nminlen - 3] & 0x7F)][(haystack[i + nminlen - 2])][(haystack[i + nminlen - 1] >> STRSTRSHIFT)] & ((size_t)1 << (haystack[i + nminlen - 1] & STRSTRMASK)))) {
				i += nminlen - 3;
			}
		}
	}

	COUNT_FRETURN(count);

	return (-1); // NOT FOUND
}

#endif /* STR_OPS_SEARCH_C */
