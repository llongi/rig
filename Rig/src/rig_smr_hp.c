/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_smr_hp.c 1137 2012-11-13 17:07:28Z llongi $
 */

#include "rig_internal.h"
#include <atomic_ops.h>
#include <stdio.h>
#include "support/array_stack.c"

#define RIG_SMR_HP_COUNT 8 // HP[0]: curr, HP[1]: prev, HP[2]: misc, HP[3]: misc (*2 for iterators)
#define RIG_SMR_HP_THRESHOLD 64 // based on RIG_SMR_HP_COUNT * expected thread number (8), multiple of 32

static int rig_smr_hp_ptr_cmp(const void *a, const void *b);

struct rig_smr_hp_record {
	atomic_ops_ptr HP[RIG_SMR_HP_COUNT] CACHELINE_ALIGNED;
	struct array_stack retire_list;
	atomic_ops_uint in_use;
	RIG_SMR_HP_Record next;
};

#if defined(SYSTEM_TLS_SUPPORT)
	static SYSTEM_TLS_DECL RIG_SMR_HP_Record HP_Record = NULL;
#else
	static RIG_TLS RIG_SMR_HP_TLS_Key = NULL;

	static void rig_smr_hp_construct(void) ATTR_CONSTRUCTOR;

	static void rig_smr_hp_construct(void) {
		// The TLS key must be initialized only once, only one thread can get here
		RIG_SMR_HP_TLS_Key = rig_tls_init();
		NULLCHECK_EXIT(RIG_SMR_HP_TLS_Key);
	}
#endif

static void rig_smr_hp_destruct(void) ATTR_DESTRUCTOR;

static void rig_smr_hp_destruct(void) {
	rig_smr_hp_mem_scan_full();
	// TODO: check sanity of remaining records (HPs all NULL)
	// TODO: cleanup array_stack (using array_stack_destroy())
	// TODO: cleanup hp_records themselves

#if !defined(SYSTEM_TLS_SUPPORT)
	rig_tls_destroy(&RIG_SMR_HP_TLS_Key);
#endif
}

static atomic_ops_ptr  RIG_SMR_HP_List_Head = ATOMIC_OPS_PTR_INIT(NULL);
static atomic_ops_uint RIG_SMR_HP_List_Length = ATOMIC_OPS_UINT_INIT(0);


RIG_SMR_HP_Record rig_smr_hp_record_get(void) {
#if !defined(SYSTEM_TLS_SUPPORT)
	// Then we check if we already have a RIG_SMR_HP_Record saved for this thread
	RIG_SMR_HP_Record HP_Record = rig_tls_get(RIG_SMR_HP_TLS_Key);
#endif

	// If HP_Record == NULL, this thread has not registered any RIG_SMR_HP_Record structure
	if (HP_Record == NULL) {
		// Let's search if there's any old record lying around
		HP_Record = atomic_ops_ptr_load(&RIG_SMR_HP_List_Head, ATOMIC_OPS_FENCE_NONE);

		while (HP_Record != NULL) {
			if (atomic_ops_uint_load(&HP_Record->in_use, ATOMIC_OPS_FENCE_ACQUIRE) == 0
			 && atomic_ops_uint_cas(&HP_Record->in_use, 0, 1, ATOMIC_OPS_FENCE_ACQUIRE)) {
				// Got it!
				break;
			}

			HP_Record = HP_Record->next;
		}

		// Didn't find an old record, need to allocate one myself
		if (HP_Record == NULL) {
			HP_Record = rig_mem_alloc_aligned(sizeof(*HP_Record), 0, CACHELINE_SIZE, 0);
			NULLCHECK_EXIT(HP_Record);

			// Initialize values
			// UNROLLED LOOP FOLLOWS (based on RIG_SMR_HP_COUNT)
			atomic_ops_ptr_store(&HP_Record->HP[0], NULL, ATOMIC_OPS_FENCE_NONE);
			atomic_ops_ptr_store(&HP_Record->HP[1], NULL, ATOMIC_OPS_FENCE_NONE);
			atomic_ops_ptr_store(&HP_Record->HP[2], NULL, ATOMIC_OPS_FENCE_NONE);
			atomic_ops_ptr_store(&HP_Record->HP[3], NULL, ATOMIC_OPS_FENCE_NONE);
			atomic_ops_ptr_store(&HP_Record->HP[4], NULL, ATOMIC_OPS_FENCE_NONE);
			atomic_ops_ptr_store(&HP_Record->HP[5], NULL, ATOMIC_OPS_FENCE_NONE);
			atomic_ops_ptr_store(&HP_Record->HP[6], NULL, ATOMIC_OPS_FENCE_NONE);
			atomic_ops_ptr_store(&HP_Record->HP[7], NULL, ATOMIC_OPS_FENCE_NONE);

			array_stack_init(&HP_Record->retire_list, sizeof(void *));

			atomic_ops_uint_store(&HP_Record->in_use, 1, ATOMIC_OPS_FENCE_NONE);

			// Link the new RIG_SMR_HP_Record into the main RIG_SMR_HP_List
			atomic_ops_uint_inc(&RIG_SMR_HP_List_Length, ATOMIC_OPS_FENCE_FULL);

			while (true) {
				RIG_SMR_HP_Record head = atomic_ops_ptr_load(&RIG_SMR_HP_List_Head, ATOMIC_OPS_FENCE_NONE);

				HP_Record->next = head;

				if (atomic_ops_ptr_cas(&RIG_SMR_HP_List_Head, head, HP_Record, ATOMIC_OPS_FENCE_FULL)) {
					break;
				}
			}
		}

#if !defined(SYSTEM_TLS_SUPPORT)
		// Set the thread specific value correctly
		rig_tls_set(RIG_SMR_HP_TLS_Key, HP_Record);
#endif
	}

	return (HP_Record);
}

void rig_smr_hp_record_release(void) {
#if !defined(SYSTEM_TLS_SUPPORT)
	RIG_SMR_HP_Record HP_Record = rig_tls_get(RIG_SMR_HP_TLS_Key);
#endif

	// Retire RIG_SMR_HP_Record by disabling it and setting its thread
	// specific value to NULL, note we don't have to explicitly set
	// the HP records to NULL, as they get initialized to NULL when
	// the RIG_SMR_HP_Record is instantiated, and the algorithms must take
	// care of resetting them to NULL when they are finished using them,
	// so at this point they already should be all set to NULL.
	if (HP_Record != NULL) {
		// Ensure the HPs are NULLed (not resetting a HP to NULL after usage is an error!)
		for (size_t i = 0; i < RIG_SMR_HP_COUNT; i++) {
			rig_acheck_msg(atomic_ops_ptr_load(&HP_Record->HP[i], ATOMIC_OPS_FENCE_NONE) == NULL,
				"failed to set all Hazard Pointers back to NULL");
		}

		// Try to free what you can
		rig_smr_hp_mem_scan();

		// TODO: what to do with left-over memory? (we can retire once no HPs reference it anymore)

		atomic_ops_uint_store(&HP_Record->in_use, 0, ATOMIC_OPS_FENCE_RELEASE);

#if defined(SYSTEM_TLS_SUPPORT)
		HP_Record = NULL;
#else
		rig_tls_set(RIG_SMR_HP_TLS_Key, NULL);
#endif

		atomic_ops_fence(ATOMIC_OPS_FENCE_FULL);
	}
}

void *rig_smr_hp_get(RIG_SMR_HP_Record hp_record, size_t hp) {
	NULLCHECK_EXIT(hp_record);
	rig_acheck_msg(hp < RIG_SMR_HP_COUNT, "HP value too high");

	return (atomic_ops_ptr_load(&hp_record->HP[hp], ATOMIC_OPS_FENCE_ACQUIRE));
}

void rig_smr_hp_set(RIG_SMR_HP_Record hp_record, size_t hp, void *ptr) {
	NULLCHECK_EXIT(hp_record);
	rig_acheck_msg(hp < RIG_SMR_HP_COUNT, "HP value too high");

	atomic_ops_ptr_store(&hp_record->HP[hp], ptr, ATOMIC_OPS_FENCE_ACQUIRE);
}

void rig_smr_hp_release(RIG_SMR_HP_Record hp_record, size_t hp) {
	NULLCHECK_EXIT(hp_record);
	rig_acheck_msg(hp < RIG_SMR_HP_COUNT, "HP value too high");

	atomic_ops_ptr_store(&hp_record->HP[hp], NULL, ATOMIC_OPS_FENCE_RELEASE);
}

void rig_smr_hp_mem_retire(void *memory_ptr) {
	// If pointer is NULL, we do nothing at all, same as the standard free()
	if (memory_ptr != NULL) {
		RIG_SMR_HP_Record hp_record = rig_smr_hp_record_get();

		// Push the pointer onto the retire list and execute a scan of it,
		// if the retired pointers threshold was reached
		array_stack_push(&hp_record->retire_list, &memory_ptr);

		if (array_stack_count(&hp_record->retire_list) >= RIG_SMR_HP_THRESHOLD) {
			rig_smr_hp_mem_scan();
		}
	}
}

void rig_smr_hp_mem_retire_noscan(void *memory_ptr) {
	// If pointer is NULL, we do nothing at all, same as the standard free()
	if (memory_ptr != NULL) {
		RIG_SMR_HP_Record hp_record = rig_smr_hp_record_get();

		// Push the pointer onto the retire list, but don't execute any scan
		array_stack_push(&hp_record->retire_list, &memory_ptr);
	}
}

static int rig_smr_hp_ptr_cmp(const void *a, const void *b) {
	return ((*(const void * const *)a > *(const void * const *)b) - (*(const void * const *)a < *(const void * const *)b));
}

void rig_smr_hp_mem_scan(void) {
#if !defined(SYSTEM_TLS_SUPPORT)
	RIG_SMR_HP_Record HP_Record = rig_tls_get(RIG_SMR_HP_TLS_Key);
#endif

	// We only scan if the RIG_SMR_HP_Record is defined, as we only take
	// the pointers in the thread specific retire list into consideration here,
	// for a more forceful/complete approach, see rig_smr_hp_mem_scan_full().
	// Also check that we have pointers in retire list to actually check, as it's
	// possible there are none, and that would trigger the zero-heap check.
	if ((HP_Record != NULL) && (array_stack_count(&HP_Record->retire_list) != 0)) {
		// Pop all pointers from the thread specific retire list
		size_t tmp_retire_list_length = 0;
		void **tmp_retire_list = rig_mem_alloc(0, array_stack_count(&HP_Record->retire_list) * sizeof(void *));
		NULLCHECK_EXIT(tmp_retire_list);

		void **ptr;
		while ((ptr = array_stack_pop(&HP_Record->retire_list)) != NULL) {
			tmp_retire_list[tmp_retire_list_length] = *ptr;
			tmp_retire_list_length++;
		}

		// Get the global data
		RIG_SMR_HP_Record curr = atomic_ops_ptr_load(&RIG_SMR_HP_List_Head, ATOMIC_OPS_FENCE_ACQUIRE);

		// Copy all hazard pointer values from active RIG_SMR_HP_Records
		size_t tmp_hp_list_len = 0;
		void **tmp_hp_list = rig_mem_alloc(0, atomic_ops_uint_load(&RIG_SMR_HP_List_Length, ATOMIC_OPS_FENCE_NONE) * RIG_SMR_HP_COUNT * sizeof(void *));
		NULLCHECK_EXIT(tmp_hp_list);

		while (curr != NULL) {
			if (atomic_ops_uint_load(&curr->in_use, ATOMIC_OPS_FENCE_ACQUIRE) == 1) {
				// UNROLLED LOOP FOLLOWS (based on RIG_SMR_HP_COUNT)
				tmp_hp_list[tmp_hp_list_len] = atomic_ops_ptr_load(&curr->HP[0], ATOMIC_OPS_FENCE_NONE);
				tmp_hp_list_len++;
				tmp_hp_list[tmp_hp_list_len] = atomic_ops_ptr_load(&curr->HP[1], ATOMIC_OPS_FENCE_NONE);
				tmp_hp_list_len++;
				tmp_hp_list[tmp_hp_list_len] = atomic_ops_ptr_load(&curr->HP[2], ATOMIC_OPS_FENCE_NONE);
				tmp_hp_list_len++;
				tmp_hp_list[tmp_hp_list_len] = atomic_ops_ptr_load(&curr->HP[3], ATOMIC_OPS_FENCE_NONE);
				tmp_hp_list_len++;
				tmp_hp_list[tmp_hp_list_len] = atomic_ops_ptr_load(&curr->HP[4], ATOMIC_OPS_FENCE_NONE);
				tmp_hp_list_len++;
				tmp_hp_list[tmp_hp_list_len] = atomic_ops_ptr_load(&curr->HP[5], ATOMIC_OPS_FENCE_NONE);
				tmp_hp_list_len++;
				tmp_hp_list[tmp_hp_list_len] = atomic_ops_ptr_load(&curr->HP[6], ATOMIC_OPS_FENCE_NONE);
				tmp_hp_list_len++;
				tmp_hp_list[tmp_hp_list_len] = atomic_ops_ptr_load(&curr->HP[7], ATOMIC_OPS_FENCE_NONE);
				tmp_hp_list_len++;
			}

			curr = curr->next;
		}

		// Save pointer to tmp_hp_list for later freeing, before it gets changed
		void *tmp_hp_list_orig = tmp_hp_list;

		// Sort values in tmp_hp_list for faster lookup later
		qsort(tmp_hp_list, tmp_hp_list_len, sizeof(void *), &rig_smr_hp_ptr_cmp);

		// Discard NULL values at the front, shrinking the size of tmp_hp_list as
		// you go, if tmp_hp_list_len reaches 0, there are no active HPs at all left
		while ((*tmp_hp_list == NULL) && (tmp_hp_list_len > 0)) {
			tmp_hp_list++;
			tmp_hp_list_len--;
		}

		if (tmp_hp_list_len == 0) {
			// If there are no active HPs at all, we can just free everything
			// directly, as you can't find elements in an empty array!
			for (size_t i = 0; i < tmp_retire_list_length; i++) {
				rig_mem_free(tmp_retire_list[i]);
			}
		}
		else {
			// Lookup retire list values in tmp_hp_list, if not present, we can safely
			// recycle the memory (free() it), else we re-add it back to the retire list
			// for later scan passes to examine
			for (size_t i = 0; i < tmp_retire_list_length; i++) {
				if (bsearch(&tmp_retire_list[i], tmp_hp_list, tmp_hp_list_len, sizeof(void *), &rig_smr_hp_ptr_cmp) == NULL) {
					rig_mem_free(tmp_retire_list[i]);
				}
				else {
					array_stack_push(&HP_Record->retire_list, &tmp_retire_list[i]);
				}
			}
		}

		// Free used resources
		rig_mem_free(tmp_retire_list);
		rig_mem_free(tmp_hp_list_orig);
	}
}

void rig_smr_hp_mem_scan_full(void) {
#if !defined(SYSTEM_TLS_SUPPORT)
	RIG_SMR_HP_Record HP_Record = rig_tls_get(RIG_SMR_HP_TLS_Key);
#endif
	bool hp_record_borrowed = false;

	// If we have no RIG_SMR_HP_Record, lets see if there is a free one around
	if (HP_Record == NULL) {
		HP_Record = atomic_ops_ptr_load(&RIG_SMR_HP_List_Head, ATOMIC_OPS_FENCE_NONE);

		while (HP_Record != NULL) {
			if (atomic_ops_uint_load(&HP_Record->in_use, ATOMIC_OPS_FENCE_ACQUIRE) == 0
			 && atomic_ops_uint_cas(&HP_Record->in_use, 0, 1, ATOMIC_OPS_FENCE_ACQUIRE)) {
				// Got it!
				break;
			}

			HP_Record = HP_Record->next;
		}

		if (HP_Record == NULL) {
			// There are no inactive records right now, so nothing to scan
			return;
		}

		// Set this so we can retire this record once finished
		hp_record_borrowed = true;
	}

	// Lets search for other inactive RIG_SMR_HP_Records, lock them, pop their
	// memory pointers, and unlock them again
	RIG_SMR_HP_Record curr = atomic_ops_ptr_load(&RIG_SMR_HP_List_Head, ATOMIC_OPS_FENCE_NONE);

	while (curr != NULL) {
		if (atomic_ops_uint_load(&curr->in_use, ATOMIC_OPS_FENCE_ACQUIRE) == 0
		 && atomic_ops_uint_cas(&curr->in_use, 0, 1, ATOMIC_OPS_FENCE_ACQUIRE)) {
			// Locked it! Lets pop its pointers ...
			void **ptr;
			while ((ptr = array_stack_pop(&curr->retire_list)) != NULL) {
				array_stack_push(&HP_Record->retire_list, ptr);
			}

			atomic_ops_uint_store(&curr->in_use, 0, ATOMIC_OPS_FENCE_RELEASE);
		}

		curr = curr->next;
	}

	// Force scan now
	rig_smr_hp_mem_scan();

	// If we borrowed this RIG_SMR_HP_Record just for the full scan, we
	// set it back to inactive now that we're finished
	if (hp_record_borrowed) {
		atomic_ops_uint_store(&HP_Record->in_use, 0, ATOMIC_OPS_FENCE_RELEASE);

#if defined(SYSTEM_TLS_SUPPORT)
		HP_Record = NULL;

		atomic_ops_fence(ATOMIC_OPS_FENCE_FULL);
#endif
	}
}

void rig_smr_hp_debug_info(bool print_list) {
	RIG_SMR_HP_Record curr = atomic_ops_ptr_load(&RIG_SMR_HP_List_Head, ATOMIC_OPS_FENCE_ACQUIRE);

	printf("SMR_HP list head address: %p\n", (void *)&RIG_SMR_HP_List_Head);
	printf("SMR_HP list head value: %p\n", (void *)curr);
	printf("SMR_HP list length address: %p\n", (void *)&RIG_SMR_HP_List_Length);
	printf("SMR_HP list length value: %zu\n\n", atomic_ops_uint_load(&RIG_SMR_HP_List_Length, ATOMIC_OPS_FENCE_NONE));

	if (print_list) {
		while (curr != NULL) {
			printf("== SMR_HP_Record at %p ==\n", (void *)curr);

			for (size_t i = 0; i < RIG_SMR_HP_COUNT; i++) {
				printf("HP[%zu] = %p\n", i, atomic_ops_ptr_load(&curr->HP[i], ATOMIC_OPS_FENCE_NONE));
			}

			printf("retire list count = %zu\n", array_stack_count(&curr->retire_list));

			printf("in_use = %zu\n\n", atomic_ops_uint_load(&curr->in_use, ATOMIC_OPS_FENCE_NONE));

			curr = curr->next;
		}
	}
}
