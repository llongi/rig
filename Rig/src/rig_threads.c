/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_threads.c 1084 2012-06-09 00:38:04Z llongi $
 */

#include "rig_internal.h"
#include <atomic_ops.h>

// Internal flags
#define RIG_THREAD_STARTED ((uint16_t)(1 << 15))

// Internal functions
static inline bool thread_ops_start(RIG_THREAD thr);
static inline bool thread_ops_join(RIG_THREAD thr, void *retval[]);
static inline bool thread_ops_detach(RIG_THREAD thr);

static inline bool thread_ops_tls_init(RIG_TLS tls);
static inline bool thread_ops_tls_destroy(RIG_TLS tls);

static inline bool thread_ops_mxlock_init(RIG_MXLOCK mxl);
static inline bool thread_ops_mxlock_destroy(RIG_MXLOCK mxl);

static inline bool thread_ops_condvar_init(RIG_CONDVAR cond);
static inline bool thread_ops_condvar_destroy(RIG_CONDVAR cond);

static inline bool thread_ops_rwlock_init(RIG_RWLOCK rwl);
static inline bool thread_ops_rwlock_destroy(RIG_RWLOCK rwl);

static void *rig_thread_starter(void *thr);
static void rig_thread_cleanup(void *arg);

// Include OS/threading-library specific implementations
#if defined(HAVE_PTHREADS)
	#include "thread_ops/rig_pthreads.c"
#elif defined(HAVE_WIN32_THREADS)
	#include "thread_ops/rig_win32_threads.c"
#else
	#error Threading library not supported.
#endif


/**
 * INTERNAL
 * General thread cleanup function. Currently takes care of:
 * - SMR cleanup (retire HP and Epoch records)
 *
 * @param arg
 *     void * for compatibility, not used, is always NULL
 */
static void rig_thread_cleanup(void *arg) {
	UNUSED(arg);

	// Retire HPRecord (implicit scan)
	rig_smr_hp_record_release();

	// Retire EpochRecord
	rig_smr_epoch_record_release();
}


/**
 * Initialize and return a Thread-group.
 * A Thread-group is Rig's way to manage threads: it keeps them together by
 * groups, each executing the same, specified task (see rig_thread_start()).
 *
 * @param flags
 *     flags to influence Thread-group behavior, currently valid are:
 *     - RIG_THREAD_DETACHED (starts threads as detached right away)
 * @param nr_threads
 *     number of threads in this Thread-group, must be > 0
 *
 * @return
 *     Thread-group data, NULL on error.
 *     On error, the following error codes are set:
 *     - EINVAL (invalid arguments passed)
 *     - ENOMEM (insufficient memory)
 */
RIG_THREAD rig_thread_init(uint16_t flags, size_t nr_threads) {
	CHECK_PERMITTED_FLAGS(flags, RIG_THREAD_DETACHED);

	if (nr_threads == 0) {
		ERRET(EINVAL, NULL);
	}

	RIG_THREAD thr = rig_mem_alloc(sizeof(*thr), sizeof(RIG_THREAD_TYPE) * nr_threads);
	NULLCHECK_ERRET(thr, ENOMEM, NULL);

	thr->flags = flags;
	thr->nr_threads = nr_threads;
	thr->start_routine = NULL;
	thr->arg = NULL;

	return (thr);
}

/**
 * Destroy specified Thread-group and set pointer to NULL.
 * If the Thread-group is still working, the running threads get detached
 * and are allowed to finish on their own.
 *
 * @param *thr
 *     pointer to Thread-group data
 */
void rig_thread_destroy(RIG_THREAD *thr) {
	NULLCHECK_EXIT(thr);

	// If a valid pointer already contains NULL, nothing to do!
	if (*thr == NULL) {
		return;
	}

	// Need to detach the threads here to ensure they're not left
	// hanging around afterwards with no means to terminate them
	if ((TEST_BITFIELD((*thr)->flags, RIG_THREAD_STARTED))
	 && (!TEST_BITFIELD((*thr)->flags, RIG_THREAD_DETACHED))) {
		rig_thread_detach(*thr);
	}

	rig_mem_free(*thr);
	*thr = NULL;
}

/**
 * Start the threads in the specified Thread-group and assign them a task.
 * A task is specified by a function of the form 'void *func(void *arg)',
 * taking one void * argument and returning a void *.
 *
 * @param thr
 *     Thread-group data
 * @param *start_routine
 *     pointer to function, this is the task that is going to be executed
 * @param arg
 *     the argument passed to the start_routine specified above, can be NULL
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EAGAIN (insufficient resources, other than memory)
 *     - EALREADY (Thread-group already running)
 *     - ENOMEM (insufficient memory)
 */
bool rig_thread_start(RIG_THREAD thr, void *(*start_routine)(void *arg), void *arg) {
	NULLCHECK_EXIT(thr);
	NULLCHECK_EXIT(start_routine);

	if (TEST_BITFIELD(thr->flags, RIG_THREAD_STARTED)) {
		ERRET(EALREADY, false);
	}

	thr->start_routine = start_routine;
	thr->arg = arg;

	if (!thread_ops_start(thr)) {
		ERRET(errno, false);
	}

	SET_BITFIELD(thr->flags, RIG_THREAD_STARTED);

	return (true);
}

/**
 * Join the running threads in the specified Thread-group, and optionally get
 * their return values. After a successful join, the Thread-group's state
 * changes to not running.
 *
 * @param thr
 *     Thread-group data
 * @param retval[]
 *     array in which to put pointers returned by a successful join,
 *     NULL means to ignore and discard the returned values
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EDEADLK (deadlock, join with each other or join to oneself)
 *     - EINVAL (Thread-group not running or not joinable anymore)
 */
bool rig_thread_join(RIG_THREAD thr, void *retval[]) {
	NULLCHECK_EXIT(thr);

	if (!TEST_BITFIELD(thr->flags, RIG_THREAD_STARTED)) {
		ERRET(EINVAL, false);
	}

	if (TEST_BITFIELD(thr->flags, RIG_THREAD_DETACHED)) {
		ERRET(EINVAL, false);
	}

	if (!thread_ops_join(thr, retval)) {
		VERIFY_ERRET((errno == EDEADLK) || (errno == EINVAL));
		ERRET(errno, false);
	}

	RESET_BITFIELD(thr->flags, RIG_THREAD_STARTED);

	return (true);
}

/**
 * Detach the running threads in the specified Thread-group.
 * Once detached, threads cannot be made joinable again!
 *
 * @param thr
 *     Thread-group data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EALREADY (Thread-group already detached)
 *     - EINVAL (Thread-group not running or not joinable)
 */
bool rig_thread_detach(RIG_THREAD thr) {
	NULLCHECK_EXIT(thr);

	if (!TEST_BITFIELD(thr->flags, RIG_THREAD_STARTED)) {
		ERRET(EINVAL, false);
	}

	if (TEST_BITFIELD(thr->flags, RIG_THREAD_DETACHED)) {
		ERRET(EALREADY, false);
	}

	if (!thread_ops_detach(thr)) {
		VERIFY_ERRET(errno == EINVAL);
		ERRET(errno, false);
	}

	SET_BITFIELD(thr->flags, RIG_THREAD_DETACHED);

	return (true);
}


/**
 * INTERNAL
 * Rig-specific Thread ID infrastructure, consisting of a counter, which provides
 * the needed unique IDs and is initialized at library load-time and destroyed at
 * unload-time, and a TLS size_t (initially set to 0), so that each thread can
 * remember its own ID. If no load-time TLS is present, we use run-time TLS.
 */
static RIG_COUNTER RIG_ThreadID_Counter = NULL;

#if defined(SYSTEM_TLS_SUPPORT)
	static SYSTEM_TLS_DECL size_t thread_id = 0;
#else
	static RIG_TLS RIG_ThreadID_TlsKey = NULL;
#endif

static void rig_threadid_construct(void) ATTR_CONSTRUCTOR;

static void rig_threadid_construct(void) {
	// Initialize thread counter to get unique IDs
	RIG_ThreadID_Counter = rig_counter_init(2, 0);
	NULLCHECK_EXIT(RIG_ThreadID_Counter);

#if !defined(SYSTEM_TLS_SUPPORT)
	// The TLS key must be initialized only once, only one thread can get here
	RIG_ThreadID_TlsKey = rig_tls_init();
	NULLCHECK_EXIT(RIG_ThreadID_TlsKey);
#endif
}

static void rig_threadid_destruct(void) ATTR_DESTRUCTOR;

static void rig_threadid_destruct(void) {
	rig_counter_destroy(&RIG_ThreadID_Counter);

#if !defined(SYSTEM_TLS_SUPPORT)
	rig_tls_destroy(&RIG_ThreadID_TlsKey);
#endif
}

/**
 * Get a valid Thread ID for the current thread, as returned internally by Rig.
 * This is not the same as rig_thread_tid(), which, in contrast, returns an ID
 * gotten from OS-specific calls.
 * If you need guaranteed unique IDs, starting at 1, and with the main thread
 * always having ID 1, use this function. Note that the ID is initialized lazily,
 * meaning that a thread acquires an ID only after you call this function for
 * the first time in its context, and then keeps that unique ID for the remainder
 * of its lifetime.
 * ID 0 is reserved and never used as a Thread ID, allowing for its use as a
 * place-holder "no-thread" value.
 *
 * @return
 *     Thread ID of current thread (Rig-specific)
 */
size_t rig_thread_id(void) {
#if !defined(SYSTEM_TLS_SUPPORT)
	void *thread_id_tls = rig_tls_get(RIG_ThreadID_TlsKey);
	size_t thread_id = (size_t)thread_id_tls;
#endif

	if (thread_id == 0) {
		if (rig_thread_is_main()) {
			thread_id = 1;
		}
		else {
			rig_acheck_msg(rig_counter_get_and_add(RIG_ThreadID_Counter, 1, &thread_id), "unique Thread IDs exhausted!");
		}

#if !defined(SYSTEM_TLS_SUPPORT)
		rig_acheck_msg(rig_tls_set(RIG_ThreadID_TlsKey, (void *)thread_id), "insufficient memory to save Thread ID!");
#endif
	}

	return (thread_id);
}


/**
 * Initialize and return a TLS (Thread Local Storage) key.
 *
 * @return
 *     TLS key data, NULL on error.
 *     On error, the following error codes are set:
 *     - EAGAIN (insufficient resources, other than memory)
 *     - ENOMEM (insufficient memory)
 */
RIG_TLS rig_tls_init(void) {
	RIG_TLS tls = rig_mem_alloc(sizeof(*tls), 0);
	NULLCHECK_ERRET(tls, ENOMEM, NULL);

	if (!thread_ops_tls_init(tls)) {
		rig_mem_free(tls);
		ERRET(errno, NULL);
	}

	return (tls);
}

/**
 * Destroy specified TLS (Thread Local Storage) key and set pointer to NULL.
 *
 * @param *tls
 *     pointer to TLS key data
 */
void rig_tls_destroy(RIG_TLS *tls) {
	NULLCHECK_EXIT(tls);

	// If a valid pointer already contains NULL, nothing to do!
	if (*tls == NULL) {
		return;
	}

	thread_ops_tls_destroy(*tls);

	rig_mem_free(*tls);
	*tls = NULL;
}


/**
 * Initialize and return a Mutual Exclusion (Mutex) lock.
 *
 * @return
 *     Mutex Lock data, NULL on error.
 *     On error, the following error codes are set:
 *     - EAGAIN (insufficient resources, other than memory)
 *     - ENOMEM (insufficient memory)
 */
RIG_MXLOCK rig_mxlock_init(void) {
	RIG_MXLOCK mxl = rig_mem_alloc(sizeof(*mxl), 0);
	NULLCHECK_ERRET(mxl, ENOMEM, NULL);

	if (!thread_ops_mxlock_init(mxl)) {
		rig_mem_free(mxl);
		ERRET(errno, NULL);
	}

	return (mxl);
}

/**
 * Destroy specified Mutual Exclusion (Mutex) lock and set pointer to NULL.
 *
 * @param *mxl
 *     pointer to Mutex Lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EBUSY (lock still in use)
 */
bool rig_mxlock_destroy(RIG_MXLOCK *mxl) {
	NULLCHECK_EXIT(mxl);

	// If a valid pointer already contains NULL, nothing to do!
	if (*mxl == NULL) {
		return (true);
	}

	if (!thread_ops_mxlock_destroy(*mxl)) {
		ERRET(EBUSY, false);
	}

	rig_mem_free(*mxl);
	*mxl = NULL;

	return (true);
}


/**
 * Initialize and return a Condition Variable.
 *
 * @return
 *     Condition Variable data, NULL on error.
 *     On error, the following error codes are set:
 *     - EAGAIN (insufficient resources, other than memory)
 *     - ENOMEM (insufficient memory)
 */
RIG_CONDVAR rig_condvar_init(void) {
	RIG_CONDVAR cond = rig_mem_alloc(sizeof(*cond), 0);
	NULLCHECK_ERRET(cond, ENOMEM, NULL);

	if (!thread_ops_condvar_init(cond)) {
		rig_mem_free(cond);
		ERRET(errno, NULL);
	}

	return (cond);
}

/**
 * Destroy specified Condition Variable and set pointer to NULL.
 *
 * @param *cond
 *     pointer to Condition Variable data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EBUSY (condition variable still in use)
 */
bool rig_condvar_destroy(RIG_CONDVAR *cond) {
	NULLCHECK_EXIT(cond);

	// If a valid pointer already contains NULL, nothing to do!
	if (*cond == NULL) {
		return (true);
	}

	if (!thread_ops_condvar_destroy(*cond)) {
		ERRET(EBUSY, false);
	}

	rig_mem_free(*cond);
	*cond = NULL;

	return (true);
}


/**
 * Initialize and return a Read/Write (RW) lock.
 *
 * @return
 *     Read/Write lock data, NULL on error.
 *     On error, the following error codes are set:
 *     - EAGAIN (insufficient resources, other than memory)
 *     - ENOMEM (insufficient memory)
 */
RIG_RWLOCK rig_rwlock_init(void) {
	RIG_RWLOCK rwl = rig_mem_alloc(sizeof(*rwl), 0);
	NULLCHECK_ERRET(rwl, ENOMEM, NULL);

	if (!thread_ops_rwlock_init(rwl)) {
		rig_mem_free(rwl);
		ERRET(errno, NULL);
	}

	return (rwl);
}

/**
 * Destroy specified Read/Write (RW) lock and set pointer to NULL.
 *
 * @param *rwl
 *     pointer to Read/Write lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EBUSY (lock still in use)
 */
bool rig_rwlock_destroy(RIG_RWLOCK *rwl) {
	NULLCHECK_EXIT(rwl);

	// If a valid pointer already contains NULL, nothing to do!
	if (*rwl == NULL) {
		return (true);
	}

	if (!thread_ops_rwlock_destroy(*rwl)) {
		ERRET(EBUSY, false);
	}

	rig_mem_free(*rwl);
	*rwl = NULL;

	return (true);
}


// Micro-lock static configuration
#define RIG_MLOCK_SPIN_MAX 20000

// Micro-lock data
struct rig_mlock {
	atomic_ops_uint mlock CACHELINE_ALIGNED;
	atomic_ops_uint owner_id;
	uint16_t flags; // read-only value
};

/**
 * Initialize and return a Micro-Lock.
 *
 * @param flags
 *     modify lock behavior.
 *     Supported flags are:
 *     - RIG_MLOCK_RECURSIVE: support recursive lock acquisition
 *
 * @return
 *     Micro-Lock data, NULL on error.
 *     On error, the following error codes are set:
 *     - ENOMEM (insufficient memory)
 */
RIG_MLOCK rig_mlock_init(uint16_t flags) {
	CHECK_PERMITTED_FLAGS(flags, RIG_MLOCK_RECURSIVE);

	RIG_MLOCK ml = rig_mem_alloc_aligned(sizeof(*ml), 0, CACHELINE_SIZE, RIG_MEM_ALLOC_ALIGN_PAD);
	NULLCHECK_ERRET(ml, ENOMEM, NULL);

	atomic_ops_uint_store(&ml->mlock, 0, ATOMIC_OPS_FENCE_NONE);
	atomic_ops_uint_store(&ml->owner_id, 0, ATOMIC_OPS_FENCE_NONE);
	ml->flags = flags;

	atomic_ops_fence(ATOMIC_OPS_FENCE_RELEASE);

	return (ml);
}

/**
 * Destroy specified Micro-Lock and set pointer to NULL.
 *
 * @param *ml
 *     pointer to Micro-Lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EBUSY (lock still in use)
 */
bool rig_mlock_destroy(RIG_MLOCK *ml) {
	NULLCHECK_EXIT(ml);

	// If a valid pointer already contains NULL, nothing to do!
	if (*ml == NULL) {
		return (true);
	}

	// Detect if the lock is still used somewhere
	if (atomic_ops_uint_load(&(*ml)->mlock, ATOMIC_OPS_FENCE_ACQUIRE) != 0) {
		ERRET(EBUSY, false);
	}

	rig_mem_free_aligned(*ml);
	*ml = NULL;

	return (true);
}

/**
 * Lock the specified Micro-Lock, waiting on it if it can't be acquired.
 * Recursive locking is supported, up to a maximum limit.
 *
 * @param ml
 *     Micro-Lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EAGAIN (maximum number of recursive locks reached or non-recursive lock)
 */
bool rig_mlock_lock(RIG_MLOCK ml) {
	NULLCHECK_EXIT(ml);

	size_t tid = rig_thread_id();

	// I own the mutex lock, take the fast path
	if (atomic_ops_uint_load(&ml->owner_id, ATOMIC_OPS_FENCE_NONE) == tid) {
		if (TEST_BITFIELD(ml->flags, RIG_MLOCK_RECURSIVE)) {
			// I'm the owner, recursive locking, if possible!
			if (atomic_ops_uint_load(&ml->mlock, ATOMIC_OPS_FENCE_NONE) == SIZE_MAX) {
				ERRET(EAGAIN, false);
			}

			atomic_ops_uint_inc(&ml->mlock, ATOMIC_OPS_FENCE_ACQUIRE);

			return (true);
		}
		else {
			ERRET(EAGAIN, false);
		}
	}

	size_t spin = 0;

	while (true) {
		if ((atomic_ops_uint_load(&ml->mlock, ATOMIC_OPS_FENCE_NONE) == 0)
		 && (atomic_ops_uint_cas(&ml->mlock, 0, 1, ATOMIC_OPS_FENCE_NONE))) {
			atomic_ops_uint_store(&ml->owner_id, tid, ATOMIC_OPS_FENCE_ACQUIRE);

			return (true);
		}

		spin++;

		if (spin == RIG_MLOCK_SPIN_MAX) {
			spin = 0;
			rig_thread_yield();
		}
	}
}

/**
 * Try locking the specified Micro-Lock, returning immediately if it can't be acquired.
 * Recursive locking is supported, up to a maximum limit.
 *
 * @param ml
 *     Micro-Lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EAGAIN (maximum number of recursive locks reached or non-recursive lock)
 *     - EBUSY (lock already held by a thread)
 */
bool rig_mlock_trylock(RIG_MLOCK ml) {
	NULLCHECK_EXIT(ml);

	size_t tid = rig_thread_id();

	// I own the mutex lock, take the fast path
	if (atomic_ops_uint_load(&ml->owner_id, ATOMIC_OPS_FENCE_NONE) == tid) {
		if (TEST_BITFIELD(ml->flags, RIG_MLOCK_RECURSIVE)) {
			// I'm the owner, recursive locking, if possible!
			if (atomic_ops_uint_load(&ml->mlock, ATOMIC_OPS_FENCE_NONE) == SIZE_MAX) {
				ERRET(EAGAIN, false);
			}

			atomic_ops_uint_inc(&ml->mlock, ATOMIC_OPS_FENCE_ACQUIRE);

			return (true);
		}
		else {
			ERRET(EAGAIN, false);
		}
	}

	if ((atomic_ops_uint_load(&ml->mlock, ATOMIC_OPS_FENCE_NONE) == 0)
	 && (atomic_ops_uint_cas(&ml->mlock, 0, 1, ATOMIC_OPS_FENCE_NONE))) {
		atomic_ops_uint_store(&ml->owner_id, tid, ATOMIC_OPS_FENCE_ACQUIRE);

		return (true);
	}

	ERRET(EBUSY, false);
}

/**
 * Unlock the specified Micro-Lock.
 *
 * @param ml
 *     Micro-Lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EPERM (current thread doesn't hold the lock or not locked at all)
 */
bool rig_mlock_unlock(RIG_MLOCK ml) {
	NULLCHECK_EXIT(ml);

	size_t mlock = atomic_ops_uint_load(&ml->mlock, ATOMIC_OPS_FENCE_NONE);

	if (mlock == 0) {
		ERRET(EPERM, false);
	}

	if (atomic_ops_uint_load(&ml->owner_id, ATOMIC_OPS_FENCE_NONE) != rig_thread_id()) {
		ERRET(EPERM, false);
	}

	if (mlock == 1) {
		atomic_ops_uint_store(&ml->owner_id, 0, ATOMIC_OPS_FENCE_NONE);
	}

	atomic_ops_uint_dec(&ml->mlock, ATOMIC_OPS_FENCE_RELEASE);

	return (true);
}

/**
 * Check if the specified Micro-Lock is locked by some thread.
 *
 * @param ml
 *     Micro-Lock data
 *
 * @return
 *     boolean, true if the lock is held by someone, false otherwise.
 */
bool rig_mlock_islocked(RIG_MLOCK ml) {
	NULLCHECK_EXIT(ml);

	return (atomic_ops_uint_load(&ml->mlock, ATOMIC_OPS_FENCE_ACQUIRE) != 0);
}


/**
 * The Micro-Read/Write lock uses two variables to keep its state, giving it the
 * possibility to do extensive checking of its usage, reporting deadlocks, locks
 * not being unlocked by the owner thread, and so on. It optionally also fully
 * supports recursion in both reader and writer acquisition.
 * Following the meaning of the various states of the two variables:
 *
 * --- mrwlock (global) ---
 * 0 0...0   no locks held by anyone
 * 0 X...X   X threads holding read-locks
 * 1 0...0   1 thread holding a write-lock
 * 1 X...X   X threads holding read-locks, reserved for write by 1 thread, no new
 *           read-locks will be handed out, the writer thread waits for 1 0...0
 *
 * --- owned (thread-local) ---
 * 0 0...0   the thread holds no locks
 * 0 X...X   the thread holds X read-locks (if not recursive, X == 1)
 * 1 0...0   NOT USED
 * 1 X...X   the thread holds X write-locks (if not recursive, X == 1)
 *
 */

// Micro-Read/Write lock static configuration
#define RIG_MRWLOCK_SPIN_MAX 20000
#define RIG_MRWLOCK_WRLOCK_BIT ((size_t)1 << ((sizeof(size_t) * 8) - 1))

// Micro-Read/Write lock data
struct rig_mrwlock {
	atomic_ops_uint mrwlock CACHELINE_ALIGNED;
	struct rig_tls owned; // Directly embed rig_tls for performance and locality
	uint16_t flags; // read-only value
};

/**
 * Initialize and return a Micro-Read/Write (MRW) lock.
 *
 * @param flags
 *     modify lock behavior.
 *     Supported flags are:
 *     - RIG_MRWLOCK_RECURSIVE_READ: support recursive read-side lock acquisition
 *     - RIG_MRWLOCK_RECURSIVE_WRITE: support recursive write-side lock acquisition
 *     - RIG_MRWLOCK_RECURSIVE: support recursive lock acquisition (read & write)
 *
 * @return
 *     Micro-Read/Write (MRW) lock data, NULL on error.
 *     On error, the following error codes are set:
 *     - EAGAIN (insufficient resources, other than memory)
 *     - ENOMEM (insufficient memory)
 */
RIG_MRWLOCK rig_mrwlock_init(uint16_t flags) {
	CHECK_PERMITTED_FLAGS(flags, RIG_MRWLOCK_RECURSIVE_READ | RIG_MRWLOCK_RECURSIVE_WRITE);

	RIG_MRWLOCK mrwl = rig_mem_alloc_aligned(sizeof(*mrwl), 0, CACHELINE_SIZE, RIG_MEM_ALLOC_ALIGN_PAD);
	NULLCHECK_ERRET(mrwl, ENOMEM, NULL);

	atomic_ops_uint_store(&mrwl->mrwlock, 0, ATOMIC_OPS_FENCE_NONE);
	if (!thread_ops_tls_init(&mrwl->owned)) {
		rig_mem_free_aligned(mrwl);
		ERRET(errno, NULL);
	}
	mrwl->flags = flags;

	atomic_ops_fence(ATOMIC_OPS_FENCE_RELEASE);

	return (mrwl);
}

/**
 * Destroy specified Micro-Read/Write (MRW) lock and set pointer to NULL.
 *
 * @param *mrwl
 *     pointer to Micro-Read/Write lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EBUSY (lock still in use)
 */
bool rig_mrwlock_destroy(RIG_MRWLOCK *mrwl) {
	NULLCHECK_EXIT(mrwl);

	// If a valid pointer already contains NULL, nothing to do!
	if (*mrwl == NULL) {
		return (true);
	}

	// Detect if the lock is still used somewhere
	if (atomic_ops_uint_load(&(*mrwl)->mrwlock, ATOMIC_OPS_FENCE_ACQUIRE) != 0) {
		ERRET(EBUSY, false);
	}

	thread_ops_tls_destroy(&(*mrwl)->owned);

	rig_mem_free_aligned(*mrwl);
	*mrwl = NULL;

	return (true);
}

/**
 * Lock the specified Micro-Read/Write (MRW) lock for reading, waiting on it if it can't be acquired.
 * Recursive locking of read locks is supported, up to a maximum limit.
 * Downgrading a write lock to a read lock is not supported.
 *
 * @param mrwl
 *     Micro-Read/Write lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EAGAIN (maximum number of recursive read locks reached or non-recursive lock)
 *     - EDEADLK (thread already holds lock for writing)
 */
bool rig_mrwlock_rdlock(RIG_MRWLOCK mrwl) {
	NULLCHECK_EXIT(mrwl);

	// Get thread-local lock status data
	void *owned = rig_tls_get(&mrwl->owned);

	// If the thread already holds a write-lock, attempting a read-lock deadlocks
	// NO DOWNGRADE SUPPORTED!
	if ((size_t)owned & RIG_MRWLOCK_WRLOCK_BIT) {
		ERRET(EDEADLK, false);
	}

	// The thread doesn't hold a write-lock at this point, so, if it instead
	// already holds a read-lock, we can recursively read-lock right away
	if ((size_t)owned > 0) {
		if (TEST_BITFIELD(mrwl->flags, RIG_MRWLOCK_RECURSIVE_READ)) {
			if ((size_t)owned == (RIG_MRWLOCK_WRLOCK_BIT - 1)) {
				ERRET(EAGAIN, false);
			}

			rig_tls_set(&mrwl->owned, (void *)((size_t)owned + 1));

			return (true);
		}
		else {
			ERRET(EAGAIN, false);
		}
	}

	// The thread held nothing, so we must obtain the lock fully
	size_t spin = 0;
	size_t mrwlock;

	while (true) {
		mrwlock = atomic_ops_uint_load(&mrwl->mrwlock, ATOMIC_OPS_FENCE_NONE);

		if ((mrwlock < (RIG_MRWLOCK_WRLOCK_BIT - 1))
		 && (atomic_ops_uint_cas(&mrwl->mrwlock, mrwlock, mrwlock + 1, ATOMIC_OPS_FENCE_NONE))) {
			rig_tls_set(&mrwl->owned, (void *)((size_t)1));

			atomic_ops_fence(ATOMIC_OPS_FENCE_ACQUIRE);

			return (true);
		}
		else if (mrwlock == (RIG_MRWLOCK_WRLOCK_BIT - 1)) {
			ERRET(EAGAIN, false);
		}
		else {
			spin++;

			if (spin == RIG_MRWLOCK_SPIN_MAX) {
				spin = 0;
				rig_thread_yield();
			}
		}
	}
}

/**
 * Try locking the specified Micro-Read/Write (MRW) lock for reading, returning immediately if it can't be acquired.
 * Recursive locking of read locks is supported, up to a maximum limit.
 * Downgrading a write lock to a read lock is not supported.
 *
 * @param mrwl
 *     Micro-Read/Write lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EAGAIN (maximum number of recursive read locks reached or non-recursive lock)
 *     - EBUSY (lock already held by a thread)
 *     - EDEADLK (thread already holds lock for writing)
 */
bool rig_mrwlock_tryrdlock(RIG_MRWLOCK mrwl) {
	NULLCHECK_EXIT(mrwl);

	// Get thread-local lock status data
	void *owned = rig_tls_get(&mrwl->owned);

	// If the thread already holds a write-lock, attempting a read-lock deadlocks
	// NO DOWNGRADE SUPPORTED!
	if ((size_t)owned & RIG_MRWLOCK_WRLOCK_BIT) {
		ERRET(EDEADLK, false);
	}

	// The thread doesn't hold a write-lock at this point, so, if it instead
	// already holds a read-lock, we can recursively read-lock right away
	if ((size_t)owned > 0) {
		if (TEST_BITFIELD(mrwl->flags, RIG_MRWLOCK_RECURSIVE_READ)) {
			if ((size_t)owned == (RIG_MRWLOCK_WRLOCK_BIT - 1)) {
				ERRET(EAGAIN, false);
			}

			rig_tls_set(&mrwl->owned, (void *)((size_t)owned + 1));

			return (true);
		}
		else {
			ERRET(EAGAIN, false);
		}
	}

	// The thread held nothing, so we must obtain the lock fully
	size_t mrwlock = atomic_ops_uint_load(&mrwl->mrwlock, ATOMIC_OPS_FENCE_NONE);

	if ((mrwlock < (RIG_MRWLOCK_WRLOCK_BIT - 1))
	 && (atomic_ops_uint_cas(&mrwl->mrwlock, mrwlock, mrwlock + 1, ATOMIC_OPS_FENCE_NONE))) {
		rig_tls_set(&mrwl->owned, (void *)((size_t)1));

		atomic_ops_fence(ATOMIC_OPS_FENCE_ACQUIRE);

		return (true);
	}
	else if (mrwlock == (RIG_MRWLOCK_WRLOCK_BIT - 1)) {
		ERRET(EAGAIN, false);
	}
	else {
		ERRET(EBUSY, false);
	}
}

/**
 * Lock the specified Micro-Read/Write (MRW) lock for writing, waiting on it if it can't be acquired.
 * Recursive locking of a write lock is supported, up to a maximum limit.
 * Upgrading a read lock to a write lock is not supported, you must first
 * release all read locks that are being held.
 *
 * @param mrwl
 *     Micro-Read/Write lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EAGAIN (maximum number of recursive write locks reached or non-recursive lock)
 *     - EDEADLK (thread already holds lock for reading)
 */
bool rig_mrwlock_wrlock(RIG_MRWLOCK mrwl) {
	NULLCHECK_EXIT(mrwl);

	// Get thread-local lock status data
	void *owned = rig_tls_get(&mrwl->owned);

	// If the thread already holds a read-lock, attempting a write-lock deadlocks
	// NO UPGRADE SUPPORTED!
	if ((!((size_t)owned & RIG_MRWLOCK_WRLOCK_BIT)) && ((size_t)owned != 0)) {
		ERRET(EDEADLK, false);
	}

	// The thread doesn't hold a read-lock at this point, so, if it instead
	// already holds a write-lock, we can recursively write-lock right away
	if ((size_t)owned > RIG_MRWLOCK_WRLOCK_BIT) {
		if (TEST_BITFIELD(mrwl->flags, RIG_MRWLOCK_RECURSIVE_WRITE)) {
			if ((size_t)owned == SIZE_MAX) {
				ERRET(EAGAIN, false);
			}

			rig_tls_set(&mrwl->owned, (void *)((size_t)owned + 1));

			return (true);
		}
		else {
			ERRET(EAGAIN, false);
		}
	}

	// The thread held nothing, so we must obtain the lock fully
	size_t spin = 0;
	size_t mrwlock;

	while (true) {
		mrwlock = atomic_ops_uint_load(&mrwl->mrwlock, ATOMIC_OPS_FENCE_NONE);

		if ((mrwlock == 0)
		 && (atomic_ops_uint_cas(&mrwl->mrwlock, 0, RIG_MRWLOCK_WRLOCK_BIT, ATOMIC_OPS_FENCE_NONE))) {
			rig_tls_set(&mrwl->owned, (void *)(RIG_MRWLOCK_WRLOCK_BIT + 1));

			atomic_ops_fence(ATOMIC_OPS_FENCE_ACQUIRE);

			return (true);
		}
		else if ((mrwlock < RIG_MRWLOCK_WRLOCK_BIT)
		 && (atomic_ops_uint_cas(&mrwl->mrwlock, mrwlock, mrwlock + RIG_MRWLOCK_WRLOCK_BIT, ATOMIC_OPS_FENCE_NONE))) {
			// Reserve lock for writer access by adding RIG_MRWLOCK_WRLOCK_BIT
			spin = 0;

			while (atomic_ops_uint_load(&mrwl->mrwlock, ATOMIC_OPS_FENCE_NONE) != RIG_MRWLOCK_WRLOCK_BIT) {
				spin++;

				if (spin == RIG_MRWLOCK_SPIN_MAX) {
					spin = 0;
					rig_thread_yield();
				}
			}

			rig_tls_set(&mrwl->owned, (void *)(RIG_MRWLOCK_WRLOCK_BIT + 1));

			atomic_ops_fence(ATOMIC_OPS_FENCE_ACQUIRE);

			return (true);
		}
		else {
			spin++;

			if (spin == RIG_MRWLOCK_SPIN_MAX) {
				spin = 0;
				rig_thread_yield();
			}
		}
	}
}

/**
 * Try locking the specified Micro-Read/Write (MRW) lock for writing, returning immediately if it can't be acquired.
 * Recursive locking of a write lock is supported, up to a maximum limit.
 * Upgrading a read lock to a write lock is not supported, you must first
 * release all read locks that are being held.
 *
 * @param mrwl
 *     Micro-Read/Write lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EAGAIN (maximum number of recursive write locks reached or non-recursive lock)
 *     - EBUSY (lock already held by a thread)
 *     - EDEADLK (thread already holds lock for reading)
 */
bool rig_mrwlock_trywrlock(RIG_MRWLOCK mrwl) {
	NULLCHECK_EXIT(mrwl);

	// Get thread-local lock status data
	void *owned = rig_tls_get(&mrwl->owned);

	// If the thread already holds a read-lock, attempting a write-lock deadlocks
	// NO UPGRADE SUPPORTED!
	if ((!((size_t)owned & RIG_MRWLOCK_WRLOCK_BIT)) && ((size_t)owned != 0)) {
		ERRET(EDEADLK, false);
	}

	// The thread doesn't hold a read-lock at this point, so, if it instead
	// already holds a write-lock, we can recursively write-lock right away
	if ((size_t)owned > RIG_MRWLOCK_WRLOCK_BIT) {
		if (TEST_BITFIELD(mrwl->flags, RIG_MRWLOCK_RECURSIVE_WRITE)) {
			if ((size_t)owned == SIZE_MAX) {
				ERRET(EAGAIN, false);
			}

			rig_tls_set(&mrwl->owned, (void *)((size_t)owned + 1));

			return (true);
		}
		else {
			ERRET(EAGAIN, false);
		}
	}

	// The thread held nothing, so we must obtain the lock fully
	size_t mrwlock = atomic_ops_uint_load(&mrwl->mrwlock, ATOMIC_OPS_FENCE_NONE);

	if ((mrwlock == 0)
	 && (atomic_ops_uint_cas(&mrwl->mrwlock, 0, RIG_MRWLOCK_WRLOCK_BIT, ATOMIC_OPS_FENCE_NONE))) {
		rig_tls_set(&mrwl->owned, (void *)(RIG_MRWLOCK_WRLOCK_BIT + 1));

		atomic_ops_fence(ATOMIC_OPS_FENCE_ACQUIRE);

		return (true);
	}

	ERRET(EBUSY, false);
}

/**
 * Unlock the specified Micro-Read/Write (MRW) lock.
 *
 * @param mrwl
 *     Micro-Read/Write lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EPERM (current thread doesn't hold the lock or not locked at all)
 */
bool rig_mrwlock_unlock(RIG_MRWLOCK mrwl) {
	NULLCHECK_EXIT(mrwl);

	size_t mrwlock = atomic_ops_uint_load(&mrwl->mrwlock, ATOMIC_OPS_FENCE_NONE);

	if (mrwlock == 0) {
		ERRET(EPERM, false);
	}

	// Get thread-local lock status data
	void *owned = rig_tls_get(&mrwl->owned);

	if ((size_t)owned == 0) {
		ERRET(EPERM, false);
	}

	if (mrwlock == RIG_MRWLOCK_WRLOCK_BIT) {
		if ((size_t)owned == (RIG_MRWLOCK_WRLOCK_BIT + 1)) {
			rig_tls_set(&mrwl->owned, (void *)((size_t)0));
			atomic_ops_uint_store(&mrwl->mrwlock, 0, ATOMIC_OPS_FENCE_RELEASE);
		}
		else {
			rig_tls_set(&mrwl->owned, (void *)((size_t)owned - 1));
		}
	}
	else {
		if ((size_t)owned == 1) {
			rig_tls_set(&mrwl->owned, (void *)((size_t)0));
			atomic_ops_uint_dec(&mrwl->mrwlock, ATOMIC_OPS_FENCE_RELEASE);
		}
		else {
			rig_tls_set(&mrwl->owned, (void *)((size_t)owned - 1));
		}
	}

	return (true);
}

/**
 * Check if the specified Micro-Read/Write (MRW) lock is locked by some thread.
 *
 * @param mrwl
 *     Micro-Read/Write lock data
 *
 * @return
 *     boolean, true if the lock is held by someone, false otherwise.
 */
bool rig_mrwlock_islocked(RIG_MRWLOCK mrwl) {
	NULLCHECK_EXIT(mrwl);

	return (atomic_ops_uint_load(&mrwl->mrwlock, ATOMIC_OPS_FENCE_ACQUIRE) != 0);
}

/**
 * Check if the specified Micro-Read/Write (MRW) lock is locked by some thread
 * for reading.
 *
 * @param mrwl
 *     Micro-Read/Write lock data
 *
 * @return
 *     boolean, true if the lock is held by someone for reading, false otherwise.
 */
bool rig_mrwlock_isrdlocked(RIG_MRWLOCK mrwl) {
	NULLCHECK_EXIT(mrwl);

	size_t mrwlock = atomic_ops_uint_load(&mrwl->mrwlock, ATOMIC_OPS_FENCE_ACQUIRE);

	return ((mrwlock != 0) && (mrwlock != RIG_MRWLOCK_WRLOCK_BIT));
}

/**
 * Check if the specified Micro-Read/Write (MRW) lock is locked by some thread
 * for writing.
 *
 * @param mrwl
 *     Micro-Read/Write lock data
 *
 * @return
 *     boolean, true if the lock is held by someone for writing, false otherwise.
 */
bool rig_mrwlock_iswrlocked(RIG_MRWLOCK mrwl) {
	NULLCHECK_EXIT(mrwl);

	return (atomic_ops_uint_load(&mrwl->mrwlock, ATOMIC_OPS_FENCE_ACQUIRE) == RIG_MRWLOCK_WRLOCK_BIT);
}
