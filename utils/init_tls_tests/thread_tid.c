#include <rig.h>
#include <stdio.h>

void *thr_func(void *arg);

void *thr_func(void *arg) {
	printf("THR id: %zu\n", rig_thread_id());
	printf("THR tid: %zu\n", rig_thread_tid());
	printf("THR is main: %i\n", rig_thread_is_main());

	return (NULL);
}

int main(void) {
	// main() at start
	printf("MAIN pid: %zu\n", rig_misc_pid());
	printf("MAIN id: %zu\n", rig_thread_id());
	printf("MAIN tid: %zu\n", rig_thread_tid());
	printf("MAIN is main: %i\n", rig_thread_is_main());

	RIG_THREAD thr = rig_thread_init(0, NUM_THREADS);

	rig_thread_start(thr, &thr_func, NULL);

	rig_thread_join(thr, NULL);

	rig_thread_destroy(&thr);

	// main() at finish
	printf("MAINF pid: %zu\n", rig_misc_pid());
	printf("MAINF id: %zu\n", rig_thread_id());
	printf("MAINF tid: %zu\n", rig_thread_tid());
	printf("MAINF is main: %i\n", rig_thread_is_main());

	return (EXIT_SUCCESS);
}
