#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <sbb.h>

typedef struct ListStruct *LIST;

LIST list_init(size_t capacity, bool allowdups);
bool list_destroy(LIST *l);
bool list_insert(LIST l, uintptr_t *item);
bool list_remove(LIST l, uintptr_t *item);
bool list_contains(LIST l, uintptr_t *item);
bool list_get(LIST l, uintptr_t *item);
bool list_look(LIST l, uintptr_t *item);
size_t list_size(LIST l);
bool list_empty(LIST l);
bool list_full(LIST l);

typedef struct NodeStruct *Node;

struct ListStruct {
	Node head; // read-only value (permanent sentinel node)
	bool allowdups; // read-only value
	size_t capacity; // read-only value
	SBB_A_UINT count;
};

struct NodeStruct {
	SBB_AM_PTR next;
	size_t key;
	uintptr_t data;
};

LIST list_init(size_t capacity, bool allowdups) {
	// Allocate memory for the list
	LIST list = malloc(sizeof(*list));
	if (list == NULL) {
		errno = ENOMEM;
		return (NULL);
	}

	// Allocate memory for the sentinel node
	Node sentinel = malloc(sizeof(*sentinel));
	if (sentinel == NULL) {
		errno = ENOMEM;
		return (NULL);
	}

	// Initialize the needed values
	sbb_atomic_markptr_set(&sentinel->next, NULL, false);
	sentinel->key = 0;
	sentinel->data = 0; // usually some form of memset(sentinel->data, 0), depends on data
	list->head = sentinel;
	list->allowdups = allowdups;
	list->capacity = (capacity == 0) ? (SIZE_MAX) : (capacity);
	sbb_atomic_uint_set(&list->count, 0);

	return (list);
}

bool list_destroy(LIST *l) {
	// Traverse the list and remove all nodes (sentinel included)
	Node curr = (*l)->head, succ = NULL;
	while (curr != NULL) {
		succ = sbb_atomic_markptr_get(&curr->next, NULL);
		free(curr);
		curr = succ;
	}

	// Destroy the lock and the list
	free(*l);
	*l = NULL;

	// Cleanup retired nodes
	smr_scan();

	// NOTE: this is only example code, you'd really have to make sure
	// no one is using the list before destroying it, which one could
	// do using some form of reference counting for example, but that
	// is way out of scope of this article and what it wants to show

	return (true);
}

static inline void list_traverse(Node head, size_t key, uintptr_t *item, Node *eprev, Node *ecurr, SMR_HPRecord hprec) {
	bool mark = false;
	Node prev = NULL, curr = NULL, succ = NULL;

retry:
	// Reset used Hazard Pointers
	smr_set_hp(hprec, 0, NULL);
	smr_set_hp(hprec, 1, NULL);

	prev = head;
	curr = sbb_atomic_markptr_get(&prev->next, NULL);

	while (curr != NULL) {
		smr_set_hp(hprec, 0, curr); // Set Hazard Pointer
		if (curr != sbb_atomic_ptr_get(&prev->next)) {
			goto retry;
		}
		succ = sbb_atomic_markptr_get(&curr->next, &mark);

		if (mark) {
			if (!sbb_atomic_markptr_cas(&prev->next, curr, false, succ, false)) {
				goto retry;
			}

			// Reset used Hazard Pointers
			smr_set_hp(hprec, 0, NULL);

			smr_retire_mem(curr);

			curr = succ;
		}
		else {
			if (curr->key == key && item != NULL && curr->data == *item) { // usually some form of !memcmp(curr->data, *item), depends on data
				break;
			}

			if (curr->key > key) {
				break;
			}

			prev = curr;
			smr_set_hp(hprec, 1, prev); // Set Hazard Pointer
			curr = succ;
		}
	}

	*eprev = prev;
	*ecurr = curr;
}

bool list_insert(LIST l, uintptr_t *item) {
	// Allocate memory for the new element
	Node node = malloc(sizeof(*node));
	if (node == NULL) {
		errno = ENOMEM;
		return (false);
	}

	// Set the content of the new element
	size_t key = *item; // usually some form of hash(*item), depends on data
	node->key = key;
	node->data = *item; // usually some form of memcpy(node->data, *item), depends on data

	while (true) {
		size_t count = sbb_atomic_uint_get(&l->count);

		if (count == l->capacity)  {
			// List full
			free(node);

			errno = EXFULL;
			return (false);
		}

		if (sbb_atomic_uint_cas(&l->count, count, count + 1)) {
			break;
		}
	}

	// Get pointer to this thread's SMR_HPRecord
	SMR_HPRecord hprec = smr_get_hprecord();
	Node prev = NULL, curr = NULL;

	while (true) {
		// Find first occourrence of item or the right next key (if not present)
		list_traverse(l->head, key, item, &prev, &curr, hprec);

		if (!l->allowdups && curr != NULL && curr->key == key) {
			sbb_atomic_uint_dec(&l->count);

			// Reset used Hazard Pointers
			smr_set_hp(hprec, 0, NULL);
			smr_set_hp(hprec, 1, NULL);

			// No duplicates allowed and item already present, return
			free(node);

			errno = EEXIST;
			return (false);
		}

		// Link the new element in
		sbb_atomic_markptr_set(&node->next, curr, false);

		if (sbb_atomic_markptr_cas(&prev->next, curr, false, node, false)) {
			// Reset used Hazard Pointers
			smr_set_hp(hprec, 0, NULL);
			smr_set_hp(hprec, 1, NULL);

			return (true);
		}
	}
}

bool list_remove(LIST l, uintptr_t *item) {
	size_t key = *item; // usually some form of hash(*item), depends on data

	// Get pointer to this thread's SMR_HPRecord
	SMR_HPRecord hprec = smr_get_hprecord();
	Node prev = NULL, curr = NULL;

	while (true) {
		// Find first occourrence of item or the right next key (if not present)
		list_traverse(l->head, key, item, &prev, &curr, hprec);

		if (curr != NULL && curr->key == key) {
			// Item present, remove it
			Node succ = sbb_atomic_markptr_get(&curr->next, NULL);

			// Mark node for deletion (logical removal)
			if (!sbb_atomic_markptr_cas(&curr->next, succ, false, succ, true)) {
				continue;
			}

			sbb_atomic_uint_dec(&l->count);

			// Reset used Hazard Pointers
			smr_set_hp(hprec, 0, NULL);

			// Attempt physical removal
			if (sbb_atomic_markptr_cas(&prev->next, curr, false, succ, false)) {
				smr_retire_mem(curr);
			}

			// Reset used Hazard Pointers
			smr_set_hp(hprec, 1, NULL);

			return (true);
		}

		// Reset used Hazard Pointers
		smr_set_hp(hprec, 0, NULL);
		smr_set_hp(hprec, 1, NULL);

		errno = ENOENT;
		return (false);
	}
}

bool list_contains(LIST l, uintptr_t *item) {
	size_t key = *item; // usually some form of hash(*item), depends on data

	// Get pointer to this thread's SMR_HPRecord
	SMR_HPRecord hprec = smr_get_hprecord();
	Node prev = NULL, curr = NULL;

	// Find first occourrence of item or the right next key (if not present)
	list_traverse(l->head, key, item, &prev, &curr, hprec);

	if (curr != NULL && curr->key == key) {
		// Reset used Hazard Pointers
		smr_set_hp(hprec, 0, NULL);
		smr_set_hp(hprec, 1, NULL);

		// Item present
		return (true);
	}

	// Reset used Hazard Pointers
	smr_set_hp(hprec, 0, NULL);
	smr_set_hp(hprec, 1, NULL);

    errno = ENOENT;
    return (false);
}

bool list_get(LIST l, uintptr_t *item) {
	// Get pointer to this thread's SMR_HPRecord
	SMR_HPRecord hprec = smr_get_hprecord();

	bool mark = false;
	Node prev = NULL, curr = NULL, succ = NULL;

retry:
	// Reset used Hazard Pointers
	smr_set_hp(hprec, 0, NULL);

	prev = l->head;
	curr = sbb_atomic_markptr_get(&prev->next, NULL);

	while (curr != NULL) {
		smr_set_hp(hprec, 0, curr); // Set Hazard Pointer
		if (curr != sbb_atomic_ptr_get(&prev->next)) {
			goto retry;
		}
		succ = sbb_atomic_markptr_get(&curr->next, &mark);

		if (mark) {
			if (!sbb_atomic_markptr_cas(&prev->next, curr, false, succ, false)) {
				goto retry;
			}

			// Reset used Hazard Pointers
			smr_set_hp(hprec, 0, NULL);

			smr_retire_mem(curr);

			curr = succ;
		}
		else {
			// Mark node for deletion (logical removal)
			if (!sbb_atomic_markptr_cas(&curr->next, succ, false, succ, true)) {
				goto retry;
			}

			sbb_atomic_uint_dec(&l->count);

			*item = curr->data; // usually some form of memcpy(*item, curr->data), depends on data

			// Reset used Hazard Pointers
			smr_set_hp(hprec, 0, NULL);

			// Attempt physical removal
			if (sbb_atomic_markptr_cas(&prev->next, curr, false, succ, false)) {
				smr_retire_mem(curr);
			}

			return (true);
		}
	}

	// Reset used Hazard Pointers
	smr_set_hp(hprec, 0, NULL);

	// Empty list
	errno = ENOENT;
	return (false);
}

bool list_look(LIST l, uintptr_t *item) {
	// Get pointer to this thread's SMR_HPRecord
	SMR_HPRecord hprec = smr_get_hprecord();

	bool mark = false;
	Node prev = NULL, curr = NULL, succ = NULL;

retry:
	// Reset used Hazard Pointers
	smr_set_hp(hprec, 0, NULL);

	prev = l->head;
	curr = sbb_atomic_markptr_get(&prev->next, NULL);

	while (curr != NULL) {
		smr_set_hp(hprec, 0, curr); // Set Hazard Pointer
		if (curr != sbb_atomic_ptr_get(&prev->next)) {
			goto retry;
		}
		succ = sbb_atomic_markptr_get(&curr->next, &mark);

		if (mark) {
			if (!sbb_atomic_markptr_cas(&prev->next, curr, false, succ, false)) {
				goto retry;
			}

			// Reset used Hazard Pointers
			smr_set_hp(hprec, 0, NULL);

			smr_retire_mem(curr);

			curr = succ;
		}
		else {
			*item = curr->data; // usually some form of memcpy(*item, curr->data), depends on data

			// Reset used Hazard Pointers
			smr_set_hp(hprec, 0, NULL);

			return (true);
		}
	}

	// Reset used Hazard Pointers
	smr_set_hp(hprec, 0, NULL);

	// Empty list
	errno = ENOENT;
	return (false);
}

size_t list_size(LIST l) {
	return (sbb_atomic_uint_get(&l->count));
}

bool list_empty(LIST l) {
	return (sbb_atomic_uint_get(&l->count) == 0);
}

bool list_full(LIST l) {
	return (sbb_atomic_uint_get(&l->count) == l->capacity);
}
