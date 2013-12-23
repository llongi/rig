/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: test_rig_hash.c 977 2011-05-03 13:32:50Z llongi $
 */

#include "tests.h"
#include "rig_config.h"

Suite *test_rig_hash(void);

int main(void) {
	SRunner *sr = srunner_create(test_rig_hash());

	srunner_run_all(sr, CK_VERBOSE);
	int failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return ((failed == 0) ? (EXIT_SUCCESS) : (EXIT_FAILURE));
}

/******************************************************************************/

START_TEST(test_rig_hash_normal) {
	ck_assert(rig_hash(rand1k, 0, RIG_HASH_DEFAULT) == 0);
	ck_assert(rig_hash(rand1k, 0, RIG_HASH_DIRECT) == 0);

	for (size_t i = 1; i < 33; i++) {
		ck_assert(rig_hash(rand1k, i, RIG_HASH_DEFAULT) != 0);
	}

#if defined(SYSTEM_BIGENDIAN)
	ck_assert(rig_hash(rand1k, 1, RIG_HASH_DIRECT) == 0xFD);
	ck_assert(rig_hash(rand1k, 2, RIG_HASH_DIRECT) == 0xFDEA);
	ck_assert(rig_hash(rand1k, 3, RIG_HASH_DIRECT) == 0xFDEA6D);
	ck_assert(rig_hash(rand1k, 4, RIG_HASH_DIRECT) == 0xFDEA6D92);

	#if SIZEOF_SIZE_T == 8
		ck_assert(rig_hash(rand1k, 5, RIG_HASH_DIRECT) == 0xFDEA6D92F5);
		ck_assert(rig_hash(rand1k, 6, RIG_HASH_DIRECT) == 0xFDEA6D92F51E);
		ck_assert(rig_hash(rand1k, 7, RIG_HASH_DIRECT) == 0xFDEA6D92F51E75);
		ck_assert(rig_hash(rand1k, 8, RIG_HASH_DIRECT) == 0xFDEA6D92F51E758B);
	#endif
#else
	ck_assert(rig_hash(rand1k, 1, RIG_HASH_DIRECT) == 0xFD);
	ck_assert(rig_hash(rand1k, 2, RIG_HASH_DIRECT) == 0xEAFD);
	ck_assert(rig_hash(rand1k, 3, RIG_HASH_DIRECT) == 0x6DEAFD);
	ck_assert(rig_hash(rand1k, 4, RIG_HASH_DIRECT) == 0x926DEAFD);

	#if SIZEOF_SIZE_T == 8
		ck_assert(rig_hash(rand1k, 5, RIG_HASH_DIRECT) == 0xF5926DEAFD);
		ck_assert(rig_hash(rand1k, 6, RIG_HASH_DIRECT) == 0x1EF5926DEAFD);
		ck_assert(rig_hash(rand1k, 7, RIG_HASH_DIRECT) == 0x751EF5926DEAFD);
		ck_assert(rig_hash(rand1k, 8, RIG_HASH_DIRECT) == 0x8B751EF5926DEAFD);
	#endif
#endif
} END_TEST

START_TEST(test_rig_hash_default) {
	ck_assert(rig_hash(rand1k, 4, RIG_HASH_DEFAULT) == rig_hash(rand1k, 4, RIG_HASH_DEFAULT));
	ck_assert(rig_hash(rand1k, 8, RIG_HASH_DEFAULT) == rig_hash(rand1k, 8, RIG_HASH_DEFAULT));
	ck_assert(rig_hash(rand1k, 16, RIG_HASH_DEFAULT) == rig_hash(rand1k, 16, RIG_HASH_DEFAULT));
	ck_assert(rig_hash(rand1k, rand1k_len, RIG_HASH_DEFAULT) == rig_hash(rand1k, rand1k_len, RIG_HASH_DEFAULT));
} END_TEST

START_TEST(test_rig_hash_jenkins) {
	ck_assert(rig_hash(rand1k, 4, RIG_HASH_JENKINS) == rig_hash(rand1k, 4, RIG_HASH_JENKINS));
	ck_assert(rig_hash(rand1k, 8, RIG_HASH_JENKINS) == rig_hash(rand1k, 8, RIG_HASH_JENKINS));
	ck_assert(rig_hash(rand1k, 16, RIG_HASH_JENKINS) == rig_hash(rand1k, 16, RIG_HASH_JENKINS));
	ck_assert(rig_hash(rand1k, rand1k_len, RIG_HASH_JENKINS) == rig_hash(rand1k, rand1k_len, RIG_HASH_JENKINS));
} END_TEST

START_TEST(test_rig_hash_fnv) {
	ck_assert(rig_hash(rand1k, 4, RIG_HASH_FNV) == rig_hash(rand1k, 4, RIG_HASH_FNV));
	ck_assert(rig_hash(rand1k, 8, RIG_HASH_FNV) == rig_hash(rand1k, 8, RIG_HASH_FNV));
	ck_assert(rig_hash(rand1k, 16, RIG_HASH_FNV) == rig_hash(rand1k, 16, RIG_HASH_FNV));
	ck_assert(rig_hash(rand1k, rand1k_len, RIG_HASH_FNV) == rig_hash(rand1k, rand1k_len, RIG_HASH_FNV));
} END_TEST

START_TEST(test_rig_hash_hsieh) {
	ck_assert(rig_hash(rand1k, 4, RIG_HASH_HSIEH) == rig_hash(rand1k, 4, RIG_HASH_HSIEH));
	ck_assert(rig_hash(rand1k, 8, RIG_HASH_HSIEH) == rig_hash(rand1k, 8, RIG_HASH_HSIEH));
	ck_assert(rig_hash(rand1k, 16, RIG_HASH_HSIEH) == rig_hash(rand1k, 16, RIG_HASH_HSIEH));
	ck_assert(rig_hash(rand1k, rand1k_len, RIG_HASH_HSIEH) == rig_hash(rand1k, rand1k_len, RIG_HASH_HSIEH));
} END_TEST

START_TEST(test_rig_hash_murmur2) {
	ck_assert(rig_hash(rand1k, 4, RIG_HASH_MURMUR2) == rig_hash(rand1k, 4, RIG_HASH_MURMUR2));
	ck_assert(rig_hash(rand1k, 8, RIG_HASH_MURMUR2) == rig_hash(rand1k, 8, RIG_HASH_MURMUR2));
	ck_assert(rig_hash(rand1k, 16, RIG_HASH_MURMUR2) == rig_hash(rand1k, 16, RIG_HASH_MURMUR2));
	ck_assert(rig_hash(rand1k, rand1k_len, RIG_HASH_MURMUR2) == rig_hash(rand1k, rand1k_len, RIG_HASH_MURMUR2));
} END_TEST

START_TEST(test_rig_hash_murmur3) {
	ck_assert(rig_hash(rand1k, 4, RIG_HASH_MURMUR3) == rig_hash(rand1k, 4, RIG_HASH_MURMUR3));
	ck_assert(rig_hash(rand1k, 8, RIG_HASH_MURMUR3) == rig_hash(rand1k, 8, RIG_HASH_MURMUR3));
	ck_assert(rig_hash(rand1k, 16, RIG_HASH_MURMUR3) == rig_hash(rand1k, 16, RIG_HASH_MURMUR3));
	ck_assert(rig_hash(rand1k, rand1k_len, RIG_HASH_MURMUR3) == rig_hash(rand1k, rand1k_len, RIG_HASH_MURMUR3));
} END_TEST

START_TEST(test_rig_hash_jenkins_oaat) {
	ck_assert(rig_hash(rand1k, 4, RIG_HASH_JENKINS_OAAT) == rig_hash(rand1k, 4, RIG_HASH_JENKINS_OAAT));
	ck_assert(rig_hash(rand1k, 8, RIG_HASH_JENKINS_OAAT) == rig_hash(rand1k, 8, RIG_HASH_JENKINS_OAAT));
	ck_assert(rig_hash(rand1k, 16, RIG_HASH_JENKINS_OAAT) == rig_hash(rand1k, 16, RIG_HASH_JENKINS_OAAT));
	ck_assert(rig_hash(rand1k, rand1k_len, RIG_HASH_JENKINS_OAAT) == rig_hash(rand1k, rand1k_len, RIG_HASH_JENKINS_OAAT));
} END_TEST

START_TEST(test_rig_hash_murmur2_oaat) {
	ck_assert(rig_hash(rand1k, 4, RIG_HASH_MURMUR2_OAAT) == rig_hash(rand1k, 4, RIG_HASH_MURMUR2_OAAT));
	ck_assert(rig_hash(rand1k, 8, RIG_HASH_MURMUR2_OAAT) == rig_hash(rand1k, 8, RIG_HASH_MURMUR2_OAAT));
	ck_assert(rig_hash(rand1k, 16, RIG_HASH_MURMUR2_OAAT) == rig_hash(rand1k, 16, RIG_HASH_MURMUR2_OAAT));
	ck_assert(rig_hash(rand1k, rand1k_len, RIG_HASH_MURMUR2_OAAT) == rig_hash(rand1k, rand1k_len, RIG_HASH_MURMUR2_OAAT));
} END_TEST

START_TEST(test_rig_hash_nullptr) {
	rig_hash(NULL, 0, RIG_HASH_DEFAULT);
} END_TEST

Suite *test_rig_hash(void) {
	Suite *s = suite_create("test_rig_hash");

	TCASE_ADD(rig_hash_normal);
	TCASE_ADD(rig_hash_default);
	TCASE_ADD(rig_hash_jenkins);
	TCASE_ADD(rig_hash_fnv);
	TCASE_ADD(rig_hash_hsieh);
	TCASE_ADD(rig_hash_murmur2);
	TCASE_ADD(rig_hash_murmur3);
	TCASE_ADD(rig_hash_jenkins_oaat);
	TCASE_ADD(rig_hash_murmur2_oaat);
	TCASE_ADD_EXIT(rig_hash_nullptr, EXIT_FAILURE);

	return (s);
}
