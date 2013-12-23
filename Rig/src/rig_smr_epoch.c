/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_smr_epoch.c 1137 2012-11-13 17:07:28Z llongi $
 */

#include "rig_internal.h"
#include <atomic_ops.h>
#include <stdio.h>
#include "support/array_stack.c"

#define RIG_SMR_EPOCH_THRESHOLD 64 // multiple of 32

typedef struct rig_smr_epoch_record *RIG_SMR_Epoch_Record;

static inline RIG_SMR_Epoch_Record rig_smr_epoch_record_get(void);
static inline void rig_smr_epoch_advance(RIG_SMR_Epoch_Record epoch_record);
static inline uintptr_t rig_smr_epoch_update(RIG_SMR_Epoch_Record epoch_record);
static inline void rig_smr_epoch_free_memory(RIG_SMR_Epoch_Record epoch_record, uintptr_t delta);

struct rig_smr_epoch_record {
	atomic_ops_uint critical_section CACHELINE_ALIGNED;
	atomic_ops_uint local_epoch;
	atomic_ops_uint in_use;
	size_t current_retire_list;
	struct array_stack retire_lists[3];
	RIG_SMR_Epoch_Record next;
};

#if defined(SYSTEM_TLS_SUPPORT)
	static SYSTEM_TLS_DECL RIG_SMR_Epoch_Record Epoch_Record = NULL;
#else
	static RIG_TLS RIG_SMR_Epoch_TLS_Key = NULL;

	static void rig_smr_epoch_construct(void) ATTR_CONSTRUCTOR;

	static void rig_smr_epoch_construct(void) {
		// The TLS key must be initialized only once, only one thread can get here
		RIG_SMR_Epoch_TLS_Key = rig_tls_init();
		NULLCHECK_EXIT(RIG_SMR_Epoch_TLS_Key);
	}

	static void rig_smr_epoch_destruct(void) ATTR_DESTRUCTOR;

	static void rig_smr_epoch_destruct(void) {
		// TODO: check sanity of remaining records (critical_section == 0)
		// TODO: cleanup array_stack (using array_stack_destroy())
		// TODO: cleanup epoch_records themselves
		rig_tls_destroy(&RIG_SMR_Epoch_TLS_Key);
	}
#endif

static atomic_ops_uint RIG_SMR_Epoch_Global_Epoch = ATOMIC_OPS_UINT_INIT(0);

static atomic_ops_ptr  RIG_SMR_Epoch_List_Head = ATOMIC_OPS_PTR_INIT(NULL);
static atomic_ops_uint RIG_SMR_Epoch_List_Length = ATOMIC_OPS_UINT_INIT(0);


static inline RIG_SMR_Epoch_Record rig_smr_epoch_record_get(void) {
#if !defined(SYSTEM_TLS_SUPPORT)
	RIG_SMR_Epoch_Record Epoch_Record = rig_tls_get(RIG_SMR_Epoch_TLS_Key);
#endif

	// If Epoch_Record == NULL, this thread has not registered any RIG_SMR_Epoch_Record structure
	if (Epoch_Record == NULL) {
		// Let's search if there's any old record lying around
		Epoch_Record = atomic_ops_ptr_load(&RIG_SMR_Epoch_List_Head, ATOMIC_OPS_FENCE_NONE);

		while (Epoch_Record != NULL) {
			if (atomic_ops_uint_load(&Epoch_Record->in_use, ATOMIC_OPS_FENCE_ACQUIRE) == 0
			 && atomic_ops_uint_cas(&Epoch_Record->in_use, 0, 1, ATOMIC_OPS_FENCE_ACQUIRE)) {
				// Got it!
				break;
			}

			Epoch_Record = Epoch_Record->next;
		}

		// Didn't find an old record, need to allocate one myself
		if (Epoch_Record == NULL) {
			Epoch_Record = rig_mem_alloc_aligned(sizeof(*Epoch_Record), 0, CACHELINE_SIZE, 0);
			NULLCHECK_EXIT(Epoch_Record);

			// Initialize values
			atomic_ops_uint_store(&Epoch_Record->critical_section, 0, ATOMIC_OPS_FENCE_NONE);
			atomic_ops_uint_store(&Epoch_Record->local_epoch, 0, ATOMIC_OPS_FENCE_NONE);
			atomic_ops_uint_store(&Epoch_Record->in_use, 1, ATOMIC_OPS_FENCE_NONE);
			Epoch_Record->current_retire_list = 0;

			array_stack_init(&Epoch_Record->retire_lists[0], sizeof(void *));
			array_stack_init(&Epoch_Record->retire_lists[1], sizeof(void *));
			array_stack_init(&Epoch_Record->retire_lists[2], sizeof(void *));

			// Link the new RIG_SMR_Epoch_Record into the main RIG_SMR_Epoch_List
			atomic_ops_uint_inc(&RIG_SMR_Epoch_List_Length, ATOMIC_OPS_FENCE_FULL);

			while (true) {
				RIG_SMR_Epoch_Record head = atomic_ops_ptr_load(&RIG_SMR_Epoch_List_Head, ATOMIC_OPS_FENCE_NONE);

				Epoch_Record->next = head;

				if (atomic_ops_ptr_cas(&RIG_SMR_Epoch_List_Head, head, Epoch_Record, ATOMIC_OPS_FENCE_FULL)) {
					break;
				}
			}
		}

#if !defined(SYSTEM_TLS_SUPPORT)
		// Set the thread specific value correctly
		rig_tls_set(RIG_SMR_Epoch_TLS_Key, Epoch_Record);
#endif
	}

	return (Epoch_Record);
}

void rig_smr_epoch_record_release(void) {
#if !defined(SYSTEM_TLS_SUPPORT)
	RIG_SMR_Epoch_Record Epoch_Record = rig_tls_get(RIG_SMR_Epoch_TLS_Key);
#endif

	if (Epoch_Record != NULL) {
		rig_acheck_msg(atomic_ops_uint_load(&Epoch_Record->critical_section, ATOMIC_OPS_FENCE_NONE) == 0,
			"unclosed critical section (perhaps incorrect recursion?)");

		// Empty all lists by freeing memory after at last three epochs have passed, helping along if needed.
		uintptr_t delta = rig_smr_epoch_update(Epoch_Record);

		while (delta < 3) {
			rig_smr_epoch_advance(Epoch_Record);
			delta += rig_smr_epoch_update(Epoch_Record);
		}

		rig_smr_epoch_free_memory(Epoch_Record, delta);

		rig_acheck_msg(array_stack_count(&Epoch_Record->retire_lists[0]) == 0, "retire list 0 not empty");
		rig_acheck_msg(array_stack_count(&Epoch_Record->retire_lists[1]) == 0, "retire list 1 not empty");
		rig_acheck_msg(array_stack_count(&Epoch_Record->retire_lists[2]) == 0, "retire list 2 not empty");

		// Reset to initial values
		atomic_ops_uint_store(&Epoch_Record->local_epoch, 0, ATOMIC_OPS_FENCE_NONE);
		Epoch_Record->current_retire_list = 0;

		atomic_ops_uint_store(&Epoch_Record->in_use, 0, ATOMIC_OPS_FENCE_RELEASE);

#if defined(SYSTEM_TLS_SUPPORT)
		Epoch_Record = NULL;
#else
		rig_tls_set(RIG_SMR_Epoch_TLS_Key, NULL);
#endif

		atomic_ops_fence(ATOMIC_OPS_FENCE_FULL);
	}
}

static inline void rig_smr_epoch_advance(RIG_SMR_Epoch_Record epoch_record) {
	uintptr_t global_epoch = atomic_ops_uint_load(&RIG_SMR_Epoch_Global_Epoch, ATOMIC_OPS_FENCE_NONE);

	// Check if someone advanced the global epoch already in the meantime
	if (atomic_ops_uint_load(&epoch_record->local_epoch, ATOMIC_OPS_FENCE_NONE) != global_epoch) {
		// Epoch did go forward, not by us, but it did!
		return;
	}

	RIG_SMR_Epoch_Record curr = atomic_ops_ptr_load(&RIG_SMR_Epoch_List_Head, ATOMIC_OPS_FENCE_NONE);

	while (curr != NULL) {
		if (atomic_ops_uint_load(&curr->in_use, ATOMIC_OPS_FENCE_ACQUIRE) == 1
		 && atomic_ops_uint_load(&curr->critical_section, ATOMIC_OPS_FENCE_ACQUIRE) >= 1
		 && atomic_ops_uint_load(&curr->local_epoch, ATOMIC_OPS_FENCE_ACQUIRE) != global_epoch) {
			// Can't advance epoch, since someone is lagging behind, or someone already has done it
			// and is thus further along!
			return;
		}

		curr = curr->next;
	}

	// We don't care if this fails or not, as failure means the system advanced anyway by someone else!
	atomic_ops_uint_cas(&RIG_SMR_Epoch_Global_Epoch, global_epoch, global_epoch + 1, ATOMIC_OPS_FENCE_FULL);
}

static inline uintptr_t rig_smr_epoch_update(RIG_SMR_Epoch_Record epoch_record) {
	uintptr_t global_epoch = atomic_ops_uint_load(&RIG_SMR_Epoch_Global_Epoch, ATOMIC_OPS_FENCE_NONE);

	// If the epoch didn't change, there's nothing to update
	if (atomic_ops_uint_load(&epoch_record->local_epoch, ATOMIC_OPS_FENCE_NONE) == global_epoch) {
		return (0);
	}

	// Calculate difference between global and local epochs
	uintptr_t delta = global_epoch - atomic_ops_uint_load(&epoch_record->local_epoch, ATOMIC_OPS_FENCE_NONE);

	// Update local epoch to global one
	atomic_ops_uint_store(&epoch_record->local_epoch, global_epoch, ATOMIC_OPS_FENCE_NONE);

	return (delta);
}

static inline void rig_smr_epoch_free_memory(RIG_SMR_Epoch_Record epoch_record, uintptr_t delta) {
	if (delta != 0) {
		// Always jump to next limbo list, wrapping around at three
		epoch_record->current_retire_list++;

		if (epoch_record->current_retire_list == 3) {
			epoch_record->current_retire_list = 0;
		}

		// Clear the current list before using it anew
		void **ptr;
		while ((ptr = array_stack_pop(&epoch_record->retire_lists[epoch_record->current_retire_list])) != NULL) {
			rig_mem_free(*ptr);
		}

		// If several epochs have passed, we can also cleanup the other retire lists
		if (delta >= 2) {
			rig_smr_epoch_free_memory(epoch_record, 1);

			if (delta >= 3) {
				rig_smr_epoch_free_memory(epoch_record, 1);
			}
		}
	}
}

void rig_smr_epoch_critical_enter(void) {
	RIG_SMR_Epoch_Record epoch_record = rig_smr_epoch_record_get();

	if (atomic_ops_uint_load(&epoch_record->critical_section, ATOMIC_OPS_FENCE_NONE) >= 1) {
		atomic_ops_uint_inc(&epoch_record->critical_section, ATOMIC_OPS_FENCE_ACQUIRE);
		return;
	}

	uintptr_t delta = rig_smr_epoch_update(epoch_record);
	rig_smr_epoch_free_memory(epoch_record, delta);

	atomic_ops_uint_store(&epoch_record->critical_section, 1, ATOMIC_OPS_FENCE_FULL);
}

void rig_smr_epoch_critical_exit(void) {
	RIG_SMR_Epoch_Record epoch_record = rig_smr_epoch_record_get();

	rig_acheck_msg(atomic_ops_uint_load(&epoch_record->critical_section, ATOMIC_OPS_FENCE_NONE) >= 1,
		"exiting from critical section that was never entered");

	if (atomic_ops_uint_load(&epoch_record->critical_section, ATOMIC_OPS_FENCE_NONE) > 1) {
		atomic_ops_uint_dec(&epoch_record->critical_section, ATOMIC_OPS_FENCE_RELEASE);
		return;
	}

	atomic_ops_uint_store(&epoch_record->critical_section, 0, ATOMIC_OPS_FENCE_FULL);

	// If over the threshold, try to advance the global epoch. If you fail because someone else did,
	// no worry, the system made progress. If you fail because not everyone active has yet observed
	// the current epoch, just wait, it'll happen.
	if (array_stack_count(&epoch_record->retire_lists[epoch_record->current_retire_list]) >= RIG_SMR_EPOCH_THRESHOLD) {
		rig_smr_epoch_advance(epoch_record);
	}
}

void rig_smr_epoch_mem_retire(void *memory_ptr) {
	// If pointer is NULL, we do nothing at all, same as the standard free()
	if (memory_ptr != NULL) {
		RIG_SMR_Epoch_Record epoch_record = rig_smr_epoch_record_get();

		// Push the pointer onto the current retire list
		array_stack_push(&epoch_record->retire_lists[epoch_record->current_retire_list], &memory_ptr);
	}
}

void rig_smr_epoch_debug_info(bool print_list) {
	printf("SMR_Epoch global Epoch (before) = %zu\n\n",
		atomic_ops_uint_load(&RIG_SMR_Epoch_Global_Epoch, ATOMIC_OPS_FENCE_ACQUIRE));

	RIG_SMR_Epoch_Record curr = atomic_ops_ptr_load(&RIG_SMR_Epoch_List_Head, ATOMIC_OPS_FENCE_ACQUIRE);

	printf("SMR_Epoch list head address: %p\n", (void *)&RIG_SMR_Epoch_List_Head);
	printf("SMR_Epoch list head value: %p\n", (void *)curr);
	printf("SMR_Epoch list length address: %p\n", (void *)&RIG_SMR_Epoch_List_Length);
	printf("SMR_Epoch list length value: %zu\n\n",
		atomic_ops_uint_load(&RIG_SMR_Epoch_List_Length, ATOMIC_OPS_FENCE_NONE));

	if (print_list) {
		while (curr != NULL) {
			printf("== SMR_Epoch_Record at %p ==\n", (void *)curr);

			printf("critical_section = %zu\n", atomic_ops_uint_load(&curr->critical_section, ATOMIC_OPS_FENCE_NONE));
			printf("local_epoch = %zu\n", atomic_ops_uint_load(&curr->local_epoch, ATOMIC_OPS_FENCE_NONE));
			printf("in_use = %zu\n", atomic_ops_uint_load(&curr->in_use, ATOMIC_OPS_FENCE_NONE));

			printf("current_retire_list = %zu\n", curr->current_retire_list);
			printf("retire list 0 count = %zu\n", array_stack_count(&curr->retire_lists[0]));
			printf("retire list 1 count = %zu\n", array_stack_count(&curr->retire_lists[1]));
			printf("retire list 2 count = %zu\n\n", array_stack_count(&curr->retire_lists[2]));

			curr = curr->next;
		}
	}

	printf("SMR_Epoch global Epoch (after) = %zu\n\n",
		atomic_ops_uint_load(&RIG_SMR_Epoch_Global_Epoch, ATOMIC_OPS_FENCE_RELEASE));
}
