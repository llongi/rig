/**
 * This file is part of the Rig project.
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 *
 * @copyright  (c) the Rig project
 * @author     Luca Longinotti <chtekk@longitekk.com>
 * @license    BSD 2-clause
 * @version    $Id: test_rig_list.c 1134 2012-11-11 08:56:23Z llongi $
 */

#include "tests.h"

Suite *test_rig_list_init(void);
Suite *test_rig_list_destroy(void);
Suite *test_rig_list_clear(void);
Suite *test_rig_list_add(void);
Suite *test_rig_list_del(void);
Suite *test_rig_list_find(void);
Suite *test_rig_list_get(void);
Suite *test_rig_list_peek(void);

Suite *test_rig_list_iter_begin(void);
Suite *test_rig_list_iter_end(void);
Suite *test_rig_list_iter_next(void);
Suite *test_rig_list_iter_delete(void);

Suite *test_rig_list_duplicate(void);

int main(void) {
	SRunner *sr = srunner_create(test_rig_list_init());
	srunner_add_suite(sr, test_rig_list_destroy());
	srunner_add_suite(sr, test_rig_list_clear());
	srunner_add_suite(sr, test_rig_list_add());
	srunner_add_suite(sr, test_rig_list_del());
	srunner_add_suite(sr, test_rig_list_find());
	srunner_add_suite(sr, test_rig_list_get());
	srunner_add_suite(sr, test_rig_list_peek());

	srunner_add_suite(sr, test_rig_list_iter_begin());
	srunner_add_suite(sr, test_rig_list_iter_end());
	srunner_add_suite(sr, test_rig_list_iter_next());
	srunner_add_suite(sr, test_rig_list_iter_delete());

	srunner_add_suite(sr, test_rig_list_duplicate());

	srunner_run_all(sr, CK_VERBOSE);
	int failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return ((failed == 0) ? (EXIT_SUCCESS) : (EXIT_FAILURE));
}


RIG_LIST list = NULL;

static void setup_empty_list(void) {
	list = rig_list_init(10, RIG_LIST_NODUPS, NULL, NULL);
	ck_assert(list != NULL);
}

static void setup_list(void) {
	list = rig_list_init(10, RIG_LIST_NODUPS, NULL, NULL);
	ck_assert(list != NULL);

	ck_assert(rig_list_count(list) == 0);
	ck_assert(rig_list_capacity(list) == 10);
	ck_assert(rig_list_add(list, (void *)50));
	ck_assert(rig_list_add(list, (void *)40));
	ck_assert(rig_list_add(list, (void *)30));
	ck_assert(rig_list_add(list, (void *)20));
	ck_assert(rig_list_add(list, (void *)10));
	ck_assert(rig_list_count(list) == 5);
	ck_assert(rig_list_capacity(list) == 10);
}

static void teardown_list(void) {
	rig_list_destroy(&list);
	ck_assert(list == NULL);
}

/******************************************************************************/

START_TEST(test_rig_list_init_normal) {
	ck_assert(rig_list_init(0, 0, NULL, NULL) != NULL);
	ck_assert(rig_list_init(10, 0, NULL, NULL) != NULL);
	ck_assert(rig_list_init(SIZE_MAX, 0, NULL, NULL) != NULL);

	ck_assert(rig_list_init(0, RIG_LIST_NODUPS, NULL, NULL) != NULL);
	ck_assert(rig_list_init(10, RIG_LIST_NODUPS, NULL, NULL) != NULL);
	ck_assert(rig_list_init(SIZE_MAX, RIG_LIST_NODUPS, NULL, NULL) != NULL);
} END_TEST

START_TEST(test_rig_list_init_error) {
	ck_assert(rig_list_init(0, (1 << 5), NULL, NULL) == NULL && errno == EINVAL);
	ck_assert(rig_list_init(0, (1 << 15), NULL, NULL) == NULL && errno == EINVAL);
	ck_assert(rig_list_init(0, (1 << 5) | (1 << 15), NULL, NULL) == NULL && errno == EINVAL);
	ck_assert(rig_list_init(0, (1 << 5) | (1 << 15) | RIG_LIST_NODUPS, NULL, NULL) == NULL && errno == EINVAL);
} END_TEST

Suite *test_rig_list_init(void) {
	Suite *s = suite_create("test_rig_list_init");

	TCASE_ADD(rig_list_init_normal);
	TCASE_ADD(rig_list_init_error);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_list_destroy_normal) {
	rig_list_destroy(&list);
	ck_assert(list == NULL);
} END_TEST

START_TEST(test_rig_list_destroy_newref) {
	RIG_LIST newref = rig_list_newref(list);
	rig_list_destroy(&newref);
	ck_assert(newref == NULL);

	rig_list_destroy(&list);
	ck_assert(list == NULL);
} END_TEST

START_TEST(test_rig_list_destroy_null) {
	list = NULL;
	rig_list_destroy(&list);
} END_TEST

START_TEST(test_rig_list_destroy_nullptr) {
	rig_list_destroy(NULL);
} END_TEST

Suite *test_rig_list_destroy(void) {
	Suite *s = suite_create("test_rig_list_destroy");

	TCASE_ADD_FIXTURE(rig_list_destroy_normal, &setup_list, NULL);
	TCASE_ADD_FIXTURE(rig_list_destroy_newref, &setup_list, NULL);
	TCASE_ADD_EXIT(rig_list_destroy_null, EXIT_FAILURE);
	TCASE_ADD_EXIT(rig_list_destroy_nullptr, EXIT_FAILURE);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_list_clear_normal) {

} END_TEST

START_TEST(test_rig_list_clear_error) {

} END_TEST

Suite *test_rig_list_clear(void) {
	Suite *s = suite_create("test_rig_list_clear");

	TCASE_ADD(rig_list_clear_normal);
	TCASE_ADD(rig_list_clear_error);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_list_add_normal) {

} END_TEST

START_TEST(test_rig_list_add_error) {

} END_TEST

Suite *test_rig_list_add(void) {
	Suite *s = suite_create("test_rig_list_add");

	TCASE_ADD(rig_list_add_normal);
	TCASE_ADD(rig_list_add_error);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_list_del_normal) {

} END_TEST

START_TEST(test_rig_list_del_error) {

} END_TEST

Suite *test_rig_list_del(void) {
	Suite *s = suite_create("test_rig_list_del");

	TCASE_ADD(rig_list_del_normal);
	TCASE_ADD(rig_list_del_error);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_list_find_normal) {

} END_TEST

START_TEST(test_rig_list_find_error) {

} END_TEST

Suite *test_rig_list_find(void) {
	Suite *s = suite_create("test_rig_list_find");

	TCASE_ADD(rig_list_find_normal);
	TCASE_ADD(rig_list_find_error);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_list_get_normal) {

} END_TEST

START_TEST(test_rig_list_get_error) {

} END_TEST

Suite *test_rig_list_get(void) {
	Suite *s = suite_create("test_rig_list_get");

	TCASE_ADD(rig_list_get_normal);
	TCASE_ADD(rig_list_get_error);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_list_peek_normal) {

} END_TEST

START_TEST(test_rig_list_peek_error) {

} END_TEST

Suite *test_rig_list_peek(void) {
	Suite *s = suite_create("test_rig_list_peek");

	TCASE_ADD(rig_list_peek_normal);
	TCASE_ADD(rig_list_peek_error);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_list_iter_begin_normal) {

} END_TEST

START_TEST(test_rig_list_iter_begin_error) {

} END_TEST

Suite *test_rig_list_iter_begin(void) {
	Suite *s = suite_create("test_rig_list_iter_begin");

	TCASE_ADD(rig_list_iter_begin_normal);
	TCASE_ADD(rig_list_iter_begin_error);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_list_iter_end_normal) {

} END_TEST

START_TEST(test_rig_list_iter_end_error) {

} END_TEST

Suite *test_rig_list_iter_end(void) {
	Suite *s = suite_create("test_rig_list_iter_end");

	TCASE_ADD(rig_list_iter_end_normal);
	TCASE_ADD(rig_list_iter_end_error);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_list_iter_next_normal) {

} END_TEST

START_TEST(test_rig_list_iter_next_error) {

} END_TEST

Suite *test_rig_list_iter_next(void) {
	Suite *s = suite_create("test_rig_list_iter_next");

	TCASE_ADD(rig_list_iter_next_normal);
	TCASE_ADD(rig_list_iter_next_error);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_list_iter_delete_normal) {

} END_TEST

START_TEST(test_rig_list_iter_delete_error) {

} END_TEST

Suite *test_rig_list_iter_delete(void) {
	Suite *s = suite_create("test_rig_list_iter_delete");

	TCASE_ADD(rig_list_iter_delete_normal);
	TCASE_ADD(rig_list_iter_delete_error);

	return (s);
}

/******************************************************************************/

START_TEST(test_rig_list_duplicate_normal) {

} END_TEST

START_TEST(test_rig_list_duplicate_error) {

} END_TEST

Suite *test_rig_list_duplicate(void) {
	Suite *s = suite_create("test_rig_list_duplicate");

	TCASE_ADD(rig_list_duplicate_normal);
	TCASE_ADD(rig_list_duplicate_error);

	return (s);
}
