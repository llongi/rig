#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>

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
	Node head;
	Node tail;
	pthread_mutex_t mxlock;
	size_t capacity;
	size_t count;
};

struct NodeStruct {
	Node next;
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
	sentinel->next = NULL;
	sentinel->data = 0;
	queue->head = sentinel;
	queue->tail = sentinel;
	pthread_mutex_init(&queue->mxlock, NULL);
	queue->capacity = (capacity == 0) ? (SIZE_MAX) : (capacity);
	queue->count = 0;

	return (queue);
}

bool queue_destroy(QUEUE *q) {
	pthread_mutex_lock(&(*q)->mxlock);

	// Traverse the list and remove all nodes (sentinel included)
	Node curr = (*q)->head, succ = NULL;
	while (curr != NULL) {
		succ = curr->next;
		free(curr);
		curr = succ;
	}

	pthread_mutex_unlock(&(*q)->mxlock);

	// Destroy the lock and the queue
	pthread_mutex_destroy(&(*q)->mxlock);
	free(*q);
	*q = NULL;

	// NOTE: this is only example code, you'd really have to make sure
	// no one is using the mutex before destroying it, which one could
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
	node->data = *item;
	node->next = NULL;

	pthread_mutex_lock(&q->mxlock);

	if (q->count == q->capacity)  {
		// List full
		pthread_mutex_unlock(&q->mxlock);
		free(node);

		errno = EXFULL;
		return (false);
	}

	// Link the new element in
	q->tail->next = node;
	q->tail = node;

	q->count++;

	pthread_mutex_unlock(&q->mxlock);

	return (true);
}

bool queue_get(QUEUE q, uintptr_t *item) {
	pthread_mutex_lock(&q->mxlock);

	Node prev = q->head, curr = prev->next;

	if (curr == NULL) {
		// Empty list
		pthread_mutex_unlock(&q->mxlock);

		errno = ENOENT;
		return (false);
	}

	// Replace sentinel node
	q->head = curr;

	// Get first element content
	*item = curr->data;

	q->count--;

	pthread_mutex_unlock(&q->mxlock);

	free(prev);

	return (true);
}

bool queue_look(QUEUE q, uintptr_t *item) {
	pthread_mutex_lock(&q->mxlock);

	Node curr = q->head->next;

	if (curr == NULL) {
		// Empty list
		pthread_mutex_unlock(&q->mxlock);

		errno = ENOENT;
		return (false);
	}

	// Get first element content
	*item = curr->data;

	pthread_mutex_unlock(&q->mxlock);

	return (true);
}

size_t queue_size(QUEUE q) {
	pthread_mutex_lock(&q->mxlock);

	size_t size = q->count;

	pthread_mutex_unlock(&q->mxlock);

	return (size);
}

bool queue_empty(QUEUE q) {
	pthread_mutex_lock(&q->mxlock);

	bool empty = (q->count == 0);

	pthread_mutex_unlock(&q->mxlock);

	return (empty);
}

bool queue_full(QUEUE q) {
	pthread_mutex_lock(&q->mxlock);

	bool full = (q->count == q->capacity);

	pthread_mutex_unlock(&q->mxlock);

	return (full);
}
