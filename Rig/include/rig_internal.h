/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_internal.h 1133 2012-11-10 20:27:17Z llongi $
 */

#ifndef RIG_INTERNAL_H
#define RIG_INTERNAL_H 1

#include "rig.h"
#include "rig_config.h"

#define VERIFY_ERRET(err) rig_acheck_msg(err, "unexpected error return value")

// Check flags content: bits 0-9 are for public use, 10-15 are reserved
// for internal use and cannot be set when flags get passed as an argument.
#define CHECK_PERMITTED_FLAGS(f, flags) if ((f) & (~(flags))) { rig_erret(EINVAL, NULL); }

// Macros to access the string information (allow easier implementation switch)
#define FLAGS(s)	(s)->flags
#define REFCOUNT(s)	(s)->refcount
#define MAXLEN(s)	(s)->maxlen
#define LEN(s)		(s)->len
#define POS(s)		(s)->pos
#define LOCK(s)		(s)->lock
#define STR(s)		(s)->str

// Macros to calculate maxlen, len and str; automatically taking pos into account
#define MAXLEN_POS(s)	(MAXLEN(s) - POS(s))
#define LEN_POS(s)		(LEN(s) - POS(s))
#define STR_POS(s)		(STR(s) + POS(s))

// RWLOCK READ Lock operation for thread-safety
#define RW_RDLOCK(s) if (LOCK(s) != NULL) rig_mrwlock_rdlock(LOCK(s))
// RWLOCK WRITE Lock operation for thread-safety
#define RW_WRLOCK(s) if (LOCK(s) != NULL) rig_mrwlock_wrlock(LOCK(s))
// RWLOCK Unlock operation for thread-safety
#define RW_UNLOCK(s) if (LOCK(s) != NULL) rig_mrwlock_unlock(LOCK(s))

// Iterator Hazard Pointer Management
#define CHECK_ITER_HPS \
	RIG_SMR_HP_Record hprec = rig_smr_hp_record_get(); \
	if ((rig_smr_hp_get(hprec, 4) != NULL) \
	 || (rig_smr_hp_get(hprec, 5) != NULL) \
	 || (rig_smr_hp_get(hprec, 6) != NULL) \
	 || (rig_smr_hp_get(hprec, 7) != NULL)) { \
		ERRET(EALREADY, NULL); \
	}

#define RESET_ITER_HPS \
	RIG_SMR_HP_Record hprec = rig_smr_hp_record_get(); \
	rig_smr_hp_release(hprec, 4); \
	rig_smr_hp_release(hprec, 5); \
	rig_smr_hp_release(hprec, 6); \
	rig_smr_hp_release(hprec, 7);

#endif /* RIG_INTERNAL_H */
