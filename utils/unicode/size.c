#include <stdio.h>
#include "tables/utf8_casefold.h"
#include "tables/utf8_lower_upper.h"
#include "tables/utf8_upper_lower.h"

int main(void) {
	printf("    CF_2: %6zu %6zu %6zu\n", sizeof(UTF8_CASEFOLD_2B), sizeof(*UTF8_CASEFOLD_2B), sizeof(UTF8_CASEFOLD_2B) / sizeof(*UTF8_CASEFOLD_2B));
	printf("    CF_3: %6zu %6zu %6zu\n", sizeof(UTF8_CASEFOLD_3B), sizeof(*UTF8_CASEFOLD_3B), sizeof(UTF8_CASEFOLD_3B) / sizeof(*UTF8_CASEFOLD_3B));
	printf("    CF_4: %6zu %6zu %6zu\n", sizeof(UTF8_CASEFOLD_4B), sizeof(*UTF8_CASEFOLD_4B), sizeof(UTF8_CASEFOLD_4B) / sizeof(*UTF8_CASEFOLD_4B));
	printf("CF_2_DBL: %6zu %6zu %6zu\n", sizeof(UTF8_CASEFOLD_2B_DBL), sizeof(*UTF8_CASEFOLD_2B_DBL), sizeof(UTF8_CASEFOLD_2B_DBL) / sizeof(*UTF8_CASEFOLD_2B_DBL));
	printf("CF_3_DBL: %6zu %6zu %6zu\n", sizeof(UTF8_CASEFOLD_3B_DBL), sizeof(*UTF8_CASEFOLD_3B_DBL), sizeof(UTF8_CASEFOLD_3B_DBL) / sizeof(*UTF8_CASEFOLD_3B_DBL));
	//printf("CF_4_DBL: %6zu %6zu %6zu\n", sizeof(UTF8_CASEFOLD_4B_DBL), sizeof(*UTF8_CASEFOLD_4B_DBL), sizeof(UTF8_CASEFOLD_4B_DBL) / sizeof(*UTF8_CASEFOLD_4B_DBL));
	printf("\n");
	printf("    LU_2: %6zu %6zu %6zu\n", sizeof(UTF8_LOWER_UPPER_2B), sizeof(*UTF8_LOWER_UPPER_2B), sizeof(UTF8_LOWER_UPPER_2B) / sizeof(*UTF8_LOWER_UPPER_2B));
	printf("    LU_3: %6zu %6zu %6zu\n", sizeof(UTF8_LOWER_UPPER_3B), sizeof(*UTF8_LOWER_UPPER_3B), sizeof(UTF8_LOWER_UPPER_3B) / sizeof(*UTF8_LOWER_UPPER_3B));
	printf("    LU_4: %6zu %6zu %6zu\n", sizeof(UTF8_LOWER_UPPER_4B), sizeof(*UTF8_LOWER_UPPER_4B), sizeof(UTF8_LOWER_UPPER_4B) / sizeof(*UTF8_LOWER_UPPER_4B));
	printf("LU_2_DBL: %6zu %6zu %6zu\n", sizeof(UTF8_LOWER_UPPER_2B_DBL), sizeof(*UTF8_LOWER_UPPER_2B_DBL), sizeof(UTF8_LOWER_UPPER_2B_DBL) / sizeof(*UTF8_LOWER_UPPER_2B_DBL));
	printf("LU_3_DBL: %6zu %6zu %6zu\n", sizeof(UTF8_LOWER_UPPER_3B_DBL), sizeof(*UTF8_LOWER_UPPER_3B_DBL), sizeof(UTF8_LOWER_UPPER_3B_DBL) / sizeof(*UTF8_LOWER_UPPER_3B_DBL));
	//printf("LU_4_DBL: %6zu %6zu %6zu\n", sizeof(UTF8_LOWER_UPPER_4B_DBL), sizeof(*UTF8_LOWER_UPPER_4B_DBL), sizeof(UTF8_LOWER_UPPER_4B_DBL) / sizeof(*UTF8_LOWER_UPPER_4B_DBL));
	printf("\n");
	printf("    UL_2: %6zu %6zu %6zu\n", sizeof(UTF8_UPPER_LOWER_2B), sizeof(*UTF8_UPPER_LOWER_2B), sizeof(UTF8_UPPER_LOWER_2B) / sizeof(*UTF8_UPPER_LOWER_2B));
	printf("    UL_3: %6zu %6zu %6zu\n", sizeof(UTF8_UPPER_LOWER_3B), sizeof(*UTF8_UPPER_LOWER_3B), sizeof(UTF8_UPPER_LOWER_3B) / sizeof(*UTF8_UPPER_LOWER_3B));
	printf("    UL_4: %6zu %6zu %6zu\n", sizeof(UTF8_UPPER_LOWER_4B), sizeof(*UTF8_UPPER_LOWER_4B), sizeof(UTF8_UPPER_LOWER_4B) / sizeof(*UTF8_UPPER_LOWER_4B));
	printf("UL_2_DBL: %6zu %6zu %6zu\n", sizeof(UTF8_UPPER_LOWER_2B_DBL), sizeof(*UTF8_UPPER_LOWER_2B_DBL), sizeof(UTF8_UPPER_LOWER_2B_DBL) / sizeof(*UTF8_UPPER_LOWER_2B_DBL));
	printf("UL_3_DBL: %6zu %6zu %6zu\n", sizeof(UTF8_UPPER_LOWER_3B_DBL), sizeof(*UTF8_UPPER_LOWER_3B_DBL), sizeof(UTF8_UPPER_LOWER_3B_DBL) / sizeof(*UTF8_UPPER_LOWER_3B_DBL));
	//printf("UL_4_DBL: %6zu %6zu %6zu\n", sizeof(UTF8_UPPER_LOWER_4B_DBL), sizeof(*UTF8_UPPER_LOWER_4B_DBL), sizeof(UTF8_UPPER_LOWER_4B_DBL) / sizeof(*UTF8_UPPER_LOWER_4B_DBL));
	printf("\n");
	printf("   TOTAL: %6zu\n",
		  sizeof(UTF8_CASEFOLD_2B) + sizeof(UTF8_CASEFOLD_3B) + sizeof(UTF8_CASEFOLD_4B)
		+ sizeof(UTF8_CASEFOLD_2B_DBL) + sizeof(UTF8_CASEFOLD_3B_DBL)// + sizeof(UTF8_CASEFOLD_4B_DBL)
		+ sizeof(UTF8_LOWER_UPPER_2B) + sizeof(UTF8_LOWER_UPPER_3B) + sizeof(UTF8_LOWER_UPPER_4B)
		+ sizeof(UTF8_LOWER_UPPER_2B_DBL) + sizeof(UTF8_LOWER_UPPER_3B_DBL) //+ sizeof(UTF8_LOWER_UPPER_4B_DBL)
		+ sizeof(UTF8_UPPER_LOWER_2B) + sizeof(UTF8_UPPER_LOWER_3B) + sizeof(UTF8_UPPER_LOWER_4B)
		+ sizeof(UTF8_UPPER_LOWER_2B_DBL) + sizeof(UTF8_UPPER_LOWER_3B_DBL)// + sizeof(UTF8_UPPER_LOWER_4B_DBL)
	);
	return 0;
}
