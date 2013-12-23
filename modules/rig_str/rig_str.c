/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_str.c 1005 2011-09-14 17:47:02Z llongi $
 */

#include "rig_internal.h"
#include "str_ops/str_ops.c"
#include "str_ops/str_ops_search.c"
#include "str_ops/str_ops_utf8.c"

/*
 * Rig String Data Definitions
 */

static inline RIG internal_str_init(size_t maxlen, uint16_t flags, const uint8_t *s, size_t s_len);
static inline bool internal_str_ncpy(RIG dest, size_t dest_pos, CONST_RIG src, size_t len);

#define UTF8_LEN_CONV(s, len) if ((TEST_BITFIELD(FLAGS(s), RIG_STR_UTF8)) && (len != 0)) len = str_ops_strlen(STR_POS(s), len, true, false)
#define LEN_CONV(s, len) if ((len == 0) || (LEN_POS(s) < len)) len = LEN_POS(s)

#define RIG_STR_WROPEN ((uint16_t)(1 << 15))


/*
 * Rig String Implementation
 */

/**
 * Allocate a Rig string, specifying its maximum length.
 *
 * @param maxlen
 *     maximum possible length of the Rig string.
 *     Can be between 1 and SSIZE_MAX.
 *
 * @param flags
 *     flags to control/modify Rig string behavior:
 *     RIG_STR_THREADSAFE - makes the string thread-safe
 *     RIG_STR_BINARY - marks the string as a binary string (supporting embedded NULLs)
 *     RIG_STR_UTF8 - changes the way position operates and the way length limits are
 *     interpreted to correctly support UTF-8 in strings
 *
 * @return
 *     pointer to a Rig string.
 *     NULL if an error occurred, setting errno to:
 *         EINVAL - invalid value(s) passed as parameter(s)
 *         ENOMEM - memory allocation for the string failed
 *         ENOLCK - failed to initialize the lock (for thread-safety)
 */
RIG rig_str_init(size_t maxlen, uint16_t flags) {
	// Parameter validation (partial, together with rig_str_init_from_nstr)
	if (maxlen == 0) {
		ERRET(EINVAL, NULL);
	}

	return (rig_str_init_from_nstr(maxlen, flags, NULL, 0));
}

/**
 * Allocate a Rig string, with the ability of specifying its maximum length,
 * using a normal C string as initializer, which will be copied in its entirety
 * or not at all (see RIG_STR_EXTERNAL flag).
 *
 * @param maxlen
 *     maximum possible length of the Rig string.
 *     Can be between 0 and SSIZE_MAX.
 *     0 means to compute the maximum length from the length of
 *     the normal C string pointed to by the third parameter
 *     (up to a maximum length of SSIZE_MAX),
 *     which may not be NULL in this case or point to an empty string.
 *
 * @param flags
 *     flags to control/modify Rig string behavior:
 *     RIG_STR_THREADSAFE - makes the string thread-safe
 *     RIG_STR_BINARY - marks the string as a binary string (supporting embedded NULLs)
 *     RIG_STR_UTF8 - changes the way position operates and the way length limits are
 *     interpreted to correctly support UTF-8 in strings
 *     RIG_STR_IMMUTABLE - makes the string immutable
 *     RIG_STR_EXTERNAL - will use the external string passed as parameter directly,
 *     with no further copying of it done (careful about its persistence though!)
 *
 * @param *s
 *     pointer to a normal C string used to initialize the Rig string.
 *     NULL will initialize an empty Rig string.
 *
 * @return
 *     pointer to a Rig string.
 *     NULL if an error occurred, setting errno to:
 *         EINVAL - invalid value(s) passed as parameter(s)
 *         ENOMEM - memory allocation for the string failed
 *         ENOLCK - failed to initialize the lock (for thread-safety)
 */
RIG rig_str_init_from_str(size_t maxlen, uint16_t flags, const char *s) {
	// Parameter validation done in rig_str_init_from_nstr
	return (rig_str_init_from_nstr(maxlen, flags, s, 0));
}

/**
 * Allocate a Rig string, with the ability of specifying its maximum length,
 * using a normal C string as initializer, which will be copied in its entirety,
 * just up to a user-defined length, or not at all (see RIG_STR_EXTERNAL flag).
 *
 * @param maxlen
 *     maximum possible length of the Rig string.
 *     Can be between 0 and SSIZE_MAX.
 *     0 means to compute the maximum length from the length of
 *     the normal C string pointed to by the third parameter
 *     (up to a maximum length of SSIZE_MAX),
 *     which may not be NULL in this case or point to an empty string.
 *
 * @param flags
 *     flags to control/modify Rig string behavior:
 *     RIG_STR_THREADSAFE - makes the string thread-safe
 *     RIG_STR_BINARY - marks the string as a binary string (supporting embedded NULLs)
 *     RIG_STR_UTF8 - changes the way position operates and the way length limits are
 *     interpreted to correctly support UTF-8 in strings
 *     RIG_STR_IMMUTABLE - makes the string immutable
 *     RIG_STR_EXTERNAL - will use the external string passed as parameter directly,
 *     with no further copying of it done (careful about its persistence though!)
 *
 * @param *s
 *     pointer to a normal C string used to initialize the Rig string.
 *     NULL will initialize an empty Rig string.
 *
 * @param slen
 *     maximum length that shall be copied from the string pointed to by *s.
 *
 * @return
 *     pointer to a Rig string.
 *     NULL if an error occurred, setting errno to:
 *         EINVAL - invalid value(s) passed as parameter(s)
 *         ENOMEM - memory allocation for the string failed
 *         ENOLCK - failed to initialize the lock (for thread-safety)
 */
RIG rig_str_init_from_nstr(size_t maxlen, uint16_t flags, const char *s, size_t s_len) {
	// Parameter validation and flags tests
	CHECK_PERMITTED_FLAGS(flags, RIG_STR_BINARY | RIG_STR_EXTERNAL | RIG_STR_IMMUTABLE | RIG_STR_THREADSAFE | RIG_STR_UTF8);

	if ((maxlen > SSIZE_MAX) // maxlen and slen must be between 0 and SSIZE_MAX
	|| (s_len > SSIZE_MAX) // them being unsigned integer plus those checks ensure it
	|| ((maxlen == 0) && (s == NULL)) // if maxlen 0, s must be defined, as we get the length from there
	|| ((s == NULL) && (s_len != 0)) // if s NULL, slen's only logical and valid value is 0
	|| ((maxlen != 0) && (s_len > maxlen)) // if maxlen is not 0, it has to be bigger than slen
	|| ((TEST_BITFIELD(flags, RIG_STR_BINARY)) && (TEST_BITFIELD(flags, RIG_STR_UTF8))) // a string can't have both binary and UTF-8 content
	|| ((TEST_BITFIELD(flags, RIG_STR_BINARY)) && (s != NULL) && (s_len == 0)) // if we pass a binary string, we must also pass the length
	|| ((TEST_BITFIELD(flags, RIG_STR_IMMUTABLE)) && (s == NULL)) // it doesn't make sense to create an immutable string with no content at all
	|| ((TEST_BITFIELD(flags, RIG_STR_IMMUTABLE)) && (maxlen != 0)) // with maxlen == 0, we can shrink it to the minimum size, based on string input
	|| ((TEST_BITFIELD(flags, RIG_STR_EXTERNAL)) && (s == NULL)) // this means to directly use s as the string content, so s must exist
	|| ((TEST_BITFIELD(flags, RIG_STR_EXTERNAL)) && (maxlen != 0))) { // with maxlen == 0, we can shrink it to the minimum size, based on string input
		ERRET(EINVAL, NULL);
	}

	// Calculate the maximum length of the input string, if present
	size_t str_len = 0;

	if (s != NULL) {
		if (TEST_BITFIELD(flags, RIG_STR_BINARY)) {
			// For binary strings, we must trust the user
			str_len = s_len;
		}
		else {
			// For normal strings, we reliably use NULLs as stops
			if (TEST_BITFIELD(flags, RIG_STR_UTF8)) {
				str_len = str_ops_strlen((const uint8_t *)s, s_len, true, false);
			}
			else {
				str_len = str_ops_strlen((const uint8_t *)s, s_len, false, false);
			}
		}

		// Forbid empty strings from being passed
		if (str_len == 0) {
			ERRET(EINVAL, NULL);
		}
	}

	// Set maximum length to string length, if so desired
	if (maxlen == 0) {
		maxlen = str_len;
	}

	return (internal_str_init(maxlen, flags, (const uint8_t *)s, str_len));
}

/**
 * Duplicate a Rig string.
 *
 * @param s
 *     pointer to a Rig string.
 *
 * @param flags
 *     flags to control/modify Rig string behavior:
 *     RIG_STR_THREADSAFE - makes the string thread-safe
 *     RIG_STR_BINARY - marks the string as a binary string (supporting embedded NULLs)
 *     RIG_STR_UTF8 - changes the way position operates and the way length limits are
 *     interpreted to correctly support UTF-8 in strings
 *     RIG_STR_IMMUTABLE - makes the string immutable
 *     RIG_STR_EXTERNAL - will use the external string passed as parameter directly,
 *     with no further copying of it done (careful about its persistence though!),
 *     this only works if the Rig string we copy from was already EXTERNAL
 *     Note that we support also creating a non-BINARY Rig string from one that's
 *     declared as BINARY, in this case we'll only copy up to the first NULL byte
 *     contained in the BINARY string and adjust string length accordingly.
 *
 * @return
 *     pointer to a new Rig string,
 *     with same content as the one passed as parameter.
 *     NULL if an error occurred, setting errno to:
 *         EINVAL - invalid value(s) passed as parameter(s)
 *         ENOMEM - memory allocation for the string failed
 *         ENOLCK - failed to initialize the lock (for thread-safety)
 */
RIG rig_str_duplicate(CONST_RIG s, uint16_t flags) {
	// Parameter validation
	NULLCHECK_EXIT(s);
	CHECK_PERMITTED_FLAGS(flags, RIG_STR_BINARY | RIG_STR_EXTERNAL | RIG_STR_IMMUTABLE | RIG_STR_THREADSAFE | RIG_STR_UTF8);

	RW_RDLOCK(s);

	// Forbid going from non-EXTERNAL to EXTERNAL
	if ((!TEST_BITFIELD(FLAGS(s), RIG_STR_EXTERNAL)) && (TEST_BITFIELD(flags, RIG_STR_EXTERNAL))) {
		RW_UNLOCK(s);
		ERRET(EINVAL, NULL);
	}

	size_t str_len = 0;

	if ((TEST_BITFIELD(FLAGS(s), RIG_STR_BINARY)) && (!TEST_BITFIELD(flags, RIG_STR_BINARY))) {
		// In the case of going from a BINARY to a non-BINARY string, calculate
		// the length to copy based on where the first NULL byte is
		str_len = str_ops_strlen(STR(s), LEN(s), false, false);
	}
	else {
		// Copy the full string
		str_len = LEN(s);
	}

	RIG str = internal_str_init(MAXLEN(s), flags, STR(s), str_len);

	RW_UNLOCK(s);

	return (str);
}

/**
 * Create a new reference to a Rig string.
 *
 * @param s
 *     pointer to a Rig string.
 *
 * @return
 *     pointer to the same Rig string.
 *     false if an error occurred, setting errno to:
 *         EINVAL - invalid value(s) passed as parameter(s)
 */
RIG rig_str_newref(RIG s) {
	// Parameter validation
	NULLCHECK_EXIT(s);

	// Increment the reference count by one
	if (REFCOUNT(s) != NULL) {
		rig_acheck_msg(rig_counter_inc(REFCOUNT(s)),
			"more references than physically possible, this should never happen!");
	}

	// Maybe return NULL when we're trying to reference a literal?

	return (s);
}

/**
 * Deallocate a Rig string.
 * Set the pointer to NULL and eventually free its memory.
 *
 * @param *s
 *     address of a pointer to a Rig string.
 *
 * @return
 *     boolean value.
 *     false if an error occurred, setting errno to:
 *         EINVAL - invalid value(s) passed as parameter(s)
 */
bool rig_str_destroy(RIG *s) {
	// Parameter validation
	NULLCHECK_EXIT(s);
	NULLCHECK_EXIT(*s);

	// TODO: We may need to check if this is a literal, and not allow freeing?
	// This may break if called on a RIG_LIT, but that's wrong API usage anyway

	// Decrement reference count and test, if zero:
	// destroy lock, deallocate memory and set pointer to NULL
	if (rig_counter_dec_and_test(REFCOUNT(*s))) {
		if (LOCK(*s) != NULL) {
			rig_mrwlock_destroy(&LOCK(*s));
		}

		// Do not free external strings, we don't manage their memory
		if (!TEST_BITFIELD(FLAGS(*s), RIG_STR_EXTERNAL)) {
			rig_mem_free(STR(*s));
		}

		rig_counter_destroy(&REFCOUNT(*s));
		rig_mem_free(*s);
	}
	else {
		rig_acheck_msg(errno == 0,
			"reference count already zero, uncounted references exist!");
	}

	*s = NULL;

	return (true);
}

/**
 * Change the maximum length of a Rig string.
 *
 * @param *s
 *     address of a pointer to a Rig string.
 *
 * @param maxlen
 *     new maximum length of the Rig string.
 *     Can be between 0 and SSIZE_MAX.
 *     0 means to set the maximum length to the actual
 *     length of the Rig string (minimum being a length of 1).
 *
 * @return
 *     boolean value.
 *     false if an error occurred, setting errno to:
 *         EINVAL - invalid value(s) passed as parameter(s)
 *         ENOMEM - memory reallocation for the string failed
 *         EALREADY - the new maxlen corresponds to the old one
 */
bool rig_str_resize(RIG s, size_t new_maxlen) {
	// Parameter validation
	NULLCHECK_EXIT(s);

	if (new_maxlen > SSIZE_MAX) {
		ERRET(EINVAL, false);
	}

	// Disallow resize on external strings, we don't manage their memory
	if (TEST_BITFIELD(FLAGS(s), RIG_STR_EXTERNAL)) {
		ERRET(EINVAL, false);
	}

	RW_WRLOCK(s);

	// Set maximum length to the string's actual length,
	// but no less than a length of 1
	if (new_maxlen == 0) {
		new_maxlen = (LEN(s) >= 1) ? (LEN(s)) : (1);
	}

	// If the new max length corresponds to what is already set,
	// we have nothing to do, so we return EALREADY
	if (new_maxlen == MAXLEN(s)) {
		RW_UNLOCK(s);
		ERRET(EALREADY, false);
	}
	else {
		// Reallocate the memory to the specified size
		uint8_t *str = rig_mem_realloc(STR(s), 1, new_maxlen);
		if (str == NULL) {
			RW_UNLOCK(s);
			ERRET(ENOMEM, false);
		}

		// Update the pointer to the Rig string
		STR(s) = str;

		// Reset the needed values
		MAXLEN(s) = new_maxlen;
		POS(s) = 0; // ALWAYS BE AWARE OF THIS !!!

		if (new_maxlen < LEN(s)) {
			// We need to truncate the string and reset the length data in case
			// the new maximum length is actually smaller than the old string length
			LEN(s) = new_maxlen;

			// Truncate with a NULL character for compatibility reasons
			STR(s)[LEN(s)] = '\0';
		}
	}

	RW_UNLOCK(s);

	return (true);
}

/**
 * Reset a Rig string to its empty state.
 *
 * @param s
 *     pointer to a Rig string.
 *
 * @return
 *     boolean value.
 *     false if an error occurred, setting errno to:
 *         EINVAL - invalid value(s) passed as parameter(s)
 */
bool rig_str_reset(RIG s) {
	// Parameter validation
	NULLCHECK_EXIT(s);

	RW_WRLOCK(s);

	// Reset values to empty
	LEN(s) = 0;
	POS(s) = 0; // ALWAYS BE AWARE OF THIS !!!
	STR(s)[0] = '\0'; // Truncate with a NULL character for compatibility reasons

	RW_UNLOCK(s);

	return (true);
}

/**
 * Print debug info on a specified Rig string.
 *
 * @param s
 *     pointer to a Rig string.
 *
 * @return
 *     boolean value.
 *     false if an error occurred, setting errno to:
 *         EINVAL - invalid value(s) passed as parameter(s)
 */
bool rig_str_debuginfo(CONST_RIG s) {
	// Parameter validation
	NULLCHECK_EXIT(s);

	RW_RDLOCK(s);

	// Print out the debug info
	/*printf("=== Structure rig_str (at address %p) ===\n" \
		"maxlen: %zu\n" \
		"len: %zu\n" \
		"pos: %zu\n" \
		"refcount: %zu\n" \
		"flags: %s, %s, %s, %s, %s\n" \
		"str: %s (at address %p)\n" \
		"lock: %s (at address %p)\n" \
		"(using %zu (struct) + %zu (string) = %zu bytes)\n\n",
		(const void *)s,
		MAXLEN(s),
		LEN(s),
		POS(s),
		(REFCOUNT(s) != NULL) ? (rig_counter_get(REFCOUNT(s))) : (0),
		(TEST_BITFIELD(FLAGS(s), RIG_STR_THREADSAFE)) ? ("THREADSAFE") : ("-"),
		(TEST_BITFIELD(FLAGS(s), RIG_STR_BINARY)) ? ("BINARY") : ("-"),
		(TEST_BITFIELD(FLAGS(s), RIG_STR_UTF8)) ? ("UTF8") : ("-"),
		(TEST_BITFIELD(FLAGS(s), RIG_STR_IMMUTABLE)) ? ("IMMUTABLE") : ("-"),
		(TEST_BITFIELD(FLAGS(s), RIG_STR_EXTERNAL)) ? ("EXTERNAL") : ("-"),
		STR(s), (void *)STR(s),
		(LOCK(s) == NULL) ? ("disabled") : ("enabled"), (void *)LOCK(s),
		sizeof(*s), MAXLEN(s) + 1,
		sizeof(*s) + MAXLEN(s) + 1);*/

	RW_UNLOCK(s);

	return (true);
}

/**
 * Return the maximum length for a Rig string on this system.
 *
 * @return
 *     maximum length for a Rig string on this system.
 */
ssize_t rig_str_maxlen(void) {
	return (SSIZE_MAX);
}

/**
 * Get a read-only pointer to the Rig string content.
 * Will take position into account and point to the right character.
 * You can use this for compatibility with external functions
 * which expect a normal char pointer.
 * Use this ONLY if the string is not modified by the external function.
 *
 * @param s
 *     pointer to a Rig string.
 *
 * @return
 *     read-only pointer to the string content.
 *     NULL if an error occurred, setting errno to:
 *         EINVAL - invalid value(s) passed as parameter(s)
 */
const char *rig_str_getstr_ro(CONST_RIG s) {
	// Parameter validation
	NULLCHECK_EXIT(s);

	RW_RDLOCK(s);

	// Return read-only pointer to string content
	const char *strp = (const char *)STR(s);
	return (strp + POS(s));
}

/**
 * Get a pointer to the Rig string content.
 * Will take position into account and point to the right character.
 * You can use this for compatibility with external functions
 * which expect a normal char pointer.
 * Use this if the string is modified in any way.
 * Remember the string has to be NULL-terminated.
 *
 * @param s
 *     pointer to a Rig string.
 *
 * @return
 *     pointer to the string content.
 *     NULL if an error occurred, setting errno to:
 *         EINVAL - invalid value(s) passed as parameter(s)
 */
char *rig_str_getstr_rw(RIG s) {
	// Parameter validation
	NULLCHECK_EXIT(s);

	RW_WRLOCK(s);

	// Set "open for write" flag
	SET_BITFIELD(FLAGS(s), RIG_STR_WROPEN);

	// Return pointer to string content
	char *strp = (char *)STR(s);
	return (strp + POS(s));
}

bool rig_str_returnstr(RIG s, size_t len) {
	// Parameter validation
	NULLCHECK_EXIT(s);

	if (TEST_BITFIELD(FLAGS(s), RIG_STR_WROPEN)) {
		// Recover to acceptable values if len is too big
		if ((len != 0) && ((len + POS(s)) > MAXLEN(s))) {
			len = MAXLEN(s) - POS(s);
		}

		// "open for write" flag was set, let's assume the string was changed
		// and rebuild its length data
		if (TEST_BITFIELD(FLAGS(s), RIG_STR_BINARY)) {
			// For binary strings, if len is zero, the length
			// doesn't change, if it's non zero, it does
			if (len != 0) {
				LEN(s) = len + POS(s);
			}
		}
		else {
			// For normal strings, we can calculate their length using
			// NULL characters as stops, which is completely reliable, if the
			// user requests it by passing zero as len, else we trust the user
			if (len != 0) {
				LEN(s) = len + POS(s);
			}
			else {
				LEN(s) = str_ops_strlen(STR_POS(s), MAXLEN_POS(s), false, false) + POS(s);
			}
		}

		// Always terminate with a NULL character for compatibility reasons
		STR(s)[LEN(s)] = '\0';

		// Reset "open for write" flag
		RESET_BITFIELD(FLAGS(s), RIG_STR_WROPEN);
	}

	RW_UNLOCK(s);

	return (true);
}

/**
 * Set the position at which we operate in the string.
 * This is a cursor indicating where we are in the string.
 * This is always 0 unless you set it yourself using this function.
 * Also remember that a call to rig_str_duplicate(),
 * rig_str_resize() or rig_str_reset() will reset it to 0.
 *
 * @param s
 *     pointer to a Rig string.
 */
bool rig_str_setpos(RIG s, ssize_t pos) {
	// Parameter validation
	NULLCHECK_EXIT(s);

	RW_WRLOCK(s);

	// UTF-8 offset support
	if ((TEST_BITFIELD(FLAGS(s), RIG_STR_UTF8)) && (pos > 0)) {
		pos = (ssize_t)str_ops_strlen(STR(s), (size_t)pos, true, false);
	} // TODO: need negative offset support here too

	// Support negative offsets
	if (pos < 0) {
		pos += (ssize_t)LEN(s);
	}

	// Offset validation
	if (pos < 0 || pos > (ssize_t)LEN(s)) {
		RW_UNLOCK(s);
		ERRET(EINVAL, false);
	}

	POS(s) = (size_t)pos;

	RW_UNLOCK(s);

	return (true);
}

bool rig_str_setpos_ptr(RIG s, uintptr_t pos_ptr) {
	// Parameter validation
	NULLCHECK_EXIT(s);

	RW_WRLOCK(s);

	size_t pos = pos_ptr - (uintptr_t)STR(s);

	if (pos > LEN(s)) {
		RW_UNLOCK(s);
		ERRET(EINVAL, false);
	}

	POS(s) = pos;

	RW_UNLOCK(s);

	return (true);
}

ssize_t rig_str_getpos(CONST_RIG s) {
	// Parameter validation
	NULLCHECK_EXIT(s);

	// Return position
	RW_RDLOCK(s);

	size_t pos = POS(s);

	if ((TEST_BITFIELD(FLAGS(s), RIG_STR_UTF8)) && (pos != 0)) {
		pos = str_ops_strlen(STR(s), pos, false, true);
	}

	RW_UNLOCK(s);

	return ((ssize_t)pos);
}

/**
 * Return the maximum length this string can reach.
 * Takes current position into account.
 * You can use this for compatibility with external functions.
 *
 * @param s
 *     pointer to a Rig string.
 *
 * @return
 *     maximum length of the string (integer).
 *     0 if an error occurred, setting errno to:
 *         EINVAL - invalid value(s) passed as parameter(s)
 */
ssize_t rig_str_getmaxlen(CONST_RIG s) {
	// Parameter validation
	NULLCHECK_EXIT(s);

	// Return maximum length
	RW_RDLOCK(s);
	size_t maxlen = MAXLEN_POS(s);
	RW_UNLOCK(s);

	return ((ssize_t)maxlen);
}

/**
 * Return the actual length of this string.
 * Takes current position into account.
 * You can use this for compatibility with external functions.
 *
 * @param s
 *     pointer to a Rig string.
 *
 * @return
 *     actual length of the string (integer).
 *     0 if an error occurred, setting errno to:
 *         EINVAL - invalid value(s) passed as parameter(s)
 */
ssize_t rig_str_getlen(CONST_RIG s) {
	// Parameter validation
	NULLCHECK_EXIT(s);

	// Return actual length
	RW_RDLOCK(s);
	size_t len = LEN_POS(s);
	RW_UNLOCK(s);

	return ((ssize_t)len);
}

bool rig_str_nappend(RIG dest, CONST_RIG src, size_t len) {
	// Parameter validation
	NULLCHECK_EXIT(dest);
	NULLCHECK_EXIT(src);

	RW_RDLOCK(src);

	UTF8_LEN_CONV(src, len);
	LEN_CONV(src, len);

	RW_WRLOCK(dest);

	bool ret = internal_str_ncpy(dest, dest->len, src, len);

	RW_UNLOCK(dest);
	RW_UNLOCK(src);

	return (ret);
}

bool rig_str_nprepend(RIG dest, CONST_RIG src, size_t len) {
	// Parameter validation
	NULLCHECK_EXIT(dest);
	NULLCHECK_EXIT(src);

	RW_RDLOCK(src);

	UTF8_LEN_CONV(src, len);
	LEN_CONV(src, len);

	RW_WRLOCK(dest);

	// Check if we have enough space left in dest
	if (len > (dest->maxlen - dest->len)) {
		RW_UNLOCK(dest);
		RW_UNLOCK(src);
		ERRET(EINVAL, false);
	}

	// Move already present content in dest, then copy src over
	str_ops_copy(dest->str + len, dest->str, dest->len);
	str_ops_copy(dest->str, STR_POS(src), len);

	dest->len += len;

	// Always terminate with a NULL character for compatibility reasons
	dest->str[dest->len] = '\0';

	RW_UNLOCK(dest);
	RW_UNLOCK(src);

	return (true);
}

bool rig_str_ncopy(RIG dest, CONST_RIG src, size_t len) {
	// Parameter validation
	NULLCHECK_EXIT(dest);
	NULLCHECK_EXIT(src);

	RW_RDLOCK(src);

	UTF8_LEN_CONV(src, len);
	LEN_CONV(src, len);

	RW_WRLOCK(dest);

	bool ret = internal_str_ncpy(dest, dest->pos, src, len);

	RW_UNLOCK(dest);
	RW_UNLOCK(src);

	return (ret);
}

ssize_t rig_str_npcopy(RIG dest, CONST_RIG src, size_t len) {
	// Parameter validation
	NULLCHECK_EXIT(dest);
	NULLCHECK_EXIT(src);

	RW_RDLOCK(src);

	UTF8_LEN_CONV(src, len);
	LEN_CONV(src, len);

	RW_WRLOCK(dest);

	ssize_t ret = -1;

	if (internal_str_ncpy(dest, dest->pos, src, len)) {
		ret = (ssize_t)dest->len;
	}

	RW_UNLOCK(dest);
	RW_UNLOCK(src);

	return (ret);
}

ssize_t rig_str_chr(CONST_RIG s, uint32_t c) {
	// Parameter validation
	NULLCHECK_EXIT(s);

	uint8_t clen = str_ops_utf8_charlen(c);

	if (clen == 0) {
		ERRET(EINVAL, -1);
	}

	// Build the little string to search for correctly, avoiding endianness issues
	uint8_t cp[4];

	if (clen > 1) {
		cp[0] = (uint8_t)((c & 0xFF000000) >> 24);
		cp[1] = (uint8_t)((c & 0x00FF0000) >> 16);
		cp[2] = (uint8_t)((c & 0x0000FF00) >> 8);
		cp[3] = (uint8_t)((c & 0x000000FF));
	}

	RW_RDLOCK(s);

	ssize_t pos;

	if (clen == 1) {
		pos = str_ops_search_byte(STR_POS(s), LEN_POS(s), (const uint8_t)c, MODE_SEARCH, NULL);
	}
	else {
		struct str_ops_search_multibyte_prep mp;
		str_ops_search_multibyte_prep(&mp, cp + (4 - clen), clen);
		pos = str_ops_search_multibyte(&mp, STR_POS(s), LEN_POS(s), MODE_SEARCH, NULL);
	}

	RW_UNLOCK(s);

	return (pos);
}

ssize_t rig_str_rchr(CONST_RIG s, uint32_t c) {
	// Parameter validation
	NULLCHECK_EXIT(s);

	uint8_t clen = str_ops_utf8_charlen(c);

	if (clen == 0) {
		ERRET(EINVAL, -1);
	}

	// Build the little string to search for correctly, avoiding endianness issues
	uint8_t cp[4];

	if (clen > 1) {
		cp[0] = (uint8_t)((c & 0xFF000000) >> 24);
		cp[1] = (uint8_t)((c & 0x00FF0000) >> 16);
		cp[2] = (uint8_t)((c & 0x0000FF00) >> 8);
		cp[3] = (uint8_t)((c & 0x000000FF));
	}

	RW_RDLOCK(s);

	ssize_t pos;

	if (clen == 1) {
		pos = str_ops_search_byte_reverse(STR_POS(s), LEN_POS(s), (const uint8_t)c, MODE_SEARCH, NULL);
	}
	else {
		struct str_ops_search_multibyte_reverse_prep mrp;
		str_ops_search_multibyte_reverse_prep(&mrp, cp + (4 - clen), clen);
		pos = str_ops_search_multibyte_reverse(&mrp, STR_POS(s), LEN_POS(s), MODE_SEARCH, NULL);
	}

	RW_UNLOCK(s);

	return (pos);
}

ssize_t rig_str_str(CONST_RIG haystack, CONST_RIG needle) {

}

ssize_t rig_str_rstr(CONST_RIG haystack, CONST_RIG needle) {

}

ssize_t rig_str_casestr(CONST_RIG haystack, CONST_RIG needle) {

}

ssize_t rig_str_rcasestr(CONST_RIG haystack, CONST_RIG needle) {

}

ssize_t rig_str_pbrk(CONST_RIG s, CONST_RIG accept) {

}

ssize_t rig_str_cpbrk(CONST_RIG s, CONST_RIG reject) {

}

ssize_t rig_str_spn(CONST_RIG s, CONST_RIG accept) {

}

ssize_t rig_str_cspn(CONST_RIG s, CONST_RIG reject) {

}

bool rig_str_strip(RIG s, CONST_RIG strip) {

}

bool rig_str_lstrip(RIG s, CONST_RIG strip) {

}

bool rig_str_rstrip(RIG s, CONST_RIG strip) {

}

bool rig_str_nset(RIG s, uint8_t c, size_t len) {
	// Parameter validation
	NULLCHECK_EXIT(s);

	RW_WRLOCK(s);

	UTF8_LEN_CONV(s, len);
	LEN_CONV(s, len);

	str_ops_set(STR_POS(s), c, len);

	RW_UNLOCK(s);

	return (true);
}

bool rig_str_flatten(RIG s) {

}

bool rig_str_reverse(RIG s) {

}

bool rig_str_replace(RIG s, CONST_RIG find, CONST_RIG replace) {

}

bool rig_str_replace_all(RIG s, CONST_RIG find, CONST_RIG replace) {

}

RIG *rig_str_split(CONST_RIG s, uint32_t c) {

}

bool rig_str_join(RIG s, uint32_t c, RIG *strings) {

}

ssize_t rig_str_nlen(CONST_RIG s, size_t len) {

}

int rig_str_ncmp(CONST_RIG s1,CONST_RIG s2, size_t len) {
	// Comparison functions have no parameter validation, as there
	// is no clear way to return a useful error in a useful way,
	// still, to not fail on NULL, we treat it as a special case
	if (s1 == NULL && s2 == NULL) return (0);
	if (s1 != NULL && s2 == NULL) return (1);
	if (s1 == NULL && s2 != NULL) return (-1);

	RW_RDLOCK(s1);
	RW_RDLOCK(s2);

	UTF8_LEN_CONV(s1, len);

	// Compute the maximal length we may have to compare for each string
	size_t s1ltc = LEN_POS(s1);
	size_t s2ltc = LEN_POS(s2);

	if (len != 0) {
		s1ltc = (len > s1ltc) ? (s1ltc) : (len);
		s2ltc = (len > s2ltc) ? (s2ltc) : (len);
	}

	// If the length of the two strings to compare doesn't match,
	// they can't be equal and we can return the appropriate value,
	// else we'll need to compare byte-wise
	int ret;

	if (s1ltc != s2ltc) {
		ret = (s1ltc > s2ltc) ? (1) : (-1);
	}
	else {
		ret = str_ops_cmp(STR_POS(s1), STR_POS(s2), s1ltc);
	}

	RW_UNLOCK(s2);
	RW_UNLOCK(s1);

	return (ret);
}

int rig_str_ncasecmp(CONST_RIG s1, CONST_RIG s2, size_t len) {

}

/** Internal functions implementation */

static inline RIG internal_str_init(size_t maxlen, uint16_t flags, const uint8_t *s, size_t s_len) {
	// Allocate memory for the Rig struct
	RIG str = rig_mem_alloc(sizeof(*str), 0);
	NULLCHECK_ERRET(str, ENOMEM, NULL);

	// Check if we want to be thread-safe or not
	if (TEST_BITFIELD(flags, RIG_STR_THREADSAFE)) {
		// Initialize the lock
		LOCK(str) = rig_mrwlock_init(0);
		if (LOCK(str) == NULL) {
			rig_mem_free(str);
			ERRET(ENOLCK, NULL);
		}
	}
	else {
		// Set the lock to NULL if we need no thread-safety
		LOCK(str) = NULL;
	}

	// Set the needed values
	MAXLEN(str) = maxlen;
	LEN(str) = s_len;
	POS(str) = 0; // ALWAYS BE AWARE OF THIS !!!
	REFCOUNT(str) = rig_counter_init(1, 0);
	if (REFCOUNT(str) == NULL) {
		if (LOCK(str) != NULL) {
			rig_mrwlock_destroy(&(LOCK(str)));
		}
		rig_mem_free(str);
		ERRET(ENOMEM, NULL);
	}
	FLAGS(str) = flags;

	if (TEST_BITFIELD(flags, RIG_STR_EXTERNAL)) {
		// Set the string to the input directly
		STR(str) = (uint8_t *)s;
	}
	else {
		// Allocate memory for the Rig string
		STR(str) = rig_mem_alloc(1, maxlen);
		if (STR(str) == NULL) {
			if (LOCK(str) != NULL) {
				rig_mrwlock_destroy(&(LOCK(str)));
			}
			rig_mem_free(str);
			ERRET(ENOMEM, NULL);
		}

		if (s != NULL) {
			str_ops_copy(STR(str), s, s_len);
		}

		// Always terminate with a NULL character for compatibility reasons
		STR(str)[LEN(str)] = '\0';
	}

	return (str);
}

static inline bool internal_str_ncpy(RIG dest, size_t dest_pos, CONST_RIG src, size_t len) {
	// Check if we have enough space left in dest
	if (len > (dest->maxlen - dest_pos)) {
		ERRET(EINVAL, false);
	}

	dest->len = dest_pos + len;

	str_ops_copy(dest->str + dest_pos, STR_POS(src), len);

	// Always terminate with a NULL character for compatibility reasons
	dest->str[dest->len] = '\0';

	return (true);
}
