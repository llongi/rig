/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_list.c 1140 2012-11-14 19:51:34Z llongi $
 */

#include "rig_internal.h"
#include <atomic_ops.h>
#include "support/array_stack.c"

/*
 * Rig List Data Definitions
 */
#if RIG_LIST_SMR_TYPE == 1
	#define SMR_HP_CURR 0
	#define SMR_HP_PREV 1
	#define SMR_HP_KEYN 2
#endif

/** Types */
typedef struct KeyNodeStruct *KeyNode;
typedef struct NodeStruct *Node;

/** Structures */
struct rig_list {
	KeyNode khead CACHELINE_ALIGNED;
	RIG_COUNTER count;
	RIG_COUNTER refcount;
	int (*cmp)(void *data, void *item); // comparator function
	size_t (*hash)(void *item); // hash function
	uint16_t flags; // read-only value
};

struct KeyNodeStruct {
	atomic_ops_flagptr next; // Next Node (first one)
	size_t mkey; // Master-Key
	atomic_ops_ptr knext; // Next KeyNode
};

struct NodeStruct {
	atomic_ops_flagptr next; // Next Node
	size_t skey; // Sub-Key (carries duplicate count)
	void *data; // Data
};

#define KEY_INTERVAL 5 // A KeyNode is responsible for every 2^N keys
#define MKEY_MASK_HI (((size_t)-1) << (KEY_INTERVAL))
#define SKEY_SHIFT ((sizeof(size_t) * 8) - (KEY_INTERVAL))
#define SKEY_MASK_LO (((size_t)-1) >> (KEY_INTERVAL))
#define SKEY_MASK_HI (~(SKEY_MASK_LO))

static int list_default_cmp(void *data, void *item);
static size_t list_default_hash(void *item);

static inline void list_traverse_keynodes(const KeyNode khead, size_t mkey, KeyNode * const ekprev, KeyNode * const ekcurr) ATTR_ALWAYSINLINE;
static inline void list_traverse_nodes(const KeyNode khead, Node head, size_t skey, void *item, int (*cmp)(void *data, void *item),
	Node * const eprev, Node * const ecurr, size_t * const dup_count, RIG_SMR_HP_Record hprec) ATTR_ALWAYSINLINE;
static inline KeyNode list_add_keynode(const KeyNode lhead, size_t mkey) ATTR_ALWAYSINLINE;
static inline bool list_get_first(const KeyNode lhead, Node * const eprev, Node * const ecurr, RIG_SMR_HP_Record hprec) ATTR_ALWAYSINLINE;


/*
 * Rig List Implementation
 */

/**
 * INTERNAL
 * Default comparator function.
 *
 * @param data
 *     data in the node
 * @param item
 *     item searched for
 *
 * @return
 *     zero if equal, else non-zero
 */
static int list_default_cmp(void *data, void *item) {
	return ((data == item) ? (0) : (1));
}

/**
 * INTERNAL
 * Default hash function.
 *
 * @param item
 *     item searched for
 *
 * @return
 *     hash value
 */
static size_t list_default_hash(void *item) {
	return ((size_t)item);
}

/**
 * Initialize a lock-free ordered list/set.
 * This data structure is intended for low-level, high-performance purposes, and thus only accepts and returns memory
 * addresses. Their content and its persistence is the responsibility of the application programmer.
 *
 * @param capacity
 *     maximum number of elements the list can contain, 0 means unlimited
 * @param flags
 *     flags to modify list behavior, the following are currently supported:
 *     - RIG_LIST_NOCOUNT (do not count elements, capacity is not enforced)
 *     - RIG_LIST_NODUPS (disallow duplicate elements in the list)
 * @param cmp
 *     comparator function, checks if the element currently being examined is the element we're searching for
 * @param hash
 *     hash function, generates a hash to identify the searched for element
 *
 * @return
 *     list pointer, NULL on error.
 *     On error, the following error codes are set:
 *     - EINVAL (invalid arguments passed)
 *     - ENOMEM (insufficient memory)
 */
RIG_LIST rig_list_init(size_t capacity, uint16_t flags, int (*cmp)(void *data, void *item), size_t (*hash)(void *item)) {
	CHECK_PERMITTED_FLAGS(flags, RIG_LIST_NOCOUNT | RIG_LIST_NODUPS);

	// Allocate memory for the list
	RIG_LIST l = rig_mem_alloc_aligned(sizeof(*l), 0, CACHELINE_SIZE, 0);
	NULLCHECK_ERRET(l, ENOMEM, NULL);

	// Allocate memory for the sentinel KeyNode
	KeyNode khead = rig_mem_alloc(sizeof(*khead), 0);
	NULLCHECK_ERRET_CLEANUP(khead, ENOMEM, NULL, rig_mem_free_aligned(l));

	// Initialize the counters
	RIG_COUNTER refcount = rig_counter_init(1, 0);
	NULLCHECK_ERRET_CLEANUP(refcount, ENOMEM, NULL, rig_mem_free_aligned(l); rig_mem_free(khead));

	RIG_COUNTER count = NULL;
	if (!TEST_BITFIELD(flags, RIG_LIST_NOCOUNT)) { // Support not counting elements
		count = rig_counter_init(0, capacity);
		NULLCHECK_ERRET_CLEANUP(count, ENOMEM, NULL,
			rig_mem_free_aligned(l); rig_mem_free(khead); rig_counter_destroy(&refcount));
	}

	// Initialize the needed values
	atomic_ops_flagptr_store(&khead->next, NULL, false, ATOMIC_OPS_FENCE_NONE);
	khead->mkey = 0;
	atomic_ops_ptr_store(&khead->knext, NULL, ATOMIC_OPS_FENCE_NONE);

	l->khead = khead;
	l->count = count;
	l->refcount = refcount;
	l->cmp = (cmp != NULL) ? (cmp) : (&list_default_cmp);
	l->hash = (hash != NULL) ? (hash) : (&list_default_hash);
	l->flags = flags;

	atomic_ops_fence(ATOMIC_OPS_FENCE_FULL);

	return (l);
}

/**
 * Get a new reference to the list, increasing the reference count by one.
 *
 * @param l
 *     list pointer
 *
 * @return
 *     list pointer
 */
RIG_LIST rig_list_newref(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	rig_acheck_msg(rig_counter_inc(l->refcount), "more references than physically possible");

	return (l);
}

/**
 * Destroy specified list reference and set pointer to NULL.
 * If reference count reaches zero, proceed to full destruction.
 *
 * @param *l
 *     pointer to list pointer
 */
void rig_list_destroy(RIG_LIST *l) {
	NULLCHECK_EXIT(l);
	NULLCHECK_EXIT(*l);

	if (rig_counter_dec_and_test((*l)->refcount)) {
		// Traverse the list and remove all nodes (sentinel included)
		KeyNode kcurr = (*l)->khead, ksucc = NULL;
		Node curr = NULL, succ = NULL;

		while (kcurr != NULL) {
			curr = atomic_ops_flagptr_load(&kcurr->next, NULL, ATOMIC_OPS_FENCE_NONE);

			while (curr != NULL) {
				succ = atomic_ops_flagptr_load(&curr->next, NULL, ATOMIC_OPS_FENCE_NONE);

				// Free directly here, as there are no shared references anymore around, no SMR is required!
				rig_mem_free(curr);

				curr = succ;
			}

			ksucc = atomic_ops_ptr_load(&kcurr->knext, ATOMIC_OPS_FENCE_NONE);

			// Free directly here, as there are no shared references anymore around, no SMR is required!
			rig_mem_free(kcurr);

			kcurr = ksucc;
		}

		// Destroy counters and list
		rig_counter_destroy(&(*l)->refcount);
		rig_counter_destroy(&(*l)->count);
		rig_mem_free_aligned(*l);
	}
	else {
		rig_acheck_msg(errno == 0, "reference count already zero, uncounted references exist");
	}

	*l = NULL;

	atomic_ops_fence(ATOMIC_OPS_FENCE_FULL);

#if RIG_LIST_SMR_TYPE == 1
	// Explicit SMR HP scan
	rig_smr_hp_mem_scan();
#endif
}

/**
 * Clear the list by getting elements from it until it's empty.
 *
 * @param l
 *     list pointer
 */
void rig_list_clear(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	while (rig_list_get(l) != NULL) { ; }
}

/**
 * INTERNAL
 * Traverse through KeyNodes, starting from khead, and search for the one with
 * the specified mkey, returning then pointers to the previous and current nodes.
 *
 * @param khead
 *     Starting point for traversal
 * @param mkey
 *     Key to search for
 * @param *ekprev
 *     Pointer in which to store reference to previous node
 * @param *ekcurr
 *     Pointer in which to store reference to current node
 */
static inline void list_traverse_keynodes(const KeyNode khead, size_t mkey, KeyNode * const ekprev, KeyNode * const ekcurr) {
	KeyNode kprev = NULL, kcurr = NULL;

	// NOTE: khead is lhead (which by virtue of being the sentinel node
	// is always accessible)
	kprev = khead;
	kcurr = atomic_ops_ptr_load(&kprev->knext, ATOMIC_OPS_FENCE_NONE);

	while (kcurr != NULL) {
		if (kcurr->mkey == mkey) {
			break;
		}

		if (kcurr->mkey > mkey) {
			break;
		}

		kprev = kcurr;
		kcurr = atomic_ops_ptr_load(&kcurr->knext, ATOMIC_OPS_FENCE_NONE);
	}

	*ekprev = kprev;
	*ekcurr = kcurr;
}

/**
 * INTERNAL
 * Traverse through Nodes, starting from khead, and search for the one with
 * the specified skey, returning then pointers to the previous and current nodes.
 * Either just use skey for search if the comparison function is NULL, or
 * execute the comparison on item otherwise.
 * The duplicate count can also be gotten back when needed.
 * The nodes are protected by the relevant Hazard Pointers, which are updated.
 *
 * @param khead
 *     Starting point for initial traversal and fallback (KeyNode)
 * @param head
 *     Starting point for optimized re-traversal
 * @param skey
 *     Key to search for
 * @param item
 *     Item to search for
 * @param *cmp
 *     Pointer to comparison function for the item, can be NULL
 * @param *eprev
 *     Pointer in which to store reference to previous node
 * @param *ecurr
 *     Pointer in which to store reference to current node
 * @param *dup_count
 *     Pointer in which to store the duplicate count
 * @param hprec
 *     Hazard Pointer Record
 */
static inline void list_traverse_nodes(const KeyNode khead, Node head, size_t skey, void *item, int (*cmp)(void *data, void *item),
	Node * const eprev, Node * const ecurr, size_t * const dup_count, RIG_SMR_HP_Record hprec) {
#if RIG_LIST_SMR_TYPE != 1
	UNUSED(hprec);
#endif
	bool mark = false;
	Node prev = NULL, curr = NULL, succ = NULL;

	// NOTE: SMR_HP_KEYN is always protecting khead here, and head is either
	// khead or an old prev, which is protected by SMR_HP_PREV on entry.
	prev = head;
retry:
	curr = atomic_ops_flagptr_load(&prev->next, &mark, ATOMIC_OPS_FENCE_ACQUIRE);
	// We care about the mark here, because if curr is NULL, a marked prev,
	// which is possible, could slip through (by bypassing the whole loop),
	// which would later lead to problems and is invalid behavior.
	if (mark) {
		prev = (Node)khead;
		goto retry;
	}

	while (curr != NULL) {
#if RIG_LIST_SMR_TYPE == 1
		rig_smr_hp_set(hprec, SMR_HP_CURR, curr);
		if (curr != atomic_ops_flagptr_load_full(&prev->next, NULL, ATOMIC_OPS_FENCE_ACQUIRE)) {
			goto retry;
		}
#endif

		succ = atomic_ops_flagptr_load(&curr->next, &mark, ATOMIC_OPS_FENCE_NONE);

		if (mark) {
			// Attempt physical removal
			if (!atomic_ops_flagptr_cas(&prev->next, curr, false, succ, false, ATOMIC_OPS_FENCE_FULL)) {
				goto retry;
			}

#if RIG_LIST_SMR_TYPE == 1
			rig_smr_hp_release(hprec, SMR_HP_CURR);
			rig_smr_hp_mem_retire(curr);
#endif
#if RIG_LIST_SMR_TYPE == 2
			rig_smr_epoch_mem_retire(curr);
#endif

			curr = succ;
		}
		else {
			if (((curr->skey & SKEY_MASK_HI) == skey) && (cmp != NULL) && ((*cmp)(curr->data, item) == 0)) {
				break;
			}

			if ((curr->skey & SKEY_MASK_HI) > skey) {
				break;
			}

			prev = curr;
#if RIG_LIST_SMR_TYPE == 1
			rig_smr_hp_set(hprec, SMR_HP_PREV, prev);
#endif
			curr = succ;
		}
	}

	*eprev = prev;
	*ecurr = curr;

	if (dup_count != NULL) {
		// We're interested in the dup_count only when adding items to the list,
		// and we always add to the back (between last of a given skey and the
		// next bigger skey), as such we only care about prev here.
		// NOTE: prev is always defined and protected, so we can just access it,
		// but it may be the KeyNode (when adding the first Node or a Node with
		// skey smaller than the first one), so by accessing skey we'd actually
		// access mkey, which is wrong, as it stores the rest of the real key
		// in those bits; hence the check that prev is not the head KeyNode.
		if ((prev != (Node)khead) && ((prev->skey & SKEY_MASK_HI) == skey)) {
			*dup_count = (prev->skey & SKEY_MASK_LO) + 1;
		}
		else {
			*dup_count = 1;
		}
	}
}

/**
 * INTERNAL
 * Either add a new KeyNode at the appropriate location and return it, or
 * directly return an already present, matching one.
 *
 * @param lhead
 *     Starting point for traversal (list head)
 * @param mkey
 *     Key to search for
 * @param hprec
 *     Hazard Pointer Record
 *
 * @return
 *     KeyNode corresponding to specified key, NULL on error.
 *     On error, the following error codes are set:
 *     - ENOMEM (insufficient memory to add the new KeyNode)
 */
static inline KeyNode list_add_keynode(const KeyNode lhead, size_t mkey) {
	KeyNode kprev = NULL, kcurr = NULL, khead = NULL, knode = NULL;

	khead = lhead;

	while (true) {
		// Find first occurrence of item or the right next key (if not present)
		list_traverse_keynodes(khead, mkey, &kprev, &kcurr);

		if ((kcurr != NULL) && (kcurr->mkey == mkey)) {
			// Found key-node, cleanup temporary one (if exists)
			if (knode != NULL) {
				rig_mem_free(knode);
			}

			break;
		}

		// Key-node not present, create and add it
		if (knode == NULL) {
			knode = rig_mem_alloc(sizeof(*knode), 0);
			NULLCHECK_ERRET(knode, ENOMEM, NULL);

			// Set the content of the new key-node
			atomic_ops_flagptr_store(&knode->next, NULL, false, ATOMIC_OPS_FENCE_NONE);
			knode->mkey = mkey;
		}

		// Link the new element in
		atomic_ops_ptr_store(&knode->knext, kcurr, ATOMIC_OPS_FENCE_NONE);

		if (!atomic_ops_ptr_cas(&kprev->knext, kcurr, knode, ATOMIC_OPS_FENCE_FULL)) {
			khead = kprev;
			continue;
		}

		// Key-node added
		kcurr = knode;

		break;
	}

	return (kcurr);
}

/**
 * Add a new item to the list.
 *
 * @param l
 *     list pointer
 * @param item
 *     data pointer (memory management caller responsibility)
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EINVAL (NULL pointer passed, invalid value)
 *     - ENOMEM (insufficient memory to add the new item)
 *     - EXFULL (maximum capacity reached)
 *     - EEXIST (item already present)
 */
bool rig_list_add(RIG_LIST l, void *item) {
	NULLCHECK_EXIT(l);
	NULLCHECK_ERRET(item, EINVAL, false);

	// Allocate memory for the new element
	Node node = rig_mem_alloc(sizeof(*node), 0);
	NULLCHECK_ERRET(node, ENOMEM, false);

	// Check if there's still place for the new element
	if ((l->count != NULL) && (!rig_counter_inc(l->count))) {
		rig_mem_free(node);

		// List full
		ERRET(EXFULL, false);
	}

	// Set the content of the new element
	size_t key = (*(l->hash))(item);
	size_t mkey = key & MKEY_MASK_HI;
	size_t skey = key << SKEY_SHIFT;

	node->data = item;

	KeyNode khead = NULL;
	Node prev = NULL, curr = NULL, head = NULL;
	size_t dup_count = 0;

	khead = list_add_keynode(l->khead, mkey);

	if (khead == NULL) {
		// Roll-back global changes
		if (l->count != NULL) { // Counting supported
			rig_acheck_msg(rig_counter_dec(l->count), "removing non-counted node, this should never happen!");
		}
		rig_mem_free(node);

		// Failed to allocate memory for KeyNode!
		ERRET(ENOMEM, false);
	}

#if RIG_LIST_SMR_TYPE == 1
	// Get pointer to this thread's RIG_SMR_HP_Record
	RIG_SMR_HP_Record hprec = rig_smr_hp_record_get();

	// khead contains the key-node, transfer its protection to SMR_HP_KEYN
	rig_smr_hp_set(hprec, SMR_HP_KEYN, khead);
#else
	RIG_SMR_HP_Record hprec = NULL;
#endif
#if RIG_LIST_SMR_TYPE == 2
	rig_smr_epoch_critical_enter();
#endif

	head = (Node)khead;

	while (true) {
		// Find first occurrence of item or the right next key (if not present)
		if (TEST_BITFIELD(l->flags, RIG_LIST_NODUPS)) {
			list_traverse_nodes(khead, head, skey, item, l->cmp, &prev, &curr, &dup_count, hprec);

			if ((curr != NULL) && ((curr->skey & SKEY_MASK_HI) == skey)) {
				// Roll-back global changes
				if (l->count != NULL) { // Counting supported
					rig_acheck_msg(rig_counter_dec(l->count), "removing non-counted node, this should never happen!");
				}
				rig_mem_free(node);

#if RIG_LIST_SMR_TYPE == 1
				// Reset used Hazard Pointers
				rig_smr_hp_release(hprec, SMR_HP_CURR);
				rig_smr_hp_release(hprec, SMR_HP_PREV);
				rig_smr_hp_release(hprec, SMR_HP_KEYN);
#endif
#if RIG_LIST_SMR_TYPE == 2
				rig_smr_epoch_critical_exit();
#endif

				// Item already exists!
				ERRET(EEXIST, false);
			}
		}
		else {
			list_traverse_nodes(khead, head, skey, item, NULL, &prev, &curr, &dup_count, hprec);
		}

		// We accept duplicates, up to dup_count's maximum value (which
		// corresponds to SKEY_MASK_LO), after that we return a generic error.
		if (dup_count == SKEY_MASK_LO) {
			// Roll-back global changes
			if (l->count != NULL) { // Counting supported
				rig_acheck_msg(rig_counter_dec(l->count), "removing non-counted node, this should never happen!");
			}
			rig_mem_free(node);

#if RIG_LIST_SMR_TYPE == 1
			// Reset used Hazard Pointers
			rig_smr_hp_release(hprec, SMR_HP_CURR);
			rig_smr_hp_release(hprec, SMR_HP_PREV);
			rig_smr_hp_release(hprec, SMR_HP_KEYN);
#endif
#if RIG_LIST_SMR_TYPE == 2
			rig_smr_epoch_critical_exit();
#endif

			// Maximum number of duplicates reached, hash-slot is full!
			ERRET(EXFULL, false);
		}

		// Set the duplicate count and the key correctly
		node->skey = skey | dup_count;

		// Link the new element in
		atomic_ops_flagptr_store(&node->next, curr, false, ATOMIC_OPS_FENCE_NONE);

		if (!atomic_ops_flagptr_cas(&prev->next, curr, false, node, false, ATOMIC_OPS_FENCE_FULL)) {
			head = prev;
			continue;
		}

#if RIG_LIST_SMR_TYPE == 1
		// Reset used Hazard Pointers
		rig_smr_hp_release(hprec, SMR_HP_CURR);
		rig_smr_hp_release(hprec, SMR_HP_PREV);
		rig_smr_hp_release(hprec, SMR_HP_KEYN);
#endif
#if RIG_LIST_SMR_TYPE == 2
		rig_smr_epoch_critical_exit();
#endif

		// Item added
		return (true);
	}
}

/**
 * Remove the specified item from the list.
 *
 * @param l
 *     List data
 * @param item
 *     data pointer (memory management caller responsibility)
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EINVAL (NULL pointer passed, invalid value)
 *     - ENOENT (item not found)
 */
bool rig_list_del(RIG_LIST l, void *item) {
	NULLCHECK_EXIT(l);
	NULLCHECK_ERRET(item, EINVAL, false);

	size_t key = (*(l->hash))(item);
	size_t mkey = key & MKEY_MASK_HI;
	size_t skey = key << SKEY_SHIFT;

	KeyNode kprev = NULL, kcurr = NULL, khead = NULL;
	Node prev = NULL, curr = NULL, head = NULL;

	khead = l->khead;

	// Find first occurrence of item or the right next key (if not present)
	list_traverse_keynodes(khead, mkey, &kprev, &kcurr);

	if ((kcurr == NULL) || (kcurr->mkey != mkey)) {
		// Key-node absent, which means the key doesn't exist
		ERRET(ENOENT, false);
	}

	khead = NULL;

#if RIG_LIST_SMR_TYPE == 1
	// Get pointer to this thread's RIG_SMR_HP_Record
	RIG_SMR_HP_Record hprec = rig_smr_hp_record_get();

	// kcurr contains the key-node, transfer its protection to SMR_HP_KEYN
	rig_smr_hp_set(hprec, SMR_HP_KEYN, kcurr);
#else
	RIG_SMR_HP_Record hprec = NULL;
#endif
#if RIG_LIST_SMR_TYPE == 2
	rig_smr_epoch_critical_enter();
#endif

	head = (Node)kcurr;

retry:
	// Find first occurrence of item or the right next key (if not present)
	list_traverse_nodes(kcurr, head, skey, item, l->cmp, &prev, &curr, NULL, hprec);

	if ((curr != NULL) && ((curr->skey & SKEY_MASK_HI) == skey)) {
		// Item present, remove it
		Node succ = atomic_ops_flagptr_load(&curr->next, NULL, ATOMIC_OPS_FENCE_NONE);

		// Mark node for deletion (logical removal)
		if (!atomic_ops_flagptr_cas(&curr->next, succ, false, succ, true, ATOMIC_OPS_FENCE_FULL)) {
			head = prev;
			goto retry;
		}

		if (l->count != NULL) { // Counting supported
			rig_acheck_msg(rig_counter_dec(l->count), "removing non-counted node, this should never happen!");
		}

		// Attempt physical removal
		if (atomic_ops_flagptr_cas(&prev->next, curr, false, succ, false, ATOMIC_OPS_FENCE_FULL)) {
#if RIG_LIST_SMR_TYPE == 1
			rig_smr_hp_release(hprec, SMR_HP_CURR);
			rig_smr_hp_mem_retire(curr);
#endif
#if RIG_LIST_SMR_TYPE == 2
			rig_smr_epoch_mem_retire(curr);
#endif
		}
#if RIG_LIST_SMR_TYPE == 1
		else {
			rig_smr_hp_release(hprec, SMR_HP_CURR);
		}

		// Reset used Hazard Pointers
		rig_smr_hp_release(hprec, SMR_HP_PREV);
		rig_smr_hp_release(hprec, SMR_HP_KEYN);
#endif
#if RIG_LIST_SMR_TYPE == 2
		rig_smr_epoch_critical_exit();
#endif

		// Item removed
		return (true);
	}

#if RIG_LIST_SMR_TYPE == 1
	// Reset used Hazard Pointers
	rig_smr_hp_release(hprec, SMR_HP_CURR);
	rig_smr_hp_release(hprec, SMR_HP_PREV);
	rig_smr_hp_release(hprec, SMR_HP_KEYN);
#endif
#if RIG_LIST_SMR_TYPE == 2
	rig_smr_epoch_critical_exit();
#endif

	// Empty list
	ERRET(ENOENT, false);
}

/**
 * Find the specified item in the list.
 *
 * @param l
 *     List data
 * @param item
 *     data pointer (memory management caller responsibility)
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - EINVAL (NULL pointer passed, invalid value)
 *     - ENOENT (item not found)
 */
bool rig_list_find(RIG_LIST l, void *item) {
	NULLCHECK_EXIT(l);
	NULLCHECK_ERRET(item, EINVAL, false);

	size_t key = (*(l->hash))(item);
	size_t mkey = key & MKEY_MASK_HI;
	size_t skey = key << SKEY_SHIFT;

	KeyNode kprev = NULL, kcurr = NULL, khead = NULL;
	Node prev = NULL, curr = NULL, head = NULL;

	khead = l->khead;

	// Find first occurrence of item or the right next key (if not present)
	list_traverse_keynodes(khead, mkey, &kprev, &kcurr);

	if ((kcurr == NULL) || (kcurr->mkey != mkey)) {
		// Key-node absent, which means the key doesn't exist
		ERRET(ENOENT, false);
	}

	khead = NULL;

#if RIG_LIST_SMR_TYPE == 1
	// Get pointer to this thread's RIG_SMR_HP_Record
	RIG_SMR_HP_Record hprec = rig_smr_hp_record_get();

	// kcurr contains the key-node, transfer its protection to SMR_HP_KEYN
	rig_smr_hp_set(hprec, SMR_HP_KEYN, kcurr);
#else
	RIG_SMR_HP_Record hprec = NULL;
#endif
#if RIG_LIST_SMR_TYPE == 2
	rig_smr_epoch_critical_enter();
#endif

	head = (Node)kcurr;

	// Find first occurrence of item or the right next key (if not present)
	list_traverse_nodes(kcurr, head, skey, item, l->cmp, &prev, &curr, NULL, hprec);

	if ((curr != NULL) && ((curr->skey & SKEY_MASK_HI) == skey)) {
#if RIG_LIST_SMR_TYPE == 1
		// Reset used Hazard Pointers
		rig_smr_hp_release(hprec, SMR_HP_CURR);
		rig_smr_hp_release(hprec, SMR_HP_PREV);
		rig_smr_hp_release(hprec, SMR_HP_KEYN);
#endif
#if RIG_LIST_SMR_TYPE == 2
		rig_smr_epoch_critical_exit();
#endif

		// Item present
		return (true);
	}

#if RIG_LIST_SMR_TYPE == 1
	// Reset used Hazard Pointers
	rig_smr_hp_release(hprec, SMR_HP_CURR);
	rig_smr_hp_release(hprec, SMR_HP_PREV);
	rig_smr_hp_release(hprec, SMR_HP_KEYN);
#endif
#if RIG_LIST_SMR_TYPE == 2
	rig_smr_epoch_critical_exit();
#endif

	// Empty list
	ERRET(ENOENT, false);
}

/**
 * INTERNAL
 * Get the first node in the list.
 *
 * @param lhead
 *     Starting point for traversal (list head)
 * @param *eprev
 *     Pointer in which to store reference to previous node
 * @param *ecurr
 *     Pointer in which to store reference to current node
 * @param hprec
 *     Hazard Pointer Record
 *
 * @return
 *     boolean indicating success.
 *     On error, the following error codes are set:
 *     - ENOENT (empty list, nothing to return)
 */
static inline bool list_get_first(const KeyNode lhead, Node * const eprev, Node * const ecurr, RIG_SMR_HP_Record hprec) {
#if RIG_LIST_SMR_TYPE != 1
	UNUSED(hprec);
#endif
	bool mark = false;
	KeyNode kprev = NULL, kcurr = NULL;
	Node prev = NULL, curr = NULL, succ = NULL;

retry:
	kprev = lhead;
	kcurr = atomic_ops_ptr_load(&kprev->knext, ATOMIC_OPS_FENCE_NONE);

	while (kcurr != NULL) {
		if (atomic_ops_flagptr_load(&kcurr->next, NULL, ATOMIC_OPS_FENCE_ACQUIRE) != NULL) {
			// Found non-marked key-node with a non-empty sublist, explore it
			prev = (Node)kcurr;
#if RIG_LIST_SMR_TYPE == 1
			rig_smr_hp_set(hprec, SMR_HP_PREV, prev);
#endif
			curr = atomic_ops_flagptr_load(&prev->next, NULL, ATOMIC_OPS_FENCE_ACQUIRE);
			// We don't care about the mark here, because prev, being a KeyNode,
			// will never be marked, or it possibly could be only if curr
			// is NULL, which anyway leads to restarting from the list head.

			while (curr != NULL) {
#if RIG_LIST_SMR_TYPE == 1
				rig_smr_hp_set(hprec, SMR_HP_CURR, curr);
				if (curr != atomic_ops_flagptr_load_full(&prev->next, NULL, ATOMIC_OPS_FENCE_ACQUIRE)) {
					goto retry;
				}
#endif

				succ = atomic_ops_flagptr_load(&curr->next, &mark, ATOMIC_OPS_FENCE_NONE);

				if (mark) {
					// Attempt physical removal
					if (!atomic_ops_flagptr_cas(&prev->next, curr, false, succ, false, ATOMIC_OPS_FENCE_FULL)) {
						goto retry;
					}

#if RIG_LIST_SMR_TYPE == 1
					rig_smr_hp_release(hprec, SMR_HP_CURR);
					rig_smr_hp_mem_retire(curr);
#endif
#if RIG_LIST_SMR_TYPE == 2
					rig_smr_epoch_mem_retire(curr);
#endif

					curr = succ;
				}
				else {
					*eprev = prev;
					*ecurr = curr;

					return (true);
				}
			}

			goto retry; // The sublist, which looked non-empty, is suddenly empty
		}

		kprev = kcurr;
		kcurr = atomic_ops_ptr_load(&kcurr->knext, ATOMIC_OPS_FENCE_NONE);
	}

	// Empty list
	ERRET(ENOENT, false);
}

/**
 * Remove the first item from the list and return it.
 *
 * @param l
 *     List data
 *
 * @return
 *     Data pointer, NULL on error.
 *     On error, the following error codes are set:
 *     - ENOENT (empty list, nothing to return)
 */
void *rig_list_get(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	Node prev = NULL, curr = NULL;

#if RIG_LIST_SMR_TYPE == 1
	// Get pointer to this thread's RIG_SMR_HP_Record
	RIG_SMR_HP_Record hprec = rig_smr_hp_record_get();
#else
	RIG_SMR_HP_Record hprec = NULL;
#endif
#if RIG_LIST_SMR_TYPE == 2
	rig_smr_epoch_critical_enter();
#endif

retry:
	if (list_get_first(l->khead, &prev, &curr, hprec)) {
		// Item present, remove it
		Node succ = atomic_ops_flagptr_load(&curr->next, NULL, ATOMIC_OPS_FENCE_NONE);

		// Mark node for deletion (logical removal)
		if (!atomic_ops_flagptr_cas(&curr->next, succ, false, succ, true, ATOMIC_OPS_FENCE_FULL)) {
			goto retry;
		}

		if (l->count != NULL) { // Counting supported
			rig_acheck_msg(rig_counter_dec(l->count), "removing non-counted node, this should never happen!");
		}

		void *item = curr->data;

		// Attempt physical removal
		if (atomic_ops_flagptr_cas(&prev->next, curr, false, succ, false, ATOMIC_OPS_FENCE_FULL)) {
#if RIG_LIST_SMR_TYPE == 1
			rig_smr_hp_release(hprec, SMR_HP_CURR);
			rig_smr_hp_mem_retire(curr);
#endif
#if RIG_LIST_SMR_TYPE == 2
			rig_smr_epoch_mem_retire(curr);
#endif
		}
#if RIG_LIST_SMR_TYPE == 1
		else {
			rig_smr_hp_release(hprec, SMR_HP_CURR);
		}

		// Reset used Hazard Pointers
		rig_smr_hp_release(hprec, SMR_HP_PREV);
#endif
#if RIG_LIST_SMR_TYPE == 2
		rig_smr_epoch_critical_exit();
#endif

		return (item);
	}

#if RIG_LIST_SMR_TYPE == 1
	// Reset used Hazard Pointers
	rig_smr_hp_release(hprec, SMR_HP_CURR);
	rig_smr_hp_release(hprec, SMR_HP_PREV);
#endif
#if RIG_LIST_SMR_TYPE == 2
	rig_smr_epoch_critical_exit();
#endif

	// Empty list
	ERRET(ENOENT, NULL);
}

/**
 * Return the first item from the list, without removing it.
 *
 * @param l
 *     List data
 *
 * @return
 *     Data pointer, NULL on error.
 *     On error, the following error codes are set:
 *     - ENOENT (empty list, nothing to return)
 */
void *rig_list_peek(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	Node prev = NULL, curr = NULL;

#if RIG_LIST_SMR_TYPE == 1
	// Get pointer to this thread's RIG_SMR_HP_Record
	RIG_SMR_HP_Record hprec = rig_smr_hp_record_get();
#else
	RIG_SMR_HP_Record hprec = NULL;
#endif
#if RIG_LIST_SMR_TYPE == 2
	rig_smr_epoch_critical_enter();
#endif

	if (list_get_first(l->khead, &prev, &curr, hprec)) {
		void *item = curr->data;

#if RIG_LIST_SMR_TYPE == 1
		// Reset used Hazard Pointers
		rig_smr_hp_release(hprec, SMR_HP_CURR);
		rig_smr_hp_release(hprec, SMR_HP_PREV);
#endif
#if RIG_LIST_SMR_TYPE == 2
		rig_smr_epoch_critical_exit();
#endif

		return (item);
	}

#if RIG_LIST_SMR_TYPE == 1
	// Reset used Hazard Pointers
	rig_smr_hp_release(hprec, SMR_HP_CURR);
	rig_smr_hp_release(hprec, SMR_HP_PREV);
#endif
#if RIG_LIST_SMR_TYPE == 2
	rig_smr_epoch_critical_exit();
#endif

	// Empty list
	ERRET(ENOENT, NULL);
}

/**
 * Returns true if the list is currently empty, false otherwise.
 *
 * @param l
 *     List data
 *
 * @return
 *     boolean indicating emptiness
 */
bool rig_list_empty(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	if (l->count != NULL) { // Counting supported
		return (rig_counter_get(l->count) == 0);
	}

	return (false);
}

/**
 * Returns true if the list is currently full, false otherwise.
 *
 * @param l
 *     List data
 *
 * @return
 *     boolean indicating fullness
 */
bool rig_list_full(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	if (l->count != NULL) { // Counting supported
		return (rig_counter_get(l->count) == rig_counter_get_max(l->count));
	}

	return (false);
}

/**
 * Returns the number of items currently held in the list.
 *
 * @param l
 *     List data
 *
 * @return
 *     number of items in list
 */
size_t rig_list_count(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	if (l->count != NULL) { // Counting supported
		return (rig_counter_get(l->count));
	}

	return (0);
}

/**
 * Returns the maximum capacity of the list.
 *
 * @param l
 *     List data
 *
 * @return
 *     maximum list capacity
 */
size_t rig_list_capacity(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	if (l->count != NULL) { // Counting supported
		return (rig_counter_get_max(l->count));
	}

	return (SIZE_MAX);
}


struct rig_list_iter {
	RIG_LIST list;
	size_t last_skey;
	bool completed;
};

#define SMR_IHP_CURR 4
#define SMR_IHP_PREV 5
#define SMR_IHP_KEYN 6

/**
 * Create an iterator over a list.
 *
 * @param l
 *     List to iterate over
 *
 * @return
 *     List iterator data, NULL on error.
 *     On error, the following error codes are set:
 *     - EALREADY (another iterator already exists)
 *     - ENOMEM (insufficient memory)
 */
RIG_LIST_ITER rig_list_iter_begin(RIG_LIST l) {
	NULLCHECK_EXIT(l);

#if RIG_LIST_SMR_TYPE == 1
	CHECK_ITER_HPS;
#endif

	// Allocate memory for the iterator
	RIG_LIST_ITER iter = rig_mem_alloc(sizeof(*iter), 0);
	NULLCHECK_ERRET(iter, ENOMEM, NULL);

	// Remember the data structure and increase the reference count
	iter->list = rig_list_newref(l);
	iter->last_skey = 0;
	iter->completed = false;

#if RIG_LIST_SMR_TYPE == 1
	// Set the last HP to a dummy value to ensure no other iterators can be
	// created before this one gets dismissed, to avoid chaos with the HPs.
	// NOTE: this does not prevent SMR from reclaiming memory, as an unaligned
	// address never appears there, seeing as atomic operations usually expect
	// and/or perform better on aligned addresses anyway.
	rig_smr_hp_set(hprec, 7, (void *)0x0101);
#endif

	return (iter);
}

/**
 * Iterate over a list and get a pointer to the next data item.
 * No guarantees are made about the availability of the content the returned
 * address points to. Memory management is the caller's responsibility.
 *
 * @param iter
 *     List iterator
 *
 * @return
 *     Pointer to data item, NULL on error.
 *     On error, the following error codes are set:
 *     - ENOENT (empty list, end reached)
 */
void *rig_list_iter_next(RIG_LIST_ITER iter) {
	NULLCHECK_EXIT(iter);

	if (iter->completed) {
		ERRET(ENOENT, NULL);
	}

	bool mark = false;
	KeyNode kprev = NULL, kcurr = NULL;
	Node prev = NULL, curr = NULL, succ = NULL;

#if RIG_LIST_SMR_TYPE == 1
	// Get pointer to this thread's RIG_SMR_HP_Record
	RIG_SMR_HP_Record hprec = rig_smr_hp_record_get();

	// We first need to get a valid KeyNode to work from ...
	if (rig_smr_hp_get(hprec, SMR_IHP_KEYN) == NULL) {
		kprev = iter->list->khead;
nextkeynode:
		kcurr = atomic_ops_ptr_load(&kprev->knext, ATOMIC_OPS_FENCE_NONE);

		rig_smr_hp_release(hprec, SMR_IHP_CURR);
		rig_smr_hp_release(hprec, SMR_IHP_PREV);

		while (kcurr != NULL) {
			if (atomic_ops_flagptr_load(&kcurr->next, NULL, ATOMIC_OPS_FENCE_ACQUIRE) != NULL) {
				break;
			}

			kprev = kcurr;
			kcurr = atomic_ops_ptr_load(&kcurr->knext, ATOMIC_OPS_FENCE_NONE);
		}

		// kcurr contains a protected KeyNode with a non-NULL sub-list now
		rig_smr_hp_set(hprec, SMR_IHP_KEYN, kcurr);

		// kcurr might be NULL, meaning we have an empty list
		if (kcurr == NULL) {
			iter->completed = true;
			ERRET(ENOENT, NULL);
		}
	}
	else {
		kcurr = rig_smr_hp_get(hprec, SMR_IHP_KEYN);
	}

	// ... And then get an actual Node from which to copy the needed data
	if (rig_smr_hp_get(hprec, SMR_IHP_CURR) == NULL) {
		iter->last_skey = 0;
retrykcurr:
		prev = (Node)kcurr;
	}
	else {
		prev = rig_smr_hp_get(hprec, SMR_IHP_CURR);
	}

	rig_smr_hp_set(hprec, SMR_IHP_PREV, prev);

	curr = atomic_ops_flagptr_load(&prev->next, NULL, ATOMIC_OPS_FENCE_ACQUIRE);
	// We don't care about the mark here, because prev, being a KeyNode,
	// will never be marked, or it possibly could be only if curr
	// is NULL, which anyway leads to hopping to the next KeyNode.
	// This holds even when prev is a node from SMR_IHP_PREV: if it is marked,
	// that will be discovered inside the loop and we'll restart from the
	// current KeyNode; unless curr is NULL, in which case we don't care if prev
	// is marked, as we just hop to the next KeyNode, as we have reached one
	// valid (at some point) end of the current sub-list.

	while (curr != NULL) {
		rig_smr_hp_set(hprec, SMR_IHP_CURR, curr);
		if (curr != atomic_ops_flagptr_load_full(&prev->next, NULL, ATOMIC_OPS_FENCE_ACQUIRE)) {
			goto retrykcurr;
		}

		succ = atomic_ops_flagptr_load(&curr->next, &mark, ATOMIC_OPS_FENCE_NONE);

		if (mark) {
			// Attempt physical removal
			if (!atomic_ops_flagptr_cas(&prev->next, curr, false, succ, false, ATOMIC_OPS_FENCE_FULL)) {
				goto retrykcurr;
			}

			rig_smr_hp_release(hprec, SMR_IHP_CURR);
			rig_smr_hp_mem_retire_noscan(curr);

			curr = succ;
		}
		else {
			if (curr->skey > iter->last_skey) {
				break;
			}

			prev = curr;
			rig_smr_hp_set(hprec, SMR_IHP_PREV, prev);
			curr = succ;
		}
	}

	// If the sub-list is empty, let's skip to the next KeyNode
	if (curr == NULL) {
		kprev = kcurr;
		goto nextkeynode;
	}

	// Save skey, so we know what to skip in the event of failure next time
	iter->last_skey = curr->skey;

	return (curr->data);
#else
	return (NULL);
#endif
}

/**
 * Delete the current element from the list.
 *
 * @param iter
 *     List iterator
 */
void rig_list_iter_delete(RIG_LIST_ITER iter) {
	NULLCHECK_EXIT(iter);

	if (iter->completed) {
		return;
	}

	bool mark = false;
	Node prev = NULL, curr = NULL, succ = NULL;

#if RIG_LIST_SMR_TYPE == 1
	// Get pointer to this thread's RIG_SMR_HP_Record
	RIG_SMR_HP_Record hprec = rig_smr_hp_record_get();

	if (rig_smr_hp_get(hprec, SMR_IHP_CURR) != NULL) {
		prev = rig_smr_hp_get(hprec, SMR_IHP_PREV);
		curr = rig_smr_hp_get(hprec, SMR_IHP_CURR);
retrysucc:
		succ = atomic_ops_flagptr_load(&curr->next, &mark, ATOMIC_OPS_FENCE_NONE);

		if (!mark) {
			if (atomic_ops_flagptr_cas(&curr->next, succ, false, succ, true, ATOMIC_OPS_FENCE_FULL)) {
				if (iter->list->count != NULL) { // Counting supported
					rig_acheck_msg(rig_counter_dec(iter->list->count),
						"removing non-counted node, this should never happen!");
				}

				if (atomic_ops_flagptr_cas(&prev->next, curr, false, succ, false, ATOMIC_OPS_FENCE_FULL)) {
					rig_smr_hp_release(hprec, SMR_IHP_CURR);
					rig_smr_hp_mem_retire_noscan(curr);
				}
			}
			else {
				goto retrysucc;
			}
		}

		rig_smr_hp_set(hprec, SMR_IHP_CURR, prev);
	}
#endif
}

/**
 * Destroy specified list iterator and set pointer to NULL.
 *
 * @param *iter
 *     pointer to list iterator
 */
void rig_list_iter_end(RIG_LIST_ITER *iter) {
	NULLCHECK_EXIT(iter);

	// If a valid pointer already contains NULL, nothing to do!
	if (*iter == NULL) {
		return;
	}

	// Decrease reference count of the data structure
	rig_list_destroy(&(*iter)->list);

	// Free iterator memory
	rig_mem_free(*iter);
	*iter = NULL;

#if RIG_LIST_SMR_TYPE == 1
	RESET_ITER_HPS;

	// Explicit SMR scan
	rig_smr_hp_mem_scan();
#endif
}


/**
 * Create a full copy of a lock-free list.
 *
 * @param l
 *     List to copy
 *
 * @return
 *     New list copy data, NULL on error.
 *     On error, the following error codes are set:
 *     - EALREADY (iterator already present, but required for copy!)
 *     - ENOMEM (insufficient memory)
 */
RIG_LIST rig_list_duplicate(RIG_LIST l) {
	NULLCHECK_EXIT(l);

	// Needed functions: ds_init, ds_destroy, ds_capacity,
	// ds_iter_begin, ds_iter_end, ds_iter_next, ds_push/put/add

	RIG_LIST dup_l = rig_list_init(rig_list_capacity(l), l->flags, l->cmp, l->hash);
	NULLCHECK_ERRET(dup_l, ENOMEM, NULL);

	RIG_LIST_ITER iter = rig_list_iter_begin(l);
	NULLCHECK_ERRET_CLEANUP(iter, errno, NULL, rig_list_destroy(&dup_l));

	struct array_stack tmp_stack;
	array_stack_init(&tmp_stack, sizeof(void *));

	void *ptr;

	while (((ptr = rig_list_iter_next(iter)) != NULL) && (array_stack_count(&tmp_stack) < rig_list_capacity(dup_l))) {
		array_stack_push(&tmp_stack, ptr);
	}

	rig_list_iter_end(&iter);

	while ((ptr = array_stack_pop(&tmp_stack)) != NULL) {
		if (!rig_list_add(dup_l, ptr)) {
			rig_acheck_msg(errno != EEXIST, "EEXIST should never happen during list duplication!");

			if (errno == ENOMEM) {
				rig_list_destroy(&dup_l);
				array_stack_destroy(&tmp_stack);

				ERRET(ENOMEM, NULL);
			}

			// Ignore EINVAL, impossible since we're getting from a valid ds!
			break; // Exit the loop on EXFULL, we're done!
		}
	}

	array_stack_destroy(&tmp_stack);

	return (dup_l);
}
