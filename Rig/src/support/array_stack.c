/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: array_stack.c 1049 2012-05-29 05:29:40Z llongi $
 */

#ifndef ARRAY_STACK_C
#define ARRAY_STACK_C 1

#include <string.h>
#include "sys/system_all.h"

#define ARRAY_STACK_BlockSize 32 // must be power of 2 (mask)

typedef struct array_stack *ARRAY_STACK;
typedef struct array_stack_block *ARRAY_STACK_BLOCK;

struct array_stack {
	ARRAY_STACK_BLOCK top;
	ARRAY_STACK_BLOCK next_top;
	size_t count;
	size_t elem_size;
};

struct array_stack_block {
	ARRAY_STACK_BLOCK prev;
	uint8_t block[];
};

// You need to declare a structure of type array_stack yourself and make sure it
// is initialized correctly, you can use array_stack_init() for this, it
// simply expects the single element size as an argument.
// Use a pointer to it then for push/pop/count operations.
// To avoid memory leaks, once you've finished using the array stack, you must
// call the array_stack_destroy() function. If you want to reuse the array stack,
// you have to call array_stack_init() again only if you change the element size.
// NOTE: this is absolutely not thread-safe, never use it on shared data!
// Also, the pointer returned by pop() points to internal memory for performance
// reasons, and is guaranteed valid only until the next call to pop() happens!

// Initialize an array stack
static inline void array_stack_init(ARRAY_STACK stack, size_t elem_size) ATTR_ALWAYSINLINE;
// Destroy an array stack, cleaning its state
static inline void array_stack_destroy(ARRAY_STACK stack) ATTR_ALWAYSINLINE;
// Copy element size bytes from ptr to the stack
static inline void array_stack_push(ARRAY_STACK stack, void *ptr) ATTR_ALWAYSINLINE;
// Return a pointer to the next element in the stack, NULL if empty
static inline void *array_stack_pop(ARRAY_STACK stack) ATTR_ALWAYSINLINE;
// Return the current element count of the stack
static inline size_t array_stack_count(ARRAY_STACK stack) ATTR_ALWAYSINLINE;


static inline void array_stack_init(ARRAY_STACK stack, size_t elem_size) {
	stack->top = NULL;
	stack->next_top = NULL;
	stack->count = 0;
	stack->elem_size = elem_size;
}

static inline void array_stack_destroy(ARRAY_STACK stack) {
	if (stack->next_top != NULL) {
		rig_mem_free(stack->next_top);
		stack->next_top = NULL;
	}

	ARRAY_STACK_BLOCK prev = NULL;

	while (stack->top != NULL) {
		prev = stack->top->prev;
		rig_mem_free(stack->top);
		stack->top = prev;
	}

	stack->count = 0;
}

static inline void array_stack_push(ARRAY_STACK stack, void *ptr) {
	// Nothing to copy onto the stack
	if (ptr == NULL) {
		return;
	}

	// Either allocate new memory block when needed to hold new elements,
	// or reuse an already present, empty one
	if ((stack->count & (ARRAY_STACK_BlockSize - 1)) == 0) {
		if (stack->next_top == NULL) {
			ARRAY_STACK_BLOCK nblock = rig_mem_alloc(sizeof(*nblock), ARRAY_STACK_BlockSize * stack->elem_size);
			NULLCHECK_EXIT(nblock);

			nblock->prev = stack->top;
			stack->top = nblock;
		}
		else {
			stack->top = stack->next_top;
			stack->next_top = NULL;
		}
	}

	memcpy(&stack->top->block[(stack->count & (ARRAY_STACK_BlockSize - 1)) * stack->elem_size], ptr, stack->elem_size);
	stack->count++;
}

static inline void *array_stack_pop(ARRAY_STACK stack) {
	// Empty stack
	if (stack->count == 0) {
		return (NULL);
	}

	// Deallocate old memory blocks when falling back
	if (((stack->count & (ARRAY_STACK_BlockSize - 1)) == (ARRAY_STACK_BlockSize - 1)) && (stack->next_top != NULL)) {
		rig_mem_free(stack->next_top);
		stack->next_top = NULL;
	}

	stack->count--;
	void *ptr = &stack->top->block[(stack->count & (ARRAY_STACK_BlockSize - 1)) * stack->elem_size];

	// Switch top back in preparation of possible block freeing, save on last one
	if ((stack->count & (ARRAY_STACK_BlockSize - 1)) == 0) {
		stack->next_top = stack->top;
		stack->top = stack->top->prev;
	}

	return (ptr);
}

static inline size_t array_stack_count(ARRAY_STACK stack) {
	return (stack->count);
}

#endif /* ARRAY_STACK_C */
