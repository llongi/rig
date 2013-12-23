/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: hsieh.c 1031 2012-05-21 08:01:00Z llongi $
 */

static inline uint32_t hash_hsieh_32(const uint8_t *key, size_t len);

/**
 * Code adapted from:
 * "SuperFastHash, by Paul Hsieh (BSD 3-clause License)"
 * http://www.azillionmonkeys.com/qed/hash.html
 */

#define get16bits(d) ((uint32_t)(*((const uint16_t *)(d))))

static inline uint32_t hash_hsieh_32(const uint8_t *key, size_t len) {
	uint32_t hash, tmp;
	uint8_t rem;

	hash = (uint32_t)len;
	rem = len & 0x03;
	len >>= 2;

	/* Main loop */
	for (; len > 0; len--) {
		hash  += get16bits(key);
		tmp    = (get16bits(key + sizeof(uint16_t)) << 11) ^ hash;
		hash   = (hash << 16) ^ tmp;
		key   += 2 * sizeof(uint16_t);
		hash  += hash >> 11;
	}

	/* Handle end cases */
	switch (rem) {
		case 3: hash += get16bits(key);
				hash ^= hash << 16;
				hash ^= (uint32_t)key[sizeof(uint16_t)] << 18;
				hash += hash >> 11;
				break;
		case 2: hash += get16bits(key);
				hash ^= hash << 11;
				hash += hash >> 17;
				break;
		case 1: hash += *key;
				hash ^= hash << 10;
				hash += hash >> 1;
				break;
	}

	/* Force "avalanching" of final 127 bits */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return (hash);
}
