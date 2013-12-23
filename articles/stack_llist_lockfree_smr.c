#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <sbb.h>

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
	SBB_A_PTR top;
	size_t capacity; // read-only value
	SBB_A_UINT count;
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
	sbb_atomic_ptr_set(&stack->top, NULL);
	stack->capacity = (capacity == 0) ? (SIZE_MAX) : (capacity);
	sbb_atomic_uint_set(&stack->count, 0);

	return (stack);
}

bool stack_destroy(STACK *s) {
	// Traverse the list and remove all nodes (sentinel included)
	Node curr = sbb_atomic_ptr_get(&(*s)->top), succ = NULL;
	while (curr != NULL) {
		succ = curr->next;
		free(curr);
		curr = succ;
	}

	// Destroy the stack
	free(*s);
	*s = NULL;

	// Cleanup retired nodes
	smr_scan();

	// NOTE: this is only example code, you'd really have to make sure
	// no one is using the stack before destroying it, which one could
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

	while (true) {
		size_t count = sbb_atomic_uint_get(&s->count);

		if (count == s->capacity)  {
			// List full
			free(node);

			errno = EXFULL;
			return (false);
		}

		if (sbb_atomic_uint_cas(&s->count, count, count + 1)) {
			break;
		}
	}

	Node top = NULL;

	while (true) {
		top = sbb_atomic_ptr_get(&s->top);

		node->next = top;

		if (sbb_atomic_ptr_cas(&s->top, top, node)) {
			return (true);
		}
	}
}

bool stack_pop(STACK s, uintptr_t *item) {
	// Get pointer to this thread's SMR_HPRecord
	SMR_HPRecord hprec = smr_get_hprecord();
	Node top = NULL;

	while (true) {
		// Reset used Hazard Pointers
		smr_set_hp(hprec, 0, NULL);

		top = sbb_atomic_ptr_get(&s->top);

		if (top == NULL) {
			// Empty list
			errno = ENOENT;
			return (false);
		}

		smr_set_hp(hprec, 0, top); // Set Hazard Pointer
		if (top != sbb_atomic_ptr_get(&s->top)) {
			continue;
		}

		if (sbb_atomic_ptr_cas(&s->top, top, top->next)) {
			sbb_atomic_uint_dec(&s->count);
			*item = top->data;

			// Reset used Hazard Pointers
			smr_set_hp(hprec, 0, NULL);

			smr_retire_mem(top);

			return (true);
		}
	}
}

bool stack_look(STACK s, uintptr_t *item) {
	// Get pointer to this thread's SMR_HPRecord
	SMR_HPRecord hprec = smr_get_hprecord();
	Node top = NULL;

	while (true) {
		// Reset used Hazard Pointers
		smr_set_hp(hprec, 0, NULL);

		top = sbb_atomic_ptr_get(&s->top);

		if (top == NULL) {
			// Empty list
			errno = ENOENT;
			return (false);
		}

		smr_set_hp(hprec, 0, top); // Set Hazard Pointer
		if (top != sbb_atomic_ptr_get(&s->top)) {
			continue;
		}

		*item = top->data;

		// Reset used Hazard Pointers
		smr_set_hp(hprec, 0, NULL);

		return (true);
	}
}

size_t stack_size(STACK s) {
	return (sbb_atomic_uint_get(&s->count));
}

bool stack_empty(STACK s) {
	return (sbb_atomic_uint_get(&s->count) == 0);
}

bool stack_full(STACK s) {
	return (sbb_atomic_uint_get(&s->count) == s->capacity);
}
