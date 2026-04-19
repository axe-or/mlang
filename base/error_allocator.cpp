#include "memory.hpp"

static
AllocatorResult null_allocator_func(
	void*, AllocatorMode,
	void*,
	usize, usize,
	usize, usize
){
	return {};
}

static
AllocatorResult panic_allocator_func(
	void*, AllocatorMode,
	void*,
	usize, usize,
	usize, usize
){
	panic("attempt to use panic allocator");
}

Allocator null_allocator(){
	return {
		nullptr,
		null_allocator_func,
	};
}

Allocator panic_allocator(){
	return {
		nullptr,
		panic_allocator_func,
	};
}
