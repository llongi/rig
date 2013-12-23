#include "commonbench.h"

void *dts_init(size_t capacity) {
	return (rig_stack_init(capacity, RIG_STACK_NOCOUNT));
}

void dts_destroy(void **dts) {
	rig_stack_destroy((RIG_STACK *)dts);
}

void *thr_function(void *dts) {
	for (size_t i = 1; i < 1000000; i++) {
		rig_stack_push(dts, (void *)i);
	}

	for (size_t i = 0; i < 500000; i++) {
		rig_stack_pop(dts);
	}

	for (size_t i = 0; i < 500000; i++) {
		rig_stack_peek(dts);
	}

	for (size_t i = 1; i < 500000; i++) {
		rig_stack_push(dts, (void *)i);
	}

	for (size_t i = 1; i < 1000000; i++) {
		rig_stack_push(dts, (void *)i);
		rig_stack_pop(dts);
		rig_stack_peek(dts);
	}

	for (size_t i = 0; i < 1000000; i++) {
		rig_stack_pop(dts);
	}

	return (NULL);
}
