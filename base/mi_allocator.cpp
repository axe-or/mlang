#include "memory.hpp"

#include "mimalloc.h"

static
AllocatorResult mimalloc_allocator_func(
	void*,
	AllocatorMode mode,
	void* ptr,
	usize, usize, /* Old layout */
	usize new_size, usize new_align /* New Layout */
){
	AllocatorResult res = {.ptr = nullptr};

	switch(mode){
	case Mem_Query:
		res.modes = Mem_Alloc | Mem_Realloc | Mem_Free;
		break;

	case Mem_Alloc:
		res.ptr = mi_zalloc_aligned(new_size, new_align);
		break;

	case Mem_Realloc:
		res.ptr = mi_realloc_aligned(ptr, new_size, new_align);
		break;

	case Mem_Free:
		mi_free(ptr);
		break;

	case Mem_FreeAll:
		/* Unsupported */
		break;
	}

	return res;
}

Allocator heap_allocator(){
	return { nullptr, mimalloc_allocator_func };
}
