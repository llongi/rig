/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_counter.c 1110 2012-10-20 20:32:28Z llongi $
 */

#include "rig_internal.h"
#include <atomic_ops.h>

struct rig_counter {
	atomic_ops_uint counter CACHELINE_ALIGNED;
	size_t maximum_value; // read-only value
};


/**
 * Initialize an atomic counter object, which can keep track of values between
 * 0 and a defined upper limit (with SIZE_MAX as the default and biggest value).
 *
 * @param initial_value
 *     initial value from which to start counting, must be between 0 and max_val
 * @param maximum_value
 *     biggest value the counter can hold, between 1 and SIZE_MAX, as a
 *     convenience, 0 means to automatically use the biggest possible value
 *
 * @return
 *     counter object data, NULL on error.
 *     On error, the following error codes are set:
 *     - EINVAL (invalid initial value passed)
 *     - ENOMEM (insufficient memory)
 */
RIG_COUNTER rig_counter_init(size_t initial_value, size_t maximum_value) {
	// max_val == 0 means to use the maximum possible size
	if (maximum_value == 0) {
		maximum_value = SIZE_MAX;
	}

	if (initial_value > maximum_value) {
		ERRET(EINVAL, NULL);
	}

	RIG_COUNTER c = rig_mem_alloc_aligned(sizeof(*c), 0, CACHELINE_SIZE, RIG_MEM_ALLOC_ALIGN_PAD);
	NULLCHECK_ERRET(c, ENOMEM, NULL);

	atomic_ops_uint_store(&c->counter, initial_value, ATOMIC_OPS_FENCE_NONE);
	c->maximum_value = maximum_value;

	atomic_ops_fence(ATOMIC_OPS_FENCE_RELEASE);

	return (c);
}

/**
 * Destroy specified atomic counter object and set pointer to NULL.
 *
 * @param *c
 *     pointer to counter object data
 */
void rig_counter_destroy(RIG_COUNTER *c) {
	NULLCHECK_EXIT(c);

	// If a valid pointer already contains NULL, nothing to do!
	if (*c == NULL) {
		return;
	}

	rig_mem_free_aligned(*c);
	*c = NULL;
}

/**
 * Get the upper limit (maximal possible value) of this counter.
 *
 * @param c
 *     counter object data
 *
 * @return
 *     maximal value of counter
 */
size_t rig_counter_get_max(RIG_COUNTER c) {
	NULLCHECK_EXIT(c);

	return (c->maximum_value);
}

/**
 * Get the current value of this counter.
 *
 * @param c
 *     counter object data
 *
 * @return
 *     current value of counter
 */
size_t rig_counter_get(RIG_COUNTER c) {
	NULLCHECK_EXIT(c);

	return (atomic_ops_uint_load(&c->counter, ATOMIC_OPS_FENCE_ACQUIRE));
}

/**
 * Get the current value of this counter and set a new value.
 *
 * @param c
 *     counter object data
 * @param new_value
 *     new value to store in the counter, between 0 and the maximal allowed value
 * @param *old_value
 *     pointer to memory in which to store the old value of the counter, before
 *     the store of new_val, can be NULL, which reduces this to a simple set;
 *     'bool rig_counter_set(RIG_COUNTER c, size_t new_value)' is provided for
 *     convenience as an alias
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - ERANGE (new value bigger than counter's range)
 */
bool rig_counter_get_and_set(RIG_COUNTER c, size_t new_value, size_t *old_value) {
	NULLCHECK_EXIT(c);

	if (new_value > c->maximum_value) {
		if (old_value != NULL) {
			*old_value = atomic_ops_uint_load(&c->counter, ATOMIC_OPS_FENCE_ACQUIRE);
		}

		ERRET(ERANGE, false);
	}

	if (old_value != NULL) {
		*old_value = atomic_ops_uint_swap(&c->counter, new_value, ATOMIC_OPS_FENCE_ACQUIRE);
	}
	else {
		atomic_ops_uint_store(&c->counter, new_value, ATOMIC_OPS_FENCE_ACQUIRE);
	}

	return (true);
}

/**
 * Increment the current value of this counter by one.
 *
 * @param c
 *     counter object data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - ERANGE (increment would result in value out of counter's range)
 */
bool rig_counter_inc(RIG_COUNTER c) {
	NULLCHECK_EXIT(c);

	size_t count;

	while (true) {
		count = atomic_ops_uint_load(&c->counter, ATOMIC_OPS_FENCE_NONE);

		if (count == c->maximum_value) {
			ERRET(ERANGE, false);
		}

		if (atomic_ops_uint_cas(&c->counter, count, count + 1, ATOMIC_OPS_FENCE_FULL)) {
			break;
		}
	}

	return (true);
}

/**
 * Decrement the current value of this counter by one.
 *
 * @param c
 *     counter object data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - ERANGE (decrement would result in value out of counter's range)
 */
bool rig_counter_dec(RIG_COUNTER c) {
	NULLCHECK_EXIT(c);

	size_t count;

	while (true) {
		count = atomic_ops_uint_load(&c->counter, ATOMIC_OPS_FENCE_NONE);

		if (count == 0) {
			ERRET(ERANGE, false);
		}

		if (atomic_ops_uint_cas(&c->counter, count, count - 1, ATOMIC_OPS_FENCE_FULL)) {
			break;
		}
	}

	return (true);
}

/**
 * Decrement the current value of this counter by one and test if the new value
 * is now equal to zero.
 *
 * @param c
 *     counter object data
 *
 * @return
 *     boolean indicating success.
 *     Note that false can also indicate that the decrement was successful, but
 *     the new value is not zero. In that case, errno is set explicitly to 0,
 *     and should be checked accordingly.
 *     On error, the following error codes are set:
 *     - ERANGE (decrement would result in value out of counter's range)
 */
bool rig_counter_dec_and_test(RIG_COUNTER c) {
	NULLCHECK_EXIT(c);

	size_t count;

	while (true) {
		count = atomic_ops_uint_load(&c->counter, ATOMIC_OPS_FENCE_NONE);

		if (count == 0) {
			ERRET(ERANGE, false);
		}

		if (atomic_ops_uint_cas(&c->counter, count, count - 1, ATOMIC_OPS_FENCE_FULL)) {
			break;
		}
	}

	errno = 0;
	return (count == 1);
}

/**
 * Get the current value of this counter and add a number to it.
 *
 * @param c
 *     counter object data
 * @param add_to_value
 *     value to add to the counter, between SSIZE_MIN and SSIZE_MAX (signed)
 * @param *old_value
 *     pointer to memory in which to store the old value of the counter, before
 *     the addition of add_val, can be NULL, which reduces this to a simple add;
 *     'bool rig_counter_add(RIG_COUNTER c, ssize_t add_to_value)' is provided for
 *     convenience as an alias
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - ERANGE (addition would result in value out of counter's range)
 */
bool rig_counter_get_and_add(RIG_COUNTER c, ssize_t add_to_value, size_t *old_value) {
	NULLCHECK_EXIT(c);

	// Nothing to add, so we don't change the value at all, and just put
	// the current value into old_value if requested.
	if (add_to_value == 0) {
		if (old_value != NULL) {
			*old_value = atomic_ops_uint_load(&c->counter, ATOMIC_OPS_FENCE_ACQUIRE);
		}

		return (true);
	}

	// Check that, if add_to_value is positive, it isn't bigger than the allowed
	// maximum, which would clearly be a range error, and do this outside the
	// loop, to avoid superfluous repetition (doesn't depend on loop variables).
	if ((add_to_value > 0) && ((size_t)add_to_value > c->maximum_value)) {
		if (old_value != NULL) {
			*old_value = atomic_ops_uint_load(&c->counter, ATOMIC_OPS_FENCE_ACQUIRE);
		}

		ERRET(ERANGE, false);
	}

	size_t count;

	while (true) {
		count = atomic_ops_uint_load(&c->counter, ATOMIC_OPS_FENCE_NONE);

		if ((add_to_value > 0) ? (count > c->maximum_value - (size_t)add_to_value) : (count < (size_t)-add_to_value)) {
			if (old_value != NULL) {
				*old_value = count;
			}

			ERRET(ERANGE, false);
		}

		if (atomic_ops_uint_cas(&c->counter, count, count + (size_t)add_to_value, ATOMIC_OPS_FENCE_FULL)) {
			break;
		}
	}

	if (old_value != NULL) {
		*old_value = count;
	}

	return (true);
}
