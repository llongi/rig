#include <rig.h>
#include <stdio.h>

void *thr_function(void *dts);

void *thr_function(void *dts) {
	for (size_t i = 0; i < 50; i++) {
		rig_stack_push(dts, &i);
	}

	return (NULL);
}

int main(void) {
	RIG_STACK dts = rig_stack_init(0, sizeof(size_t));

	RIG_THREAD thr = rig_thread_init(0, 8);
	rig_thread_start(thr, &thr_function, dts);
	rig_thread_join(thr, NULL);
	rig_thread_destroy(&thr);

	RIG_STACK_ITER iter = rig_stack_iter_begin(dts);

	size_t *elem = NULL;
	while ((elem = rig_stack_iter_next(iter)) != NULL) {
		printf("%zu\n", *elem);
	}

	rig_stack_iter_end(&iter);

	rig_stack_destroy(&dts);

	return (0);
}
