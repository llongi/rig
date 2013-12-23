/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_misc.c 1110 2012-10-20 20:32:28Z llongi $
 */

#include "rig_internal.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>


// Implementation of our own assert-like fail-fast function
void rig_acheck_exit(const char *expression, const char *function, const char *file, unsigned int line, const char *failure_message) {
	fprintf(stderr, "Check failed: '%s' in %s, at %s:%d, message: %s\n", expression, function, file, line, failure_message);

	// NULL check failures are user failures or grave system failures (ENOMEM in a critical path, ...), we can't help there
	if (memcmp(failure_message, "NULL check failed", 17)) {
		fprintf(stderr, "Please report this to the Rig developers at https://dev.longitekk.com/Rig/ as soon as possible!\n");
	}

	exit(EXIT_FAILURE);
}

// Implementation of our own error reporting function with logging capabilities
void rig_error_log(int error, const char *function, const char *file, unsigned int line, const char *failure_message) {
#if !defined(DEBUG)
	// Only print serious errors to stderr if not in DEBUG mode
	if ((error == EAGAIN)
	 || (error == EALREADY)
	 || (error == EDEADLK)
	 || (error == ENAVAIL)
	 || (error == ENOLCK)
	 || (error == ENOMEM)
	 || (error == EPERM)) {
#endif
		fprintf(stderr, "Error reported: '%d' by %s, at %s:%d, message: %s\n", error, function, file, line, failure_message);
#if !defined(DEBUG)
	}
#endif
}


/**
 * Get the Process ID (PID) of the calling process.
 *
 * @return
 *     current Process ID
 */
size_t rig_misc_pid(void) {
#if defined(SYSTEM_OS_UNIX) || defined(SYSTEM_OS_MACOSX)
	return ((size_t)getpid());
#elif defined(SYSTEM_OS_WIN32)
	return ((size_t)GetCurrentProcessId());
#else
	#error System OS undefined or unsupported.
#endif
}
