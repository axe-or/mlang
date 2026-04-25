#include "dyn_array_test.cpp"
#include "slice_test.cpp"
#include "string_test.cpp"
#include "arena_test.cpp"
#include "bitset_test.cpp"
#include "static_array_test.cpp"

void base_tests(TestRunner* r){
	tests_add(r, dyn_array_test);
	tests_add(r, slice_test);
	tests_add(r, string_test);
	tests_add(r, arena_test);
	tests_add(r, bitset_test);
	tests_add(r, static_array_test);
}

