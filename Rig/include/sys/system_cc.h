/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: system_cc.h 848 2011-02-09 14:18:19Z llongi $
 */

#ifndef SYSTEM_CC_H
#define SYSTEM_CC_H 1

#undef SYSTEM_CC_GNUCC
#undef SYSTEM_CC_CLANG
#undef SYSTEM_CC_INTEL
#undef SYSTEM_CC_SUNCC
#undef SYSTEM_CC_TINYC
#undef SYSTEM_CC_CYGWIN
#undef SYSTEM_CC_MINGW
#undef SYSTEM_CC_MSVC
#undef SYSTEM_CC_IBMXL
#undef SYSTEM_CC_BORLAND
#undef SYSTEM_CC_WATCOM
#undef SYSTEM_CC_MWERKS
#undef SYSTEM_CC_PGI
#undef SYSTEM_CC_DMC
#undef SYSTEM_CC_RVC

// GNU CC support
#if defined(__GNUC__)
	#if defined(__GNUC_PATCHLEVEL__)
		#define SYSTEM_CC_GNUCC (((__GNUC__) * 10000) + ((__GNUC_MINOR__) * 100) + (__GNUC_PATCHLEVEL__))
	#else
		#define SYSTEM_CC_GNUCC (((__GNUC__) * 10000) + ((__GNUC_MINOR__) * 100))
	#endif
#endif

// Clang support
#if defined(__clang__)
	#define SYSTEM_CC_CLANG (((__clang_major__) * 10000) + ((__clang_minor__) * 100) + (__clang_patchlevel__))
#endif

// Intel CC support
#if defined(__INTEL_COMPILER) || defined(__ICC) || defined(__ICL)
	#define SYSTEM_CC_INTEL (__INTEL_COMPILER)
#endif

// Sun Studio support
#if defined(__SUNPRO_C)
	#define SYSTEM_CC_SUNCC (__SUNPRO_C)
#endif

// Tiny C support
#if defined(__TINYC__)
	#define SYSTEM_CC_TINYC 1
#endif

// Cygwin support
#if defined(__CYGWIN__)
	#define SYSTEM_CC_CYGWIN 1
#endif

// MinGW support
#if defined(__MINGW32__)
	#define SYSTEM_CC_MINGW (((__MINGW32_MAJOR_VERSION) * 100) + (__MINGW32_MINOR_VERSION))
#endif

// Microsoft Visual C++ support
#if defined(_MSC_VER)
	#define SYSTEM_CC_MSVC (_MSC_VER)
#endif

// IBM XL support
#if defined(__xlc__)
	#define SYSTEM_CC_IBMXL (__xlc__)
#endif

#if defined(__IBMC__)
	// We override an eventual definition based upon the old macros with the new ones
	#undef SYSTEM_CC_IBMXL

	#define SYSTEM_CC_IBMXL (__IBMC__)
#endif

// Borland C++ support
#if defined(__BORLANDC__)
	#define SYSTEM_CC_BORLAND (__BORLANDC__)
#endif

#if defined(__CODEGEARC__)
	// We override an eventual definition based upon the old macros with the new ones
	#undef SYSTEM_CC_BORLAND

	#define SYSTEM_CC_BORLAND (__CODEGEARC__)
#endif

// Watcom C++ support
#if defined(__WATCOMC__)
	#define SYSTEM_CC_WATCOM (__WATCOMC__)
#endif

// Metrowerks CodeWarrior support
#if defined(__MWERKS__)
	#define SYSTEM_CC_MWERKS (__MWERKS__)
#endif

// Portland Group support
#if defined(__PGI)
	#define SYSTEM_CC_PGI 1
#endif

// Digital Mars support
#if defined(__DMC__)
	#define SYSTEM_CC_DMC (__DMC__)
#endif

// RealView C support
#if defined(__CC_ARM)
	#define SYSTEM_CC_RVC (__ARMCC_VERSION)
#endif

#endif /* SYSTEM_CC_H */
