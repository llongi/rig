/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: system_all.h 1045 2012-05-27 09:25:13Z llongi $
 */

#ifndef SYSTEM_ALL_H
#define SYSTEM_ALL_H 1

// Aggregate all system defines and implement a few advanced features
#include "system_cc.h"
#include "system_cpu.h"
#include "system_libc.h"
#include "system_os.h"

// Alignment specification support (with defines for cache line alignment)
#undef ATTR_ALIGNED
#undef CACHELINE_ALIGNED
#undef CACHELINE_ALONE

#if !defined(CACHELINE_SIZE)
	#define CACHELINE_SIZE 64 // Default (big enough for almost all processors)
	// Must be power of two!
#endif

#if defined(SYSTEM_CC_GNUCC) || defined(SYSTEM_CC_SUNCC)
	#define ATTR_ALIGNED(x) __attribute__ ((__aligned__ (x)))
#else
	#define ATTR_ALIGNED(x)
#endif

#define CACHELINE_ALIGNED ATTR_ALIGNED(CACHELINE_SIZE)
#define CACHELINE_ALONE(t, v) t v CACHELINE_ALIGNED; uint8_t PAD_##v[CACHELINE_SIZE - (sizeof(t) & (CACHELINE_SIZE - 1))]

// Library constructor/destructor support
#undef ATTR_CONSTRUCTOR
#undef ATTR_DESTRUCTOR

#if defined(SYSTEM_CC_GNUCC) || defined(SYSTEM_CC_SUNCC)
	#define ATTR_CONSTRUCTOR __attribute__ ((__constructor__))
	#define ATTR_DESTRUCTOR __attribute__ ((__destructor__))
#else
	#define ATTR_CONSTRUCTOR
	#define ATTR_DESTRUCTOR
#endif

// Forced inlining support
#undef ATTR_ALWAYSINLINE

#if defined(SYSTEM_CC_GNUCC)
	#define ATTR_ALWAYSINLINE __attribute__ ((__always_inline__))
#else
	#define ATTR_ALWAYSINLINE
#endif

// Load-time TLS support (run-time available via Rig/Pthreads/Win32-Threads)
#undef SYSTEM_TLS_SUPPORT
#undef SYSTEM_TLS_DECL

#if defined(SYSTEM_OS_LINUX) \
 && (defined(SYSTEM_LIBC_GLIBC) && SYSTEM_LIBC_GLIBC >= 206)
	#if (defined(SYSTEM_CC_GNUCC) && SYSTEM_CC_GNUCC >= 40102) \
	 || (defined(SYSTEM_CC_CLANG) && SYSTEM_CC_CLANG >= 20800) \
	 || (defined(SYSTEM_CC_INTEL) && SYSTEM_CC_INTEL >= 900) \
	 || (defined(SYSTEM_CC_SUNCC) && SYSTEM_CC_SUNCC >= 0x590)
		#define SYSTEM_TLS_SUPPORT 1
		#define SYSTEM_TLS_DECL __thread
	#endif
#elif (defined(SYSTEM_OS_FREEBSD) && SYSTEM_OS_FREEBSD >= 700041)
	#if (defined(SYSTEM_CC_GNUCC) && SYSTEM_CC_GNUCC >= 40102) \
	 || (defined(SYSTEM_CC_CLANG) && SYSTEM_CC_CLANG >= 20800)
		#define SYSTEM_TLS_SUPPORT 1
		#define SYSTEM_TLS_DECL __thread
	#endif
#elif defined(SYSTEM_OS_SOLARIS)
	#if (defined(SYSTEM_CC_GNUCC) && SYSTEM_CC_GNUCC >= 30402) \
	 || (defined(SYSTEM_CC_SUNCC) && SYSTEM_CC_SUNCC >= 0x590)
		#define SYSTEM_TLS_SUPPORT 1
		#define SYSTEM_TLS_DECL __thread
	#endif
#elif defined(SYSTEM_OS_WIN32)
	#if (defined(SYSTEM_CC_MSVC) && SYSTEM_CC_MSVC >= 1400) \
	 || (defined(SYSTEM_CC_INTEL) && SYSTEM_CC_INTEL >= 900)
		#define SYSTEM_TLS_SUPPORT 1
		#define SYSTEM_TLS_DECL __declspec(thread)
	#endif
#endif

#endif /* SYSTEM_ALL_H */
