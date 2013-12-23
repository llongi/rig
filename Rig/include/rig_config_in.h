/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_config_in.h 1018 2011-09-26 16:57:00Z llongi $
 */

#ifndef RIG_CONFIG_H
#define RIG_CONFIG_H 1

#include "sys/system_all.h"

#cmakedefine HAVE_STDBOOL_H 1
#cmakedefine HAVE_STDLIB_H 1
#cmakedefine HAVE_STDINT_H 1
#cmakedefine HAVE_STDARG_H 1
#cmakedefine HAVE_ERRNO_H 1

#cmakedefine HAVE_SCHED_H 1
#cmakedefine HAVE_UNISTD_H 1

#cmakedefine HAVE_POSIX_MEMALIGN 1
#cmakedefine HAVE_ALIGNED_MALLOC 1
#cmakedefine HAVE_SCHED_YIELD 1

#cmakedefine SIZEOF_SHORT_INT     @SIZEOF_SHORT_INT@
#cmakedefine SIZEOF_INT           @SIZEOF_INT@
#cmakedefine SIZEOF_LONG_INT      @SIZEOF_LONG_INT@
#cmakedefine SIZEOF_LONG_LONG_INT @SIZEOF_LONG_LONG_INT@
#cmakedefine SIZEOF_SIZE_T        @SIZEOF_SIZE_T@
#cmakedefine SIZEOF_VOID_PTR      @SIZEOF_VOID_PTR@

#cmakedefine SYSTEM_BIGENDIAN 1

#cmakedefine HAVE_PTHREADS 1
#cmakedefine HAVE_WIN32_THREADS 1

#endif /* RIG_CONFIG_H */
