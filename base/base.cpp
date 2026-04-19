#include "string.cpp"
#include "arena.cpp"
#include "error_allocator.cpp"

extern "C" int printf(cstring, ...);

void* operator new (size_t, void* p, Nat){ return p; }

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

#if defined(BUILD_PLATFORM_WINDOWS)
	#include "virtual_windows.cpp"
#elif defined(BUILD_PLATFORM_LINUX)
	#include "virtual_linux.cpp"

	#if !defined(BUILD_USE_MIMALLOC)
		#include "heap_linux.cpp"
	#endif
#endif

#if defined(BUILD_USE_MIMALLOC)
	#include "mi_allocator.cpp"
#endif
