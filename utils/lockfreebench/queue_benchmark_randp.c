#include "commonbench.h"

void *dts_init(size_t capacity) {
	return (rig_queue_init(capacity, RIG_QUEUE_NOCOUNT));
}

void dts_destroy(void **dts) {
	rig_queue_destroy((RIG_QUEUE *)dts);
}

struct ops {
	size_t TOTAL_OPS;
	size_t PUT, putp;
	size_t LOOK, lookp;
	size_t GET, getp;
};

void *thr_function(void *dts) {
	unsigned int seed = 0x1234 * (rig_thread_id() % NUM_THREADS);
	size_t r, memp;

	struct ops th_ops = {5000000, .PUT = 40, .LOOK = 10, .GET = 50};
	th_ops.TOTAL_OPS /= 100;

	while (th_ops.TOTAL_OPS--) {
		th_ops.putp = th_ops.lookp = th_ops.getp = 0;

		while (true) {
			for (r = ((size_t)rand_r(&seed) % 10) + 1; th_ops.putp < th_ops.PUT && r > 0; th_ops.putp++, r--) {
				memp = ((size_t)rand_r(&seed)) + 1;
				rig_queue_put(dts, (void *)memp);
			}

			for (r = ((size_t)rand_r(&seed) % 10) + 1; th_ops.lookp < th_ops.LOOK && r > 0; th_ops.lookp++, r--) {
				rig_queue_peek(dts);
			}

			for (r = ((size_t)rand_r(&seed) % 10) + 1; th_ops.getp < th_ops.GET && r > 0; th_ops.getp++, r--) {
				rig_queue_get(dts);
			}

			if (th_ops.putp  == th_ops.PUT
			 && th_ops.lookp == th_ops.LOOK
			 && th_ops.getp  == th_ops.GET) {
				break;
			}
		}
	}

	return (NULL);
}
