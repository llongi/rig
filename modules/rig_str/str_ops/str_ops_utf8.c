/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: str_ops_utf8.c 897 2011-03-08 03:32:22Z llongi $
 */

#ifndef STR_OPS_UTF8_C
#define STR_OPS_UTF8_C 1

#include "utf8/utf8.h"

static inline uint8_t str_ops_utf8_charlen(uint32_t uchar);


static inline uint8_t str_ops_utf8_charlen(uint32_t uchar) {
	// If ASCII, return immediately
	if (uchar <= 0x7F) {
		return (1);
	}
	else {
		// Filter out over-long sequences
		if ((uchar & 0x0000FF00) == 0x0000C000
		 || (uchar & 0x0000FF00) == 0x0000C100
		 || (uchar & 0x00FFF000) == 0x00E08000
		 || (uchar & 0x00FFF000) == 0x00E09000
		 || (uchar & 0xFFF00000) == 0xF0800000) {
			return (0);
		}

		// Determine UTF-8 character length
		if (((uchar & 0x0000FF00) >= 0xC200 && (uchar & 0x0000FF00) <= 0xDF00)
		 && ((uchar & 0x000000FF) >= 0x80 && (uchar & 0x000000FF) <= 0xBF)) {
			return (2);
		}
		else if (((uchar & 0x00FF0000) >= 0xE00000 && (uchar & 0x00FF0000) <= 0xEF0000)
		 && ((uchar & 0x0000FF00) >= 0x8000 && (uchar & 0x0000FF00) <= 0xBF00)
		 && ((uchar & 0x000000FF) >= 0x80 && (uchar & 0x000000FF) <= 0xBF)) {
			return (3);
		}
		else if (((uchar & 0xFF000000) >= 0xF0000000 && (uchar & 0xFF000000) <= 0xF4000000)
		 && ((uchar & 0x00FF0000) >= 0x800000 && (uchar & 0x00FF0000) <= 0xBF0000)
		 && ((uchar & 0x0000FF00) >= 0x8000 && (uchar & 0x0000FF00) <= 0xBF00)
		 && ((uchar & 0x000000FF) >= 0x80 && (uchar & 0x000000FF) <= 0xBF)) {
			return (4);
		}
	}

	// Invalid character
	return (0);
}

#endif /* STR_OPS_UTF8_C */
