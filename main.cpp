#include "base/base.hpp"
#include "base/memory.hpp"
#include "base/virtual.hpp"
#include "base/dyn_array.hpp"
#include "base/arena.hpp"
#include "base/map.hpp"

extern "C" int printf(cstring, ...);

int main(){
	virtual_init();
	auto allocator = Arena::from_virtual(2 * mem_gigabyte);



}

//// Single library TU
#include "base/base.cpp"
#include "base/libcpp_shim.cpp"
// TODO: Get rid of this
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
