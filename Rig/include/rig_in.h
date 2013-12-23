/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_in.h 1133 2012-11-10 20:27:17Z llongi $
 */

#ifndef RIG_H
#define RIG_H 1

// Version definitions and strings
#define RIG_VERSION ((@PROJECT_VERSION_MAJOR@ * 10000) + (@PROJECT_VERSION_MINOR@ * 100) + @PROJECT_VERSION_PATCH@)
#define RIG_NAME_STRING "@PROJECT_NAME@"
#define RIG_VERSION_STRING "@PROJECT_VERSION@"
#define RIG_PACKAGE_STRING "@PROJECT_NAME@ @PROJECT_VERSION@"

// Compile-time features availability information
#cmakedefine RIG_STACK_PRECISE_ITERATOR 1
#cmakedefine RIG_QUEUE_PRECISE_ITERATOR 1
#cmakedefine RIG_STACK_SMR_TYPE @RIG_STACK_SMR_TYPE@
#cmakedefine RIG_QUEUE_SMR_TYPE @RIG_QUEUE_SMR_TYPE@
#cmakedefine RIG_LIST_SMR_TYPE @RIG_LIST_SMR_TYPE@

// Main includes (always present!)
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

// GCC Function Attributes definitions
#if defined(__GNUC__)
	#define ATTR_NORETURN   __attribute__ ((__noreturn__))
	#define ATTR_WARNUNUSED __attribute__ ((__warn_unused_result__))
#else
	#define ATTR_NORETURN
	#define ATTR_WARNUNUSED
#endif

// Assert-like functionality (but always active!)
void rig_acheck_exit(const char *expr, const char *func, const char *file, unsigned int line, const char *failmsg) ATTR_NORETURN;
#define rig_acheck_msg(expr, failmsg) ((expr)) ? ((void)0) : (rig_acheck_exit(#expr, __func__, __FILE__, __LINE__, (failmsg)))
#define rig_acheck(expr) rig_acheck_msg(expr, "-")

// Error reporting functionality with adaptive logging
void rig_error_log(int err, const char *func, const char *file, unsigned int line, const char *failmsg);
#define rig_erret_msg(err, ret, failmsg) rig_error_log(err, __func__, __FILE__, __LINE__, (failmsg)); errno = (err); return (ret)
#define rig_erret(err, ret) rig_erret_msg(err, ret, "-")

// Memory allocation functions
#define RIG_MEM_ALLOC_ALIGN_DEFAULT  16
#define RIG_MEM_ALLOC_ALIGN_VOIDPTR  0
#define RIG_MEM_ALLOC_ALIGN_PAGESIZE 1
#define RIG_MEM_ALLOC_ALIGN_PAD ((uint16_t)(1 << 0))

void *rig_mem_alloc(size_t trusted, size_t untrusted) ATTR_WARNUNUSED;
void *rig_mem_realloc(void *mem, size_t trusted, size_t untrusted) ATTR_WARNUNUSED;
void rig_mem_free(void *mem);

void *rig_mem_alloc_aligned(size_t trusted, size_t untrusted, size_t align, uint16_t flags) ATTR_WARNUNUSED;
// TODO: void *rig_mem_realloc_aligned(void *mem, size_t trusted, size_t untrusted, size_t align, uint16_t flags) ATTR_WARNUNUSED;
void rig_mem_free_aligned(void *mem);

size_t rig_mem_pagesize(void) ATTR_WARNUNUSED;

// Miscellaneous functions
size_t rig_misc_pid(void) ATTR_WARNUNUSED;
// TODO: void rig_misc_processname(const char *pname);

// Get ssize_t definition (if missing)
#if !defined(__ssize_t_defined) && !defined(_SSIZE_T)
	#if SIZE_MAX == UINT16_MAX
		typedef int16_t ssize_t;
	#elif SIZE_MAX == UINT32_MAX
		typedef int32_t ssize_t;
	#elif SIZE_MAX == UINT64_MAX
		typedef int64_t ssize_t;
	#else
		#error size_t is not a 16, 32 or 64 bit type. Cannot define ssize_t accordingly.
	#endif

	#define __ssize_t_defined 1
	#define _SSIZE_T 1
#endif

// Define ssize_t limits
#define SSIZE_MAX ((ssize_t)(((size_t)-1) >> 1))
#define SSIZE_MIN (-SSIZE_MAX - 1)

// Useful miscellaneous macros
#define UNUSED(var) (void)(var)
#define ERRET(err, ret) rig_erret(err, ret)
#define ERRET_CLEANUP(err, ret, ...) __VA_ARGS__; rig_erret(err, ret)
#define NULLCHECK_ERRET(var, err, ret) if ((var) == NULL) { rig_erret_msg(err, ret, "NULL check failed"); }
#define NULLCHECK_ERRET_CLEANUP(var, err, ret, ...) if ((var) == NULL) { __VA_ARGS__; rig_erret_msg(err, ret, "NULL check failed"); }
#define NULLCHECK_EXIT(var) rig_acheck_msg((var) != NULL, "NULL check failed")
#define SET_BITFIELD(bf, bit) (bf) |= (uint16_t)(bit)
#define RESET_BITFIELD(bf, bit) (bf) &= (uint16_t)(~(bit))
#define TEST_BITFIELD(bf, bit) ((bf) & (uint16_t)(bit))
#define SWAP_VAR(type, x, y) { type tmpv; tmpv = (x); (x) = (y); (y) = tmpv; }

// Non-zero if X is misaligned on a given boundary
#define MISALIGNED_SIZET(X)   ((uintptr_t)(X) & (sizeof(size_t)   - 1))
#define MISALIGNED_UINT16T(X) ((uintptr_t)(X) & (sizeof(uint16_t) - 1))
#define MISALIGNED_UINT32T(X) ((uintptr_t)(X) & (sizeof(uint32_t) - 1))
#define MISALIGNED_UINT64T(X) ((uintptr_t)(X) & (sizeof(uint64_t) - 1))

// Non-zero if X is aligned on a given boundary
#define ALIGNED_SIZET(X)   (!MISALIGNED_SIZET(X))
#define ALIGNED_UINT16T(X) (!MISALIGNED_UINT16T(X))
#define ALIGNED_UINT32T(X) (!MISALIGNED_UINT32T(X))
#define ALIGNED_UINT64T(X) (!MISALIGNED_UINT64T(X))

// Define general purpose bit masks (for size_t values)
#define RIG_LOWBITSMASK  (((size_t)-1) / 0xFF)
#define RIG_HIGHBITSMASK (RIG_LOWBITSMASK * 0x80)

// Non-zero if there is a NULL-byte in X (X is size_t)
#define RIG_FINDNULL(X) (((X) - RIG_LOWBITSMASK) & ~(X) & RIG_HIGHBITSMASK)

// Count the number of NULL-bytes in X (X is size_t)
#define RIG_COUNTNULL(X) (((RIG_FINDNULL(X) >> 7) * RIG_LOWBITSMASK) >> ((sizeof(size_t) - 1) * 8))

/*
 * Rig Counter Functions
 */

typedef struct rig_counter *RIG_COUNTER;

RIG_COUNTER rig_counter_init(size_t init_val, size_t max_val) ATTR_WARNUNUSED;
void rig_counter_destroy(RIG_COUNTER *cnt);
size_t rig_counter_get_max(RIG_COUNTER cnt) ATTR_WARNUNUSED;
size_t rig_counter_get(RIG_COUNTER cnt) ATTR_WARNUNUSED;
bool rig_counter_get_and_set(RIG_COUNTER cnt, size_t new_val, size_t *old_val);
static inline bool rig_counter_set(RIG_COUNTER cnt, size_t new_val) { return (rig_counter_get_and_set(cnt, new_val, NULL)); }
bool rig_counter_inc(RIG_COUNTER cnt);
bool rig_counter_dec(RIG_COUNTER cnt);
bool rig_counter_dec_and_test(RIG_COUNTER cnt) ATTR_WARNUNUSED;
bool rig_counter_get_and_add(RIG_COUNTER cnt, ssize_t add_val, size_t *old_val);
static inline bool rig_counter_add(RIG_COUNTER cnt, ssize_t add_val) { return (rig_counter_get_and_add(cnt, add_val, NULL)); }

/*
 * Rig Thread Functions
 */

#define RIG_THREAD_DETACHED ((uint16_t)(1 << 0))

typedef struct rig_thread *RIG_THREAD;

RIG_THREAD rig_thread_init(uint16_t flags, size_t nr_threads) ATTR_WARNUNUSED;
void rig_thread_destroy(RIG_THREAD *thr);
bool rig_thread_start(RIG_THREAD thr, void *(*start_routine)(void *arg), void *arg);
void rig_thread_exit(void *retval);
void rig_thread_yield(void);
bool rig_thread_join(RIG_THREAD thr, void *retval[]);
bool rig_thread_detach(RIG_THREAD thr);

size_t rig_thread_id(void) ATTR_WARNUNUSED;
size_t rig_thread_tid(void) ATTR_WARNUNUSED;
bool rig_thread_is_main(void) ATTR_WARNUNUSED;

typedef struct rig_tls *RIG_TLS;

RIG_TLS rig_tls_init(void) ATTR_WARNUNUSED;
void rig_tls_destroy(RIG_TLS *tls);
void *rig_tls_get(RIG_TLS tls) ATTR_WARNUNUSED;
bool rig_tls_set(RIG_TLS tls, void *val);

typedef struct rig_mxlock *RIG_MXLOCK;

RIG_MXLOCK rig_mxlock_init(void) ATTR_WARNUNUSED;
bool rig_mxlock_destroy(RIG_MXLOCK *mxl);
bool rig_mxlock_lock(RIG_MXLOCK mxl);
bool rig_mxlock_trylock(RIG_MXLOCK mxl) ATTR_WARNUNUSED;
bool rig_mxlock_unlock(RIG_MXLOCK mxl);

typedef struct rig_condvar *RIG_CONDVAR;

RIG_CONDVAR rig_condvar_init(void) ATTR_WARNUNUSED;
bool rig_condvar_destroy(RIG_CONDVAR *cond);
bool rig_condvar_wait(RIG_CONDVAR cond, RIG_MXLOCK mxl);
bool rig_condvar_signal(RIG_CONDVAR cond);
bool rig_condvar_signal_all(RIG_CONDVAR cond);

typedef struct rig_rwlock *RIG_RWLOCK;

RIG_RWLOCK rig_rwlock_init(void) ATTR_WARNUNUSED;
bool rig_rwlock_destroy(RIG_RWLOCK *rwl);
bool rig_rwlock_rdlock(RIG_RWLOCK rwl);
bool rig_rwlock_tryrdlock(RIG_RWLOCK rwl) ATTR_WARNUNUSED;
bool rig_rwlock_wrlock(RIG_RWLOCK rwl);
bool rig_rwlock_trywrlock(RIG_RWLOCK rwl) ATTR_WARNUNUSED;
bool rig_rwlock_unlock(RIG_RWLOCK rwl);

#define RIG_MLOCK_RECURSIVE ((uint16_t)(1 << 0))

typedef struct rig_mlock *RIG_MLOCK;

RIG_MLOCK rig_mlock_init(uint16_t flags) ATTR_WARNUNUSED;
bool rig_mlock_destroy(RIG_MLOCK *ml);
bool rig_mlock_lock(RIG_MLOCK ml);
bool rig_mlock_trylock(RIG_MLOCK ml) ATTR_WARNUNUSED;
bool rig_mlock_unlock(RIG_MLOCK ml);
bool rig_mlock_islocked(RIG_MLOCK ml) ATTR_WARNUNUSED;

#define RIG_MRWLOCK_RECURSIVE_READ  ((uint16_t)(1 << 0))
#define RIG_MRWLOCK_RECURSIVE_WRITE ((uint16_t)(1 << 1))
#define RIG_MRWLOCK_RECURSIVE (RIG_MRWLOCK_RECURSIVE_READ | RIG_MRWLOCK_RECURSIVE_WRITE)

typedef struct rig_mrwlock *RIG_MRWLOCK;

RIG_MRWLOCK rig_mrwlock_init(uint16_t flags) ATTR_WARNUNUSED;
bool rig_mrwlock_destroy(RIG_MRWLOCK *mrwl);
bool rig_mrwlock_rdlock(RIG_MRWLOCK mrwl);
bool rig_mrwlock_tryrdlock(RIG_MRWLOCK mrwl) ATTR_WARNUNUSED;
bool rig_mrwlock_wrlock(RIG_MRWLOCK mrwl);
bool rig_mrwlock_trywrlock(RIG_MRWLOCK mrwl) ATTR_WARNUNUSED;
bool rig_mrwlock_unlock(RIG_MRWLOCK mrwl);
bool rig_mrwlock_islocked(RIG_MRWLOCK mrwl) ATTR_WARNUNUSED;
bool rig_mrwlock_isrdlocked(RIG_MRWLOCK mrwl) ATTR_WARNUNUSED;
bool rig_mrwlock_iswrlocked(RIG_MRWLOCK mrwl) ATTR_WARNUNUSED;

/*
 * Rig Hashing Functions
 */

// Hash algorithm flags
#define RIG_HASH_DEFAULT 0x0000
#define RIG_HASH_JENKINS 0x0001
#define RIG_HASH_FNV     0x0002
#define RIG_HASH_HSIEH   0x0003
#define RIG_HASH_MURMUR2 0x0004
#define RIG_HASH_MURMUR3 0x0005
#define RIG_HASH_JENKINS_OAAT 0x0006
#define RIG_HASH_MURMUR2_OAAT 0x0007

// Hash behavior flags
#define RIG_HASH_DIRECT ((uint16_t)(1 << 15))

size_t rig_hash(const uint8_t *key, size_t key_len, uint16_t hash_flags) ATTR_WARNUNUSED;

/*
 * Rig SMR Functions
 */

typedef struct rig_smr_hp_record *RIG_SMR_HP_Record;

RIG_SMR_HP_Record rig_smr_hp_record_get(void) ATTR_WARNUNUSED;
void rig_smr_hp_record_release(void);
void *rig_smr_hp_get(RIG_SMR_HP_Record hprecord, size_t hp) ATTR_WARNUNUSED;
void rig_smr_hp_set(RIG_SMR_HP_Record hprecord, size_t hp, void *ptr);
void rig_smr_hp_release(RIG_SMR_HP_Record hprecord, size_t hp);
void rig_smr_hp_mem_retire(void *mem);
void rig_smr_hp_mem_retire_noscan(void *mem);
void rig_smr_hp_mem_scan(void);
void rig_smr_hp_mem_scan_full(void);
void rig_smr_hp_debug_info(bool print_list);

void rig_smr_epoch_record_release(void);
void rig_smr_epoch_critical_enter(void);
void rig_smr_epoch_critical_exit(void);
void rig_smr_epoch_mem_retire(void *mem);
void rig_smr_epoch_debug_info(bool print_list);

/*
 * Rig List Functions
 */

#define RIG_LIST_NOCOUNT ((uint16_t)(1 << 0))
#define RIG_LIST_NODUPS  ((uint16_t)(1 << 1))

typedef struct rig_list *RIG_LIST;

RIG_LIST rig_list_init(size_t capacity, uint16_t flags,
	int (*cmp)(void *data, void *item), size_t (*hash)(void *item)) ATTR_WARNUNUSED;
RIG_LIST rig_list_newref(RIG_LIST l) ATTR_WARNUNUSED;
void rig_list_destroy(RIG_LIST *l);
void rig_list_clear(RIG_LIST l);
bool rig_list_add(RIG_LIST l, void *item);
bool rig_list_del(RIG_LIST l, void *item);
bool rig_list_find(RIG_LIST l, void *item) ATTR_WARNUNUSED;
void *rig_list_get(RIG_LIST l);
void *rig_list_peek(RIG_LIST l);
bool rig_list_empty(RIG_LIST l) ATTR_WARNUNUSED;
bool rig_list_full(RIG_LIST l) ATTR_WARNUNUSED;
size_t rig_list_count(RIG_LIST l) ATTR_WARNUNUSED;
size_t rig_list_capacity(RIG_LIST l) ATTR_WARNUNUSED;

typedef struct rig_list_iter *RIG_LIST_ITER;

RIG_LIST_ITER rig_list_iter_begin(RIG_LIST l) ATTR_WARNUNUSED;
void *rig_list_iter_next(RIG_LIST_ITER iter) ATTR_WARNUNUSED;
void rig_list_iter_delete(RIG_LIST_ITER iter);
void rig_list_iter_end(RIG_LIST_ITER *iter);

RIG_LIST rig_list_duplicate(RIG_LIST l) ATTR_WARNUNUSED;

/*
 * Rig Queue Functions
 */

#define RIG_QUEUE_NOCOUNT ((uint16_t)(1 << 0))

typedef struct rig_queue *RIG_QUEUE;

RIG_QUEUE rig_queue_init(size_t capacity, uint16_t flags) ATTR_WARNUNUSED;
RIG_QUEUE rig_queue_newref(RIG_QUEUE q) ATTR_WARNUNUSED;
void rig_queue_destroy(RIG_QUEUE *q);
void rig_queue_clear(RIG_QUEUE q);
bool rig_queue_put(RIG_QUEUE q, void *item);
void *rig_queue_get(RIG_QUEUE q);
void *rig_queue_peek(RIG_QUEUE q);
bool rig_queue_empty(RIG_QUEUE q) ATTR_WARNUNUSED;
bool rig_queue_full(RIG_QUEUE q) ATTR_WARNUNUSED;
size_t rig_queue_count(RIG_QUEUE q) ATTR_WARNUNUSED;
size_t rig_queue_capacity(RIG_QUEUE q) ATTR_WARNUNUSED;

typedef struct rig_queue_iter *RIG_QUEUE_ITER;

RIG_QUEUE_ITER rig_queue_iter_begin(RIG_QUEUE q) ATTR_WARNUNUSED;
void *rig_queue_iter_next(RIG_QUEUE_ITER iter) ATTR_WARNUNUSED;
void rig_queue_iter_end(RIG_QUEUE_ITER *iter);

RIG_QUEUE rig_queue_duplicate(RIG_QUEUE q) ATTR_WARNUNUSED;

/*
 * Rig Stack Functions
 */

#define RIG_STACK_NOCOUNT ((uint16_t)(1 << 0))

typedef struct rig_stack *RIG_STACK;

RIG_STACK rig_stack_init(size_t capacity, uint16_t flags) ATTR_WARNUNUSED;
RIG_STACK rig_stack_newref(RIG_STACK s)  ATTR_WARNUNUSED;
void rig_stack_destroy(RIG_STACK *s);
void rig_stack_clear(RIG_STACK s);
bool rig_stack_push(RIG_STACK s, void *item);
void *rig_stack_pop(RIG_STACK s);
void *rig_stack_peek(RIG_STACK s);
bool rig_stack_empty(RIG_STACK s) ATTR_WARNUNUSED;
bool rig_stack_full(RIG_STACK s) ATTR_WARNUNUSED;
size_t rig_stack_count(RIG_STACK s) ATTR_WARNUNUSED;
size_t rig_stack_capacity(RIG_STACK s) ATTR_WARNUNUSED;

typedef struct rig_stack_iter *RIG_STACK_ITER;

RIG_STACK_ITER rig_stack_iter_begin(RIG_STACK s) ATTR_WARNUNUSED;
void *rig_stack_iter_next(RIG_STACK_ITER iter) ATTR_WARNUNUSED;
void rig_stack_iter_end(RIG_STACK_ITER *iter);

RIG_STACK rig_stack_duplicate(RIG_STACK s) ATTR_WARNUNUSED;

/*
 * Rig Array Functions - NOT FINALIZED, SUBJECT TO CHANGE!
 */

#define RIG_ARRAY_DUPLICATE ((uint16_t)(1 << 0))

struct rig_aspec {
	uint8_t type[2];
	uint16_t count;
	uint16_t flags;
	uint16_t id;
};

#define RIG_ASPEC(...) (RIG_ARRAY_ASP []) { __VA_ARGS__ }, (sizeof((RIG_ARRAY_ASP []) { __VA_ARGS__ }) / sizeof(RIG_ARRAY_ASP))

typedef const struct rig_aspec RIG_ARRAY_ASP;
typedef struct rig_array *RIG_ARRAY;

RIG_ARRAY rig_array_init(uint16_t flags, size_t capacity) ATTR_WARNUNUSED;
RIG_ARRAY rig_array_duplicate(RIG_ARRAY a, uint16_t flags) ATTR_WARNUNUSED;
RIG_ARRAY rig_array_newref(RIG_ARRAY a) ATTR_WARNUNUSED;
void rig_array_destroy(RIG_ARRAY *a);
void rig_array_clear(RIG_ARRAY a);
bool rig_array_add(RIG_ARRAY a, RIG_ARRAY_ASP tinfo[], size_t tsize, ...);
bool rig_array_del(RIG_ARRAY a, RIG_ARRAY_ASP tinfo[], size_t tsize, ...);
bool rig_array_find(RIG_ARRAY a, RIG_ARRAY_ASP tinfo[], size_t tsize, ...) ATTR_WARNUNUSED;
bool rig_array_find_any(RIG_ARRAY a, RIG_ARRAY_ASP tinfo[], size_t tsize, ...) ATTR_WARNUNUSED;
bool rig_array_lookup(RIG_ARRAY a, RIG_ARRAY_ASP tinfo[], size_t tsize, ...) ATTR_WARNUNUSED;
RIG_ARRAY rig_array_nested(RIG_ARRAY a, RIG_ARRAY_ASP tinfo[], size_t tsize, ...) ATTR_WARNUNUSED;
bool rig_array_empty(RIG_ARRAY a) ATTR_WARNUNUSED;
bool rig_array_full(RIG_ARRAY a) ATTR_WARNUNUSED;
size_t rig_array_count(RIG_ARRAY a) ATTR_WARNUNUSED;
size_t rig_array_capacity(RIG_ARRAY a) ATTR_WARNUNUSED;

#endif /* RIG_H */
