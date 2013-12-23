/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_mem.c 1111 2012-10-20 20:41:40Z llongi $
 */

#include "rig_internal.h"
#include <unistd.h>


/**
 * Allocate memory amounting to (trusted + untrusted), returning a pointer to it
 * on success, or NULL on failure. The division in two makes it possible to
 * easily check against zero-sized allocation, as well as certain overflows.
 * The memory is left uninitialized.
 *
 * @param trusted_size
 *     trusted size to allocate, between 0 and SIZE_MAX, use this to allocate
 *     amounts you know you can trust, such as constants and sizeof()'s
 * @param untrusted_size
 *     untrusted size to allocate, between 0 and SIZE_MAX, use this for user or
 *     environment supplied values
 *
 * @return
 *     pointer to memory, NULL on error.
 *     On error, the following error codes are set:
 *     - EINVAL (invalid size passed)
 *     - ENOMEM (insufficient memory)
 */
void *rig_mem_alloc(size_t trusted_size, size_t untrusted_size) {
	// Check total size against zero ...
	if ((trusted_size == 0) && (untrusted_size == 0)) {
		ERRET(EINVAL, NULL);
	}

	// ... and against overflow
	if ((SIZE_MAX - trusted_size) < untrusted_size) {
		ERRET(EINVAL, NULL);
	}

	// Allocate memory
	void *memory_ptr = malloc(trusted_size + untrusted_size);
	NULLCHECK_ERRET(memory_ptr, ENOMEM, NULL);

	return (memory_ptr);
}

/**
 * Resize memory previously gotten from rig_mem_alloc() to (trusted + untrusted),
 * returning a pointer to the resized memory on success or NULL on failure,
 * in which case the memory pointed to by memory_ptr won't be touched in any way.
 * The division in two makes it possible to easily check against zero-sized
 * allocation, as well as certain overflows.
 * The contents of the memory block will be unchanged up to the minimum of
 * the old and new sizes, newly allocated memory will be uninitialized.
 *
 * @param memory_ptr
 *     pointer to memory to resize, cannot be NULL
 * @param trusted_size
 *     trusted size to allocate, between 0 and SIZE_MAX, use this to allocate
 *     amounts you know you can trust, such as constants and sizeof()'s
 * @param untrusted_size
 *     untrusted size to allocate, between 0 and SIZE_MAX, use this for user or
 *     environment supplied values
 *
 * @return
 *     pointer to resized memory, NULL on error.
 *     On error, the following error codes are set:
 *     - EINVAL (invalid size passed)
 *     - ENOMEM (insufficient memory)
 */
void *rig_mem_realloc(void *memory_ptr, size_t trusted_size, size_t untrusted_size) {
	NULLCHECK_EXIT(memory_ptr);

	// Check total size against zero ...
	if ((trusted_size == 0) && (untrusted_size == 0)) {
		ERRET(EINVAL, NULL);
	}

	// ... and against overflow
	if ((SIZE_MAX - trusted_size) < untrusted_size) {
		ERRET(EINVAL, NULL);
	}

	// Reallocate memory
	void *realloc_memory_ptr = realloc(memory_ptr, trusted_size + untrusted_size);
	NULLCHECK_ERRET(realloc_memory_ptr, ENOMEM, NULL);

	return (realloc_memory_ptr);
}

/**
 * Free memory previously gotten from rig_mem_alloc() or rig_mem_realloc().
 *
 * @param memory_ptr
 *     pointer to memory to free, cannot be NULL
 */
void rig_mem_free(void *memory_ptr) {
	NULLCHECK_EXIT(memory_ptr);

	free(memory_ptr);
}


/**
 * Allocate memory amounting to (trusted + untrusted) and align it on the
 * boundary specified by the 'align' argument, returning a pointer to it
 * on success, or NULL on failure. The division in two makes it possible to
 * easily check against zero-sized allocation, as well as certain overflows.
 * The memory is left uninitialized.
 * The function tries to use OS-supplied aligned memory allocation if possible,
 * if none is available, manual alignment is done, which may waste more space.
 *
 * @param trusted_size
 *     trusted size to allocate, between 0 and SIZE_MAX, use this to allocate
 *     amounts you know you can trust, such as constants and sizeof()'s
 * @param untrusted_size
 *     untrusted size to allocate, between 0 and SIZE_MAX, use this for user or
 *     environment supplied values
 * @param alignment
 *     alignment boundary for the memory block, must be a power of two and a
 *     multiple of sizeof(void *). Maximum supported alignment is 2^60 on 64bit
 *     systems and 2^28 on 32bit systems.
 *     The following special values are supported for convenience:
 *     - RIG_MEM_ALLOC_ALIGN_DEFAULT (16): default alignment of 16 bytes
 *     - RIG_MEM_ALLOC_ALIGN_VOIDPTR  (0): align to sizeof(void *)
 *     - RIG_MEM_ALLOC_ALIGN_PAGESIZE (1): align to system's page size
 * @param flags
 *     flags to influence allocation behavior.
 *     The following special flags are supported:
 *     - RIG_MEM_ALLOC_ALIGN_PAD: pad allocation to next alignment boundary
 *
 * @return
 *     pointer to memory, NULL on error.
 *     On error, the following error codes are set:
 *     - EINVAL (invalid size or alignment passed)
 *     - ENOMEM (insufficient memory)
 */
void *rig_mem_alloc_aligned(size_t trusted_size, size_t untrusted_size, size_t alignment, uint16_t flags) {
	CHECK_PERMITTED_FLAGS(flags, RIG_MEM_ALLOC_ALIGN_PAD)

	// Check total size against zero ...
	if ((trusted_size == 0) && (untrusted_size == 0)) {
		ERRET(EINVAL, NULL);
	}

	// ... and against overflow
	if ((SIZE_MAX - trusted_size) < untrusted_size) {
		ERRET(EINVAL, NULL);
	}

	// Reduce it to one value
	size_t allocation_size = trusted_size + untrusted_size;

	// Check that alignment's value makes sense
	if (alignment > ((size_t)1 << ((sizeof(size_t) * 8) - 4))) {
		ERRET(EINVAL, NULL);
	}

	// Special alignment value: 0 means to align to sizeof(void *)
	if (alignment == RIG_MEM_ALLOC_ALIGN_VOIDPTR) {
		alignment = sizeof(void *);
	}

	// Special alignment value: 1 means to align to page size
	if (alignment == RIG_MEM_ALLOC_ALIGN_PAGESIZE) {
		alignment = rig_mem_pagesize();
	}

	// Alignment must be both a power of two and a multiple of sizeof(void *)
	if ((alignment & (alignment - 1)) || (alignment & (sizeof(void *) - 1))) {
		ERRET(EINVAL, NULL);
	}

	// Add padding to next alignment boundary
	if (TEST_BITFIELD(flags, RIG_MEM_ALLOC_ALIGN_PAD)) {
		allocation_size = (allocation_size & ~(alignment - 1)) + alignment;
	}

	// This cannot underflow since alignment is significantly smaller than SIZE_MAX
	const size_t ALIGN_SIZE_MAX = SIZE_MAX - sizeof(void *) - alignment + 1;

	// Check that there is space for eventual manual alignment
	if (allocation_size > ALIGN_SIZE_MAX) {
		ERRET(EINVAL, NULL);
	}

	// Allocate memory
	void *memory_ptr = NULL;

#if defined(HAVE_POSIX_MEMALIGN)
	if (posix_memalign(&memory_ptr, alignment, allocation_size)) {
		ERRET(ENOMEM, NULL);
	}
#elif defined(HAVE_ALIGNED_MALLOC)
	memory_ptr = _aligned_malloc(allocation_size, alignment);
	NULLCHECK_ERRET(memory_ptr, ENOMEM, NULL);
#else
	// Manual aligned malloc
	memory_ptr = rig_mem_alloc(sizeof(void *) - 1, allocation_size + alignment);
	NULLCHECK_ERRET(memory_ptr, ENOMEM, NULL);
	void **aligned_memory_ptr = ((void **)(((uintptr_t)memory_ptr + sizeof(void *) + alignment - 1) & ~(alignment - 1)));
	aligned_memory_ptr[-1] = memory_ptr;
	memory_ptr = aligned_memory_ptr;
#endif

	return (memory_ptr);
}

/**
 * Free memory previously gotten from rig_mem_alloc_aligned() or rig_mem_realloc_aligned().
 * You must use this function for aligned memory!
 *
 * @param memory_ptr
 *     pointer to memory to free, cannot be NULL
 */
void rig_mem_free_aligned(void *memory_ptr) {
	NULLCHECK_EXIT(memory_ptr);

#if defined(HAVE_POSIX_MEMALIGN)
	free(memory_ptr);
#elif defined(HAVE_ALIGNED_MALLOC)
	_aligned_free(memory_ptr);
#else
	// Manual free of aligned malloc
	rig_mem_free(((void **)memory_ptr)[-1]);
#endif
}


/**
 * Get system page size, using OS-specific calls.
 *
 * @return
 *     system's page size, in bytes
 */
size_t rig_mem_pagesize(void) {
	size_t page_size;

#if defined(SYSTEM_OS_UNIX)
	#if defined(_SC_PAGESIZE)
		page_size = (size_t)sysconf(_SC_PAGESIZE);
	#else
		page_size = (size_t)getpagesize();
	#endif
#elif defined(SYSTEM_OS_MACOSX)
	page_size = (size_t)getpagesize();
#elif defined(SYSTEM_OS_WIN32)
	SYSTEM_INFO sys_info;
	GetSystemInfo(&sys_info);
	page_size = (size_t)sys_info.dwPageSize;
#else
	#error System OS undefined or unsupported.
#endif

	return (page_size);
}
