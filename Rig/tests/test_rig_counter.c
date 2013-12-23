/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: test_rig_counter.c 1034 2012-05-22 20:00:21Z llongi $
 */

#include "tests.h"

Suite *test_rig_counter_init(void);
Suite *test_rig_counter_destroy(void);
Suite *test_rig_counter_get_max(void);
Suite *test_rig_counter_get(void);
Suite *test_rig_counter_get_and_set(void);
Suite *test_rig_counter_inc(void);
Suite *test_rig_counter_dec(void);
Suite *test_rig_counter_dec_and_test(void);
Suite *test_rig_counter_get_and_add(void);

int main(void) {
	SRunner *sr = srunner_create(test_rig_counter_init());
	srunner_add_suite(sr, test_rig_counter_destroy());
	srunner_add_suite(sr, test_rig_counter_get_max());
	srunner_add_suite(sr, test_rig_counter_get());
	srunner_add_suite(sr, test_rig_counter_get_and_set());
	srunner_add_suite(sr, test_rig_counter_inc());
	srunner_add_suite(sr, test_rig_counter_dec());
	srunner_add_suite(sr, test_rig_counter_dec_and_test());
	srunner_add_suite(sr, test_rig_counter_get_and_add());

	srunner_run_all(sr, CK_VERBOSE);
	int failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return ((failed == 0) ? (EXIT_SUCCESS) : (EXIT_FAILURE));
}


RIG_COUNTER cnt = NULL;

static void setup_counter(void) {
	cnt = rig_counter_init(1, 10);
	ck_assert(cnt != NULL);
}

static void teardown_counter(void) {
	rig_counter_destroy(&cnt);
	ck_assert(cnt == NULL);
}

/******************************************************************************/

START_TEST(test_rig_counter_init_normal) {
	ck_assert(rig_counter_init(0, 0) != NULL);
	ck_assert(rig_counter_init(10, 0) != NULL);
	ck_assert(rig_counter_init(SIZE_MAX, 0) != NULL);

	ck_assert(rig_counter_init(0, 100) != NULL);
	ck_assert(rig_counter_init(10, 100) != NULL);
	ck_assert(rig_counter_init(100, 100) != NULL);

	ck_assert(rig_counter_init(0, SIZE_MAX) != NULL);
	ck_assert(rig_counter_init(10, SIZE_MAX) != NULL);
	ck_assert(rig_counter_init(SIZE_MAX, SIZE_MAX) != NULL);
} END_TEST

START_TEST(test_rig_counter_init_error) {
	ck_assert(rig_counter_init(101, 100) == NULL && errno == EINVAL);
	ck_assert(rig_counter_init(200, 100) == NULL && errno == EINVAL);
	ck_assert(rig_counter_init(SIZE_MAX, 100) == NULL && errno == EINVAL);
} END_TEST

Suite *test_rig_counter_init(void) {
	Suite *s = suite_create("test_rig_counter_init");

	TCASE_ADD(rig_counter_init_normal);
	TCASE_ADD(rig_counter_init_error);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_counter_destroy_normal) {
	rig_counter_destroy(&cnt);
	ck_assert(cnt == NULL);
} END_TEST

START_TEST(test_rig_counter_destroy_null) {
	cnt = NULL;
	rig_counter_destroy(&cnt);
	ck_assert(cnt == NULL);
} END_TEST

START_TEST(test_rig_counter_destroy_nullptr) {
	rig_counter_destroy(NULL);
} END_TEST

Suite *test_rig_counter_destroy(void) {
	Suite *s = suite_create("test_rig_counter_destroy");

	TCASE_ADD_FIXTURE(rig_counter_destroy_normal, &setup_counter, NULL);
	TCASE_ADD(rig_counter_destroy_null);
	TCASE_ADD_EXIT(rig_counter_destroy_nullptr, EXIT_FAILURE);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_counter_get_max_normal) {
	ck_assert(rig_counter_get_max(cnt) == 10);
} END_TEST

START_TEST(test_rig_counter_get_max_nullptr) {
	rig_counter_get_max(NULL);
} END_TEST

Suite *test_rig_counter_get_max(void) {
	Suite *s = suite_create("test_rig_counter_get_max");

	TCASE_ADD_FIXTURE(rig_counter_get_max_normal, &setup_counter, &teardown_counter);
	TCASE_ADD_EXIT(rig_counter_get_max_nullptr, EXIT_FAILURE);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_counter_get_normal) {
	ck_assert(rig_counter_get(cnt) == 1);
} END_TEST

START_TEST(test_rig_counter_get_nullptr) {
	rig_counter_get(NULL);
} END_TEST

Suite *test_rig_counter_get(void) {
	Suite *s = suite_create("test_rig_counter_get");

	TCASE_ADD_FIXTURE(rig_counter_get_normal, &setup_counter, &teardown_counter);
	TCASE_ADD_EXIT(rig_counter_get_nullptr, EXIT_FAILURE);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_counter_get_and_set_normal) {
	ck_assert(rig_counter_get_and_set(cnt, 0, NULL));
	ck_assert(rig_counter_get(cnt) == 0);

	ck_assert(rig_counter_get_and_set(cnt, 5, NULL));
	ck_assert(rig_counter_get(cnt) == 5);

	ck_assert(rig_counter_get_and_set(cnt, 10, NULL));
	ck_assert(rig_counter_get(cnt) == 10);
} END_TEST

START_TEST(test_rig_counter_get_and_set_normal_oldval) {
	size_t old_val = 0;

	ck_assert(rig_counter_get_and_set(cnt, 0, &old_val));
	ck_assert(rig_counter_get(cnt) == 0);
	ck_assert(old_val == 1);

	ck_assert(rig_counter_get_and_set(cnt, 5, &old_val));
	ck_assert(rig_counter_get(cnt) == 5);
	ck_assert(old_val == 0);

	ck_assert(rig_counter_get_and_set(cnt, 10, &old_val));
	ck_assert(rig_counter_get(cnt) == 10);
	ck_assert(old_val == 5);
} END_TEST

START_TEST(test_rig_counter_get_and_set_error) {
	ck_assert(!rig_counter_get_and_set(cnt, 11, NULL) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);

	ck_assert(!rig_counter_get_and_set(cnt, 100, NULL) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);

	ck_assert(!rig_counter_get_and_set(cnt, SIZE_MAX, NULL) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);
} END_TEST

START_TEST(test_rig_counter_get_and_set_error_oldval) {
	size_t old_val = 0;

	ck_assert(!rig_counter_get_and_set(cnt, 11, &old_val) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);
	ck_assert(old_val == 1);

	ck_assert(!rig_counter_get_and_set(cnt, 100, &old_val) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);
	ck_assert(old_val == 1);

	ck_assert(!rig_counter_get_and_set(cnt, SIZE_MAX, &old_val) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);
	ck_assert(old_val == 1);
} END_TEST

START_TEST(test_rig_counter_get_and_set_nullptr) {
	rig_counter_get_and_set(NULL, 0, NULL);
} END_TEST

Suite *test_rig_counter_get_and_set(void) {
	Suite *s = suite_create("test_rig_counter_get_and_set");

	TCASE_ADD_FIXTURE(rig_counter_get_and_set_normal, &setup_counter, &teardown_counter);
	TCASE_ADD_FIXTURE(rig_counter_get_and_set_normal_oldval, &setup_counter, &teardown_counter);
	TCASE_ADD_FIXTURE(rig_counter_get_and_set_error, &setup_counter, &teardown_counter);
	TCASE_ADD_FIXTURE(rig_counter_get_and_set_error_oldval, &setup_counter, &teardown_counter);
	TCASE_ADD_EXIT(rig_counter_get_and_set_nullptr, EXIT_FAILURE);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_counter_inc_normal) {
	for (size_t i = 2; i < 11; i++) {
		ck_assert(rig_counter_inc(cnt));
		ck_assert(rig_counter_get(cnt) == i);
	}
} END_TEST

START_TEST(test_rig_counter_inc_error) {
	ck_assert(rig_counter_set(cnt, 10));
	ck_assert(rig_counter_get(cnt) == 10);

	ck_assert(!rig_counter_inc(cnt) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 10);
} END_TEST

START_TEST(test_rig_counter_inc_nullptr) {
	rig_counter_inc(NULL);
} END_TEST

Suite *test_rig_counter_inc(void) {
	Suite *s = suite_create("test_rig_counter_inc");

	TCASE_ADD_FIXTURE(rig_counter_inc_normal, &setup_counter, &teardown_counter);
	TCASE_ADD_FIXTURE(rig_counter_inc_error, &setup_counter, &teardown_counter);
	TCASE_ADD_EXIT(rig_counter_inc_nullptr, EXIT_FAILURE);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_counter_dec_normal) {
	ck_assert(rig_counter_set(cnt, 10));
	ck_assert(rig_counter_get(cnt) == 10);

	for (size_t i = 9; i > 0; i--) {
		ck_assert(rig_counter_dec(cnt));
		ck_assert(rig_counter_get(cnt) == i);
	}
} END_TEST

START_TEST(test_rig_counter_dec_error) {
	ck_assert(rig_counter_dec(cnt));
	ck_assert(rig_counter_get(cnt) == 0);

	ck_assert(!rig_counter_dec(cnt) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 0);
} END_TEST

START_TEST(test_rig_counter_dec_nullptr) {
	rig_counter_dec(NULL);
} END_TEST

Suite *test_rig_counter_dec(void) {
	Suite *s = suite_create("test_rig_counter_dec");

	TCASE_ADD_FIXTURE(rig_counter_dec_normal, &setup_counter, &teardown_counter);
	TCASE_ADD_FIXTURE(rig_counter_dec_error, &setup_counter, &teardown_counter);
	TCASE_ADD_EXIT(rig_counter_dec_nullptr, EXIT_FAILURE);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_counter_dec_and_test_normal) {
	ck_assert(rig_counter_set(cnt, 10));
	ck_assert(rig_counter_get(cnt) == 10);

	for (size_t i = 9; i > 0; i--) {
		ck_assert(!rig_counter_dec_and_test(cnt) && errno == 0);
		ck_assert(rig_counter_get(cnt) == i);
	}

	ck_assert(rig_counter_dec_and_test(cnt));
	ck_assert(rig_counter_get(cnt) == 0);
} END_TEST

START_TEST(test_rig_counter_dec_and_test_error) {
	ck_assert(rig_counter_dec_and_test(cnt));
	ck_assert(rig_counter_get(cnt) == 0);

	ck_assert(!rig_counter_dec_and_test(cnt) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 0);
} END_TEST

START_TEST(test_rig_counter_dec_and_test_nullptr) {
	rig_counter_dec_and_test(NULL);
} END_TEST

Suite *test_rig_counter_dec_and_test(void) {
	Suite *s = suite_create("test_rig_counter_dec_and_test");

	TCASE_ADD_FIXTURE(rig_counter_dec_and_test_normal, &setup_counter, &teardown_counter);
	TCASE_ADD_FIXTURE(rig_counter_dec_and_test_error, &setup_counter, &teardown_counter);
	TCASE_ADD_EXIT(rig_counter_dec_and_test_nullptr, EXIT_FAILURE);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_counter_get_and_add_normal) {
	ck_assert(rig_counter_get_and_add(cnt, 0, NULL));
	ck_assert(rig_counter_get(cnt) == 1);

	ck_assert(rig_counter_get_and_add(cnt, -1, NULL));
	ck_assert(rig_counter_get(cnt) == 0);

	ck_assert(rig_counter_get_and_add(cnt, 1, NULL));
	ck_assert(rig_counter_get(cnt) == 1);

	ck_assert(rig_counter_get_and_add(cnt, 9, NULL));
	ck_assert(rig_counter_get(cnt) == 10);

	ck_assert(rig_counter_get_and_add(cnt, -10, NULL));
	ck_assert(rig_counter_get(cnt) == 0);
} END_TEST

START_TEST(test_rig_counter_get_and_add_normal_oldval) {
	size_t old_val = 0;

	ck_assert(rig_counter_get_and_add(cnt, 0, &old_val));
	ck_assert(rig_counter_get(cnt) == 1);
	ck_assert(old_val == 1);

	ck_assert(rig_counter_get_and_add(cnt, -1, &old_val));
	ck_assert(rig_counter_get(cnt) == 0);
	ck_assert(old_val == 1);

	ck_assert(rig_counter_get_and_add(cnt, 1, &old_val));
	ck_assert(rig_counter_get(cnt) == 1);
	ck_assert(old_val == 0);

	ck_assert(rig_counter_get_and_add(cnt, 9, &old_val));
	ck_assert(rig_counter_get(cnt) == 10);
	ck_assert(old_val == 1);

	ck_assert(rig_counter_get_and_add(cnt, -10, &old_val));
	ck_assert(rig_counter_get(cnt) == 0);
	ck_assert(old_val == 10);
} END_TEST

START_TEST(test_rig_counter_get_and_add_error) {
	ck_assert(!rig_counter_get_and_add(cnt, 10, NULL) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);

	ck_assert(!rig_counter_get_and_add(cnt, 100, NULL) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);

	ck_assert(!rig_counter_get_and_add(cnt, SSIZE_MAX, NULL) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);

	ck_assert(!rig_counter_get_and_add(cnt, -2, NULL) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);

	ck_assert(!rig_counter_get_and_add(cnt, -100, NULL) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);

	ck_assert(!rig_counter_get_and_add(cnt, SSIZE_MIN, NULL) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);
} END_TEST

START_TEST(test_rig_counter_get_and_add_error_oldval) {
	size_t old_val = 0;

	ck_assert(!rig_counter_get_and_add(cnt, 10, &old_val) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);
	ck_assert(old_val == 1);

	ck_assert(!rig_counter_get_and_add(cnt, 100, &old_val) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);
	ck_assert(old_val == 1);

	ck_assert(!rig_counter_get_and_add(cnt, SSIZE_MAX, &old_val) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);
	ck_assert(old_val == 1);

	ck_assert(!rig_counter_get_and_add(cnt, -2, &old_val) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);
	ck_assert(old_val == 1);

	ck_assert(!rig_counter_get_and_add(cnt, -100, &old_val) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);
	ck_assert(old_val == 1);

	ck_assert(!rig_counter_get_and_add(cnt, SSIZE_MIN, &old_val) && errno == ERANGE);
	ck_assert(rig_counter_get(cnt) == 1);
	ck_assert(old_val == 1);
} END_TEST

START_TEST(test_rig_counter_get_and_add_nullptr) {
	rig_counter_get_and_add(NULL, 0, NULL);
} END_TEST

Suite *test_rig_counter_get_and_add(void) {
	Suite *s = suite_create("test_rig_counter_get_and_add");

	TCASE_ADD_FIXTURE(rig_counter_get_and_add_normal, &setup_counter, &teardown_counter);
	TCASE_ADD_FIXTURE(rig_counter_get_and_add_normal_oldval, &setup_counter, &teardown_counter);
	TCASE_ADD_FIXTURE(rig_counter_get_and_add_error, &setup_counter, &teardown_counter);
	TCASE_ADD_FIXTURE(rig_counter_get_and_add_error_oldval, &setup_counter, &teardown_counter);
	TCASE_ADD_EXIT(rig_counter_get_and_add_nullptr, EXIT_FAILURE);

	return (s);
}
