/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: system_libc.h 844 2011-02-09 08:02:28Z llongi $
 */

#ifndef SYSTEM_LIBC_H
#define SYSTEM_LIBC_H 1

#undef SYSTEM_LIBC_GLIBC
#undef SYSTEM_LIBC_UCLIBC

#undef SYSTEM_LIBCXX_GLIBCXX
#undef SYSTEM_LIBCXX_INTELCXX

#include <limits.h>

// GNU libc support
#if defined(__GNU_LIBRARY__)
	#if defined(__GNU_LIBRARY_MINOR__)
		#define SYSTEM_LIBC_GLIBC (((__GNU_LIBRARY__) * 100) + (__GNU_LIBRARY_MINOR__))
	#else
		#define SYSTEM_LIBC_GLIBC ((__GNU_LIBRARY__) * 100)
	#endif
#endif

#if defined(__GLIBC__)
	// We override an eventual definition based upon the old macros with the new ones
	#undef SYSTEM_LIBC_GLIBC

	#if defined(__GLIBC_MINOR__)
		#define SYSTEM_LIBC_GLIBC (((__GLIBC__) * 100) + (__GLIBC_MINOR__))
	#else
		#define SYSTEM_LIBC_GLIBC ((__GLIBC__) * 100)
	#endif
#endif

// uClibc support
#if defined(__UCLIBC__)
	#if defined(__UCLIBC_MINOR__) && defined(__UCLIBC_SUBLEVEL__)
		#define SYSTEM_LIBC_UCLIBC (((__UCLIBC_MAJOR__) * 10000) + ((__UCLIBC_MINOR__) * 100) + (__UCLIBC_SUBLEVEL__))
	#elif defined(__UCLIBC_MINOR__)
		#define SYSTEM_LIBC_UCLIBC (((__UCLIBC_MAJOR__) * 10000) + ((__UCLIBC_MINOR__) * 100))
	#else
		#define SYSTEM_LIBC_UCLIBC ((__UCLIBC_MAJOR__) * 10000)
	#endif
#endif

// GNU libstdc++ support
#if defined(__GLIBCPP__)
	#define SYSTEM_LIBCXX_GLIBCXX (__GLIBCPP__)
#endif

#if defined(__GLIBCXX__)
	// We override an eventual definition based upon the old macros with the new ones
	#undef SYSTEM_LIBCXX_GLIBCXX

	#define SYSTEM_LIBCXX_GLIBCXX (__GLIBCXX__)
#endif

// Intel C++ Run-Time Libraries support
#if defined(__INTEL_CXXLIB_ICC)
	#define SYSTEM_LIBCXX_INTELCXX 1
#endif

#endif /* SYSTEM_LIBC_H */
