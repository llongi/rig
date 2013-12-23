#include <stdlib.h>
#include <stdio.h>
#include "tables/utf8_casefold.h"
#include "tables/utf8_lower_upper.h"
#include "tables/utf8_upper_lower.h"

#define UTF8_SEARCH_TABLE(tab, hkey, ret_len, ret) { \
	size_t l = 0; \
	size_t h = (sizeof(tab) / sizeof(*tab)) - 1; \
	while (l < h) { \
		size_t m = (l + h) >> 1; \
		if (tab[m].key < hkey) { \
			l = m + 1; \
		} \
		else { \
			h = m; \
		} \
	} \
	if ((l < (sizeof(tab) / sizeof(*tab))) && (tab[l].key == hkey)) { \
		ret_len = sizeof(tab[l].val); \
		ret = tab[l].val; \
	} \
	else { \
		ret_len = 0; \
		ret = NULL; \
	} \
}

#define UTF8_SEARCH_TABLE_DBL(tab, hkey, ret_len, ret) { \
	size_t l = 0; \
	size_t h = (sizeof(tab) / sizeof(*tab)) - 1; \
	while (l < h) { \
		size_t m = (l + h) >> 1; \
		if (tab[m].key < hkey) { \
			l = m + 1; \
		} \
		else { \
			h = m; \
		} \
	} \
	if ((l < (sizeof(tab) / sizeof(*tab))) && (tab[l].key == hkey)) { \
		ret_len = tab[l].val_len; \
		ret = tab[l].val; \
	} \
	else { \
		ret_len = 0; \
		ret = NULL; \
	} \
}

int main(void) {
	uint32_t c = 0xC3A7;

	uint8_t lower_len, lower_dbl_len;
	const uint8_t *lower, *lower_dbl;

	UTF8_SEARCH_TABLE(UTF8_LOWER_UPPER_2B, c, lower_len, lower);
	UTF8_SEARCH_TABLE_DBL(UTF8_LOWER_UPPER_2B_DBL, c, lower_dbl_len, lower_dbl);

	printf("length: %u, content: %.*s\n", lower_len, lower_len, lower);
	printf("length: %u, content: %.*s\n", lower_dbl_len, lower_dbl_len, lower_dbl);

	return 0;
}
