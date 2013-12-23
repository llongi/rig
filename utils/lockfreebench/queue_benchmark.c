#include "commonbench.h"

void *dts_init(size_t capacity) {
	return (rig_queue_init(capacity, RIG_QUEUE_NOCOUNT));
}

void dts_destroy(void **dts) {
	rig_queue_destroy((RIG_QUEUE *)dts);
}

void *thr_function(void *dts) {
	for (size_t i = 1; i < 1000000; i++) {
		rig_queue_put(dts, (void *)i);
	}

	for (size_t i = 0; i < 500000; i++) {
		rig_queue_get(dts);
	}

	for (size_t i = 0; i < 500000; i++) {
		rig_queue_peek(dts);
	}

	for (size_t i = 1; i < 500000; i++) {
		rig_queue_put(dts, (void *)i);
	}

	for (size_t i = 1; i < 1000000; i++) {
		rig_queue_put(dts, (void *)i);
		rig_queue_get(dts);
		rig_queue_peek(dts);
	}

	for (size_t i = 0; i < 1000000; i++) {
		rig_queue_get(dts);
	}

	return (NULL);
}
