#include "base/base.hpp"
#include "base/memory.hpp"

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
	Slice<f32> s = {};
	printf("srcloc: %zu\n", sizeof(sourcelocation));
}
