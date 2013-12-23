#include "commonbench.h"

void *dts_init(size_t capacity) {
	return (rig_stack_init(capacity, RIG_STACK_NOCOUNT));
}

void dts_destroy(void **dts) {
	rig_stack_destroy((RIG_STACK *)dts);
}

struct ops {
	size_t TOTAL_OPS;
	size_t PUSH, pushp;
	size_t LOOK, lookp;
	size_t POP, popp;
};

void *thr_function(void *dts) {
	unsigned int seed = 0x1234 * (rig_thread_id() % NUM_THREADS);
	size_t r, memp;

	struct ops th_ops = {5000000, .PUSH = 44, .LOOK = 10, .POP = 46};
	th_ops.TOTAL_OPS /= 100;

	while (th_ops.TOTAL_OPS--) {
		th_ops.pushp = th_ops.lookp = th_ops.popp = 0;

		while (true) {
			for (r = ((size_t)rand_r(&seed) % 10) + 1; th_ops.pushp < th_ops.PUSH && r > 0; th_ops.pushp++, r--) {
				memp = ((size_t)rand_r(&seed)) + 1;
				rig_stack_push(dts, (void *)memp);
			}

			for (r = ((size_t)rand_r(&seed) % 10) + 1; th_ops.lookp < th_ops.LOOK && r > 0; th_ops.lookp++, r--) {
				rig_stack_peek(dts);
			}

			for (r = ((size_t)rand_r(&seed) % 10) + 1; th_ops.popp < th_ops.POP && r > 0; th_ops.popp++, r--) {
				rig_stack_pop(dts);
			}

			if (th_ops.pushp == th_ops.PUSH
			 && th_ops.lookp == th_ops.LOOK
			 && th_ops.popp  == th_ops.POP) {
				break;
			}
		}
	}

	return (NULL);
}
