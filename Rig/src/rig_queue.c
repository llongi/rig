/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_queue.c 1139 2012-11-14 19:48:44Z llongi $
 */

#include "rig_internal.h"
#include <atomic_ops.h>

/*
 * Rig Queue Data Definitions
 */
#if RIG_QUEUE_SMR_TYPE == 1
	#define SMR_HP_HEAD 0
	#define SMR_HP_HNEXT 1
	#define SMR_HP_TAIL 0
	// HEAD and TAIL are never used together!
	// Only at most two distinct HPs are ever used.
#endif

/** Types */
typedef struct NodeStruct *Node;

/** Structures */
struct rig_queue {
	CACHELINE_ALONE(atomic_ops_ptr, head);
	CACHELINE_ALONE(atomic_ops_ptr, tail);
	RIG_COUNTER count;
	RIG_COUNTER refcount;
};

struct NodeStruct {
#if !defined(RIG_QUEUE_PRECISE_ITERATOR)
	atomic_ops_ptr next;
#else
	atomic_ops_flagptr next;
#endif
	void *data;
};


/*
 * Rig Queue Implementation
 */

/**
 * Initialize a lock-free queue (FIFO data structure).
 * This data structure is intended for low-level, high-performance purposes, and thus only accepts and returns memory
 * addresses. Their content and its persistence is the responsibility of the application programmer.
 *
 * @param capacity
 *     maximum number of elements the queue can contain, 0 means unlimited
 * @param flags
 *     flags to modify queue behavior, the following are currently supported:
 *     - RIG_QUEUE_NOCOUNT (do not count elements, capacity is not enforced)
 *
 * @return
 *     queue pointer, NULL on error.
 *     On error, the following error codes are set:
 *     - EINVAL (invalid arguments passed)
 *     - ENOMEM (insufficient memory)
 */
RIG_QUEUE rig_queue_init(size_t capacity, uint16_t flags) {
	CHECK_PERMITTED_FLAGS(flags, RIG_QUEUE_NOCOUNT);

	// Allocate memory for the queue
	RIG_QUEUE q = rig_mem_alloc_aligned(sizeof(*q), 0, CACHELINE_SIZE, 0);
	NULLCHECK_ERRET(q, ENOMEM, NULL);

	// Allocate memory for the sentinel node
	Node sentinel = rig_mem_alloc(sizeof(*sentinel), 0);
	NULLCHECK_ERRET_CLEANUP(sentinel, ENOMEM, NULL, rig_mem_free_aligned(q));

	// Initialize the counters
	RIG_COUNTER refcount = rig_counter_init(1, 0);
	NULLCHECK_ERRET_CLEANUP(refcount, ENOMEM, NULL, rig_mem_free_aligned(q); rig_mem_free(sentinel));

	RIG_COUNTER count = NULL;
	if (!TEST_BITFIELD(flags, RIG_QUEUE_NOCOUNT)) { // Support not counting elements
		count = rig_counter_init(0, capacity);
		NULLCHECK_ERRET_CLEANUP(count, ENOMEM, NULL,
			rig_mem_free_aligned(q); rig_mem_free(sentinel); rig_counter_destroy(&refcount));
	}

	// Initialize the needed values
#if !defined(RIG_QUEUE_PRECISE_ITERATOR)
	atomic_ops_ptr_store(&sentinel->next, NULL, ATOMIC_OPS_FENCE_NONE);
#else
	atomic_ops_flagptr_store(&sentinel->next, NULL, false, ATOMIC_OPS_FENCE_NONE);
#endif
	sentinel->data = NULL;

	atomic_ops_ptr_store(&q->head, sentinel, ATOMIC_OPS_FENCE_NONE);
	atomic_ops_ptr_store(&q->tail, sentinel, ATOMIC_OPS_FENCE_NONE);
	q->count = count;
	q->refcount = refcount;

	atomic_ops_fence(ATOMIC_OPS_FENCE_FULL);

	return (q);
}

/**
 * Get a new reference to the queue, increasing the reference count by one.
 *
 * @param q
 *     queue pointer
 *
 * @return
 *     queue pointer
 */
RIG_QUEUE rig_queue_newref(RIG_QUEUE q) {
	NULLCHECK_EXIT(q);

	rig_acheck_msg(rig_counter_inc(q->refcount), "more references than physically possible");

	return (q);
}

/**
 * Destroy specified queue reference and set pointer to NULL.
 * If reference count reaches zero, proceed to full destruction.
 *
 * @param *q
 *     pointer to queue pointer
 */
void rig_queue_destroy(RIG_QUEUE *q) {
	NULLCHECK_EXIT(q);
	NULLCHECK_EXIT(*q);

	if (rig_counter_dec_and_test((*q)->refcount)) {
		// Traverse the list and remove all nodes (sentinel included)
		Node curr = atomic_ops_ptr_load(&(*q)->head, ATOMIC_OPS_FENCE_NONE), succ = NULL;

		while (curr != NULL) {
#if !defined(RIG_QUEUE_PRECISE_ITERATOR)
			succ = atomic_ops_ptr_load(&curr->next, ATOMIC_OPS_FENCE_NONE);
#else
			succ = atomic_ops_flagptr_load(&curr->next, NULL, ATOMIC_OPS_FENCE_NONE);
#endif
			// Free directly here, as there are no shared references anymore around, no SMR is required!
			rig_mem_free(curr);

			curr = succ;
		}

		// Destroy counters and queue
		rig_counter_destroy(&(*q)->refcount);
		rig_counter_destroy(&(*q)->count);
		rig_mem_free_aligned(*q);
	}
	else {
		rig_acheck_msg(errno == 0, "reference count already zero, uncounted references exist");
	}

	*q = NULL;

	atomic_ops_fence(ATOMIC_OPS_FENCE_FULL);

#if RIG_QUEUE_SMR_TYPE == 1
	// Explicit SMR HP scan
	rig_smr_hp_mem_scan();
#endif
}

/**
 * Clear the queue by getting elements from it until it's empty.
 *
 * @param q
 *     queue pointer
 */
void rig_queue_clear(RIG_QUEUE q) {
	NULLCHECK_EXIT(q);

	while (rig_queue_get(q) != NULL) { ; }
}

/**
 * Add a new item at the end of the queue.
 *
 * @param q
 *     queue pointer
 * @param item
 *     data pointer (memory management caller responsibility)
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EINVAL (NULL pointer passed, invalid value)
 *     - ENOMEM (insufficient memory to add the new item)
 *     - EXFULL (maximum capacity reached)
 */
bool rig_queue_put(RIG_QUEUE q, void *item) {
	NULLCHECK_EXIT(q);

	// Allocate memory for the new element
	Node node = rig_mem_alloc(sizeof(*node), 0);
	NULLCHECK_ERRET(node, ENOMEM, false);

	// Check if there's still place for the new element
	if ((q->count != NULL) && (!rig_counter_inc(q->count))) {
		// Full queue
		ERRET_CLEANUP(EXFULL, false, rig_mem_free(node));
	}

	// Set the content of the new element
#if !defined(RIG_QUEUE_PRECISE_ITERATOR)
	atomic_ops_ptr_store(&node->next, NULL, ATOMIC_OPS_FENCE_NONE);
#else
	atomic_ops_flagptr_store(&node->next, NULL, false, ATOMIC_OPS_FENCE_NONE);
#endif
	node->data = item;

	Node tail = NULL, next = NULL;

#if RIG_QUEUE_SMR_TYPE == 1
	RIG_SMR_HP_Record hprec = rig_smr_hp_record_get();
#endif
#if RIG_QUEUE_SMR_TYPE == 2
	rig_smr_epoch_critical_enter();
#endif

	while (true) {
		tail = atomic_ops_ptr_load(&q->tail, ATOMIC_OPS_FENCE_ACQUIRE);
#if RIG_QUEUE_SMR_TYPE == 1
		rig_smr_hp_set(hprec, SMR_HP_TAIL, tail);
		if (tail != atomic_ops_ptr_load(&q->tail, ATOMIC_OPS_FENCE_ACQUIRE)) {
			continue;
		}
#endif

#if !defined(RIG_QUEUE_PRECISE_ITERATOR)
		next = atomic_ops_ptr_load(&tail->next, ATOMIC_OPS_FENCE_NONE);
#else
		next = atomic_ops_flagptr_load(&tail->next, NULL, ATOMIC_OPS_FENCE_NONE);
#endif

		if (next == NULL) {
#if !defined(RIG_QUEUE_PRECISE_ITERATOR)
			if (atomic_ops_ptr_cas(&tail->next, NULL, node, ATOMIC_OPS_FENCE_FULL)) {
#else
			if (atomic_ops_flagptr_cas(&tail->next, NULL, false, node, false, ATOMIC_OPS_FENCE_FULL)) {
#endif
				atomic_ops_ptr_cas(&q->tail, tail, node, ATOMIC_OPS_FENCE_NONE);

#if RIG_QUEUE_SMR_TYPE == 1
				rig_smr_hp_release(hprec, SMR_HP_TAIL);
#endif
#if RIG_QUEUE_SMR_TYPE == 2
				rig_smr_epoch_critical_exit();
#endif

				return (true);
			}
		}
		else {
			atomic_ops_ptr_cas(&q->tail, tail, next, ATOMIC_OPS_FENCE_FULL);
		}
	}
}

/**
 * Remove the first item from the front of the queue and return it.
 *
 * @param q
 *     queue pointer
 *
 * @return
 *     data pointer, NULL on error.
 *     On error, the following error codes are set:
 *     - ENOENT (empty queue, nothing to return)
 */
void *rig_queue_get(RIG_QUEUE q) {
	NULLCHECK_EXIT(q);

	Node head = NULL, next = NULL;
#if defined(RIG_QUEUE_PRECISE_ITERATOR)
	bool mark = false;
#endif

#if RIG_QUEUE_SMR_TYPE == 1
	RIG_SMR_HP_Record hprec = rig_smr_hp_record_get();
#endif
#if RIG_QUEUE_SMR_TYPE == 2
	rig_smr_epoch_critical_enter();
#endif

	while (true) {
		head = atomic_ops_ptr_load(&q->head, ATOMIC_OPS_FENCE_ACQUIRE);
#if RIG_QUEUE_SMR_TYPE == 1
		rig_smr_hp_set(hprec, SMR_HP_HEAD, head);
		if (head != atomic_ops_ptr_load(&q->head, ATOMIC_OPS_FENCE_ACQUIRE)) {
			continue;
		}
#endif

#if !defined(RIG_QUEUE_PRECISE_ITERATOR)
		next = atomic_ops_ptr_load(&head->next, ATOMIC_OPS_FENCE_ACQUIRE);
#else
		next = atomic_ops_flagptr_load(&head->next, &mark, ATOMIC_OPS_FENCE_ACQUIRE);
#endif

#if RIG_QUEUE_SMR_TYPE == 1
		rig_smr_hp_set(hprec, SMR_HP_HNEXT, next);
#endif

		if (head == atomic_ops_ptr_load(&q->head, ATOMIC_OPS_FENCE_ACQUIRE)) {
			if (next == NULL) {
#if RIG_QUEUE_SMR_TYPE == 1
				rig_smr_hp_release(hprec, SMR_HP_HEAD);
#endif
#if RIG_QUEUE_SMR_TYPE == 2
				rig_smr_epoch_critical_exit();
#endif
				// Empty queue
				ERRET(ENOENT, NULL);
			}

#if !defined(RIG_QUEUE_PRECISE_ITERATOR)
			if (atomic_ops_ptr_cas(&q->head, head, next, ATOMIC_OPS_FENCE_FULL)) {
#else
			// Mark node for deletion (logical removal)
			if ((!mark) && (atomic_ops_flagptr_cas(&head->next, next, false, next, true, ATOMIC_OPS_FENCE_FULL))) {
#endif
				Node tail = atomic_ops_ptr_load(&q->tail, ATOMIC_OPS_FENCE_NONE);
				if (head == tail) {
					atomic_ops_ptr_cas(&q->tail, tail, next, ATOMIC_OPS_FENCE_NONE);
				}

				if (q->count != NULL) { // Counting supported
					rig_acheck_msg(rig_counter_dec(q->count), "removing non-counted node");
				}

				void *item = next->data;

#if defined(RIG_QUEUE_PRECISE_ITERATOR)
				// Attempt physical removal
				if (atomic_ops_ptr_cas(&q->head, head, next, ATOMIC_OPS_FENCE_FULL)) {
#endif
#if RIG_QUEUE_SMR_TYPE == 1
					rig_smr_hp_mem_retire(head);
#endif
#if RIG_QUEUE_SMR_TYPE == 2
					rig_smr_epoch_mem_retire(head);
#endif
#if defined(RIG_QUEUE_PRECISE_ITERATOR)
				}
#endif

#if RIG_QUEUE_SMR_TYPE == 1
				rig_smr_hp_release(hprec, SMR_HP_HNEXT);
				rig_smr_hp_release(hprec, SMR_HP_HEAD);
#endif
#if RIG_QUEUE_SMR_TYPE == 2
				rig_smr_epoch_critical_exit();
#endif

				return (item);
			}

#if defined(RIG_QUEUE_PRECISE_ITERATOR)
			// Help out by advancing head (attempt physical removal)
			if (atomic_ops_ptr_cas(&q->head, head, next, ATOMIC_OPS_FENCE_FULL)) {
#if RIG_QUEUE_SMR_TYPE == 1
				rig_smr_hp_mem_retire(head);
#endif
#if RIG_QUEUE_SMR_TYPE == 2
				rig_smr_epoch_mem_retire(head);
#endif
			}
#endif
		}
	}
}

/**
 * Return the first item from the front of the queue, without removing it.
 *
 * @param q
 *     queue pointer
 *
 * @return
 *     data pointer, NULL on error.
 *     On error, the following error codes are set:
 *     - ENOENT (empty queue, nothing to return)
 */
void *rig_queue_peek(RIG_QUEUE q) {
	NULLCHECK_EXIT(q);

	Node head = NULL, next = NULL;
#if defined(RIG_QUEUE_PRECISE_ITERATOR)
	bool mark = false;
#endif

#if RIG_QUEUE_SMR_TYPE == 1
	RIG_SMR_HP_Record hprec = rig_smr_hp_record_get();
#endif
#if RIG_QUEUE_SMR_TYPE == 2
	rig_smr_epoch_critical_enter();
#endif

	while (true) {
		head = atomic_ops_ptr_load(&q->head, ATOMIC_OPS_FENCE_ACQUIRE);
#if RIG_QUEUE_SMR_TYPE == 1
		rig_smr_hp_set(hprec, SMR_HP_HEAD, head);
		if (head != atomic_ops_ptr_load(&q->head, ATOMIC_OPS_FENCE_ACQUIRE)) {
			continue;
		}
#endif

#if !defined(RIG_QUEUE_PRECISE_ITERATOR)
		next = atomic_ops_ptr_load(&head->next, ATOMIC_OPS_FENCE_ACQUIRE);
#else
		next = atomic_ops_flagptr_load(&head->next, &mark, ATOMIC_OPS_FENCE_ACQUIRE);
#endif

#if RIG_QUEUE_SMR_TYPE == 1
		rig_smr_hp_set(hprec, SMR_HP_HNEXT, next);
#endif

		if (head == atomic_ops_ptr_load(&q->head, ATOMIC_OPS_FENCE_ACQUIRE)) {
			if (next == NULL) {
#if RIG_QUEUE_SMR_TYPE == 1
				rig_smr_hp_release(hprec, SMR_HP_HEAD);
#endif
#if RIG_QUEUE_SMR_TYPE == 2
				rig_smr_epoch_critical_exit();
#endif
				// Empty queue
				ERRET(ENOENT, NULL);
			}

#if defined(RIG_QUEUE_PRECISE_ITERATOR)
			if (!mark) {
#endif
			Node tail = atomic_ops_ptr_load(&q->tail, ATOMIC_OPS_FENCE_NONE);
			if (head == tail) {
				atomic_ops_ptr_cas(&q->tail, tail, next, ATOMIC_OPS_FENCE_NONE);
			}

			void *item = next->data;

#if RIG_QUEUE_SMR_TYPE == 1
			rig_smr_hp_release(hprec, SMR_HP_HNEXT);
			rig_smr_hp_release(hprec, SMR_HP_HEAD);
#endif
#if RIG_QUEUE_SMR_TYPE == 2
			rig_smr_epoch_critical_exit();
#endif

			return (item);
#if defined(RIG_QUEUE_PRECISE_ITERATOR)
			}

			// Help out by advancing head (attempt physical removal)
			if (atomic_ops_ptr_cas(&q->head, head, next, ATOMIC_OPS_FENCE_FULL)) {
#if RIG_QUEUE_SMR_TYPE == 1
				rig_smr_hp_mem_retire(head);
#endif
#if RIG_QUEUE_SMR_TYPE == 2
				rig_smr_epoch_mem_retire(head);
#endif
			}
#endif
		}
	}
}

/**
 * Returns true if the queue is currently empty, false otherwise.
 *
 * @param q
 *     queue pointer
 *
 * @return
 *     boolean indicating emptiness
 */
bool rig_queue_empty(RIG_QUEUE q) {
	NULLCHECK_EXIT(q);

	if (q->count != NULL) { // Counting supported
		return (rig_counter_get(q->count) == 0);
	}

	return (false);
}

/**
 * Returns true if the queue is currently full, false otherwise.
 *
 * @param q
 *     queue pointer
 *
 * @return
 *     boolean indicating fullness
 */
bool rig_queue_full(RIG_QUEUE q) {
	NULLCHECK_EXIT(q);

	if (q->count != NULL) { // Counting supported
		return (rig_counter_get(q->count) == rig_counter_get_max(q->count));
	}

	return (false);
}

/**
 * Returns the number of items currently held in the queue.
 *
 * @param q
 *     queue pointer
 *
 * @return
 *     number of items in queue
 */
size_t rig_queue_count(RIG_QUEUE q) {
	NULLCHECK_EXIT(q);

	if (q->count != NULL) { // Counting supported
		return (rig_counter_get(q->count));
	}

	return (0);
}

/**
 * Returns the maximum capacity of the queue.
 *
 * @param q
 *     queue pointer
 *
 * @return
 *     maximum queue capacity
 */
size_t rig_queue_capacity(RIG_QUEUE q) {
	NULLCHECK_EXIT(q);

	if (q->count != NULL) { // Counting supported
		return (rig_counter_get_max(q->count));
	}

	return (SIZE_MAX);
}


#if defined(RIG_QUEUE_PRECISE_ITERATOR)

struct rig_queue_iter {
	RIG_QUEUE queue;
	bool completed;
};

#define SMR_IHP_CURR 4
#define SMR_IHP_PREV 5

/**
 * Create an iterator over a queue.
 *
 * @param q
 *     queue pointer
 *
 * @return
 *     queue iterator pointer, NULL on error.
 *     On error, the following error codes are set:
 *     - EALREADY (another iterator already exists)
 *     - ENOMEM (insufficient memory)
 */
RIG_QUEUE_ITER rig_queue_iter_begin(RIG_QUEUE q) {
	NULLCHECK_EXIT(q);

#if RIG_QUEUE_SMR_TYPE == 1
	CHECK_ITER_HPS;
#endif

	// Allocate memory for the iterator
	RIG_QUEUE_ITER iter = rig_mem_alloc(sizeof(*iter), 0);
	NULLCHECK_ERRET(iter, ENOMEM, NULL);

	// Remember the data structure and increase the reference count
	iter->queue = rig_queue_newref(q);
	iter->completed = false;

#if RIG_QUEUE_SMR_TYPE == 1
	// Set the last HP to a dummy value to ensure no other iterators can be
	// created before this one gets dismissed, to avoid chaos with the HPs.
	// NOTE: this does not prevent SMR from reclaiming memory, as an unaligned
	// address never appears there, seeing as atomic operations usually expect
	// and/or perform better on aligned addresses anyway.
	rig_smr_hp_set(hprec, 7, (void *)0x0101);
#endif

	return (iter);
}

/*
 * The queue's iterator is much simpler than the stack's, as you can't have
 * marked nodes left behind that you then encounter during traversal.
 * This means you don't have to check if curr->next is marked, knowing that
 * prev->next is unchanged and not marked is enough.
 * We also have only one place to restart from if we detect changes: the head,
 * so we can verify the Hazard Pointer directly, without distinguishing
 * between the pointer and/or mark change separately.
 * Also having a sentinel node present, all in all diminishes code duplication,
 * as we can easily get prev on both the first and successive iterations, and
 * then use the same code to work with curr.
 */

/**
 * Iterate over a queue and get a pointer to the next data item.
 * No guarantees are made about the availability of the content the returned
 * address points to. Memory management is the caller's responsibility.
 *
 * @param iter
 *     queue iterator pointer
 *
 * @return
 *     pointer to data, NULL on error.
 *     On error, the following error codes are set:
 *     - ENOENT (empty queue, end reached)
 */
void *rig_queue_iter_next(RIG_QUEUE_ITER iter) {
	NULLCHECK_EXIT(iter);

	if (iter->completed) {
		ERRET(ENOENT, NULL);
	}

	bool mark = false;
	Node prev = NULL, curr = NULL;

#if RIG_QUEUE_SMR_TYPE == 1
	// Get pointer to this thread's RIG_SMR_HP_Record
	RIG_SMR_HP_Record hprec = rig_smr_hp_record_get();

	if (rig_smr_hp_get(hprec, SMR_IHP_CURR) == NULL) {
restarthead:
		prev = atomic_ops_ptr_load(&iter->queue->head, ATOMIC_OPS_FENCE_ACQUIRE);

		rig_smr_hp_set(hprec, SMR_IHP_PREV, prev);

		if (prev != atomic_ops_ptr_load(&iter->queue->head, ATOMIC_OPS_FENCE_ACQUIRE)) {
			goto restarthead;
		}

		curr = atomic_ops_flagptr_load(&prev->next, &mark, ATOMIC_OPS_FENCE_ACQUIRE);

		if (mark) {
			if (atomic_ops_ptr_cas(&iter->queue->head, prev, curr, ATOMIC_OPS_FENCE_FULL)) {
				rig_smr_hp_release(hprec, SMR_IHP_PREV);
				rig_smr_hp_mem_retire_noscan(prev);
			}

			goto restarthead;
		}
	}
	else {
		prev = rig_smr_hp_get(hprec, SMR_IHP_CURR);
		rig_smr_hp_set(hprec, SMR_IHP_PREV, prev);

		curr = atomic_ops_flagptr_load(&prev->next, NULL, ATOMIC_OPS_FENCE_ACQUIRE);
	}

	if (curr == NULL) {
		rig_smr_hp_release(hprec, SMR_IHP_PREV);
		rig_smr_hp_release(hprec, SMR_IHP_CURR);

		iter->completed = true;
		ERRET(ENOENT, NULL);
	}

	rig_smr_hp_set(hprec, SMR_IHP_CURR, curr);

	if (curr != atomic_ops_flagptr_load_full(&prev->next, NULL, ATOMIC_OPS_FENCE_ACQUIRE)) {
		goto restarthead;
	}

	return (curr->data);
#else
	return (NULL);
#endif
}

/**
 * Destroy specified queue iterator and set pointer to NULL.
 *
 * @param *iter
 *     pointer to queue iterator pointer
 */
void rig_queue_iter_end(RIG_QUEUE_ITER *iter) {
	NULLCHECK_EXIT(iter);

	// If a valid pointer already contains NULL, nothing to do!
	if (*iter == NULL) {
		return;
	}

	// Decrease reference count of the data structure
	rig_queue_destroy(&(*iter)->queue);

	// Free iterator memory
	rig_mem_free(*iter);
	*iter = NULL;

#if RIG_QUEUE_SMR_TYPE == 1
	RESET_ITER_HPS;

	// Explicit SMR scan
	rig_smr_hp_mem_scan();
#endif
}

#else

/**
 * ITERATOR NOT SUPPORTED!
 *
 * @return
 *     NULL, error ENAVAIL (not available)
 */
RIG_QUEUE_ITER rig_queue_iter_begin(RIG_QUEUE q) {
	NULLCHECK_EXIT(q);

	ERRET(ENAVAIL, NULL);
}

/**
 * ITERATOR NOT SUPPORTED!
 *
 * @return
 *     NULL, error ENAVAIL (not available)
 */
void *rig_queue_iter_next(RIG_QUEUE_ITER iter) {
	NULLCHECK_EXIT(iter);

	ERRET(ENAVAIL, NULL);
}

/**
 * ITERATOR NOT SUPPORTED!
 */
void rig_queue_iter_end(RIG_QUEUE_ITER *iter) {
	NULLCHECK_EXIT(iter);
}

#endif


/**
 * Create a full copy of a lock-free queue.
 *
 * @param q
 *     queue pointer
 *
 * @return
 *     new queue pointer, NULL on error.
 *     On error, the following error codes are set:
 *     - EALREADY (iterator already present, but required for copy!)
 *     - ENOMEM (insufficient memory)
 */
RIG_QUEUE rig_queue_duplicate(RIG_QUEUE q) {
	NULLCHECK_EXIT(q);

	// Needed functions: ds_init, ds_destroy, ds_capacity,
	// ds_iter_begin, ds_iter_end, ds_iter_next, ds_push/put/add

	RIG_QUEUE dup_q = rig_queue_init(rig_queue_capacity(q), (q->count == NULL) ? (RIG_QUEUE_NOCOUNT) : (0));
	NULLCHECK_ERRET(dup_q, ENOMEM, NULL);

	RIG_QUEUE_ITER iter = rig_queue_iter_begin(q);
	NULLCHECK_ERRET_CLEANUP(iter, errno, NULL, rig_queue_destroy(&dup_q));

	void *ptr;

	while ((ptr = rig_queue_iter_next(iter)) != NULL) {
		if (!rig_queue_put(dup_q, ptr)) {
			if (errno == ENOMEM) {
				rig_queue_destroy(&dup_q);
				rig_queue_iter_end(&iter);

				ERRET(ENOMEM, NULL);
			}

			// Ignore EINVAL, impossible since we're getting from a valid ds!
			break; // Exit the loop on EXFULL, we're done!
		}
	}

	rig_queue_iter_end(&iter);

	return (dup_q);

}
