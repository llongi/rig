/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: murmur2.c 1055 2012-05-31 08:33:15Z llongi $
 */

static inline uint32_t hash_murmur2_oaat_32(const uint8_t *key, size_t len);

static inline uint64_t hash_murmur2_64(const uint8_t *key, size_t len);
static inline uint32_t hash_murmur2_32(const uint8_t *key, size_t len);

/**
 * Code adapted from:
 * "MurmurHash, by Austin Appleby (2011, Public Domain/MIT)"
 * https://code.google.com/p/smhasher/
 */

static inline uint32_t hash_murmur2_oaat_32(const uint8_t *key, size_t len) {
	uint32_t h = 0x0ECF0ECF;

	for (size_t i = 0; i < len; i++) {
		h ^= key[i];
		h *= 0x5BD1E995;
		h ^= h >> 15;
	}

	return (h);
}


static inline uint64_t hash_murmur2_64(const uint8_t *key, size_t len) {
	const uint64_t m = 0xC6A4A7935BD1E995;
	const int64_t r = 47;

	uint64_t h = 0x0ECF0ECF0ECF0ECF ^ ((uint64_t)len * m);

	const uint64_t *data = (const uint64_t *)key;
	uint64_t k;

	while (len >= 8) {
		k = *data;
		data++;

		k *= m;
		k ^= k >> r;
		k *= m;

		h ^= k;
		h *= m;

		len -= 8;
	}

	const uint8_t *data2 = (const uint8_t *)data;

	switch (len) {
		case 7: h ^= ((uint64_t)data2[6]) << 48;
		// no break
		case 6: h ^= ((uint64_t)data2[5]) << 40;
		// no break
		case 5: h ^= ((uint64_t)data2[4]) << 32;
		// no break
		case 4: h ^= ((uint64_t)data2[3]) << 24;
		// no break
		case 3: h ^= ((uint64_t)data2[2]) << 16;
		// no break
		case 2: h ^= ((uint64_t)data2[1]) << 8;
		// no break
		case 1: h ^= ((uint64_t)data2[0]);
				h *= m;
				break;
	}

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return (h);
}

static inline uint32_t hash_murmur2_32(const uint8_t *key, size_t len) {
	const uint32_t m = 0x5BD1E995;
	const int32_t r = 24;

	uint32_t h = 0x0ECF0ECF ^ (uint32_t)len;

	const uint32_t *data = (const uint32_t *)key;
	uint32_t k;

	while (len >= 4) {
		k = *data;
		data++;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		len -= 4;
	}

	const uint8_t *data2 = (const uint8_t *)data;

	switch (len) {
		case 3: h ^= ((uint32_t)data2[2]) << 16;
		// no break
		case 2: h ^= ((uint32_t)data2[1]) << 8;
		// no break
		case 1: h ^= ((uint32_t)data2[0]);
				h *= m;
				break;
	}

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return (h);
}
