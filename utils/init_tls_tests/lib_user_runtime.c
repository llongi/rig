#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

int main(void) {
	printf("this is main starting!\n");

	void *constdest = dlopen("./libconstdest.so", RTLD_NOW);

#if defined(CALL_FOO) && CALL_FOO == 1
	typedef int (*foo_func_type)(void);

	void *foosym = dlsym(constdest, "foo");
	foo_func_type foo = *((foo_func_type *)&foosym);

	printf("foo returned: %i\n", (*foo)());
#endif

	dlclose(constdest);

	printf("this is main ending!\n");

	return (EXIT_SUCCESS);
}
