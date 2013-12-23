#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <sbb.h>

typedef struct QueueStruct *QUEUE;

QUEUE queue_init(size_t capacity);
bool queue_destroy(QUEUE *q);
bool queue_put(QUEUE q, uintptr_t *item);
bool queue_get(QUEUE q, uintptr_t *item);
bool queue_look(QUEUE q, uintptr_t *item);
size_t queue_size(QUEUE q);
bool queue_empty(QUEUE q);
bool queue_full(QUEUE q);

typedef struct NodeStruct *Node;

struct QueueStruct {
	SBB_A_PTR head;
	SBB_A_PTR tail;
	size_t capacity; // read-only value
	SBB_A_UINT count;
};

struct NodeStruct {
	SBB_A_PTR next;
	uintptr_t data;
};

QUEUE queue_init(size_t capacity) {
	// Allocate memory for the queue
	QUEUE queue = malloc(sizeof(*queue));
	if (queue == NULL) {
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
	sbb_atomic_ptr_set(&sentinel->next, NULL);
	sentinel->data = 0;
	sbb_atomic_ptr_set(&queue->head, sentinel);
	sbb_atomic_ptr_set(&queue->tail, sentinel);
	queue->capacity = (capacity == 0) ? (SIZE_MAX) : (capacity);
	sbb_atomic_uint_set(&queue->count, 0);

	return (queue);
}

bool queue_destroy(QUEUE *q) {
	// Traverse the list and remove all nodes (sentinel included)
	Node curr = sbb_atomic_ptr_get(&(*q)->head), succ = NULL;
	while (curr != NULL) {
		succ = sbb_atomic_ptr_get(&curr->next);
		free(curr);
		curr = succ;
	}

	// Destroy the queue
	free(*q);
	*q = NULL;

	// Cleanup retired nodes
	smr_scan();

	// NOTE: this is only example code, you'd really have to make sure
	// no one is using the queue before destroying it, which one could
	// do using some form of reference counting for example, but that
	// is way out of scope of this article and what it wants to show

	return (true);
}

bool queue_put(QUEUE q, uintptr_t *item) {
	// Allocate memory for the new element
	Node node = malloc(sizeof(*node));
	if (node == NULL) {
		errno = ENOMEM;
		return (false);
	}

	// Set the content of the new element
	sbb_atomic_ptr_set(&node->next, NULL);
	node->data = *item;

	while (true) {
		size_t count = sbb_atomic_uint_get(&q->count);

		if (count == q->capacity)  {
			// List full
			free(node);

			errno = EXFULL;
			return (false);
		}

		if (sbb_atomic_uint_cas(&q->count, count, count + 1)) {
			break;
		}
	}

	// Get pointer to this thread's SMR_HPRecord
	SMR_HPRecord hprec = smr_get_hprecord();
	Node tail = NULL, next = NULL;

	while (true) {
		tail = sbb_atomic_ptr_get(&q->tail);
		smr_set_hp(hprec, 0, tail); // Set Hazard Pointer
		if (tail != sbb_atomic_ptr_get(&q->tail)) {
			continue;
		}
		next = sbb_atomic_ptr_get(&tail->next);

		if (next == NULL) {
			if (sbb_atomic_ptr_cas(&tail->next, NULL, node)) {
				sbb_atomic_ptr_cas(&q->tail, tail, node);

				// Reset used Hazard Pointers
				smr_set_hp(hprec, 0, NULL);

				return (true);
			}
		}
		else {
			sbb_atomic_ptr_cas(&q->tail, tail, next);
		}
	}
}

bool queue_get(QUEUE q, uintptr_t *item) {
	// Get pointer to this thread's SMR_HPRecord
	SMR_HPRecord hprec = smr_get_hprecord();
	Node head = NULL, next = NULL;

	while (true) {
		head = sbb_atomic_ptr_get(&q->head);
		smr_set_hp(hprec, 0, head); // Set Hazard Pointer
		if (head != sbb_atomic_ptr_get(&q->head)) {
			continue;
		}
		next = sbb_atomic_ptr_get(&head->next);
		smr_set_hp(hprec, 1, next); // Set Hazard Pointer

		if (head == sbb_atomic_ptr_get(&q->head)) {
			if (next == NULL) {
				// Reset used Hazard Pointers
				smr_set_hp(hprec, 0, NULL);
				smr_set_hp(hprec, 1, NULL);

				// Empty list
				errno = ENOENT;
				return (false);
			}
			else {
				if (sbb_atomic_ptr_cas(&q->head, head, next)) {
					Node tail = sbb_atomic_ptr_get(&q->tail);
					if (head == tail) {
						sbb_atomic_ptr_cas(&q->tail, tail, next);
					}

					sbb_atomic_uint_dec(&q->count);
					*item = next->data;

					// Reset used Hazard Pointers
					smr_set_hp(hprec, 0, NULL);
					smr_set_hp(hprec, 1, NULL);

					smr_retire_mem(head);

					return (true);
				}
			}
		}

		// Reset used Hazard Pointers
		// HP[0] gets reset right after
		smr_set_hp(hprec, 1, NULL);
	}
}

bool queue_look(QUEUE q, uintptr_t *item) {
	// Get pointer to this thread's SMR_HPRecord
	SMR_HPRecord hprec = smr_get_hprecord();
	Node head = NULL, next = NULL;

	while (true) {
		head = sbb_atomic_ptr_get(&q->head);
		smr_set_hp(hprec, 0, head); // Set Hazard Pointer
		if (head != sbb_atomic_ptr_get(&q->head)) {
			continue;
		}
		next = sbb_atomic_ptr_get(&head->next);
		smr_set_hp(hprec, 1, next); // Set Hazard Pointer

		if (head == sbb_atomic_ptr_get(&q->head)) {
			if (next == NULL) {
				// Reset used Hazard Pointers
				smr_set_hp(hprec, 0, NULL);
				smr_set_hp(hprec, 1, NULL);

				// Empty list
				errno = ENOENT;
				return (false);
			}
			else {
				Node tail = sbb_atomic_ptr_get(&q->tail);
				if (head == tail) {
					sbb_atomic_ptr_cas(&q->tail, tail, next);
				}

				*item = next->data;

				// Reset used Hazard Pointers
				smr_set_hp(hprec, 0, NULL);
				smr_set_hp(hprec, 1, NULL);

				return (true);
			}
		}

		// Reset used Hazard Pointers
		// HP[0] gets reset right after
		smr_set_hp(hprec, 1, NULL);
	}
}

size_t queue_size(QUEUE q) {
	return (sbb_atomic_uint_get(&q->count));
}

bool queue_empty(QUEUE q) {
	return (sbb_atomic_uint_get(&q->count) == 0);
}

bool queue_full(QUEUE q) {
	return (sbb_atomic_uint_get(&q->count) == q->capacity);
}
