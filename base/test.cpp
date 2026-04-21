#include "dyn_array_test.cpp"
#include "slice_test.cpp"
#include "string_test.cpp"
#include "arena_test.cpp"

void base_tests(TestRunner* r){
	tests_add(r, dyn_array_test);
	tests_add(r, slice_test);
	tests_add(r, string_test);
	tests_add(r, arena_test);
}

