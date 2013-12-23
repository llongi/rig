#include <stdlib.h>
#include <stdio.h>

static void constructor1(void) __attribute__ ((__constructor__ (200)));
static void destructor1(void) __attribute__ ((__destructor__ (200)));
static void constructor2(void) __attribute__ ((__constructor__ (300)));
static void destructor2(void) __attribute__ ((__destructor__ (300)));
int foo(void);

static void constructor1(void) {
	printf("constructor 1 called!\n");
}

static void destructor1(void) {
	printf("destructor 1 called!\n");
}

static void constructor2(void) {
	printf("constructor 2 called!\n");
}

static void destructor2(void) {
	printf("destructor 2 called!\n");
}

int foo(void) {
	printf("foo was called!\n");

	return (1234);
}
