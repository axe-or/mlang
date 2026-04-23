#include "base/base.hpp"
#include "base/memory.hpp"
#include "base/virtual.hpp"
#include "base/dyn_array.hpp"
#include "base/arena.hpp"
#include "base/map.hpp"
#include "base/bitset.hpp"

extern "C" int printf(cstring, ...);

template<u32 N>
void print(BitSet<N> const& s){
	printf("(%d:%d)[ ", N, s.slot_count);
	for(usize i = 0; i < N; i += 1){
		bool b = s.get(i);
		if(b) printf("\033[0;33m%d\033[0m", int(b));
		else printf("%d", int(b));

		if(i && ((i+1) % BitSet<N>::slot_width == 0) && (i+1 != N))
			printf(".");
	}
	printf(" ]\n");
}

int main(){
	virtual_init();
	auto allocator = Arena::from_virtual(2 * mem_gigabyte);
	BitSet<80> set;
	print(set);

	set.set(3, true);
	set.set(8, true);
	set.set(9, true);
	set.set(15, true);
	set.set(0, true);
	set.set(42, true);
	set.set(69, true);
	set.set(42, false);

	print(set);
	print(~set);
}

//// Single library TU
#include "base/base.cpp"
#include "base/libcpp_shim.cpp"
// TODO: Get rid of this
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
