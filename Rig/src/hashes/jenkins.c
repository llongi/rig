/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: jenkins.c 987 2011-06-24 21:43:19Z llongi $
 */

static inline uint32_t hash_jenkins_lku3_32(const uint8_t *key, size_t len);
static inline uint32_t hash_jenkins_oaat_32(const uint8_t *key, size_t len);

/**
 * Code adapted from:
 * "One-at-a-time-hash, by Bob Jenkins" for hash_jenkins_oaat_32()
 * "lookup3.c, by Bob Jenkins (May 2006, Public Domain)" for hash_jenkins_lku3_32()
 * http://burtleburtle.net/bob/hash/index.html
 * http://burtleburtle.net/bob/hash/doobs.html
 */

static inline uint32_t hash_jenkins_oaat_32(const uint8_t *key, size_t len) {
	uint32_t hash = 0;

	for (size_t i = 0; i < len; i++) {
		hash += key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return (hash);
}

/**
 * Helper macros for hash_jenkins_lku3_32()
 */

#define rot(x, k) (((x) << (k)) | ((x) >> (32 - (k))))

#define mix32(a, b, c) { \
 a -= c; a ^= rot(c,  4); c += b; \
 b -= a; b ^= rot(a,  6); a += c; \
 c -= b; c ^= rot(b,  8); b += a; \
 a -= c; a ^= rot(c, 16); c += b; \
 b -= a; b ^= rot(a, 19); a += c; \
 c -= b; c ^= rot(b,  4); b += a; \
}

#define final32(a, b, c) { \
 c ^= b; c -= rot(b, 14); \
 a ^= c; a -= rot(c, 11); \
 b ^= a; b -= rot(a, 25); \
 c ^= b; c -= rot(b, 16); \
 a ^= c; a -= rot(c,  4); \
 b ^= a; b -= rot(a, 14); \
 c ^= b; c -= rot(b, 24); \
}

static inline uint32_t hash_jenkins_lku3_32(const uint8_t *key, size_t len) {
	uint32_t a, b, c; /* internal state */

	/* set up the internal state */
	a = b = c = 0xDEADBEEF + (uint32_t)len;

	if (ALIGNED_UINT32T(key)) {
		const uint32_t *k = (const uint32_t *)key; /* read 32-bit chunks */

		/* all but last block: aligned reads and affect 32 bits of (a,b,c) */
		while (len > 12) {
			a += k[0];
			b += k[1];
			c += k[2];
			mix32(a, b, c);
			len -= 12;
			k += 3;
		}

		/* handle the last (probably partial) block */
		switch (len) {
			case 12: c += k[2]; b += k[1]; a += k[0]; break;
			case 11: c += k[2] & 0xFFFFFF00; b += k[1]; a += k[0]; break;
			case 10: c += k[2] & 0xFFFF0000; b += k[1]; a += k[0]; break;
			case 9 : c += k[2] & 0xFF000000; b += k[1]; a += k[0]; break;
			case 8 : b += k[1]; a += k[0]; break;
			case 7 : b += k[1] & 0xFFFFFF00; a += k[0]; break;
			case 6 : b += k[1] & 0xFFFF0000; a += k[0]; break;
			case 5 : b += k[1] & 0xFF000000; a += k[0]; break;
			case 4 : a += k[0]; break;
			case 3 : a += k[0] & 0xFFFFFF00; break;
			case 2 : a += k[0] & 0xFFFF0000; break;
			case 1 : a += k[0] & 0xFF000000; break;
			case 0 : return (c); /* zero length strings requires no mixing */
		}
	}
	else if (ALIGNED_UINT16T(key)) {
		const uint16_t *k = (const uint16_t *)key; /* read 16-bit chunks */

		/* all but last block: aligned reads and different mixing */
		while (len > 12) {
			a += k[0] + (((uint32_t)k[1]) << 16);
			b += k[2] + (((uint32_t)k[3]) << 16);
			c += k[4] + (((uint32_t)k[5]) << 16);
			mix32(a, b, c);
			len -= 12;
			k += 6;
		}

		/* handle the last (probably partial) block */
		const uint8_t *k8 = (const uint8_t *)k;

		switch (len) {
			case 12: c += k[4] + (((uint32_t)k[5]) << 16);
					 b += k[2] + (((uint32_t)k[3]) << 16);
					 a += k[0] + (((uint32_t)k[1]) << 16);
					 break;
			case 11: c += ((uint32_t)k8[10]) << 16;
					// no break
			case 10: c += k[4];
					 b += k[2] + (((uint32_t)k[3]) << 16);
					 a += k[0] + (((uint32_t)k[1]) << 16);
					 break;
			case 9 : c += k8[8];
					// no break
			case 8 : b += k[2] + (((uint32_t)k[3]) << 16);
					 a += k[0] + (((uint32_t)k[1]) << 16);
					 break;
			case 7 : b += ((uint32_t)k8[6]) << 16;
					// no break
			case 6 : b += k[2];
					 a += k[0] + (((uint32_t)k[1]) << 16);
					 break;
			case 5 : b += k8[4];
					// no break
			case 4 : a += k[0] + (((uint32_t)k[1]) << 16);
					 break;
			case 3 : a += ((uint32_t)k8[2]) << 16;
					// no break
			case 2 : a += k[0];
					 break;
			case 1 : a += k8[0];
					 break;
			case 0 : return (c); /* zero length strings requires no mixing */
		}
	}
	else {
		/* need to read the key one byte at a time */
		const uint8_t *k = (const uint8_t *)key;

		/* all but the last block: affect some 32 bits of (a,b,c) */
		while (len > 12) {
			a += k[0];
			a += ((uint32_t)k[1]) << 8;
			a += ((uint32_t)k[2]) << 16;
			a += ((uint32_t)k[3]) << 24;
			b += k[4];
			b += ((uint32_t)k[5]) << 8;
			b += ((uint32_t)k[6]) << 16;
			b += ((uint32_t)k[7]) << 24;
			c += k[8];
			c += ((uint32_t)k[9])  << 8;
			c += ((uint32_t)k[10]) << 16;
			c += ((uint32_t)k[11]) << 24;
			mix32(a, b, c);
			len -= 12;
			k += 12;
		}

		/* last block: affect all 32 bits of (c) */
		switch (len) {
			case 12: c += ((uint32_t)k[11]) << 24;
			// no break
			case 11: c += ((uint32_t)k[10]) << 16;
			// no break
			case 10: c += ((uint32_t)k[9])  << 8;
			// no break
			case 9 : c += k[8];
			// no break
			case 8 : b += ((uint32_t)k[7]) << 24;
			// no break
			case 7 : b += ((uint32_t)k[6]) << 16;
			// no break
			case 6 : b += ((uint32_t)k[5]) << 8;
			// no break
			case 5 : b += k[4];
			// no break
			case 4 : a += ((uint32_t)k[3]) << 24;
			// no break
			case 3 : a += ((uint32_t)k[2]) << 16;
			// no break
			case 2 : a += ((uint32_t)k[1]) << 8;
			// no break
			case 1 : a += k[0];
					 break;
			case 0 : return (c); /* zero length strings requires no mixing */
		}
	}

	final32(a, b, c);

	return (c);
}
