/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: system_os.h 847 2011-02-09 11:48:19Z llongi $
 */

#ifndef SYSTEM_OS_H
#define SYSTEM_OS_H 1

#undef SYSTEM_OS_UNIX
#undef SYSTEM_OS_LINUX
#undef SYSTEM_OS_BSD
#undef SYSTEM_OS_FREEBSD
#undef SYSTEM_OS_OPENBSD
#undef SYSTEM_OS_NETBSD
#undef SYSTEM_OS_DRAGONFLY
#undef SYSTEM_OS_BSDOS
#undef SYSTEM_OS_SVR4
#undef SYSTEM_OS_SOLARIS
#undef SYSTEM_OS_SUNOS
#undef SYSTEM_OS_OS2
#undef SYSTEM_OS_HPUX
#undef SYSTEM_OS_IRIX
#undef SYSTEM_OS_MINIX

#undef SYSTEM_OS_APPLE
#undef SYSTEM_OS_MACOS9
#undef SYSTEM_OS_MACOSX

#undef SYSTEM_OS_WIN32
#undef SYSTEM_OS_WIN64

// Unix support
#if defined(unix) || defined(__unix) || defined(__unix__)
	#include <sys/param.h>

	#define SYSTEM_OS_UNIX 1
#endif

#if defined(linux) || defined(__linux) || defined(__linux__)
	#define SYSTEM_OS_LINUX 1
#endif

#if defined(BSD) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) \
 || defined(__DragonFly__) || defined(__bsdi__) || defined(_SYSTYPE_BSD)
	#if defined(BSD)
		#define SYSTEM_OS_BSD (BSD)
	#else
		#define SYSTEM_OS_BSD 1
	#endif
#endif

#if defined(__FreeBSD__)
	#if (__FreeBSD__) >= 2
		#include <osreldate.h>

		#define SYSTEM_OS_FREEBSD (__FreeBSD_version)
	#else
		#define SYSTEM_OS_FREEBSD (__FreeBSD__)
	#endif
#endif

#if defined(__OpenBSD__)
	#if defined(OpenBSD)
		#define SYSTEM_OS_OPENBSD (OpenBSD)
	#else
		#define SYSTEM_OS_OPENBSD 1
	#endif
#endif

#if defined(__NetBSD__)
	#if defined(__NetBSD_Version__)
		#define SYSTEM_OS_NETBSD (__NetBSD_Version__)
	#else
		#define SYSTEM_OS_NETBSD 1
	#endif
#endif

#if defined(__DragonFly__)
	#define SYSTEM_OS_DRAGONFLY 1
#endif

#if defined(__bsdi__)
	#define SYSTEM_OS_BSDOS 1
#endif

#if defined(__sysv__) || defined(__SVR4) || defined(__svr4__) || defined(_SYSTYPE_SVR4)
	#define SYSTEM_OS_SVR4 1
#endif

#if defined(sun) || defined(__sun) || defined(__sun__)
	#if defined(SYSTEM_OS_SVR4)
		#define SYSTEM_OS_SOLARIS 1
	#else
		#define SYSTEM_OS_SUNOS 1
	#endif
#endif

#if defined(OS2) || defined(_OS2) || defined(__OS2) || defined(__OS2__) || defined(__TOS_OS2__)
	#define SYSTEM_OS_OS2 1
#endif

#if defined(hpux) || defined(_hpux) || defined(__hpux) || defined(__hpux__)
	#define SYSTEM_OS_HPUX 1
#endif

#if defined(sgi) || defined(__sgi) || defined(__sgi__)
	#define SYSTEM_OS_IRIX 1
#endif

#if defined(minix) || defined(__minix) || defined(__minix__)
	#define SYSTEM_OS_MINIX 1
#endif

// Apple support
#if defined(macintosh) || defined(Macintosh)
	#define SYSTEM_OS_APPLE 9
	#define SYSTEM_OS_MACOS9 1
#endif

#if defined(__APPLE__) && defined (__MACH__)
	#define SYSTEM_OS_APPLE 10
	#define SYSTEM_OS_MACOSX 1
#endif

// Windows support
#if defined(_WIN32) || defined(__WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
	#define SYSTEM_OS_WIN32 1
#endif

#if defined(_WIN64) || defined(__WIN64) || defined(__WIN64__)
	#define SYSTEM_OS_WIN64 1
#endif

#endif /* SYSTEM_OS_H */
