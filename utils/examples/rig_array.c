#include <rig.h>

RIG_ARRAY arr = rig_array_init(0); // create empty array

rig_array_insert(arr, // mass insert
	RIG_ASP({"ii"}, {"is"}, {"si"}, {"ss"}),
	0, 3,
	2, STR_LIT("bb"),
	STR_LIT("aa"), 2,
	STR_LIT("c"), STR_LIT("dd11");

RIG s;
rig_array_lookup(arr, RIG_ASP({"is"}), 3, &s); // will return false and put NULL in s
rig_array_lookup(arr, RIG_ASP({"is"}), 2, &s); // will return true and put a copy of 'bb' in s

RIG_ARRAY arr2 = rig_array_init(0); // create second empty array
rig_array_insert(arr2, // mass insert
	RIG_ASP({"ss"}, {"si"}),
	STR_LIT("a1"), STR_LIT("aaa"),
	STR_LIT("b"), 111);

rig_array_insert(arr, RIG_ASP({"ia"}), 9, arr2);

RIG_ARRAY a;
rig_array_lookup(arr, RIG_ASP({"ia"}), 9, &a); // will return true and put a copy of arr2 into a

intmax_t i;
rig_array_lookup(rig_array_nested(arr, RIG_ASP({"i"}), 9), RIG_ASP({"si"}), STR_LIT("b"), &i); // will return true and put the integer in i

rig_array_remove(rig_array_nested(arr, RIG_ASP({"i"}), 9), RIG_ASP({"s"}), STR_LIT("b")); // both this and the previous operate on the original arr2

rig_destroy(&arr);

/*
# Python
arr = {}
arr = {0: 3, 2: "bb", "aa": 2, "c": "dd11"}
arr[3] => KeyError
arr[2] => 'bb'
arr2 = {"a1": "aaa", "b": 111}
arr[9] = arr2
arr[9] => {'a1': 'aaa', 'b': 111}
arr[9]["b"] => 111
del arr[9]["b"]
del arr
*/
