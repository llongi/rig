/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: murmur3.c 1055 2012-05-31 08:33:15Z llongi $
 */

static inline uint64_t hash_murmur3_64(const uint8_t *key, size_t len);
static inline uint32_t hash_murmur3_32(const uint8_t *key, size_t len);

/**
 * Code adapted from:
 * "MurmurHash, by Austin Appleby (2011, Public Domain/MIT)"
 * https://code.google.com/p/smhasher/
 */

#define ROTL32(x, r) (x << r) | (x >> (32 - r))
#define ROTL64(x, r) (x << r) | (x >> (64 - r))

static inline uint64_t hash_murmur3_64(const uint8_t *key, size_t len) {
	const uint64_t c1 = 0x87C37B91114253D5;
	const uint64_t c2 = 0x4CF5AD432745937F;
	const size_t orig_len = len;

	uint64_t h = 0x0ECF0ECF0ECF0ECF;

	const uint64_t *data = (const uint64_t *)key;
	uint64_t k;

	while (len >= 8) {
		k = *data;
		data++;

		k *= c1;
		k  = ROTL64(k, 31);
		k *= c2;

		h ^= k;
		h  = ROTL64(h, 27);
		h  = h * 5 + 0x52DCE729;

		len -= 8;
	}

	const uint8_t *data2 = (const uint8_t *)data;
	k = 0;

	switch (len) {
		case 7: k ^= ((uint64_t)data2[6]) << 48;
		// no break
		case 6: k ^= ((uint64_t)data2[5]) << 40;
		// no break
		case 5: k ^= ((uint64_t)data2[4]) << 32;
		// no break
		case 4: k ^= ((uint64_t)data2[3]) << 24;
		// no break
		case 3: k ^= ((uint64_t)data2[2]) << 16;
		// no break
		case 2: k ^= ((uint64_t)data2[1]) << 8;
		// no break
		case 1: k ^= ((uint64_t)data2[0]);
				k *= c1;
				k  = ROTL64(k, 31);
				k *= c2;
				h ^= k;
				break;
	}

	h ^= (uint64_t)orig_len;

	h ^= h >> 33;
	h *= 0xFF51AFD7ED558CCD;
	h ^= h >> 33;
	h *= 0xC4CEB9FE1A85EC53;
	h ^= h >> 33;

	return (h);
}

static inline uint32_t hash_murmur3_32(const uint8_t *key, size_t len) {
	const uint32_t c1 = 0xCC9E2D51;
	const uint32_t c2 = 0x1B873593;
	const size_t orig_len = len;

	uint32_t h = 0x0ECF0ECF;

	const uint32_t *data = (const uint32_t *)key;
	uint32_t k;

	while (len >= 4) {
		k = *data;
		data++;

		k *= c1;
		k  = ROTL32(k, 15);
		k *= c2;

		h ^= k;
		h  = ROTL32(h, 13);
		h  = h * 5 + 0xE6546B64;

		len -= 4;
	}

	const uint8_t *data2 = (const uint8_t *)data;
	k = 0;

	switch (len) {
		case 3: k ^= ((uint32_t)data2[2]) << 16;
		// no break
		case 2: k ^= ((uint32_t)data2[1]) << 8;
		// no break
		case 1: k ^= ((uint32_t)data2[0]);
				k *= c1;
				k  = ROTL32(k, 15);
				k *= c2;
				h ^= k;
				break;
	}

	h ^= (uint32_t)orig_len;

	h ^= h >> 16;
	h *= 0x85EBCA6B;
	h ^= h >> 13;
	h *= 0xC2B2AE35;
	h ^= h >> 16;

	return (h);
}
