/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_pthreads.c 966 2011-04-30 01:16:03Z llongi $
 */

#include <pthread.h>
#include <sched.h>
#include <unistd.h>

// Includes needed for getting the Thread ID (OS-specific)
#if defined(SYSTEM_OS_LINUX)
	#include <sys/syscall.h>
#elif defined(SYSTEM_OS_FREEBSD)
	#include <sys/thr.h>
#elif defined(SYSTEM_OS_NETBSD)
	#include <lwp.h>
#endif

#define RIG_THREAD_TYPE pthread_t


// Thread-group data
struct rig_thread {
	uint16_t flags;
	size_t nr_threads;
	void *(*start_routine)(void *);
	void *arg;
	RIG_THREAD_TYPE thr[];
};

/**
 * INTERNAL ABSTRACTION
 * Implementation-specific wrapper for starting threads.
 *
 * @param thr
 *     Thread-group data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EAGAIN (insufficient resources, other than memory)
 *     - ENOMEM (insufficient memory)
 */
static inline bool thread_ops_start(RIG_THREAD thr) {
	pthread_attr_t thr_attrs;

	if (pthread_attr_init(&thr_attrs)) {
		ERRET(ENOMEM, false);
	}

	if (TEST_BITFIELD(thr->flags, RIG_THREAD_DETACHED)) {
		// This cannot fail, as thr_attrs must be valid at this point,
		// and detachstate is also always correct.
		pthread_attr_setdetachstate(&thr_attrs, PTHREAD_CREATE_DETACHED);
	}

	int ret;

	for (size_t i = 0; i < thr->nr_threads; i++) {
		if ((ret = pthread_create(&thr->thr[i], &thr_attrs, &rig_thread_starter, thr))) {
			// Detach all running threads if needed, to ensure cleanup.
			if (!TEST_BITFIELD(thr->flags, RIG_THREAD_DETACHED)) {
				for (size_t c = 0; c < i; c++) {
					pthread_detach(thr->thr[c]);
				}
			}

			// We now need to cancel the threads we already started!
			for (size_t c = 0; c < i; c++) {
				pthread_cancel(thr->thr[c]);
			}

			pthread_attr_destroy(&thr_attrs);

			// The only possible failure here is resource exhaustion, as the
			// passed values are always valid, and setting detachstate requires
			// no particular permissions.
			VERIFY_ERRET(ret == EAGAIN);
			ERRET(ret, false);
		}
	}

	pthread_attr_destroy(&thr_attrs);

	return (true);
}

/**
 * Exit from a running thread, returning the specified value.
 * This is the same as returning from the thread start function.
 *
 * @param retval
 *     void * to return, can then be picked up by a rig_thread_join()
 */
void rig_thread_exit(void *retval) {
	pthread_exit(retval);
}

/**
 * Have the calling thread relinquish the CPU, switching execution over to
 * another thread/process ready to run.
 */
void rig_thread_yield(void) {
#if defined(HAVE_SCHED_YIELD)
	sched_yield();
#else
	pthread_yield();
#endif
}

/**
 * INTERNAL ABSTRACTION
 * Implementation-specific wrapper for joining running threads.
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
 *     - EINVAL (thread not joinable anymore)
 *     - ESRCH (no thread with specified ID found)
 */
static inline bool thread_ops_join(RIG_THREAD thr, void *retval[]) {
	int ret;

	for (size_t i = 0; i < thr->nr_threads; i++) {
		if ((ret = pthread_join(thr->thr[i], (retval == NULL) ? (NULL) : (&retval[i])))) {
			VERIFY_ERRET(ret == EDEADLK || ret == EINVAL || ret == ESRCH);
			ERRET(ret, false);
		}
	}

	return (true);
}

/**
 * INTERNAL ABSTRACTION
 * Implementation-specific wrapper for detaching running threads.
 *
 * @param thr
 *     Thread-group data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EINVAL (thread not joinable)
 *     - ESRCH (no thread with specified ID found)
 */
static inline bool thread_ops_detach(RIG_THREAD thr) {
	int ret;

	for (size_t i = 0; i < thr->nr_threads; i++) {
		if ((ret = pthread_detach(thr->thr[i]))) {
			VERIFY_ERRET(ret == EINVAL || ret == ESRCH);
			ERRET(ret, false);
		}
	}

	return (true);
}


/**
 * Get a valid Thread ID for the current thread, as returned by OS-specific calls.
 * This is not the same as rig_thread_id(), which, in contrast, returns an ID
 * maintained internally by Rig.
 * If you need guaranteed unique IDs, starting at 1, and with the main thread
 * always having ID 1, use rig_thread_id() instead.
 *
 * @return
 *     Thread ID of current thread (OS-specific)
 */
size_t rig_thread_tid(void) {
	size_t ret;

#if defined(SYSTEM_OS_LINUX)
	ret = (size_t)syscall(SYS_gettid);
#elif defined(SYSTEM_OS_FREEBSD)
	long lwpid;
	thr_self(&lwpid);
	ret = (size_t)lwpid;
#elif defined(SYSTEM_OS_NETBSD)
	ret = (size_t)_lwp_self();
#elif defined(SYSTEM_OS_MACOSX)
	ret = (size_t)pthread_mach_thread_np(pthread_self());
#else
	// NOTE: Solaris returns real, useful Thread IDs here.
	// Fall back and try to cast pthread_t to size_t.
	ret = (size_t)pthread_self();
#endif

	return (ret);
}

/**
 * Find out if the current thread is the main thread, using OS-specific calls.
 *
 * @return
 *     boolean: true if current is the main thread, else false
 */
bool rig_thread_is_main(void) {
	bool ret;

#if defined(SYSTEM_OS_LINUX)
	ret = (rig_thread_tid() == rig_misc_pid());
#elif defined(SYSTEM_OS_FREEBSD) || defined(SYSTEM_OS_OPENBSD) || defined(SYSTEM_OS_MACOSX)
	ret = (pthread_main_np() != 0);
#elif defined(SYSTEM_OS_NETBSD) || defined(SYSTEM_OS_SOLARIS)
	ret = (rig_thread_tid() == 1);
#else
	#error System OS undefined or unsupported.
#endif

	return (ret);
}


// TLS key data
struct rig_tls {
	pthread_key_t tls;
};

/**
 * INTERNAL ABSTRACTION
 * Implementation-specific wrapper for initializing TLS keys.
 *
 * @param tls
 *     TLS key data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EAGAIN (insufficient resources, other than memory)
 *     - ENOMEM (insufficient memory)
 */
static inline bool thread_ops_tls_init(RIG_TLS tls) {
	int ret;

	if ((ret = pthread_key_create(&tls->tls, NULL))) {
		VERIFY_ERRET(ret == EAGAIN || ret == ENOMEM);
		ERRET(ret, false);
	}

	return (true);
}

/**
 * INTERNAL ABSTRACTION
 * Implementation-specific wrapper for destroying TLS keys.
 *
 * @param tls
 *     TLS key data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EINVAL (invalid TLS key value)
 */
static inline bool thread_ops_tls_destroy(RIG_TLS tls) {
	int ret;

	if ((ret = pthread_key_delete(tls->tls))) {
		VERIFY_ERRET(ret == EINVAL);
		ERRET(ret, false);
	}

	return (true);
}

/**
 * Get thread specific value for given TLS key.
 *
 * @param tls
 *     TLS key data
 *
 * @return
 *     TLS value
 */
void *rig_tls_get(RIG_TLS tls) {
	NULLCHECK_EXIT(tls);

	return (pthread_getspecific(tls->tls));
}

/**
 * Set thread specific value for given TLS key.
 *
 * @param tls
 *     TLS key data
 * @param val
 *     TLS value
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EINVAL (invalid TLS key value)
 *     - ENOMEM (insufficient memory)
 */
bool rig_tls_set(RIG_TLS tls, void *val) {
	NULLCHECK_EXIT(tls);

	int ret;

	if ((ret = pthread_setspecific(tls->tls, val))) {
		VERIFY_ERRET(ret == EINVAL || ret == ENOMEM);
		ERRET(ret, false);
	}

	return (true);
}


// Mutex Lock data
struct rig_mxlock {
	pthread_mutex_t mxlock;
};

/**
 * INTERNAL ABSTRACTION
 * Implementation-specific wrapper for initializing Mutual Exclusion (Mutex) locks.
 *
 * @param mxl
 *     Mutex Lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EAGAIN (insufficient resources, other than memory)
 *     - EBUSY (attempt to initialize the same Mutex lock twice)
 *     - ENOMEM (insufficient memory)
 */
static inline bool thread_ops_mxlock_init(RIG_MXLOCK mxl) {
	pthread_mutexattr_t mx_attrs;

	if (pthread_mutexattr_init(&mx_attrs)) {
		ERRET(ENOMEM, false);
	}

	// We always would like a recursive Mutex lock here, to be equivalent in
	// functionality to the Windows CriticalSections, it also makes more sense.
	// If that's not possible, let's try for at least an error-checking one.
	if (pthread_mutexattr_settype(&mx_attrs, PTHREAD_MUTEX_RECURSIVE)) {
		pthread_mutexattr_settype(&mx_attrs, PTHREAD_MUTEX_ERRORCHECK);
	}

	int ret;

	if ((ret = pthread_mutex_init(&mxl->mxlock, &mx_attrs))) {
		pthread_mutexattr_destroy(&mx_attrs);

		VERIFY_ERRET(ret == EAGAIN || ret == EBUSY || ret == ENOMEM);
		ERRET(ret, false);
	}

	pthread_mutexattr_destroy(&mx_attrs);

	return (true);
}

/**
 * INTERNAL ABSTRACTION
 * Implementation-specific wrapper for destroying Mutual Exclusion (Mutex) locks.
 *
 * @param mxl
 *     Mutex Lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EBUSY (lock still in use)
 *     - EINVAL (invalid Mutex lock)
 */
static inline bool thread_ops_mxlock_destroy(RIG_MXLOCK mxl) {
	int ret;

	if ((ret = pthread_mutex_destroy(&mxl->mxlock))) {
		VERIFY_ERRET(ret == EBUSY || ret == EINVAL);
		ERRET(ret, false);
	}

	return (true);
}

/**
 * Lock the specified Mutex, waiting on it if it can't be acquired.
 *
 * @param mxl
 *     Mutex Lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EAGAIN (maximum recursion reached, for recursive locks)
 *     - EDEADLK (thread already holds lock, for non-recursive locks)
 *     - EINVAL (invalid Mutex lock)
 */
bool rig_mxlock_lock(RIG_MXLOCK mxl) {
	NULLCHECK_EXIT(mxl);

	int ret;

	if ((ret = pthread_mutex_lock(&mxl->mxlock))) {
		VERIFY_ERRET(ret == EAGAIN || ret == EDEADLK || ret == EINVAL);
		ERRET(ret, false);
	}

	return (true);
}

/**
 * Try locking the specified Mutex, returning immediately if it can't be acquired.
 *
 * @param mxl
 *     Mutex Lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EAGAIN (maximum recursion reached, for recursive locks)
 *     - EBUSY (lock already held by a thread)
 *     - EINVAL (invalid Mutex lock)
 */
bool rig_mxlock_trylock(RIG_MXLOCK mxl) {
	NULLCHECK_EXIT(mxl);

	int ret;

	if ((ret = pthread_mutex_trylock(&mxl->mxlock))) {
		VERIFY_ERRET(ret == EAGAIN || ret == EBUSY || ret == EINVAL);
		ERRET(ret, false);
	}

	return (true);
}

/**
 * Unlock the specified Mutex.
 *
 * @param mxl
 *     Mutex Lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EINVAL (invalid Mutex lock)
 *     - EPERM (current thread doesn't hold the lock or Mutex not locked at all)
 */
bool rig_mxlock_unlock(RIG_MXLOCK mxl) {
	NULLCHECK_EXIT(mxl);

	int ret;

	if ((ret = pthread_mutex_unlock(&mxl->mxlock))) {
		VERIFY_ERRET(ret == EINVAL || ret == EPERM);
		ERRET(ret, false);
	}

	return (true);
}


// Condition Variable data
struct rig_condvar {
	pthread_cond_t condvar;
};

/**
 * INTERNAL ABSTRACTION
 * Implementation-specific wrapper for initializing Condition Variables.
 *
 * @param cond
 *     Condition Variable data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EAGAIN (insufficient resources, other than memory)
 *     - EBUSY (attempt to initialize the same Condition Variable twice)
 *     - ENOMEM (insufficient memory)
 */
static inline bool thread_ops_condvar_init(RIG_CONDVAR cond) {
	int ret;

	if ((ret = pthread_cond_init(&cond->condvar, NULL))) {
		VERIFY_ERRET(ret == EAGAIN || ret == EBUSY || ret == ENOMEM);
		ERRET(ret, false);
	}

	return (true);
}

/**
 * INTERNAL ABSTRACTION
 * Implementation-specific wrapper for destroying Condition Variables.
 *
 * @param cond
 *     Condition Variable data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EBUSY (condition variable still in use)
 *     - EINVAL (invalid Condition Variable)
 */
static inline bool thread_ops_condvar_destroy(RIG_CONDVAR cond) {
	int ret;

	if ((ret = pthread_cond_destroy(&cond->condvar))) {
		VERIFY_ERRET(ret == EBUSY || ret == EINVAL);
		ERRET(ret, false);
	}

	return (true);
}

/**
 * Release the specified Mutex and block on the Condition Variable.
 * After a successful return, the current thread shall own the Mutex lock.
 *
 * @param cond
 *     Condition Variable data
 * @param mxl
 *     Corresponding Mutex lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EINVAL (invalid Condition Variable or invalid Mutex lock)
 *     - EPERM (current thread doesn't own the Mutex lock)
 */
bool rig_condvar_wait(RIG_CONDVAR cond, RIG_MXLOCK mxl) {
	NULLCHECK_EXIT(cond);
	NULLCHECK_EXIT(mxl);

	int ret;

	if ((ret = pthread_cond_wait(&cond->condvar, &mxl->mxlock))) {
		VERIFY_ERRET(ret == EINVAL || ret == EPERM);
		ERRET(ret, false);
	}

	return (true);
}

/**
 * Signal one thread waiting on the specified Condition Variable.
 *
 * @param cond
 *     Condition Variable data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EINVAL (invalid Condition Variable)
 */
bool rig_condvar_signal(RIG_CONDVAR cond) {
	NULLCHECK_EXIT(cond);

	if (pthread_cond_signal(&cond->condvar)) {
		ERRET(EINVAL, false);
	}

	return (true);
}

/**
 * Signal all threads waiting on the specified Condition Variable.
 *
 * @param cond
 *     Condition Variable data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EINVAL (invalid Condition Variable)
 */
bool rig_condvar_signal_all(RIG_CONDVAR cond) {
	NULLCHECK_EXIT(cond);

	if (pthread_cond_broadcast(&cond->condvar)) {
		ERRET(EINVAL, false);
	}

	return (true);
}


// Read/Write Lock data
struct rig_rwlock {
	pthread_rwlock_t rwlock;
};

/**
 * INTERNAL ABSTRACTION
 * Implementation-specific wrapper for initializing Read/Write (RW) locks.
 *
 * @param rwl
 *     Read/Write lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EAGAIN (insufficient resources, other than memory)
 *     - EBUSY (attempt to initialize the same Read/Write lock twice)
 *     - ENOMEM (insufficient memory)
 */
static inline bool thread_ops_rwlock_init(RIG_RWLOCK rwl) {
	int ret;

	if ((ret = pthread_rwlock_init(&rwl->rwlock, NULL))) {
		VERIFY_ERRET(ret == EAGAIN || ret == EBUSY || ret == ENOMEM);
		ERRET(ret, false);
	}

	return (true);
}

/**
 * INTERNAL ABSTRACTION
 * Implementation-specific wrapper for destroying Read/Write (RW) locks.
 *
 * @param rwl
 *     Read/Write lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EBUSY (lock still in use)
 *     - EINVAL (invalid Read/Write lock)
 */
static inline bool thread_ops_rwlock_destroy(RIG_RWLOCK rwl) {
	int ret;

	if ((ret = pthread_rwlock_destroy(&rwl->rwlock))) {
		VERIFY_ERRET(ret == EBUSY || ret == EINVAL);
		ERRET(ret, false);
	}

	return (true);
}

/**
 * Lock the specified Read/Write (RW) lock for reading, waiting on it if it can't be acquired.
 *
 * @param rwl
 *     Read/Write lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EAGAIN (maximum number of read locks reached)
 *     - EDEADLK (thread already holds lock for writing)
 *     - EINVAL (invalid Read/Write lock)
 */
bool rig_rwlock_rdlock(RIG_RWLOCK rwl) {
	NULLCHECK_EXIT(rwl);

	int ret;

	if ((ret = pthread_rwlock_rdlock(&rwl->rwlock))) {
		VERIFY_ERRET(ret == EAGAIN || ret == EDEADLK || ret == EINVAL);
		ERRET(ret, false);
	}

	return (true);
}

/**
 * Try locking the specified Read/Write (RW) lock for reading, returning immediately if it can't be acquired.
 *
 * @param rwl
 *     Read/Write lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EAGAIN (maximum number of read locks reached)
 *     - EBUSY (lock already held by a thread)
 *     - EINVAL (invalid Read/Write lock)
 */
bool rig_rwlock_tryrdlock(RIG_RWLOCK rwl) {
	NULLCHECK_EXIT(rwl);

	int ret;

	if ((ret = pthread_rwlock_tryrdlock(&rwl->rwlock))) {
		VERIFY_ERRET(ret == EAGAIN || ret == EBUSY || ret == EINVAL);
		ERRET(ret, false);
	}

	return (true);
}

/**
 * Lock the specified Read/Write (RW) lock for writing, waiting on it if it can't be acquired.
 *
 * @param rwl
 *     Read/Write lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EDEADLK (thread already holds lock for writing or reading)
 *     - EINVAL (invalid Read/Write lock)
 */
bool rig_rwlock_wrlock(RIG_RWLOCK rwl) {
	NULLCHECK_EXIT(rwl);

	int ret;

	if ((ret = pthread_rwlock_wrlock(&rwl->rwlock))) {
		VERIFY_ERRET(ret == EDEADLK || ret == EINVAL);
		ERRET(ret, false);
	}

	return (true);
}

/**
 * Try locking the specified Read/Write (RW) lock for writing, returning immediately if it can't be acquired.
 *
 * @param rwl
 *     Read/Write lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EBUSY (lock already held by a thread)
 *     - EINVAL (invalid Read/Write lock)
 */
bool rig_rwlock_trywrlock(RIG_RWLOCK rwl) {
	NULLCHECK_EXIT(rwl);

	int ret;

	if ((ret = pthread_rwlock_trywrlock(&rwl->rwlock))) {
		VERIFY_ERRET(ret == EBUSY || ret == EINVAL);
		ERRET(ret, false);
	}

	return (true);
}

/**
 * Unlock the specified Read/Write (RW) lock.
 *
 * @param rwl
 *     Read/Write lock data
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EINVAL (invalid Read/Write lock)
 *     - EPERM (current thread doesn't hold the lock or Read/Write lock not locked at all)
 */
bool rig_rwlock_unlock(RIG_RWLOCK rwl) {
	NULLCHECK_EXIT(rwl);

	int ret;

	if ((ret = pthread_rwlock_unlock(&rwl->rwlock))) {
		VERIFY_ERRET(ret == EINVAL || ret == EPERM);
		ERRET(ret, false);
	}

	return (true);
}


/**
 * INTERNAL
 * Wrapper around the thread start function to ensure the cleanup function
 * rig_thread_cleanup(NULL) is correctly installed and called.
 * Specific to Pthreads, we use pthread_cleanup handlers for this, and
 * force execution on pop.
 *
 * @param thr
 *     void * for compatibility, effectively contains a RIG_THREAD
 *
 * @return
 *     void * as returned by the thread start function
 */
static void *rig_thread_starter(void *thr) {
	void *ret = NULL;

	pthread_cleanup_push(&rig_thread_cleanup, NULL);

	// Execute real thread routine
	ret = (*(((RIG_THREAD)thr)->start_routine))(((RIG_THREAD)thr)->arg);

	// Execute SMR cleanups always
	pthread_cleanup_pop(1);

	// Return the original return value
	return (ret);
}
