/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_stack.c 1139 2012-11-14 19:48:44Z llongi $
 */

#include "rig_internal.h"
#include <atomic_ops.h>
#include "support/array_stack.c"

/*
 * Rig Stack Data Definitions
 */
#if RIG_STACK_SMR_TYPE == 1
	#define SMR_HP_TOP 0
#endif

/** Types */
typedef struct NodeStruct *Node;

/** Structures */
struct rig_stack {
	CACHELINE_ALONE(atomic_ops_ptr, top);
	RIG_COUNTER count;
	RIG_COUNTER refcount;
};

struct NodeStruct {
#if !defined(RIG_STACK_PRECISE_ITERATOR)
	Node next;
#else
	atomic_ops_flagptr next;
#endif
	void* data;
};


/*
 * Rig Stack Implementation
 */

/**
 * Initialize a lock-free stack (LIFO data structure).
 * This data structure is intended for low-level, high-performance purposes, and thus only accepts and returns memory
 * addresses. Their content and its persistence is the responsibility of the application programmer.
 *
 * @param capacity
 *     maximum number of elements the stack can contain, 0 means unlimited
 * @param flags
 *     flags to modify stack behavior, the following are currently supported:
 *     - RIG_STACK_NOCOUNT (do not count elements, capacity is not enforced)
 *
 * @return
 *     stack pointer, NULL on error.
 *     On error, the following error codes are set:
 *     - EINVAL (invalid arguments passed)
 *     - ENOMEM (insufficient memory)
 */
RIG_STACK rig_stack_init(size_t capacity, uint16_t flags) {
	CHECK_PERMITTED_FLAGS(flags, RIG_STACK_NOCOUNT);

	// Allocate memory for the stack
	RIG_STACK s = rig_mem_alloc_aligned(sizeof(*s), 0, CACHELINE_SIZE, 0);
	NULLCHECK_ERRET(s, ENOMEM, NULL);

	// Initialize the counters
	RIG_COUNTER refcount = rig_counter_init(1, 0);
	NULLCHECK_ERRET_CLEANUP(refcount, ENOMEM, NULL, rig_mem_free_aligned(s));

	RIG_COUNTER count = NULL;
	if (!TEST_BITFIELD(flags, RIG_STACK_NOCOUNT)) { // Support not counting elements
		count = rig_counter_init(0, capacity);
		NULLCHECK_ERRET_CLEANUP(count, ENOMEM, NULL, rig_mem_free_aligned(s); rig_counter_destroy(&refcount));
	}

	// Initialize the needed values
	atomic_ops_ptr_store(&s->top, NULL, ATOMIC_OPS_FENCE_NONE);
	s->count = count;
	s->refcount = refcount;

	atomic_ops_fence(ATOMIC_OPS_FENCE_FULL);

	return (s);
}

/**
 * Get a new reference to the stack, increasing the reference count by one.
 *
 * @param s
 *     stack pointer
 *
 * @return
 *     stack pointer
 */
RIG_STACK rig_stack_newref(RIG_STACK s) {
	NULLCHECK_EXIT(s);

	rig_acheck_msg(rig_counter_inc(s->refcount), "more references than physically possible");

	return (s);
}

/**
 * Destroy specified stack reference and set pointer to NULL.
 * If reference count reaches zero, proceed to full destruction.
 *
 * @param *s
 *     pointer to stack pointer
 */
void rig_stack_destroy(RIG_STACK *s) {
	NULLCHECK_EXIT(s);
	NULLCHECK_EXIT(*s);

	if (rig_counter_dec_and_test((*s)->refcount)) {
		// Traverse the list and remove all nodes
		Node curr = atomic_ops_ptr_load(&(*s)->top, ATOMIC_OPS_FENCE_NONE), succ = NULL;

		while (curr != NULL) {
#if !defined(RIG_STACK_PRECISE_ITERATOR)
			succ = curr->next;
#else
			succ = atomic_ops_flagptr_load(&curr->next, NULL, ATOMIC_OPS_FENCE_NONE);
#endif
			// Free directly here, as there are no shared references anymore around, no SMR is required!
			rig_mem_free(curr);

			curr = succ;
		}

		// Destroy counters and stack
		rig_counter_destroy(&(*s)->refcount);
		rig_counter_destroy(&(*s)->count);
		rig_mem_free_aligned(*s);
	}
	else {
		rig_acheck_msg(errno == 0, "reference count already zero, uncounted references exist");
	}

	// NULL the stack pointer so it can't be used again afterwards
	*s = NULL;

	atomic_ops_fence(ATOMIC_OPS_FENCE_FULL);

#if RIG_STACK_SMR_TYPE == 1
	// Explicit SMR HP scan
	rig_smr_hp_mem_scan();
#endif
}

/**
 * Clear the stack by popping elements from it until it's empty.
 *
 * @param s
 *     stack pointer
 */
void rig_stack_clear(RIG_STACK s) {
	NULLCHECK_EXIT(s);

	while (rig_stack_pop(s) != NULL) { ; }
}

/**
 * Add a new item at the top of the stack.
 *
 * @param s
 *     stack pointer
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
bool rig_stack_push(RIG_STACK s, void *item) {
	NULLCHECK_EXIT(s);

	// Allocate memory for the new element
	Node node = rig_mem_alloc(sizeof(*node), 0);
	NULLCHECK_ERRET(node, ENOMEM, false);

	// Check if there's still place for the new element
	if ((s->count != NULL) && (!rig_counter_inc(s->count))) {
		// Full stack
		ERRET_CLEANUP(EXFULL, false, rig_mem_free(node));
	}

	// Set the content of the new element
	node->data = item;

	Node top = NULL;

	while (true) {
		top = atomic_ops_ptr_load(&s->top, ATOMIC_OPS_FENCE_NONE);

#if !defined(RIG_STACK_PRECISE_ITERATOR)
		node->next = top;
#else
		atomic_ops_flagptr_store(&node->next, top, false, ATOMIC_OPS_FENCE_NONE);
#endif

		if (atomic_ops_ptr_cas(&s->top, top, node, ATOMIC_OPS_FENCE_FULL)) {
			return (true);
		}
	}
}

/**
 * Remove the top item from the stack and return it.
 *
 * @param s
 *     stack pointer
 *
 * @return
 *     data pointer, NULL on error.
 *     On error, the following error codes are set:
 *     - ENOENT (empty stack, nothing to return)
 */
void *rig_stack_pop(RIG_STACK s) {
	NULLCHECK_EXIT(s);

	Node top = NULL;
#if defined(RIG_STACK_PRECISE_ITERATOR)
	Node next = NULL;
	bool mark = false;
#endif

#if RIG_STACK_SMR_TYPE == 1
	RIG_SMR_HP_Record hprec = rig_smr_hp_record_get();
#endif
#if RIG_STACK_SMR_TYPE == 2
	rig_smr_epoch_critical_enter();
#endif

	while (true) {
		top = atomic_ops_ptr_load(&s->top, ATOMIC_OPS_FENCE_ACQUIRE);
#if RIG_STACK_SMR_TYPE == 1
		rig_smr_hp_set(hprec, SMR_HP_TOP, top);
		if (top != atomic_ops_ptr_load(&s->top, ATOMIC_OPS_FENCE_ACQUIRE)) {
			continue;
		}
#endif

		if (top == NULL) {
#if RIG_STACK_SMR_TYPE == 2
			rig_smr_epoch_critical_exit();
#endif
			// Empty stack
			ERRET(ENOENT, NULL);
		}

#if !defined(RIG_STACK_PRECISE_ITERATOR)
		if (atomic_ops_ptr_cas(&s->top, top, top->next, ATOMIC_OPS_FENCE_FULL)) {
#else
		next = atomic_ops_flagptr_load(&top->next, &mark, ATOMIC_OPS_FENCE_NONE);

		// Mark node for deletion (logical removal)
		if ((!mark) && (atomic_ops_flagptr_cas(&top->next, next, false, next, true, ATOMIC_OPS_FENCE_FULL))) {
#endif
			if (s->count != NULL) { // Counting supported
				rig_acheck_msg(rig_counter_dec(s->count), "removing non-counted node");
			}

			void *item = top->data;

#if defined(RIG_STACK_PRECISE_ITERATOR)
			// Attempt physical removal
			if (atomic_ops_ptr_cas(&s->top, top, next, ATOMIC_OPS_FENCE_FULL)) {
#endif
#if RIG_STACK_SMR_TYPE == 1
				rig_smr_hp_mem_retire(top);
#endif
#if RIG_STACK_SMR_TYPE == 2
				rig_smr_epoch_mem_retire(top);
#endif
#if defined(RIG_STACK_PRECISE_ITERATOR)
			}
#endif

#if RIG_STACK_SMR_TYPE == 1
			rig_smr_hp_release(hprec, SMR_HP_TOP);
#endif
#if RIG_STACK_SMR_TYPE == 2
			rig_smr_epoch_critical_exit();
#endif

			return (item);
		}

#if defined(RIG_STACK_PRECISE_ITERATOR)
		// Help out by advancing top (attempt physical removal)
		if (atomic_ops_ptr_cas(&s->top, top, next, ATOMIC_OPS_FENCE_FULL)) {
#if RIG_STACK_SMR_TYPE == 1
			rig_smr_hp_mem_retire(top);
#endif
#if RIG_STACK_SMR_TYPE == 2
			rig_smr_epoch_mem_retire(top);
#endif
		}
#endif
	}
}

/**
 * Return the top item from the stack, without removing it.
 *
 * @param s
 *     stack pointer
 *
 * @return
 *     data pointer, NULL on error.
 *     On error, the following error codes are set:
 *     - ENOENT (empty stack, nothing to return)
 */
void *rig_stack_peek(RIG_STACK s) {
	NULLCHECK_EXIT(s);

	Node top = NULL;
#if defined(RIG_STACK_PRECISE_ITERATOR)
	Node next = NULL;
	bool mark = false;
#endif

#if RIG_STACK_SMR_TYPE == 1
	RIG_SMR_HP_Record hprec = rig_smr_hp_record_get();
#endif
#if RIG_STACK_SMR_TYPE == 2
	rig_smr_epoch_critical_enter();
#endif

	while (true) {
		top = atomic_ops_ptr_load(&s->top, ATOMIC_OPS_FENCE_ACQUIRE);
#if RIG_STACK_SMR_TYPE == 1
		rig_smr_hp_set(hprec, SMR_HP_TOP, top);
		if (top != atomic_ops_ptr_load(&s->top, ATOMIC_OPS_FENCE_ACQUIRE)) {
			continue;
		}
#endif

		if (top == NULL) {
#if RIG_STACK_SMR_TYPE == 2
			rig_smr_epoch_critical_exit();
#endif
			// Empty stack
			ERRET(ENOENT, NULL);
		}

#if defined(RIG_STACK_PRECISE_ITERATOR)
		next = atomic_ops_flagptr_load(&top->next, &mark, ATOMIC_OPS_FENCE_NONE);

		if (!mark) {
#endif
		void *item = top->data;

#if RIG_STACK_SMR_TYPE == 1
		rig_smr_hp_release(hprec, SMR_HP_TOP);
#endif
#if RIG_STACK_SMR_TYPE == 2
		rig_smr_epoch_critical_exit();
#endif

		return (item);
#if defined(RIG_STACK_PRECISE_ITERATOR)
		}

		// Help out by advancing top (attempt physical removal)
		if (atomic_ops_ptr_cas(&s->top, top, next, ATOMIC_OPS_FENCE_FULL)) {
#if RIG_STACK_SMR_TYPE == 1
			rig_smr_hp_mem_retire(top);
#endif
#if RIG_STACK_SMR_TYPE == 2
			rig_smr_epoch_mem_retire(top);
#endif
		}
#endif
	}
}

/**
 * Returns true if the stack is currently empty, false otherwise.
 *
 * @param s
 *     stack pointer
 *
 * @return
 *     boolean indicating emptiness
 */
bool rig_stack_empty(RIG_STACK s) {
	NULLCHECK_EXIT(s);

	if (s->count != NULL) { // Counting supported
		return (rig_counter_get(s->count) == 0);
	}

	return (false);
}

/**
 * Returns true if the stack is currently full, false otherwise.
 *
 * @param s
 *     stack pointer
 *
 * @return
 *     boolean indicating fullness
 */
bool rig_stack_full(RIG_STACK s) {
	NULLCHECK_EXIT(s);

	if (s->count != NULL) { // Counting supported
		return (rig_counter_get(s->count) == rig_counter_get_max(s->count));
	}

	return (false);
}

/**
 * Returns the number of items currently held in the stack.
 *
 * @param s
 *     stack pointer
 *
 * @return
 *     number of items in stack
 */
size_t rig_stack_count(RIG_STACK s) {
	NULLCHECK_EXIT(s);

	if (s->count != NULL) { // Counting supported
		return (rig_counter_get(s->count));
	}

	return (0);
}

/**
 * Returns the maximum capacity of the stack.
 *
 * @param s
 *     stack pointer
 *
 * @return
 *     maximum stack capacity
 */
size_t rig_stack_capacity(RIG_STACK s) {
	NULLCHECK_EXIT(s);

	if (s->count != NULL) { // Counting supported
		return (rig_counter_get_max(s->count));
	}

	return (SIZE_MAX);
}


#if defined(RIG_STACK_PRECISE_ITERATOR)

struct rig_stack_iter {
	RIG_STACK stack;
	bool completed;
};

#define SMR_IHP_CURR 4
#define SMR_IHP_PREV 5

/**
 * Create an iterator over a stack.
 *
 * @param s
 *     stack pointer
 *
 * @return
 *     stack iterator pointer, NULL on error.
 *     On error, the following error codes are set:
 *     - EALREADY (another iterator already exists)
 *     - ENOMEM (insufficient memory)
 */
RIG_STACK_ITER rig_stack_iter_begin(RIG_STACK s) {
	NULLCHECK_EXIT(s);

#if RIG_STACK_SMR_TYPE == 1
	CHECK_ITER_HPS;
#endif

	// Allocate memory for the iterator
	RIG_STACK_ITER iter = rig_mem_alloc(sizeof(*iter), 0);
	NULLCHECK_ERRET(iter, ENOMEM, NULL);

	// Remember the data structure and increase the reference count
	iter->stack = rig_stack_newref(s);
	iter->completed = false;

#if RIG_STACK_SMR_TYPE == 1
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
 * Marked nodes left behind
 *
 * When for example a thread doing a pop() manages to mark a node, but not yet
 * redirect Top, then another thread push()es a node successfully, making the
 * Top-redirect of the other thread's pop() fail, the stack is left as follows:
 * Top->A->B(marked)->C->...
 * This could be avoided by having push() check if the Top node is marked, and
 * if it is, help in adjusting the Top pointer, but to do this push() would
 * have to set and verify a Hazard Pointer on Top (to access Top->next), and
 * check the mark and act accordingly: a huge amount of overhead for an operation
 * which would normally not have any. Also consider that pop() (and thus look())
 * does handle this anyway as part of its normal operation.
 * Only the Iterator needs to specifically check for this case and handle it.
 */

/**
 * Iterate over a stack and get a pointer to the next data item.
 * No guarantees are made about the availability of the content the returned
 * address points to. Memory management is the caller's responsibility.
 *
 * @param iter
 *     stack iterator pointer
 *
 * @return
 *     pointer to data, NULL on error.
 *     On error, the following error codes are set:
 *     - ENOENT (empty stack, end reached)
 */
void *rig_stack_iter_next(RIG_STACK_ITER iter) {
	NULLCHECK_EXIT(iter);

	if (iter->completed) {
		ERRET(ENOENT, NULL);
	}

	bool mark = false;
	Node prev = NULL, curr = NULL, succ = NULL;

#if RIG_STACK_SMR_TYPE == 1
	// Get pointer to this thread's RIG_SMR_HP_Record
	RIG_SMR_HP_Record hprec = rig_smr_hp_record_get();

	if (rig_smr_hp_get(hprec, SMR_IHP_CURR) == NULL) {
restarttop:
		rig_smr_hp_release(hprec, SMR_IHP_PREV);

		// The first element we want data from is the Top of the stack.
		curr = atomic_ops_ptr_load(&iter->stack->top, ATOMIC_OPS_FENCE_ACQUIRE);

		// If it's NULL, the list is empty, so we're done.
		if (curr == NULL) {
			// SMR_IHP_PREV was already released
			rig_smr_hp_release(hprec, SMR_IHP_CURR);

			iter->completed = true;
			ERRET(ENOENT, NULL);
		}

		// At this point we know Top exists, so we protect it and verify it.
		rig_smr_hp_set(hprec, SMR_IHP_CURR, curr);

		// If the pointer changed (there is no flag here), Top changed, and we
		// can't guarantee it still exists, meaning we can't access it:
		// we need to restart from the beginning.
		if (curr != atomic_ops_ptr_load(&iter->stack->top, ATOMIC_OPS_FENCE_ACQUIRE)) {
			goto restarttop;
		}

		// Top may have been valid, but still have been marked itself in the meantime.
		// If it was, we try to help the Top pointer along, and restart.
		succ = atomic_ops_flagptr_load(&curr->next, &mark, ATOMIC_OPS_FENCE_NONE);

		if (mark) {
			if (atomic_ops_ptr_cas(&iter->stack->top, curr, succ, ATOMIC_OPS_FENCE_FULL)) {
				rig_smr_hp_release(hprec, SMR_IHP_CURR);
				rig_smr_hp_mem_retire_noscan(curr);
			}

			goto restarttop;
		}
	}
	else {
		// We get prev from the old curr, which is protected, and set the HP.
		prev = rig_smr_hp_get(hprec, SMR_IHP_CURR);
		rig_smr_hp_set(hprec, SMR_IHP_PREV, prev);

retrycurr:
		// We now get the current element, the one we want to get data from.
		curr = atomic_ops_flagptr_load(&prev->next, NULL, ATOMIC_OPS_FENCE_ACQUIRE);

		// If it's NULL, we're at the end of the list, so we're done.
		if (curr == NULL) {
			rig_smr_hp_release(hprec, SMR_IHP_PREV);
			rig_smr_hp_release(hprec, SMR_IHP_CURR);

			iter->completed = true;
			ERRET(ENOENT, NULL);
		}

		// At this point we know curr exists, so we protect it and verify it.
		rig_smr_hp_set(hprec, SMR_IHP_CURR, curr);

		/*
		 * Here we split the verification of the Hazard Pointer up, because
		 * depending on the pointer / flag combinations, we may not have to
		 * necessarily do everything again (optimization):
		 * 1) No change in pointer, No change in flag:
		 *     Everything is okay, curr is now protected and we can use it.
		 * 2) Pointer changed, No change in flag:
		 *    This means another iterator has relinked prev->next to take out
		 *    the possible marked nodes it's encountering (see "Marked nodes
		 *    left behind"), meaning curr is invalid, but prev is still okay,
		 *    so we can just retry to get curr, protect it and verify it.
		 *    There is no need for a restart from top.
		 * 3) No change in pointer, Flag changed:
		 *     This means prev was pop()ed from the stack, so we can't trust
		 *     what it has to say about curr (prev->next) anymore: it might be
		 *     the new Top, or it may already have been removed by other pop()s,
		 *     even before we set the HP to it, so it may even have been fully
		 *     retired: we must restart from the only safe place, the Top.
		 * 4) Pointer changed, Flag changed:
		 *     This just combines the previous two, another iterator relinked
		 *     prev->next, so curr is invalid, and we can't retry from prev,
		 *     because we can't trust what it tells us curr (prev->next) should
		 *     be, so we restart from the top as the only safe place.
		 */
		if (curr != atomic_ops_flagptr_load_full(&prev->next, &mark, ATOMIC_OPS_FENCE_ACQUIRE)) {
			if (mark) {
				goto restarttop;
			}

			goto retrycurr;
		}

		/*
		 * Now curr is what we believe to be the current node, and it's protected
		 * correctly: but we still need to check if curr itself isn't marked, it
		 * could be one of the remaining marked nodes (see "Marked nodes left
		 * behind"), or between protecting it and accessing it, several pop()s
		 * could have already removed it from the list.
		 * So if it's marked, we try to relink prev->next to curr->next, which
		 * would fix the "Marked nodes left behind" case, afterwards we jump
		 * back and retry from prev, which will ensure prev is valid (if the
		 * relink succeeded), or jump to the right place to restart from.
		 */
		succ = atomic_ops_flagptr_load(&curr->next, &mark, ATOMIC_OPS_FENCE_NONE);

		if (mark) {
			if (atomic_ops_flagptr_cas(&prev->next, curr, false, succ, false, ATOMIC_OPS_FENCE_FULL)) {
				rig_smr_hp_release(hprec, SMR_IHP_CURR);
				rig_smr_hp_mem_retire_noscan(curr);
			}

			goto retrycurr;
		}
	}

	return (curr->data);
#else
	return (NULL);
#endif
}

/**
 * Destroy specified stack iterator and set pointer to NULL.
 *
 * @param *iter
 *     pointer to stack iterator pointer
 */
void rig_stack_iter_end(RIG_STACK_ITER *iter) {
	NULLCHECK_EXIT(iter);

	// If a valid pointer already contains NULL, nothing to do!
	if (*iter == NULL) {
		return;
	}

	// Decrease reference count of the data structure
	rig_stack_destroy(&(*iter)->stack);

	// Free iterator memory
	rig_mem_free(*iter);
	*iter = NULL;

#if RIG_STACK_SMR_TYPE == 1
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
RIG_STACK_ITER rig_stack_iter_begin(RIG_STACK s) {
	NULLCHECK_EXIT(s);

	ERRET(ENAVAIL, NULL);
}

/**
 * ITERATOR NOT SUPPORTED!
 *
 * @return
 *     NULL, error ENAVAIL (not available)
 */
void *rig_stack_iter_next(RIG_STACK_ITER iter) {
	NULLCHECK_EXIT(iter);

	ERRET(ENAVAIL, NULL);
}

/**
 * ITERATOR NOT SUPPORTED!
 */
void rig_stack_iter_end(RIG_STACK_ITER *iter) {
	NULLCHECK_EXIT(iter);
}

#endif


/**
 * Create a full copy of a lock-free stack.
 *
 * @param s
 *     stack pointer
 *
 * @return
 *     new stack pointer, NULL on error.
 *     On error, the following error codes are set:
 *     - EALREADY (iterator already present, but required for copy!)
 *     - ENOMEM (insufficient memory)
 */
RIG_STACK rig_stack_duplicate(RIG_STACK s) {
	NULLCHECK_EXIT(s);

	// Needed functions: ds_init, ds_destroy, ds_capacity,
	// ds_iter_begin, ds_iter_end, ds_iter_next, ds_push/put/add

	RIG_STACK dup_s = rig_stack_init(rig_stack_capacity(s), (s->count == NULL) ? (RIG_STACK_NOCOUNT) : (0));
	NULLCHECK_ERRET(dup_s, ENOMEM, NULL);

	RIG_STACK_ITER iter = rig_stack_iter_begin(s);
	NULLCHECK_ERRET_CLEANUP(iter, errno, NULL, rig_stack_destroy(&dup_s));

	struct array_stack tmp_stack;
	array_stack_init(&tmp_stack, sizeof(void *));

	void *ptr;

	while (((ptr = rig_stack_iter_next(iter)) != NULL)
		&& (array_stack_count(&tmp_stack) < rig_stack_capacity(dup_s))) {
		array_stack_push(&tmp_stack, ptr);
	}

	rig_stack_iter_end(&iter);

	while ((ptr = array_stack_pop(&tmp_stack)) != NULL) {
		if (!rig_stack_push(dup_s, ptr)) {
			if (errno == ENOMEM) {
				rig_stack_destroy(&dup_s);
				array_stack_destroy(&tmp_stack);

				ERRET(ENOMEM, NULL);
			}

			// Ignore EINVAL, impossible since we're getting from a valid ds!
			break; // Exit the loop on EXFULL, we're done!
		}
	}

	array_stack_destroy(&tmp_stack);

	return (dup_s);
}
