#include "string.cpp"
#include "arena.cpp"
#include "error_allocator.cpp"

void* operator new (size_t, void* p, Nat){ return p; }

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
