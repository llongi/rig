#include "commonbench.h"

void *dts_init(size_t capacity) {
	return (rig_list_init(capacity, RIG_LIST_NOCOUNT, NULL, NULL));
}

void dts_destroy(void **dts) {
	rig_list_destroy((RIG_LIST *)dts);
}

void *thr_function(void *dts) {
	for (size_t i = 1; i < 10000; i++) {
		rig_list_add(dts, (void *)i);
	}

	for (size_t i = 0; i < 5000; i++) {
		rig_list_get(dts);
	}

	for (size_t i = 0; i < 5000; i++) {
		rig_list_peek(dts);
	}

	for (size_t i = 1; i < 5000; i++) {
		rig_list_add(dts, (void *)i);
	}

	for (size_t i = 4500; i < 8500; i++) {
		rig_list_add(dts, (void *)i);
		rig_list_find(dts, (void *)i);
		rig_list_del(dts, (void *)i);
		rig_list_peek(dts);
	}

	for (size_t i = 1; i < 5000; i++) {
		rig_list_del(dts, (void *)i);
	}

	for (size_t rep = 0; rep < 5; rep++) {
		for (size_t i = 500; i < 8500; i++) {
			rig_list_find(dts, (void *)i);
		}

		for (size_t i = 11000; i > 7000; i--) {
			rig_list_find(dts, (void *)i);
		}
	}

	for (size_t i = 2000; i > 0; i--) {
		rig_list_del(dts, (void *)i);
	}

	for (size_t i = 1; i < 10000; i++) {
		rig_list_del(dts, (void *)i);
	}

	return (NULL);
}
