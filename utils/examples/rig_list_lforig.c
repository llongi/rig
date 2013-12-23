/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_list_lforig.c 802 2011-01-30 03:30:45Z llongi $
 */

#include "rig_internal.h"

#include "str_ops/str_ops.c"
#include "atomic_ops/atomic_ops.c"

#define rig_atomic_ptr_get atomic_ops_ptr_get
#define rig_atomic_ptr_set atomic_ops_ptr_set
#define rig_atomic_ptr_cas atomic_ops_ptr_cas

#define rig_atomic_uint_get atomic_ops_uint_get
#define rig_atomic_uint_set atomic_ops_uint_set
#define rig_atomic_uint_cas atomic_ops_uint_cas
#define rig_atomic_uint_inc atomic_ops_uint_inc
#define rig_atomic_uint_dec atomic_ops_uint_dec

/*
 * Rig List Data Definitions
 */

#define SMR_HP_LINK 3
#define SMR_HP_CURR 0
#define SMR_HP_PREV 1

/** Types */
typedef struct LinkStruct *Link;
typedef struct NodeStruct *Node;

/** Structures */
struct rig_list {
	RIG_A_PTR link;
	size_t capacity; // read-only value
	RIG_A_INT refcount;
	uint16_t flags; // read-only value
	struct rig_typeinfo typeinfo; // read-only value
};

struct LinkStruct {
	RIG_AF_PTR next;
	RIG_A_UINT count;
};

struct NodeStruct {
	RIG_AF_PTR next;
	size_t key;
	uint8_t data[];
};

static inline bool list_traverse(Link llink, Node head, size_t key, void *item, size_t item_size, Node *eprev, Node *ecurr, RIG_SMR_HPRecord hprec);

/*
 * Rig List Implementation
 */

RIG_LIST rig_list_init(uint16_t flags, size_t capacity, const struct rig_typeinfo typeinfo) {
	// Allocate memory for the list
	RIG_LIST l = malloc(sizeof(*l));
	if (l == NULL) {
		errno = ENOMEM;
		return (NULL);
	}

	// Allocate memory for the atomic link structure
	Link llink = malloc(sizeof(*llink));
	if (llink == NULL) {
		free(l);
		errno = ENOMEM;
		return (NULL);
	}

	// Initialize the needed values
	rig_atomic_flagptr_set(&llink->next, NULL, false);
	rig_atomic_uint_set(&llink->count, 0);

	rig_atomic_ptr_set(&l->link, llink);
	l->capacity = (capacity == 0) ? (SIZE_MAX) : (capacity);
	rig_atomic_int_set(&l->refcount, 1);
	l->flags = flags;
	l->typeinfo = typeinfo;

	return (l);
}

RIG_LIST rig_list_duplicate(RIG_LIST l, uint16_t flags) {
	NULLCHECK_EXIT(l);
}

RIG_LIST rig_list_newref(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	rig_atomic_int_inc(&l->refcount);

	return (l);
}

void rig_list_destroy(RIG_LIST *l) {
	NULLCHECK_EXIT(l);
	NULLCHECK_EXIT(*l);

	if (rig_atomic_int_dec_and_test(&(*l)->refcount)) {
		// Traverse the list and remove all nodes (sentinel included)
		Link llink = rig_atomic_ptr_get(&(*l)->link);
		Node curr = rig_atomic_flagptr_get(&llink->next, NULL), succ = NULL;

		while (curr != NULL) {
			succ = rig_atomic_flagptr_get(&curr->next, NULL);
			free(curr);
			curr = succ;
		}

		// Destroy the list
		free(llink);
		free(*l);
	}

	*l = NULL;

	// Cleanup retired nodes
	rig_smr_scan();
}

bool rig_list_reset(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	// Allocate memory for the atomic link structure
	Link llink = malloc(sizeof(*llink));
	if (llink == NULL) {
		errno = ENOMEM;
		return (false);
	}

	// Initialize the needed values
	rig_atomic_flagptr_set(&llink->next, NULL, false);
	rig_atomic_uint_set(&llink->count, 0);

	// Reset link, remembering it to remove old nodes later
	Link oldllink = rig_atomic_ptr_swap(&l->link, llink);

	// Mark old pointer, so that traversing threads notice the change on restart
	Node head = (Node)oldllink;

	while (true) {
		Node hsucc = rig_atomic_flagptr_get(&head->next, NULL);

		if (rig_atomic_flagptr_cas(&head->next, hsucc, false, hsucc, true)) {
			break;
		}
	}

	// Retire old nodes
	bool mark = false;
	Node curr = NULL, succ = NULL;

	// We need no Hazard Pointers here, we don't need to protect head, as the
	// thread doing reset() is the only one that can possibly ever retire it,
	// and we don't need to protect curr either because any change that would
	// retire curr would need to relink head->next to succ, which is impossible
	// as head has its mark-bit turned on and thus other CAS on it will fail
retry:
	curr = rig_atomic_flagptr_get(&head->next, NULL);

	while (curr != NULL) {
		succ = rig_atomic_flagptr_get(&curr->next, &mark);

		if (!mark) {
			// Mark node for deletion (logical removal)
			if (!rig_atomic_flagptr_cas(&curr->next, succ, false, succ, true)) {
				goto retry;
			}
		}

		// Attempt physical removal
		if (!rig_atomic_flagptr_cas(&head->next, curr, true, succ, true)) {
			goto retry;
		}

		rig_smr_retire_mem(curr, sizeof(*curr) + l->typeinfo.size, false);

		curr = succ;
	}

	rig_smr_retire_mem(oldllink, sizeof(*oldllink), false);

	// Explicit SMR scan
	rig_smr_scan();

	return (true);
}

#define GET_AND_CHECK_HEAD \
head = (Node)llink; \
rig_atomic_flagptr_get(&head->next, &mark); \
if (mark) { \
	return (false); \
}

static inline bool list_traverse(Link llink, Node head, size_t key, void *item, size_t item_size, Node *eprev, Node *ecurr, RIG_SMR_HPRecord hprec) {
	bool mark = false;
	Node prev = NULL, curr = NULL, succ = NULL;

	// NOTE: SMR_HP_LINK is always protecting llink here, and head is either
	// llink or an old prev, which is protected by SMR_HP_PREV
retry:
	rig_smr_set_hp(hprec, SMR_HP_CURR, NULL); // Reset used Hazard Pointers
	prev = head;
	curr = rig_atomic_flagptr_get(&prev->next, &mark);
	if (mark) {
		GET_AND_CHECK_HEAD;
		goto retry;
	}

	while (curr != NULL) {
		rig_smr_set_hp(hprec, SMR_HP_CURR, curr); // Set Hazard Pointer
		if (curr != rig_atomic_ptr_get(&prev->next)) {
			GET_AND_CHECK_HEAD;
			goto retry;
		}
		succ = rig_atomic_flagptr_get(&curr->next, &mark);

		if (mark) {
			// Attempt physical removal
			if (!rig_atomic_flagptr_cas(&prev->next, curr, false, succ, false)) {
				GET_AND_CHECK_HEAD;
				goto retry;
			}

			// Reset used Hazard Pointers
			rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);

			rig_smr_retire_mem(curr, sizeof(*curr) + item_size, true);

			curr = succ;
		}
		else {
			if (curr->key == key && !str_ops_cmp(curr->data, item, item_size, NULL)) {
				break;
			}

			if (curr->key > key) {
				break;
			}

			prev = curr;
			rig_smr_set_hp(hprec, SMR_HP_PREV, prev); // Set Hazard Pointer
			curr = succ;
		}
	}

	*eprev = prev;
	*ecurr = curr;

	return (true);
}

bool rig_list_insert(RIG_LIST l, void *item) {
	NULLCHECK_EXIT(l);
	NULLCHECK_EXIT(item);

	// Allocate memory for the new element
	Node node = malloc(sizeof(*node) + l->typeinfo.size);
	if (node == NULL) {
		errno = ENOMEM;
		return (false);
	}

	// Set the content of the new element
	size_t key = rig_hash(item, l->typeinfo.size, RIG_HASH_MURMUR2);
	node->key = key;
	str_ops_copy(node->data, item, l->typeinfo.size);

	// Get pointer to this thread's RIG_SMR_HPRecord
	RIG_SMR_HPRecord hprec = rig_smr_get_hprecord();
	Link llink = NULL;
	Node prev = NULL, curr = NULL, head = NULL;

retry:
	rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);
	rig_smr_set_hp(hprec, SMR_HP_PREV, NULL);
	GET_PROTECT_CHECK_LINK(l, llink);

	// Check if there's still place for the new element
	while (true) {
		CHECK_LINK(l, llink);

		size_t count = rig_atomic_uint_get(&llink->count);

		if (count == l->capacity) {
			rig_smr_set_hp(hprec, SMR_HP_LINK, NULL);

			free(node);

			// List full
			errno = EXFULL;
			return (false);
		}

		if (rig_atomic_uint_cas(&llink->count, count, count + 1)) {
			break;
		}
	}

	head = (Node)llink;

	while (true) {
		CHECK_LINK(l, llink);

		// Find first occurrence of item or the right next key (if not present)
		if (!list_traverse(llink, head, key, item, l->typeinfo.size, &prev, &curr, hprec)) {
			continue;
		}

		if (!TEST_BITFIELD(l->flags, RIG_LIST_ALLOWDUPS) && curr != NULL && curr->key == key) {
			// Roll-back global changes
			rig_atomic_uint_dec(&llink->count);
			free(node);

			// Reset used Hazard Pointers
			rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);
			rig_smr_set_hp(hprec, SMR_HP_PREV, NULL);
			rig_smr_set_hp(hprec, SMR_HP_LINK, NULL);

			// No duplicates allowed and item already present, return
			errno = EEXIST;
			return (false);
		}

		// Link the new element in
		rig_atomic_flagptr_set(&node->next, curr, false);

		if (!rig_atomic_flagptr_cas(&prev->next, curr, false, node, false)) {
			head = prev;
			continue;
		}

		// Reset used Hazard Pointers
		rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);
		rig_smr_set_hp(hprec, SMR_HP_PREV, NULL);
		rig_smr_set_hp(hprec, SMR_HP_LINK, NULL);

		// Item added
		return (true);
	}
}

bool rig_list_remove(RIG_LIST l, void *item) {
	NULLCHECK_EXIT(l);
	NULLCHECK_EXIT(item);

	size_t key = rig_hash(item, l->typeinfo.size, RIG_HASH_MURMUR2);

	// Get pointer to this thread's RIG_SMR_HPRecord
	RIG_SMR_HPRecord hprec = rig_smr_get_hprecord();
	Link llink = NULL;
	Node prev = NULL, curr = NULL, head = NULL;

retry:
	rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);
	rig_smr_set_hp(hprec, SMR_HP_PREV, NULL);
	GET_PROTECT_CHECK_LINK(l, llink);

	head = (Node)llink;

	while (true) {
		CHECK_LINK(l, llink);

		// Find first occurrence of item or the right next key (if not present)
		if (!list_traverse(llink, head, key, item, l->typeinfo.size, &prev, &curr, hprec)) {
			continue;
		}

		if (curr != NULL && curr->key == key) {
			// Item present, remove it
			Node succ = rig_atomic_flagptr_get(&curr->next, NULL);

			// Mark node for deletion (logical removal)
			if (!rig_atomic_flagptr_cas(&curr->next, succ, false, succ, true)) {
				head = prev;
				continue;
			}

			rig_atomic_uint_dec(&llink->count);

			// Reset used Hazard Pointers
			rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);

			// Attempt physical removal
			if (rig_atomic_flagptr_cas(&prev->next, curr, false, succ, false)) {
				rig_smr_retire_mem(curr, sizeof(*curr) + l->typeinfo.size, true);
			}

			// Reset used Hazard Pointers
			rig_smr_set_hp(hprec, SMR_HP_PREV, NULL);
			rig_smr_set_hp(hprec, SMR_HP_LINK, NULL);

			// Item removed
			return (true);
		}

		// Reset used Hazard Pointers
		rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);
		rig_smr_set_hp(hprec, SMR_HP_PREV, NULL);
		rig_smr_set_hp(hprec, SMR_HP_LINK, NULL);

		// Empty list
		errno = ENOENT;
		return (false);
	}
}

bool rig_list_contains(RIG_LIST l, void *item) {
	NULLCHECK_EXIT(l);
	NULLCHECK_EXIT(item);

	size_t key = rig_hash(item, l->typeinfo.size, RIG_HASH_MURMUR2);

	// Get pointer to this thread's RIG_SMR_HPRecord
	RIG_SMR_HPRecord hprec = rig_smr_get_hprecord();
	Link llink = NULL;
	Node prev = NULL, curr = NULL, head = NULL;

retry:
	rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);
	rig_smr_set_hp(hprec, SMR_HP_PREV, NULL);
	GET_PROTECT_CHECK_LINK(l, llink);

	head = (Node)llink;

	while (true) {
		CHECK_LINK(l, llink);

		// Find first occurrence of item or the right next key (if not present)
		if (!list_traverse(llink, head, key, item, l->typeinfo.size, &prev, &curr, hprec)) {
			continue;
		}

		if (curr != NULL && curr->key == key) {
			// Reset used Hazard Pointers
			rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);
			rig_smr_set_hp(hprec, SMR_HP_PREV, NULL);
			rig_smr_set_hp(hprec, SMR_HP_LINK, NULL);

			// Item present
			return (true);
		}

		// Reset used Hazard Pointers
		rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);
		rig_smr_set_hp(hprec, SMR_HP_PREV, NULL);
		rig_smr_set_hp(hprec, SMR_HP_LINK, NULL);

		// Empty list
		errno = ENOENT;
		return (false);
	}
}

bool rig_list_get(RIG_LIST l, void *item) {
	NULLCHECK_EXIT(l);
	NULLCHECK_EXIT(item);

	// Get pointer to this thread's RIG_SMR_HPRecord
	RIG_SMR_HPRecord hprec = rig_smr_get_hprecord();
	Link llink = NULL;
	bool mark = false;
	Node prev = NULL, curr = NULL, succ = NULL;

retry:
	rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);
	GET_PROTECT_CHECK_LINK(l, llink);

	prev = (Node)llink;
	curr = rig_atomic_flagptr_get(&prev->next, &mark);
	if (mark) {
		goto retry;
	}

	while (curr != NULL) {
		rig_smr_set_hp(hprec, SMR_HP_CURR, curr); // Set Hazard Pointer
		if (curr != rig_atomic_ptr_get(&prev->next)) {
			goto retry;
		}
		succ = rig_atomic_flagptr_get(&curr->next, &mark);

		if (mark) {
			if (!rig_atomic_flagptr_cas(&prev->next, curr, false, succ, false)) {
				goto retry;
			}

			// Reset used Hazard Pointers
			rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);

			rig_smr_retire_mem(curr, sizeof(*curr) + l->typeinfo.size, true);

			curr = succ;
		}
		else {
			// Mark node for deletion (logical removal)
			if (!rig_atomic_flagptr_cas(&curr->next, succ, false, succ, true)) {
				goto retry;
			}

			rig_atomic_uint_dec(&llink->count);

			str_ops_copy(item, curr->data, l->typeinfo.size);

			// Reset used Hazard Pointers
			rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);

			// Attempt physical removal
			if (rig_atomic_flagptr_cas(&prev->next, curr, false, succ, false)) {
				rig_smr_retire_mem(curr, sizeof(*curr) + l->typeinfo.size, true);
			}

			// Reset used Hazard Pointers
			rig_smr_set_hp(hprec, SMR_HP_LINK, NULL);

			return (true);
		}
	}

	// Reset used Hazard Pointers
	rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);
	rig_smr_set_hp(hprec, SMR_HP_LINK, NULL);

	// Empty list
	errno = ENOENT;
	return (false);
}

bool rig_list_look(RIG_LIST l, void *item) {
	NULLCHECK_EXIT(l);
	NULLCHECK_EXIT(item);

	// Get pointer to this thread's RIG_SMR_HPRecord
	RIG_SMR_HPRecord hprec = rig_smr_get_hprecord();
	Link llink = NULL;
	bool mark = false;
	Node prev = NULL, curr = NULL, succ = NULL;

retry:
	rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);
	GET_PROTECT_CHECK_LINK(l, llink);

	prev = (Node)llink;
	curr = rig_atomic_flagptr_get(&prev->next, &mark);
	if (mark) {
		goto retry;
	}

	while (curr != NULL) {
		rig_smr_set_hp(hprec, SMR_HP_CURR, curr); // Set Hazard Pointer
		if (curr != rig_atomic_ptr_get(&prev->next)) {
			goto retry;
		}
		succ = rig_atomic_flagptr_get(&curr->next, &mark);

		if (mark) {
			if (!rig_atomic_flagptr_cas(&prev->next, curr, false, succ, false)) {
				goto retry;
			}

			// Reset used Hazard Pointers
			rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);

			rig_smr_retire_mem(curr, sizeof(*curr) + l->typeinfo.size, true);

			curr = succ;
		}
		else {
			str_ops_copy(item, curr->data, l->typeinfo.size);

			// Reset used Hazard Pointers
			rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);
			rig_smr_set_hp(hprec, SMR_HP_LINK, NULL);

			return (true);
		}
	}

	// Reset used Hazard Pointers
	rig_smr_set_hp(hprec, SMR_HP_CURR, NULL);
	rig_smr_set_hp(hprec, SMR_HP_LINK, NULL);

	// Empty list
	errno = ENOENT;
	return (false);
}

bool rig_list_empty(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	// Get pointer to this thread's RIG_SMR_HPRecord
	RIG_SMR_HPRecord hprec = rig_smr_get_hprecord();
	Link llink = NULL;

retry:
	GET_PROTECT_CHECK_LINK(l, llink);
	size_t ret = rig_atomic_uint_get(&llink->count);
	rig_smr_set_hp(hprec, SMR_HP_LINK, NULL);

	return (ret == 0);
}

bool rig_list_full(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	// Get pointer to this thread's RIG_SMR_HPRecord
	RIG_SMR_HPRecord hprec = rig_smr_get_hprecord();
	Link llink = NULL;

retry:
	GET_PROTECT_CHECK_LINK(l, llink);
	size_t ret = rig_atomic_uint_get(&llink->count);
	rig_smr_set_hp(hprec, SMR_HP_LINK, NULL);

	return (ret == l->capacity);
}

size_t rig_list_count(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	// Get pointer to this thread's RIG_SMR_HPRecord
	RIG_SMR_HPRecord hprec = rig_smr_get_hprecord();
	Link llink = NULL;

retry:
	GET_PROTECT_CHECK_LINK(l, llink);
	size_t ret = rig_atomic_uint_get(&llink->count);
	rig_smr_set_hp(hprec, SMR_HP_LINK, NULL);

	return (ret);
}

size_t rig_list_capacity(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	return (l->capacity);
}

struct rig_typeinfo rig_list_typeinfo(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	return (l->typeinfo);
}
