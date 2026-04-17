#include "string.cpp"

#if defined(BUILD_PLATFORM_WINDOWS)
	#include "virtual_windows.cpp"
#elif defined(BUILD_PLATFORM_LINUX)
	#include "virtual_linux.cpp"
#endif
