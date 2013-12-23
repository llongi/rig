/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_list_lock.c 802 2011-01-30 03:30:45Z llongi $
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

#include <pthread.h>

/*
 * Rig List Data Definitions
 */

/** Types */
typedef struct NodeStruct *Node;

/** Structures */
struct rig_list {
	Node head; // permanent sentinel node
	size_t capacity; // read-only value
	size_t count;
	RIG_A_UINT refcount;
	uint16_t flags; // read-only value
	struct rig_typeinfo typeinfo; // read-only value
	pthread_mutex_t mxlock; // Mutex lock
};

struct NodeStruct {
	Node next;
	size_t key;
	uint8_t data[];
};

#define RIG_LIST_THREADSAFE ((uint16_t)(1 << 9))

#define MX_LOCK(l)   if (TEST_BITFIELD((l)->flags, RIG_LIST_THREADSAFE)) pthread_mutex_lock(&(l)->mxlock)
#define MX_UNLOCK(l) if (TEST_BITFIELD((l)->flags, RIG_LIST_THREADSAFE)) pthread_mutex_unlock(&(l)->mxlock)

static inline void list_traverse(RIG_LIST l, Node head, size_t key, void *item, Node *eprev, Node *ecurr);

/*
 * Rig List Implementation
 */

RIG_LIST rig_list_init(uint16_t flags, size_t capacity, const struct rig_typeinfo typeinfo) {
	// Allocate memory for the list
	RIG_LIST list = malloc(sizeof(*list));
	if (list == NULL) {
		errno = ENOMEM;
		return (NULL);
	}

	// Allocate memory for the sentinel node
	Node sentinel = malloc(sizeof(*sentinel));
	if (sentinel == NULL) {
		free(list);
		errno = ENOMEM;
		return (NULL);
	}

	// Check if we want to be thread-safe or not
	if (TEST_BITFIELD(flags, RIG_LIST_THREADSAFE)) {
		// Initialize the lock
		if (pthread_mutex_init(&list->mxlock, NULL)) {
			free(list);
			free(sentinel);
			errno = ENOLCK;
			return (NULL);
		}
	}

	// Initialize the needed values
	sentinel->next = NULL;
	sentinel->key = 0;
	list->head = sentinel;
	list->capacity = (capacity == 0) ? (SIZE_MAX) : (capacity);
	list->count = 0;
	rig_atomic_uint_set(&list->refcount, 1);
	list->flags = flags;
	list->typeinfo = typeinfo;

	return (list);
}

RIG_LIST rig_list_duplicate(RIG_LIST l, uint16_t flags) {
	NULLCHECK_EXIT(l);
}

RIG_LIST rig_list_newref(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	rig_atomic_uint_inc(&l->refcount);

	return (l);
}

void rig_list_destroy(RIG_LIST *l) {
	NULLCHECK_EXIT(l);
	NULLCHECK_EXIT(*l);

	if (rig_atomic_uint_dec_and_test(&(*l)->refcount)) {
		MX_LOCK(*l);

		// Traverse the list and remove all nodes (sentinel included)
		Node curr = (*l)->head, succ = NULL;

		while (curr != NULL) {
			succ = curr->next;
			free(curr);
			curr = succ;
		}

		MX_UNLOCK(*l);

		// Destroy the lock and the list
		if (TEST_BITFIELD((*l)->flags, RIG_LIST_THREADSAFE)) {
			pthread_mutex_destroy(&(*l)->mxlock);
		}
		free(*l);
	}

	*l = NULL;
}

bool rig_list_reset(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	MX_LOCK(l);

	// Save head node for later use
	Node curr = l->head->next, succ = NULL;

	// Reset values to empty
	l->head->next = NULL;
	l->count = 0;

	MX_UNLOCK(l);

	// Traverse the now detached list and free all nodes
	while (curr != NULL) {
		succ = curr->next;
		free(curr);
		curr = succ;
	}

	return (true);
}

static inline void list_traverse(RIG_LIST l, Node head, size_t key, void *item, Node *eprev, Node *ecurr) {
	Node prev = head, curr = prev->next;

	while (curr != NULL) {
		if (curr->key == key && !str_ops_cmp(curr->data, item, l->typeinfo.size, NULL)) {
			break;
		}

		if (curr->key > key) {
			break;
		}

		prev = curr;
		curr = curr->next;
	}

	*eprev = prev;
	*ecurr = curr;
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

	MX_LOCK(l);

	if (l->count == l->capacity)  {
		// List full
		MX_UNLOCK(l);
		free(node);

		errno = EXFULL;
		return (false);
	}

	// Find first occourrence of item in list, or the right next key (if not present)
	Node prev, curr;
	list_traverse(l, l->head, key, item, &prev, &curr);

	if (!TEST_BITFIELD(l->flags, RIG_LIST_ALLOWDUPS) && curr != NULL && curr->key == key) {
		// No duplicates allowed and item already present, return
		MX_UNLOCK(l);
		free(node);

		errno = EEXIST;
		return (false);
	}

	// Link the new element in
	node->next = curr;
	prev->next = node;

	l->count++;

	MX_UNLOCK(l);

	return (true);
}

bool rig_list_remove(RIG_LIST l, void *item) {
	NULLCHECK_EXIT(l);
	NULLCHECK_EXIT(item);

	size_t key = rig_hash(item, l->typeinfo.size, RIG_HASH_MURMUR2);

	MX_LOCK(l);

	// Find first occourrence of item in list, or the right next key (if not present)
	Node prev, curr;
	list_traverse(l, l->head, key, item, &prev, &curr);

	if (curr != NULL && curr->key == key) {
		// Item present, remove it
		prev->next = curr->next;

		l->count--;

		MX_UNLOCK(l);

		// Free memory
		free(curr);

		return (true);
	}

	MX_UNLOCK(l);

	errno = ENOENT;
	return (false);
}

bool rig_list_contains(RIG_LIST l, void *item) {
	NULLCHECK_EXIT(l);
	NULLCHECK_EXIT(item);

	size_t key = rig_hash(item, l->typeinfo.size, RIG_HASH_MURMUR2);

	MX_LOCK(l);

	// Find first occourrence of item in list, or the right next key (if not present)
	Node prev, curr;
	list_traverse(l, l->head, key, item, &prev, &curr);

    if (curr != NULL && curr->key == key) {
        // Item found
    	MX_UNLOCK(l);

        return (true);
    }

    MX_UNLOCK(l);

    errno = ENOENT;
    return (false);
}

bool rig_list_get(RIG_LIST l, void *item) {
	NULLCHECK_EXIT(l);
	NULLCHECK_EXIT(item);

	MX_LOCK(l);

	Node prev = l->head, curr = prev->next;

	if (curr == NULL) {
		// Empty list
		MX_UNLOCK(l);

		errno = ENOENT;
		return (false);
	}

	// Remove first element
	prev->next = curr->next;

	l->count--;

	MX_UNLOCK(l);

	str_ops_copy(item, curr->data, l->typeinfo.size);

	free(curr);

	return (true);
}

bool rig_list_look(RIG_LIST l, void *item) {
	NULLCHECK_EXIT(l);
	NULLCHECK_EXIT(item);

	MX_LOCK(l);

	Node curr = l->head->next;

	if (curr == NULL) {
		// Empty list
		MX_UNLOCK(l);

		errno = ENOENT;
		return (false);
	}

	// Get first element content
	str_ops_copy(item, curr->data, l->typeinfo.size);

	MX_UNLOCK(l);

	return (true);
}

bool rig_list_empty(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	MX_LOCK(l);
	size_t ret = l->count;
	MX_UNLOCK(l);

	return (ret == 0);
}

bool rig_list_full(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	MX_LOCK(l);
	size_t ret = l->count;
	MX_UNLOCK(l);

	return (ret == l->capacity);
}

size_t rig_list_count(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	MX_LOCK(l);
	size_t ret = l->count;
	MX_UNLOCK(l);

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
