#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>

typedef struct StackStruct *STACK;

STACK stack_init(size_t capacity);
bool stack_destroy(STACK *s);
bool stack_push(STACK s, uintptr_t *item);
bool stack_pop(STACK s, uintptr_t *item);
bool stack_look(STACK s, uintptr_t *item);
size_t stack_size(STACK s);
bool stack_empty(STACK s);
bool stack_full(STACK s);

typedef struct NodeStruct *Node;

struct StackStruct {
	Node top;
	pthread_mutex_t mxlock;
	size_t capacity;
	size_t count;
};

struct NodeStruct {
	Node next;
	uintptr_t data;
};

STACK stack_init(size_t capacity) {
	// Allocate memory for the stack
	STACK stack = malloc(sizeof(*stack));
	if (stack == NULL) {
		errno = ENOMEM;
		return (NULL);
	}

	// Initialize the needed values
	stack->top = NULL;
	pthread_mutex_init(&stack->mxlock, NULL);
	stack->capacity = (capacity == 0) ? (SIZE_MAX) : (capacity);
	stack->count = 0;

	return (stack);
}

bool stack_destroy(STACK *s) {
	pthread_mutex_lock(&(*s)->mxlock);

	// Traverse the list and remove all nodes (sentinel included)
	Node curr = (*s)->top, succ = NULL;
	while (curr != NULL) {
		succ = curr->next;
		free(curr);
		curr = succ;
	}

	pthread_mutex_unlock(&(*s)->mxlock);

	// Destroy the lock and the stack
	pthread_mutex_destroy(&(*s)->mxlock);
	free(*s);
	*s = NULL;

	// NOTE: this is only example code, you'd really have to make sure
	// no one is using the mutex before destroying it, which one could
	// do using some form of reference counting for example, but that
	// is way out of scope of this article and what it wants to show

	return (true);
}

bool stack_push(STACK s, uintptr_t *item) {
	// Allocate memory for the new element
	Node node = malloc(sizeof(*node));
	if (node == NULL) {
		errno = ENOMEM;
		return (false);
	}

	// Set the content of the new element
	node->data = *item;

	pthread_mutex_lock(&s->mxlock);

	if (s->count == s->capacity)  {
		// List full
		pthread_mutex_unlock(&s->mxlock);
		free(node);

		errno = EXFULL;
		return (false);
	}

	// Link the new element in
	node->next = s->top;
	s->top = node;

	s->count++;

	pthread_mutex_unlock(&s->mxlock);

	return (true);
}

bool stack_pop(STACK s, uintptr_t *item) {
	pthread_mutex_lock(&s->mxlock);

	Node curr = s->top;

	if (curr == NULL) {
		// Empty list
		pthread_mutex_unlock(&s->mxlock);

		errno = ENOENT;
		return (false);
	}

	// Remove first element
	s->top = curr->next;

	s->count--;

	pthread_mutex_unlock(&s->mxlock);

	*item = curr->data;
	free(curr);

	return (true);
}

bool stack_look(STACK s, uintptr_t *item) {
	pthread_mutex_lock(&s->mxlock);

	Node curr = s->top;

	if (curr == NULL) {
		// Empty list
		pthread_mutex_unlock(&s->mxlock);

		errno = ENOENT;
		return (false);
	}

	// Get first element content
	*item = curr->data;

	pthread_mutex_unlock(&s->mxlock);

	return (true);
}

size_t stack_size(STACK s) {
	pthread_mutex_lock(&s->mxlock);

	size_t size = s->count;

	pthread_mutex_unlock(&s->mxlock);

	return (size);
}

bool stack_empty(STACK s) {
	pthread_mutex_lock(&s->mxlock);

	bool empty = (s->count == 0);

	pthread_mutex_unlock(&s->mxlock);

	return (empty);
}

bool stack_full(STACK s) {
	pthread_mutex_lock(&s->mxlock);

	bool full = (s->count == s->capacity);

	pthread_mutex_unlock(&s->mxlock);

	return (full);
}
