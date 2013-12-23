/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: str_ops.c 1011 2011-09-23 22:57:14Z llongi $
 */

#ifndef STR_OPS_C
#define STR_OPS_C 1

#include <string.h>

static inline void   str_ops_copy(uint8_t *dest, const uint8_t *src, size_t len);
static inline int    str_ops_cmp(const uint8_t *s1, const uint8_t *s2, size_t len);
static inline void   str_ops_set(uint8_t *s, const uint8_t c, size_t len);
static inline size_t str_ops_strlen(const uint8_t *str, size_t range, bool utf8range, bool utf8result);
static inline void   str_ops_lscsort(uint8_t **str1, size_t *strlen1, size_t strcount, size_t sortstart, size_t sortlength);


static inline void str_ops_copy(uint8_t *dest, const uint8_t *src, size_t len) {
	// Copy the string, based on length, works for binary strings too
	// Nothing to copy, so let's return directly
	if (len == 0 || dest == src) {
		return;
	}

	memmove(dest, src, len);
}

static inline int str_ops_cmp(const uint8_t *s1, const uint8_t *s2, size_t len) {
	// Compare two strings, based on length, works for binary strings too
	// Nothing to compare, so let's return directly
	if (len == 0 || s1 == s2) {
		return (0);
	}

	return (memcmp(s1, s2, len));
}

static inline void str_ops_set(uint8_t *s, const uint8_t c, size_t len) {
	// Set the string, based on length, works for binary strings too
	// Nothing to set, so let's return directly
	if (len == 0) {
		return;
	}

	memset(s, c, len);
}

static inline size_t str_ops_strlen(const uint8_t *str, size_t range, bool utf8range, bool utf8result) {
	size_t len = 0;
	size_t utf8_neglen = 0;

	while ((*str) && (MISALIGNED_SIZET(str)) && ((range == 0) || ((utf8range) ? ((len - utf8_neglen) < range) : (len < range)))) {
		len++;
		utf8_neglen += (size_t)((*str >> 6) == 0x02);
		str++;
	}

	if ((*str) && ((range == 0) || ((utf8range) ? ((len + sizeof(size_t) - 1 - utf8_neglen) < range) : ((len + sizeof(size_t) - 1) < range)))) {
		const size_t *astr = (const size_t *)str;

		while ((!RIG_FINDNULL(*astr)) && ((range == 0) || ((utf8range) ? ((len + sizeof(size_t) - 1 - utf8_neglen) < range) : ((len + sizeof(size_t) - 1) < range)))) {
			len += sizeof(size_t);
			utf8_neglen += ((((((*astr) >> 1) & (~(*astr))) >> 6) & RIG_LOWBITSMASK) * RIG_LOWBITSMASK) >> ((sizeof(size_t) - 1) * 8);
			astr++;
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

static inline void str_ops_lscsort(uint8_t **str1, size_t *strlen1, size_t strcount, size_t sortstart, size_t sortlength) {
#warning "Broken with current queue that doesn't allow NULLs, transitioning to embedded queue needed."
	size_t i, strindex, qecount;
	void *qelem;
	uint8_t sortc;
	RIG_QUEUE sortq[UINT8_MAX + 1] = { NULL };
	uint8_t *strstor[strcount];
	size_t strlenstor[strcount];
	uint8_t **str2 = strstor;
	size_t *strlen2 = strlenstor;

	for (size_t rep = 1; rep <= sortlength; rep++) {
		for (i = 0; i < strcount; i++) {
			sortc = str1[i][sortstart - rep];
			if (sortq[sortc] == NULL) {
				sortq[sortc] = rig_queue_init(strcount);
			}
			rig_queue_put(sortq[sortc], (void *)i);
		}

		for (i = 0, strindex = 0; i < UINT8_MAX + 1; i++) {
			if (sortq[i] != NULL) {
				qecount = rig_queue_count(sortq[i]);
				while (qecount--) {
					qelem = rig_queue_get(sortq[i]);
					str2[strindex] = str1[(size_t)qelem];
					strlen2[strindex] = strlen1[(size_t)qelem];
					strindex++;
				}
			}
		}

		SWAP_VAR(uint8_t **, str1, str2);
		SWAP_VAR(size_t *, strlen1, strlen2);
	}

	if (sortlength & ((size_t)1)) {
		for (i = 0; i < strcount; i++) {
			str2[i] = str1[i];
			strlen2[i] = strlen1[i];
		}
	}

	for (i = 0; i < UINT8_MAX + 1; i++) {
		if (sortq[i] != NULL) {
			rig_queue_destroy(&sortq[i]);
		}
	}
}

#endif /* STR_OPS_C */
