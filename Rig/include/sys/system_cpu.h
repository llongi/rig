/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: system_cpu.h 847 2011-02-09 11:48:19Z llongi $
 */

#ifndef SYSTEM_CPU_H
#define SYSTEM_CPU_H 1

#undef SYSTEM_CPU_X86
#undef SYSTEM_CPU_X86_64
#undef SYSTEM_CPU_IA64
#undef SYSTEM_CPU_PPC
#undef SYSTEM_CPU_SPARC
#undef SYSTEM_CPU_ARM
#undef SYSTEM_CPU_MIPS
#undef SYSTEM_CPU_ALPHA
#undef SYSTEM_CPU_HPPA
#undef SYSTEM_CPU_SH

#if defined(i386) || defined(__i386) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__) \
 || defined(_M_IX86) || defined(_X86_) || defined(__X86__) || defined(__I86__) || defined(__THW_INTEL__) || defined(__INTEL__)
#if defined(__i386__)
	#define SYSTEM_CPU_X86 386
#elif defined(__i486__)
	#define SYSTEM_CPU_X86 486
#elif defined(__i586__)
	#define SYSTEM_CPU_X86 586
#elif defined(__i686__)
	#define SYSTEM_CPU_X86 686
#elif defined(_M_IX86)
	#define SYSTEM_CPU_X86 _M_IX86
#elif defined(__I86__)
	#define SYSTEM_CPU_X86 ((__I86__) * 100)
#else
	#define SYSTEM_CPU_X86 1
#endif
#endif

#if defined(__amd64) || defined(__amd64__) || defined(__x86_64) || defined(__x86_64__) \
 || defined(_M_X64)
	#define SYSTEM_CPU_X86_64 1
#endif

#if defined(__ia64) || defined(__ia64__) || defined(_IA64) \
 || defined(__IA64) || defined(__IA64__) || defined(_M_IA64)
	#if defined(_M_IA64)
		#define SYSTEM_CPU_IA64 (_M_IA64)
	#else
		#define SYSTEM_CPU_IA64 1
	#endif
#endif

#if defined(__powerpc) || defined(__powerpc__) || defined(__POWERPC__) \
 || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC) || defined(_M_PPC)
	#if defined(_M_PPC)
		#define SYSTEM_CPU_PPC (_M_PPC)
	#else
		#define SYSTEM_CPU_PPC 1
	#endif
#endif

#if defined(__sparc) || defined(__sparc__)
	#if defined(__sparcv9)
		#define SYSTEM_CPU_SPARC 9
	#elif defined(__sparcv8)
		#define SYSTEM_CPU_SPARC 8
	#else
		#define SYSTEM_CPU_SPARC 1
	#endif
#endif

#if defined(__arm) || defined(__arm__) || defined(__TARGET_ARCH_ARM) || defined(_ARM)
	#define SYSTEM_CPU_ARM 1
#endif

#if defined(mips) || defined(__mips) || defined(__mips__) || defined(__MIPS__)
	#if defined(__mips)
		#define SYSTEM_CPU_MIPS (__mips)
	#elif defined(_MIPS_ISA)
		#define SYSTEM_CPU_MIPS (_MIPS_ISA)
	#else
		#define SYSTEM_CPU_MIPS 1
	#endif
#endif

#if defined(__alpha) || defined(__alpha__) || defined(_M_ALPHA)
	#define SYSTEM_CPU_ALPHA 1
#endif

#if defined(__hppa) || defined(__hppa__)
	#define SYSTEM_CPU_HPPA 1
#endif

#if defined(__sh) || defined(__sh__)
	#define SYSTEM_CPU_SH 1
#endif

#endif /* SYSTEM_CPU_H */
