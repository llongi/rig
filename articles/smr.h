#include <pthread.h>
#include <assert.h>

#define SMR_HP_Count 3 // HP[0]: curr, HP[1]: prev, HP[2]: succ / misc
#define SMR_RN_BlSize 16 // must be power of 2 (mask)
#define SMR_RN_Thresh 32 // multiple of BlSize (related to HP_Count * Thread_Count)

typedef struct SMR_HPRecord_Struct *SMR_HPRecord;
typedef struct SMR_RNBlock_Struct *SMR_RNBlock;

struct SMR_HPRecord_Struct {
	SMR_HPRecord next;
	SBB_A_UINT active;
	SBB_A_PTR HP[SMR_HP_Count];
	size_t rcount;
	SMR_RNBlock rlist;
};

struct SMR_RNBlock_Struct {
	SMR_RNBlock next;
	void *list[SMR_RN_BlSize];
};

#define SMR_TLSKEY_INIT_TODO 0
#define SMR_TLSKEY_INIT_INPROG 1
#define SMR_TLSKEY_INIT_DONE 2

static pthread_key_t SMR_TlsKey;
static SBB_A_UINT    SMR_TlsKey_Init = SBB_A_UINT_INIT(SMR_TLSKEY_INIT_TODO);

static SBB_A_PTR  SMR_HPListHead = SBB_A_PTR_INIT(NULL);
static SBB_A_UINT SMR_HPListLength = SBB_A_UINT_INIT(0);

SMR_HPRecord smr_get_hprecord(void);
void smr_set_hp(SMR_HPRecord hprec, size_t hp, void *ptr);
void smr_retire_mem(void *ptr);
void smr_retire_hprecord(void);
void smr_scan(void);
void smr_scan_full(void);

static inline void smr_rnblock_push(SMR_HPRecord hprec, void *ptr);
static inline void *smr_rnblock_pop(SMR_HPRecord hprec);
static inline size_t smr_array_sort(void **array, size_t array_len);
static inline bool smr_array_lookup(void **array, size_t array_len, void *key);
static void smr_tlskey_create(void);
static void smr_tlskey_delete(void);

SMR_HPRecord smr_get_hprecord(void) {
	// First we need to make sure the TLS key is correctly initialized
	if (sbb_atomic_uint_get(&SMR_TlsKey_Init) != SMR_TLSKEY_INIT_DONE) {
		smr_tlskey_create();
	}

	// Then we check if we already have a SMR_HPRecord saved for this thread
	SMR_HPRecord hprec = pthread_getspecific(SMR_TlsKey);

	// If hprec == NULL, this thread has not registered any SMR_HPRecord struct
	if (hprec == NULL) {
		// Let's search if there's any old record lying around
		hprec = sbb_atomic_ptr_get(&SMR_HPListHead);
		while (hprec != NULL) {
			if (sbb_atomic_uint_get(&hprec->active) == 0) {
				// Found an inactive record, try to use it for myself
				if (sbb_atomic_uint_cas(&hprec->active, 0, 1)) {
					// Got it!
					break;
				}
			}

			hprec = hprec->next;
		}

		// Didn't find an old record, need to allocate one myself
		if (hprec == NULL) {
			hprec = malloc(sizeof(*hprec));
			if (hprec == NULL) {
				// There's no recovery from failure here, the SMR_HPRecord
				// struct is essential to SMR
				exit(EXIT_FAILURE);
			}

			// Initialize values
			sbb_atomic_uint_set(&hprec->active, 1);

			// UNROLLED LOOP FOLLOWS (based on SMR_HP_Count)
			sbb_atomic_ptr_set(&hprec->HP[0], NULL);
			sbb_atomic_ptr_set(&hprec->HP[1], NULL);
			sbb_atomic_ptr_set(&hprec->HP[2], NULL);

			hprec->rcount = 0;
			hprec->rlist = NULL;

			// Link the new SMR_HPRecord into the main SMR_HPList
			sbb_atomic_uint_inc(&SMR_HPListLength);

			while (true) {
				SMR_HPRecord head = sbb_atomic_ptr_get(&SMR_HPListHead);

				hprec->next = head;

				if (sbb_atomic_ptr_cas(&SMR_HPListHead, head, hprec)) {
					break;
				}
			}
		}

		// Set the thread specific value correctly
		pthread_setspecific(SMR_TlsKey, hprec);
	}

	return (hprec);
}

void smr_set_hp(SMR_HPRecord hprec, size_t hp, void *ptr) {
	assert(hprec != NULL);
	assert(hp < SMR_HP_Count);

	sbb_atomic_ptr_set(&hprec->HP[hp], ptr);
}

void smr_retire_mem(void *ptr) {
	// If ptr is NULL, we do nothing at all, same as the standard free()
	if (ptr != NULL) {
		if (sbb_atomic_uint_get(&SMR_TlsKey_Init) == SMR_TLSKEY_INIT_DONE) {
			// TLS Key exists, so lets get an SMR_HPRecord
			SMR_HPRecord hprec = smr_get_hprecord();

			// Push the ptr onto the rlist and execute a scan of it,
			// if the retired ptrs threshold was reached
			smr_rnblock_push(hprec, ptr);

			if (hprec->rcount >= SMR_RN_Thresh) {
				smr_scan();
			}
		}
		else {
			// If there is no TLS Key in place, there is no active SMR
			// system, so we can just pass the ptr directly to free()
			free(ptr);
		}
	}
}

void smr_retire_hprecord(void) {
	// If there is no TLS key at all, or if there is but the thread specific
	// value is NULL, there can't be a SMR_HPRecord to retire, so we do nothing
	if (sbb_atomic_uint_get(&SMR_TlsKey_Init) == SMR_TLSKEY_INIT_DONE) {
		SMR_HPRecord hprec = pthread_getspecific(SMR_TlsKey);

		if (hprec != NULL) {
			// Retire SMR_HPRecord by deactivating it and setting its thread
			// specific value to NULL, note we don't have to explicitely set
			// the HP records to NULL, as they get initialized to NULL when
			// the SMR_HPRecord is instanciated, and the algorithms must take
			// care of resetting them to NULL when they are finished using them,
			// so at this point they already must be all set to NULL
			sbb_atomic_uint_set(&hprec->active, 0);
			pthread_setspecific(SMR_TlsKey, NULL);
		}
	}
}

void smr_scan(void) {
	// Nothing to scan if there is no TLS key (and thus no SMR system in place)
	if (sbb_atomic_uint_get(&SMR_TlsKey_Init) == SMR_TLSKEY_INIT_DONE) {
		SMR_HPRecord hprec = pthread_getspecific(SMR_TlsKey);

		// We only scan if the SMR_HPRecord is defined, as we only take
		// the ptrs in the thread specific rlist into consideration here,
		// for a more forceful/complete approach, see smr_scan_full()
		if (hprec != NULL) {
			// Pop all ptrs from the thread specific rlist
			size_t tmp_rcount = 0;
			void **tmp_rlist = malloc(sizeof(void *) * hprec->rcount);
			if (tmp_rlist == NULL) {
				// There's no recovery from failure here
				exit(EXIT_FAILURE);
			}

			void *ptr = smr_rnblock_pop(hprec);
			while (ptr != NULL) {
				tmp_rlist[tmp_rcount++] = ptr;
				ptr = smr_rnblock_pop(hprec);
			}

			// Get the global data
			SMR_HPRecord curr = sbb_atomic_ptr_get(&SMR_HPListHead);

			// Copy all hazard pointer values from active SMR_HPRecords
			size_t tmp_hpcount = 0;
			void **tmp_hplist = malloc(sizeof(void *) * SMR_HP_Count * sbb_atomic_uint_get(&SMR_HPListLength));
			if (tmp_hplist == NULL) {
				// There's no recovery from failure here
				exit(EXIT_FAILURE);
			}

			while (curr != NULL) {
				if (sbb_atomic_uint_get(&curr->active) == 1) {
					// UNROLLED LOOP FOLLOWS (based on SMR_HP_Count)
					tmp_hplist[tmp_hpcount++] = sbb_atomic_ptr_get(&curr->HP[0]);
					tmp_hplist[tmp_hpcount++] = sbb_atomic_ptr_get(&curr->HP[1]);
					tmp_hplist[tmp_hpcount++] = sbb_atomic_ptr_get(&curr->HP[2]);
				}

				curr = curr->next;
			}

			// Sort values in tmp_hplist for faster lookup later
			tmp_hpcount = smr_array_sort(tmp_hplist, tmp_hpcount);

			// Lookup rlist values in hplist, if not present, we can safely
			// recycle the memory (free()), else we re-add it back to rlist
			// for later scan passes to examine
			for (size_t i = 0; i < tmp_rcount; i++) {
				if (smr_array_lookup(tmp_hplist, tmp_hpcount, tmp_rlist[i])) {
					smr_rnblock_push(hprec, tmp_rlist[i]);
				}
				else {
					free(tmp_rlist[i]);
				}
			}

			// Free used resources
			free(tmp_rlist);
			free(tmp_hplist);
		}
	}
}

void smr_scan_full(void) {
	// Nothing to scan if there is no TLS key (and thus no SMR system in place)
	if (sbb_atomic_uint_get(&SMR_TlsKey_Init) == SMR_TLSKEY_INIT_DONE) {
		SMR_HPRecord hprec = pthread_getspecific(SMR_TlsKey);
		bool hprec_borrowed = false;

		// If we have no SMR_HPRecord, lets see if there is a free one around
		if (hprec == NULL) {
			hprec = sbb_atomic_ptr_get(&SMR_HPListHead);
			while (hprec != NULL) {
				if (sbb_atomic_uint_get(&hprec->active) == 0) {
					// Found an inactive record, try to use it for myself
					if (sbb_atomic_uint_cas(&hprec->active, 0, 1)) {
						// Got it!
						break;
					}
				}

				hprec = hprec->next;
			}

			if (hprec == NULL) {
				// There are no inactive records right now, so nothing to scan
				return;
			}

			// Set this so we can retire this record once finished
			hprec_borrowed = true;
		}

		// Lets search for other inactive SMR_HPRecords, lock them, pop their
		// ptrs, and unlock them again
		SMR_HPRecord curr = sbb_atomic_ptr_get(&SMR_HPListHead);
		while (curr != NULL) {
			if (sbb_atomic_uint_get(&curr->active) == 0) {
				// Found an inactive record, try to lock it
				if (sbb_atomic_uint_cas(&curr->active, 0, 1)) {
					// Locked it! Lets pop its ptrs ...
					void *ptr = smr_rnblock_pop(curr);
					while (ptr != NULL) {
						smr_rnblock_push(hprec, ptr);
						ptr = smr_rnblock_pop(curr);
					}

					// Unlock it
					sbb_atomic_uint_set(&curr->active, 0);
				}
			}

			curr = curr->next;
		}

		// Force scan now
		smr_scan();

		// If we borrowed this SMR_HPRecord just for the full scan, we
		// set it back to inactive now that we're finished
		if (hprec_borrowed) {
			sbb_atomic_uint_set(&hprec->active, 0);
		}
	}
}

static inline void smr_rnblock_push(SMR_HPRecord hprec, void *ptr) {
	assert(hprec != NULL);
	assert(ptr != NULL);

	// Take care of allocating new blocks when needed
	if ((hprec->rcount & (SMR_RN_BlSize - 1)) == 0) {
		SMR_RNBlock rnblock = malloc(sizeof(*rnblock));
		if (rnblock == NULL) {
			// There's no recovery from failure here, the SMR_RNBlock struct
			// is essential to SMR
			exit(EXIT_FAILURE);
		}

		// Link the new SMR_RNBlock to the struct
		rnblock->next = hprec->rlist;
		hprec->rlist = rnblock;
	}

	hprec->rlist->list[(hprec->rcount & (SMR_RN_BlSize - 1))] = ptr;
	hprec->rcount++;
}

static inline void *smr_rnblock_pop(SMR_HPRecord hprec) {
	assert(hprec != NULL);

	if (hprec->rcount == 0) {
		// No more values to pop
		return (NULL);
	}

	hprec->rcount--;
	void *ptr = hprec->rlist->list[(hprec->rcount & (SMR_RN_BlSize - 1))];

	// Take care of freeing empty blocks
	if ((hprec->rcount & (SMR_RN_BlSize - 1)) == 0) {
		// Unlink empty block from the list and free it
		SMR_RNBlock rnblock = hprec->rlist;
		hprec->rlist = rnblock->next;
		free(rnblock);
	}

	return (ptr);
}

static inline size_t smr_array_sort(void **array, size_t array_len) {
	assert(array != NULL);

	// Simple Insertion Sort implementation, as we don't expect large arrays,
	// it also takes care of ignoring NULL values by putting them at the end
	// and telling how many non-NULL values there are in the array before them
	size_t non_null = 0;

	for (size_t ins_idx = 0, rem_idx = 0; rem_idx < array_len; rem_idx++) {
		void *rem_val = array[rem_idx];
		ins_idx = rem_idx;
		while ((ins_idx > 0) && (array[ins_idx - 1] < rem_val)) {
			array[ins_idx] = array[ins_idx - 1];
			ins_idx--;
		}
		array[ins_idx] = rem_val;

		if (rem_val != NULL) {
			non_null++;
		}
	}

	return (non_null);
}

static inline bool smr_array_lookup(void **array, size_t array_len, void *key) {
	assert(array != NULL);

	if (array_len == 0) {
		return (false);
	}

	size_t low = 0;
	size_t high = array_len - 1;
	size_t mid;

	while (low < high) {
		mid = (low + high + 1) >> 1;

		if (array[mid] > key) {
			low = mid + 1;
		}
		else if (array[mid] < key) {
			high = mid - 1;
		}
		else {
			return (true);
		}
	}

	if (low == high && array[low] == key) {
		return (true);
	}

	return (false);
}

static void smr_tlskey_create(void) {
	if (sbb_atomic_uint_get(&SMR_TlsKey_Init) != SMR_TLSKEY_INIT_INPROG) {
		if (sbb_atomic_uint_cas(&SMR_TlsKey_Init, SMR_TLSKEY_INIT_TODO, SMR_TLSKEY_INIT_INPROG)) {
			// The TLS key must be initialized only once, only one thread can get here
			if (pthread_key_create(&SMR_TlsKey, NULL)) {
				// There's no recovery from failure here, the SMR_TlsKey is essential to SMR
				exit(EXIT_FAILURE);
			}

			atexit(&smr_tlskey_delete);
			atexit(&smr_scan_full);

			// TLS key initialization done!
			sbb_atomic_uint_set(&SMR_TlsKey_Init, SMR_TLSKEY_INIT_DONE);
		}
	}

	while (sbb_atomic_uint_get(&SMR_TlsKey_Init) != SMR_TLSKEY_INIT_DONE) { ; }
}

static void smr_tlskey_delete(void) {
	if (sbb_atomic_uint_get(&SMR_TlsKey_Init) == SMR_TLSKEY_INIT_DONE) {
		pthread_key_delete(SMR_TlsKey);
	}
}
