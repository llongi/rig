/*
 * Rig RWLock Data Definitions
 */

/** Configuration */
#define MAX_READERS 256
#define OWNER_CHECKING 1

/** Cleanup Functions */
static void rwlock_cleanup_decrement(void *dec);

/** Structures */
struct rig_rwlock {
	uint16_t flags;
	uint16_t active[2];
	uint16_t waiting[2];
	RIG_MUTEX lock;
	RIG_COND readers;
	RIG_COND writers;
#if OWNER_CHECKING == 1
	RIG_TLS owned;
	RIG_LIST memalloc;
#endif
};

/** Enums */
enum RW_NAMES { READERS = 0, WRITERS = 1 };

/** Macros */
#define WRITER_BIAS (TEST_BITFIELD(rwl->flags, RIG_RWLOCK_PREFER_READERS)) ? (false) : (rwl->waiting[WRITERS] > 0)
#define READER_BIAS (TEST_BITFIELD(rwl->flags, RIG_RWLOCK_PREFER_READERS)) ? (rwl->waiting[READERS] > 0) : (false)

#define GET_OR_INIT_TLS(key, val, rwl) \
	int32_t *val = rig_tls_get(key); \
	if (val == NULL) { \
		val = malloc(sizeof(*val)); \
		if (val == NULL) { \
			rig_mutex_unlock(rwl->lock); \
			ERRET(ENOMEM, false); \
		} \
		*val = 0; \
		if (!rig_tls_set(key, val)) { \
			free(val); \
			rig_mutex_unlock(rwl->lock); \
			ERRET(EFAULT, false); \
		} \
		if (!rig_list_add(rwl->memalloc, &val)) { \
			free(val); \
			rig_mutex_unlock(rwl->lock); \
			ERRET(ENOMEM, false); \
		} \
	}

/*
 * Rig RWLock Implementation
 */

static void rwlock_cleanup_decrement(void *dec) {
	uint16_t *decp = dec;
	(*decp)--;
}

RIG_RWLOCK rig_rwlock_init(uint16_t flags) {
	// Allocate memory for the RWLOCK struct
	RIG_RWLOCK rwl = malloc(sizeof(*rwl));
	if (rwl == NULL) {
		ERRET(ENOMEM, NULL);
	}

	// Initialize the mutex lock
	rwl->lock = rig_mutex_init(0);
	if (rwl->lock == NULL) {
		FREENU(rwl);
		ERRET(ENOLCK, NULL);
	}

	// Initialize the condition variable for readers
	rwl->readers = rig_cond_init(0);
	if (rwl->readers == NULL) {
		rig_mutex_destroy(&rwl->lock);
		FREENU(rwl);
		ERRET(ENOLCK, NULL);
	}

	// Initialize the condition variable for writers
	rwl->writers = rig_cond_init(0);
	if (rwl->writers == NULL) {
		rig_cond_destroy(&rwl->readers);
		rig_mutex_destroy(&rwl->lock);
		FREENU(rwl);
		ERRET(ENOLCK, NULL);
	}

#if OWNER_CHECKING == 1
	// Initialize TLS for owned locks
	// -1 means "this thread owns a write-lock"
	// 0 means "this thread holds no lock"
	// 1 means "this thread owns a read-lock"
	// 2 and more means "this thread owns as many read-locks"
	rwl->owned = rig_tls_init();
	if (rwl->owned == NULL) {
		rig_cond_destroy(&rwl->writers);
		rig_cond_destroy(&rwl->readers);
		rig_mutex_destroy(&rwl->lock);
		FREENU(rwl);
		ERRET(ENOLCK, NULL);
	}

	// Initialize the list that keeps track of allocated TLS memory
	// This list need not be thread-safe, as we only add to it when we've
	// already got the RWL mutex, and only read/remove from it during destroy()
	rwl->memalloc = rig_list_init(0, sizeof(int32_t *), 0, NULL, NULL);
	if (rwl->memalloc == NULL) {
		rig_tls_destroy(&rwl->owned);
		rig_cond_destroy(&rwl->writers);
		rig_cond_destroy(&rwl->readers);
		rig_mutex_destroy(&rwl->lock);
		FREENU(rwl);
		ERRET(ENOMEM, NULL);
	}
#endif

	// Set the needed values
	rwl->flags = flags;
	rwl->active[READERS] = 0;
	rwl->active[WRITERS] = 0;
	rwl->waiting[READERS] = 0;
	rwl->waiting[WRITERS] = 0;

	return (rwl);
}

bool rig_rwlock_destroy(RIG_RWLOCK *rwl) {
	// Parameter validation
	NULLCHECK_EXIT(rwl);
	NULLCHECK_EXIT(*rwl);

	rig_mutex_lock((*rwl)->lock);

	// Check that no one is still using the lock
	if ((*rwl)->active[READERS] > 0
	|| (*rwl)->active[WRITERS] > 0
	|| (*rwl)->waiting[READERS] > 0
	|| (*rwl)->waiting[WRITERS] > 0) {
		rig_mutex_unlock((*rwl)->lock);
		ERRET(EBUSY, false);
	}

	rig_mutex_unlock((*rwl)->lock);

	rig_mutex_destroy(&(*rwl)->lock);
	rig_cond_destroy(&(*rwl)->readers);
	rig_cond_destroy(&(*rwl)->writers);
#if OWNER_CHECKING == 1
	rig_tls_destroy(&(*rwl)->owned);
	// TODO: fix memory deallocation from list on destroy(),
	// meaning we need to iterate over all list elements here,
	// free them, and clear out the list
	rig_list_destroy(&(*rwl)->memalloc);
#endif
	FREENU(*rwl);

	return (true);
}

bool rig_rwlock_rdlock(RIG_RWLOCK rwl) {
	// Parameter validation
	NULLCHECK_EXIT(rwl);

	rig_mutex_lock(rwl->lock);

#if OWNER_CHECKING == 1
	// Get and eventually initialize TLS values correctly
	GET_OR_INIT_TLS(rwl->owned, owned_locks, rwl);

	// Deadlock detection, check if this thread already holds a write-lock
	if (*owned_locks == -1) {
		rig_mutex_unlock(rwl->lock);
		ERRET(EDEADLK, false);
	}
#endif

	if (rwl->active[WRITERS] == 1 || rwl->active[READERS] >= MAX_READERS || WRITER_BIAS) {
		rwl->waiting[READERS]++;

		RIG_THREAD_CLEANUP_PUSH(rwlock_cleanup_decrement, rwl->waiting[READERS]);
		while (rwl->active[WRITERS] == 1 || rwl->active[READERS] >= MAX_READERS || WRITER_BIAS) {
			rig_cond_wait(rwl->readers, rwl->lock);
		}
		RIG_THREAD_CLEANUP_POP;

		rwl->waiting[READERS]--;
	}

	rwl->active[READERS]++;
#if OWNER_CHECKING == 1
	(*owned_locks)++;
#endif

	rig_mutex_unlock(rwl->lock);

	return (true);
}

bool rig_rwlock_tryrdlock(RIG_RWLOCK rwl) {
	// Parameter validation
	NULLCHECK_EXIT(rwl);

	if(!rig_mutex_trylock(rwl->lock)) {
		ERRET(EBUSY, false);
	}

#if OWNER_CHECKING == 1
	// Get and eventually initialize TLS values correctly
	GET_OR_INIT_TLS(rwl->owned, owned_locks, rwl);

	// Deadlock detection, check if this thread already holds a write-lock
	if (*owned_locks == -1) {
		rig_mutex_unlock(rwl->lock);
		ERRET(EDEADLK, false);
	}
#endif

	if (rwl->active[WRITERS] == 1 || rwl->active[READERS] >= MAX_READERS || WRITER_BIAS) {
		rig_mutex_unlock(rwl->lock);
		ERRET(EBUSY, false);
	}

	rwl->active[READERS]++;
#if OWNER_CHECKING == 1
	(*owned_locks)++;
#endif

	rig_mutex_unlock(rwl->lock);

	return (true);
}

bool rig_rwlock_timedrdlock(RIG_RWLOCK rwl, uint32_t milliseconds) {
// TODO: add timed functions support
}

bool rig_rwlock_wrlock(RIG_RWLOCK rwl) {
	// Parameter validation
	NULLCHECK_EXIT(rwl);

	rig_mutex_lock(rwl->lock);

#if OWNER_CHECKING == 1
	// Get and eventually initialize TLS values correctly
	GET_OR_INIT_TLS(rwl->owned, owned_locks, rwl);

	// Deadlock detection, check if this thread already holds a write- or read-lock
	if (*owned_locks != 0) {
		rig_mutex_unlock(rwl->lock);
		ERRET(EDEADLK, false);
	}
#endif

	if (rwl->active[WRITERS] == 1 || rwl->active[READERS] > 0 || READER_BIAS) {
		rwl->waiting[WRITERS]++;

		RIG_THREAD_CLEANUP_PUSH(rwlock_cleanup_decrement, rwl->waiting[WRITERS]);
		while (rwl->active[WRITERS] == 1 || rwl->active[READERS] > 0 || READER_BIAS) {
			rig_cond_wait(rwl->writers, rwl->lock);
		}
		RIG_THREAD_CLEANUP_POP;

		rwl->waiting[WRITERS]--;
	}

	rwl->active[WRITERS] = 1;
#if OWNER_CHECKING == 1
	*owned_locks = -1;
#endif

	rig_mutex_unlock(rwl->lock);

	return (true);
}

bool rig_rwlock_trywrlock(RIG_RWLOCK rwl) {
	// Parameter validation
	NULLCHECK_EXIT(rwl);

	if(!rig_mutex_trylock(rwl->lock)) {
		ERRET(EBUSY, false);
	}

#if OWNER_CHECKING == 1
	// Get and eventually initialize TLS values correctly
	GET_OR_INIT_TLS(rwl->owned, owned_locks, rwl);

	// Deadlock detection, check if this thread already holds a write- or read-lock
	if (*owned_locks != 0) {
		rig_mutex_unlock(rwl->lock);
		ERRET(EDEADLK, false);
	}
#endif

	if (rwl->active[WRITERS] == 1 || rwl->active[READERS] > 0 || READER_BIAS) {
		rig_mutex_unlock(rwl->lock);
		ERRET(EBUSY, false);
	}

	rwl->active[WRITERS] = 1;
#if OWNER_CHECKING == 1
	*owned_locks = -1;
#endif

	rig_mutex_unlock(rwl->lock);

	return (true);
}

bool rig_rwlock_timedwrlock(RIG_RWLOCK rwl, uint32_t milliseconds) {
// TODO: add timed functions support
}

bool rig_rwlock_unlock(RIG_RWLOCK rwl) {
	// Parameter validation
	NULLCHECK_EXIT(rwl);

	rig_mutex_lock(rwl->lock);

#if OWNER_CHECKING == 1
	// Get and eventually initialize TLS values correctly
	GET_OR_INIT_TLS(rwl->owned, owned_locks, rwl);
#endif

	if (rwl->active[WRITERS] == 1) {
#if OWNER_CHECKING == 1
		// Owner checking
		if (*owned_locks != -1) {
			rig_mutex_unlock(rwl->lock);
			ERRET(EPERM, false);
		}

		*owned_locks = 0;
#endif
		rwl->active[WRITERS] = 0;

		if (TEST_BITFIELD(rwl->flags, RIG_RWLOCK_PREFER_READERS)) {
			if (rwl->waiting[READERS] > 0) {
				rig_cond_signal_all(rwl->readers);
			}
			else if (rwl->waiting[WRITERS] > 0) {
				rig_cond_signal(rwl->writers);
			}
		}
		else {
			if (rwl->waiting[WRITERS] > 0) {
				rig_cond_signal(rwl->writers);
			}
			else if (rwl->waiting[READERS] > 0) {
				rig_cond_signal_all(rwl->readers);
			}
		}
	}
	else if (rwl->active[READERS] > 0) {
#if OWNER_CHECKING == 1
		// Owner checking
		if (*owned_locks <= 0) {
			rig_mutex_unlock(rwl->lock);
			ERRET(EPERM, false);
		}

		(*owned_locks)--;
#endif
		rwl->active[READERS]--;

		if (TEST_BITFIELD(rwl->flags, RIG_RWLOCK_PREFER_READERS)) {
			if (rwl->waiting[READERS] > 0) {
				rig_cond_signal_all(rwl->readers);
			}
			else if (rwl->active[READERS] == 0
			&& rwl->waiting[WRITERS] > 0) {
				rig_cond_signal(rwl->writers);
			}
		}
		else {
			if (rwl->active[READERS] == 0
			&& rwl->waiting[WRITERS] > 0) {
				rig_cond_signal(rwl->writers);
			}
			else if (rwl->waiting[READERS] > 0) {
				rig_cond_signal_all(rwl->readers);
			}
		}
	}
	else {
		rig_mutex_unlock(rwl->lock);
		ERRET(EPERM, false);
	}

	rig_mutex_unlock(rwl->lock);

	return (true);
}
