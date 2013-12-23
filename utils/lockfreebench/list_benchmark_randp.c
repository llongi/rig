#include "commonbench.h"

void *dts_init(size_t capacity) {
	return (rig_list_init(capacity, RIG_LIST_NOCOUNT, NULL, NULL));
}

void dts_destroy(void **dts) {
	rig_list_destroy((RIG_LIST *)dts);
}

#define NUM_RANGE 125000

struct ops {
	size_t TOTAL_OPS;
	size_t ADD, addp;
	size_t FIND, findp;
	size_t DEL, delp;
	size_t LOOK, lookp;
	size_t GET, getp;
};

void *thr_function(void *dts) {
	unsigned int seed = 0x1234 * (rig_thread_id() % NUM_THREADS);
	size_t r, memp;

	struct ops th_ops = {250000, .ADD = 30, .FIND = 40, .DEL = 15, .LOOK = 5, .GET = 10};
	th_ops.TOTAL_OPS /= 100;

	while (th_ops.TOTAL_OPS--) {
		th_ops.addp = th_ops.findp = th_ops.delp = th_ops.lookp = th_ops.getp = 0;

		while (true) {
			for (r = ((size_t)rand_r(&seed) % 10) + 1; th_ops.addp < th_ops.ADD && r > 0; th_ops.addp++, r--) {
				memp = ((size_t)rand_r(&seed) % NUM_RANGE) + 1;
				rig_list_add(dts, (void *)memp);
			}

			for (r = ((size_t)rand_r(&seed) % 10) + 1; th_ops.findp < th_ops.FIND && r > 0; th_ops.findp++, r--) {
				memp = ((size_t)rand_r(&seed) % NUM_RANGE) + 1;
				rig_list_find(dts, (void *)memp);
			}

			for (r = ((size_t)rand_r(&seed) % 10) + 1; th_ops.delp < th_ops.DEL && r > 0; th_ops.delp++, r--) {
				memp = ((size_t)rand_r(&seed) % NUM_RANGE) + 1;
				rig_list_del(dts, (void *)memp);
			}

			for (r = ((size_t)rand_r(&seed) % 10) + 1; th_ops.lookp < th_ops.LOOK && r > 0; th_ops.lookp++, r--) {
				rig_list_peek(dts);
			}

			for (r = ((size_t)rand_r(&seed) % 10) + 1; th_ops.getp < th_ops.GET && r > 0; th_ops.getp++, r--) {
				rig_list_get(dts);
			}

			if (th_ops.addp  == th_ops.ADD
			 && th_ops.findp == th_ops.FIND
			 && th_ops.delp  == th_ops.DEL
			 && th_ops.lookp == th_ops.LOOK
			 && th_ops.getp  == th_ops.GET) {
				break;
			}
		}
	}

	return (NULL);
}
