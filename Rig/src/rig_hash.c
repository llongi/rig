/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: rig_hash.c 1110 2012-10-20 20:32:28Z llongi $
 */

#include "rig_internal.h"
#include "hashes/fnv.c"
#include "hashes/hsieh.c"
#include "hashes/jenkins.c"
#include "hashes/murmur2.c"
#include "hashes/murmur3.c"


/**
 * Generate a hash for a memory block, up to its specified length, while using
 * the indicated hashing algorithm.
 * These are not cryptographic hashing algorithms, don't use them for anything
 * requiring any kind of secure hash!
 * They are intended more for speed, to hash objects for use in data structures.
 * For classes of objects that have consistently the same, small size,
 * RIG_HASH_DIRECT may speed-up things a lot, by using the memory block
 * itself directly as the hash.
 *
 * @param *key
 *     Starting point of memory to hash, byte-sized, won't be modified, cannot be NULL
 * @param key_length
 *     Length of memory to hash, a length of zero will always return a hash of zero
 * @param hash_flags
 *     Hash algorithm specification, currently supported are:
 *     - RIG_HASH_DEFAULT (implementation-defined, fast and efficient choice)
 *     - RIG_HASH_JENKINS (lookup3 by Bob Jenkins)
 *     - RIG_HASH_FNV (FNV-1a by Fowler, Noll and Vo)
 *     - RIG_HASH_HSIEH (SuperFastHash by Paul Hsieh)
 *     - RIG_HASH_MURMUR2 (MurmurHash 2 by Austin Appleby)
 *     - RIG_HASH_MURMUR3 (MurmurHash 3 by Austin Appleby)
 *     Also available, mostly as fall-backs and for comparisons, are:
 *     - RIG_HASH_JENKINS_OAAT (One-at-a-time-hash by Bob Jenkins)
 *     - RIG_HASH_MURMUR2_OAAT (One-at-a-time-hash by Austin Appleby, Murmur2-based)
 *     Hash behavior specification, currently supported are:
 *     - RIG_HASH_DIRECT (for blocks smaller or equal than sizeof(size_t), use
 *       their value directly for the hash, without calling the full hash function)
 *
 * @return
 *     Hash value of the memory block
 */
size_t rig_hash(const uint8_t *key, size_t key_length, uint16_t hash_flags) {
	NULLCHECK_EXIT(key);

	size_t hash = 0;

	// Nothing to hash, return zero
	if (key_length == 0) {
		return (hash);
	}

	// Key smaller or equal to size_t, we can just use the bytes directly
	// to construct a unique hash value, if so desired
	if (TEST_BITFIELD(hash_flags, RIG_HASH_DIRECT)) {
		if (key_length < sizeof(size_t)) {
			uint8_t *hashp = (uint8_t *)&hash;

			for (size_t i = 0; i < key_length; i++) {
				hashp[i] = key[i];
			}

			return (hash);
		}

		if (key_length == sizeof(size_t)) {
			hash = (*((const size_t *)key));

			return (hash);
		}
	}

	// Key bigger than size_t, or no direct hashing allowed, we need to use
	// real hashing functions here
	switch (hash_flags & 0x0FFF) {
		case (RIG_HASH_JENKINS):
			hash = hash_jenkins_lku3_32(key, key_length);
			break;
		case (RIG_HASH_FNV):
#if SIZEOF_SIZE_T == 8
			hash = hash_fnv1a_64(key, key_length);
#else
			hash = hash_fnv1a_32(key, key_length);
#endif
			break;
		case (RIG_HASH_HSIEH):
			hash = hash_hsieh_32(key, key_length);
			break;
		case (RIG_HASH_MURMUR2):
#if SIZEOF_SIZE_T == 8
			hash = hash_murmur2_64(key, key_length);
#else
			hash = hash_murmur2_32(key, key_length);
#endif
			break;
		case (RIG_HASH_MURMUR3):
#if SIZEOF_SIZE_T == 8
			hash = hash_murmur3_64(key, key_length);
#else
			hash = hash_murmur3_32(key, key_length);
#endif
			break;
		case (RIG_HASH_JENKINS_OAAT):
			hash = hash_jenkins_oaat_32(key, key_length);
			break;
		case (RIG_HASH_MURMUR2_OAAT):
			hash = hash_murmur2_oaat_32(key, key_length);
			break;
		default:
#if SIZEOF_SIZE_T == 8
			hash = hash_murmur3_64(key, key_length);
#else
			hash = hash_murmur3_32(key, key_length);
#endif
			break;
	}

	return (hash);
}
