#include "string.cpp"
#include "arena.cpp"

void* operator new (size_t, void* p, Nat){ return p; }

#if defined(BUILD_PLATFORM_WINDOWS)
	#include "virtual_windows.cpp"
#elif defined(BUILD_PLATFORM_LINUX)
	#include "virtual_linux.cpp"
#endif
