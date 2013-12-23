#include <stdlib.h>
#include <stdio.h>

#if defined(CALL_FOO) && CALL_FOO == 1
int foo(void);
#endif

int main(void) {
	printf("this is main starting!\n");

#if defined(CALL_FOO) && CALL_FOO == 1
	printf("foo returned: %i\n", foo());
#endif

	printf("this is main ending!\n");

	return (EXIT_SUCCESS);
}
