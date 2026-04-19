#include "base/base.hpp"
#include "base/memory.hpp"
#include "base/virtual.hpp"
#include "base/dyn_array.hpp"
#include "base/arena.hpp"

extern "C" int printf(cstring, ...);

#include "base/testing.hpp"

void foo_test(Test* t){
	t->name = "Foo";

	t_expect(t, 2 + 2 == 4, "Dale");
}

void testmain(){
	auto runner = tests_create();
	tests_add(runner, foo_test);
	tests_run(runner);
}

int main(){
	virtual_init();
	testmain();
	// auto nums = make_dyn_array<f32>(heap_allocator());

	// for(int i = 0; i < 69; i++){
	// 	append(&nums, f32(i) / 2 * 5);
	// }

	// for(int i = 0; i < 69; i++){
	// 	printf("%f\n", nums[i]);
	// }

	// printf(">>> %zu %zu\n", len(nums), cap(nums));

	// String s = "Hellope, 世界";
	// for(rune r : s){
	// 	printf("U+%04x\n", r);
	// }
}


#include "base/base.cpp"
#include "libcpp_shim.cpp"
// TODO: Get rid of this
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
