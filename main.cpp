#include "base/base.hpp"
#include "base/memory.hpp"
#include "base/virtual.hpp"
#include "base/dyn_array.hpp"
#include "base/arena.hpp"

extern "C" int printf(cstring, ...);

[[noreturn]]
void trap(){
	do { __builtin_trap(); } while(1);
}

[[noreturn]]
void panic(cstring msg, sourcelocation loc){
	printf("(%s:%d) panic: %s\n", loc.file_name(), loc.line(), msg);
		trap();
}

void ensure(bool predicate, cstring msg, sourcelocation loc) {
	if(!predicate){
		printf("(%s:%d) panic: %s\n", loc.file_name(), loc.line(), msg);
		trap();
	}
}

int main(){
	virtual_init();
	auto arena = arena_from_virtual(512 * mem_megabyte);
	auto allocator = arena_allocator(&arena);

	auto nums = make_dyn_array<f32>(allocator);

	for(int i = 0; i < 20; i++){
		append(&nums, f32(i) / 2 * 5);
		printf("%f\n", nums[i]);
	}
}


#include "base/base.cpp"
#include "libcpp_shim.cpp"
// TODO: Get rid of this
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
