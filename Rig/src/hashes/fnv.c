/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: fnv.c 701 2010-08-11 07:21:27Z llongi $
 */

static inline uint64_t hash_fnv1a_64(const uint8_t *key, size_t len);
static inline uint32_t hash_fnv1a_32(const uint8_t *key, size_t len);

/**
 * Code adapted from:
 * "fnv-5.0.2, FNV-1a 32/64bit, by Fowler, Noll and Vo (June 2009, Public Domain)"
 * http://www.isthe.com/chongo/tech/comp/fnv/
 */

static inline uint64_t hash_fnv1a_64(const uint8_t *key, size_t len) {
	uint64_t hval = 0xCBF29CE484222325;

	for (size_t i = 0; i < len; i++) {
		/* xor the bottom with the current octet */
		hval ^= key[i];

		/* multiply by the 64 bit FNV magic prime mod 2^64 */
#if defined(NO_FNV_GCC_OPTIMIZATION)
		hval *= 0x0100000001B3;
#else
		hval += (hval << 1) + (hval << 4) + (hval << 5) + (hval << 7) + (hval << 8) + (hval << 40);
#endif
	}

	return (hval);
}

static inline uint32_t hash_fnv1a_32(const uint8_t *key, size_t len) {
	uint32_t hval = 0x811C9DC5;

	for (size_t i = 0; i < len; i++) {
		/* xor the bottom with the current octet */
		hval ^= key[i];

		/* multiply by the 32 bit FNV magic prime mod 2^32 */
#if defined(NO_FNV_GCC_OPTIMIZATION)
		hval *= 0x01000193;
#else
		hval += (hval << 1) + (hval << 4) + (hval << 7) + (hval << 8) + (hval << 24);
#endif
	}

	return (hval);
}
