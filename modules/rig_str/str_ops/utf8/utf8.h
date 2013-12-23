#include "utf8_casefold.h"
#include "utf8_lower_upper.h"
#include "utf8_upper_lower.h"

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
