#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>

typedef struct ListStruct *LIST;

LIST list_init(bool allowdups);
bool list_destroy(LIST *l);
bool list_insert(LIST l, uintptr_t *item);
bool list_remove(LIST l, uintptr_t *item);
bool list_contains(LIST l, uintptr_t *item);
bool list_get(LIST l, uintptr_t *item);
bool list_look(LIST l, uintptr_t *item);

typedef struct NodeStruct *Node;

struct ListStruct {
	Node head;
	pthread_mutex_t mxlock;
	bool allowdups;
};

struct NodeStruct {
	size_t key;
	Node next;
	uintptr_t data;
};

LIST list_init(bool allowdups) {
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
	sentinel->key = 0;
	sentinel->next = NULL;
	sentinel->data = 0; // usually some form of memset(sentinel->data, 0), depends on data
	list->head = sentinel;
	pthread_mutex_init(&list->mxlock, NULL);
	list->allowdups = allowdups;

	return (list);
}

bool list_destroy(LIST *l) {
	pthread_mutex_lock(&(*l)->mxlock);

	// Traverse the list and remove all nodes (sentinel included)
	Node curr = (*l)->head, succ = NULL;
	while (curr != NULL) {
		succ = curr->next;
		free(curr);
		curr = succ;
	}

	pthread_mutex_unlock(&(*l)->mxlock);

	// Destroy the lock and the list
	pthread_mutex_destroy(&(*l)->mxlock);
	free(*l);
	*l = NULL;

	// NOTE: this is only example code, you'd really have to make sure
	// no one is using the mutex before destroying it, which one could
	// do using some form of reference counting for example, but that
	// is way out of scope of this article and what it wants to show

	return (true);
}

static inline void list_traverse(Node head, size_t key, uintptr_t *item, Node *eprev, Node *ecurr) {
	Node prev = head, curr = prev->next;

	while (curr != NULL) {
		if (curr->key == key && item != NULL && curr->data == *item) { // usually some form of !memcmp(curr->data, *item), depends on data
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

	pthread_mutex_lock(&l->mxlock);

	// Find first occourrence of item in list, or the right next key (if not present)
	Node prev, curr;
	list_traverse(l->head, key, item, &prev, &curr);

	if (!l->allowdups && curr != NULL && curr->key == key) {
		// No duplicates allowed and item already present, return
		pthread_mutex_unlock(&l->mxlock);
		free(node);

		errno = EEXIST;
		return (false);
	}

	// Link the new element in
	node->next = curr;
	prev->next = node;

	pthread_mutex_unlock(&l->mxlock);

	return (true);
}

bool list_remove(LIST l, uintptr_t *item) {
	size_t key = *item; // usually some form of hash(*item), depends on data

	pthread_mutex_lock(&l->mxlock);

	// Find first occourrence of item in list, or the right next key (if not present)
	Node prev, curr;
	list_traverse(l->head, key, item, &prev, &curr);

	if (curr != NULL && curr->key == key) {
		// Item present, remove it
		prev->next = curr->next;

		pthread_mutex_unlock(&l->mxlock);

		// Free memory
		free(curr);

		return (true);
	}

	pthread_mutex_unlock(&l->mxlock);

	errno = ENOENT;
	return (false);
}

bool list_contains(LIST l, uintptr_t *item) {
	size_t key = *item; // usually some form of hash(*item), depends on data

	pthread_mutex_lock(&l->mxlock);

	// Find first occourrence of item in list, or the right next key (if not present)
	Node prev, curr;
	list_traverse(l->head, key, item, &prev, &curr);

    if (curr != NULL && curr->key == key) {
        // Item found
        pthread_mutex_unlock(&l->mxlock);

        return (true);
    }

    pthread_mutex_unlock(&l->mxlock);

    errno = ENOENT;
    return (false);
}

bool list_get(LIST l, uintptr_t *item) {
	pthread_mutex_lock(&l->mxlock);

	Node prev = l->head, curr = prev->next;

	if (curr == NULL) {
		// Empty list
		pthread_mutex_unlock(&l->mxlock);

		errno = ENOENT;
		return (false);
	}

	// Remove first element
	prev->next = curr->next;

	pthread_mutex_unlock(&l->mxlock);

	*item = curr->data; // usually some form of memcpy(*item, curr->data), depends on data
	free(curr);

	return (true);
}

bool list_look(LIST l, uintptr_t *item) {
	pthread_mutex_lock(&l->mxlock);

	Node curr = l->head->next;

	if (curr == NULL) {
		// Empty list
		pthread_mutex_unlock(&l->mxlock);

		errno = ENOENT;
		return (false);
	}

	// Get first element content
	*item = curr->data; // usually some form of memcpy(*item, curr->data), depends on data

	pthread_mutex_unlock(&l->mxlock);

	return (true);
}
