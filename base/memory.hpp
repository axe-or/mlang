#pragma once
#include "base.hpp"

// LibC declarations
extern "C" {
void* memmove(void* dest, void const* src, size_t n);
void* memcpy(void* dest, void const* src, size_t n);
void* memset(void* dest, int v, size_t n);
int memcmp(void const* lhs, void const* rhs, size_t n);
}

// Copy `n` bytes of `src` to `dest`
attribute_force_inline static
void* mem_copy(void* dest, void const* src, isize n){
	return memmove(dest, src, n);
}

// Same as mem_copy but ONLY for regions that are known not to overlap, this is usually faster than mem_copy
attribute_force_inline static
void* mem_copy_no_overlap(void* dest, void const* src, isize n){
	return memcpy(dest, src, n);
}

// Set `n` bytes of `dest` to `v`
attribute_force_inline static
void* mem_set(void* dest, u8 v, isize n){
	return memset(dest, v, n);
}

// Shorthand for using `mem_set` with v = 0
attribute_force_inline static
void* mem_zero(void* dest, isize n){
	return memset(dest, 0, n);
}

// Compare `n` bytes of memory regions lexicographically. Zero means regions are equal
attribute_force_inline static
isize mem_compare(void const* lhs, void const* rhs, isize n){
	return memcmp(lhs, rhs, n);
}

// Check if value can be used as memory alignment (i.e. non zero power of 2)
static inline
bool mem_valid_alignment(usize align){
	return align && ((align & (align - 1)) == 0);
}

// Align a pointer to a alignment. Alignment MUST be a power of 2
static inline
uintptr mem_align_forward_ptr(uintptr p, uintptr a){
	ensure(mem_valid_alignment(a), "Alignment must be a power of 2 greater than 0");
	uintptr mod = p & (a - 1); /* Fast modulo for powers of 2 */
	if(mod > 0){
		p += (a - mod);
	}
	return p;
}

constexpr usize mem_kilobyte = 1ll << 10ll;

constexpr usize mem_megabyte = 1ll << 20ll;

constexpr usize mem_gigabyte = 1ll << 30ll;

//// Allocator
enum AllocatorMode : u8 {
	Mem_Query   = 0,
	Mem_Alloc   = 1 << 0,
	Mem_Realloc = 1 << 1,
	Mem_Free    = 1 << 2,
	Mem_FreeAll = 1 << 3,
};

union AllocatorResult {
	void* ptr;
	u8    modes;
};

using AllocatorFunc = AllocatorResult (*)(
	void* impl, u8 mode,
	void* ptr,
	usize old_size, usize old_align, /* Old layout */
	usize new_size, usize new_align /* New Layout */
);

struct Allocator {
	void* _impl;
	AllocatorFunc _fn;

	void* alloc(usize size, usize align){
		return _fn(_impl, Mem_Alloc, NULL, 0, 0, size, align).ptr;
	}

	void* realloc(void* ptr, usize old_size, usize old_align, usize new_size, usize new_align){
		return _fn(_impl, Mem_Realloc, ptr, old_size, old_align, new_size, new_align).ptr;
	}

	void free(void* ptr, usize size, usize align){
		_fn(_impl, Mem_Free, ptr, size, align, 0, 0);
	}

	void free_all(){
		_fn(_impl, Mem_FreeAll, NULL, 0, 0, 0, 0);
	}

	u8 query(){
		return _fn(_impl, Mem_FreeAll, NULL, 0, 0, 0, 0).modes;
	}
};

template<typename T>
T* make(Allocator a){
	T* p = (T*)a.alloc(sizeof(T), alignof(T));
	new (p, Nat{}) T{};
	return p;
}
