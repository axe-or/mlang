#include "base.h"

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

//// Assertions
extern void abort();
extern int printf(cstring, ...);

#include <stdio.h>
#define dbg(FMT, ...) do { \
    printf("[(%s) %s:%d] " FMT "\n", __func__, __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__); \
    fflush(stdout); \
} while(0)

_Noreturn void trap(){
	abort();
	while(1);
}

_Noreturn void panic_ex(cstring msg, cstring filename, int line){
	printf("(%s:%d) panic: %s\n", filename, line, msg);
	trap();
}

void ensure_ex(bool pred, cstring msg, cstring filename, int line){
	if(!pred){
		printf("(%s:%d) assertion failed: %s\n", filename, line, msg);
		trap();
	}
}

//// Arena
#define ARENA_COMMIT_SIZE (16 * Mem_Kilobyte)

void arena_reset(Arena* a){
	ensure(a->region_count == 0, "Arena has dangling regions");
	a->offset = 0;
	a->last_allocation = NULL;
}

bool arena_owns(Arena* a, void* p){
	uintptr ptr = (uintptr)p;
	uintptr lo = (uintptr)a->data;
	uintptr hi = lo + a->reserved;

	return (ptr >= lo) && (ptr <= hi);
}

bool arena_virtual_grow(Arena* a, usize size){
	ensure(a->type == Arena_Virtual, "not a virtual arena");
	ensure(a->commit_size >= virtual_page_size(), "invalid arena");

	if(a->commited >= a->reserved){
		return false;
	}
	size = mem_align_forward_ptr(size, virtual_page_size());

	uintptr base = (uintptr)a->data;
	uintptr start = base + a->commited;
	uintptr end = base + a->reserved;
	uintptr remaining = end - start;

	if(size > remaining){
    	return false;
	}

	bool ok = virtual_commit((void*)start, size);
	ensure(ok, "failed to commit memory");

	dbg("cleaning commited memory");
	mem_set((void*)start, 69, size);

	a->commited += size;

	return ok;
}

void arena_virtual_decommit(Arena* a, usize size){
	ensure(a->type == Arena_Virtual, "Not a virtual arena");

	size = min(size, a->commited);
	virtual_decommit(a->data, size);
	a->commited = a->commited - size;
}

void* arena_alloc(Arena* a, usize size, usize align){
	if(size == 0){ return NULL; }
	uintptr base = (uintptr)a->data;
	uintptr current = base + (uintptr)a->offset;

	usize available = a->commited - (current - base);

	uintptr aligned  = mem_align_forward_ptr(current, align);
	uintptr padding  = aligned - current;
	uintptr required = padding + size;

	if(required > available){
		if(a->type == Arena_Buffer){
			return NULL;
		}

		usize to_commit = max(size, a->commit_size);
		if(!arena_virtual_grow(a, to_commit)){
			return NULL;
		}
		void* res =  arena_alloc(a, size, align);
		return res;
	}

	a->offset += required;
	void* allocation = (void*)aligned;

	mem_zero(allocation, size);

	a->last_allocation = allocation;
	a->last_allocation_size = size;

	return allocation;
}

void* arena_realloc(Arena* a, void* ptr, usize old_size, usize new_size, usize align){
	if(ptr == NULL){
    	void* res = arena_alloc(a, new_size, align);
		return res;
	}
	ensure(arena_owns(a, ptr), "Pointer not owned by arena");

	bool in_place = arena_resize(a, ptr, new_size);
	if(in_place){
		return ptr;
	}
	else {
		void* new_data = arena_alloc(a, new_size, align);
		if(new_data == NULL){ return NULL; } /* Out of memory */
		mem_copy(new_data, ptr, min(old_size, new_size));

		if(new_size > old_size){
			usize diff = new_size - old_size;
			mem_zero((u8*)new_data + old_size, diff);
		}

		return new_data;
	}
}

bool arena_resize(Arena* a, void* ptr, usize new_size){
	if(ptr == NULL){
    	return false;
	}
	ensure(arena_owns(a, ptr), "Pointer not owned by arena");

	uintptr base = (uintptr)a->data;

	if(ptr == a->last_allocation){
		uintptr last_alloc = (uintptr)a->last_allocation;
		uintptr can_commit = a->reserved - a->commited;

		if((last_alloc + new_size) > (base + a->commited)){
			if(a->type == Arena_Buffer){
				return false; /* No space left, cannot grow */
			}
			else if(new_size <= can_commit) {
				bool ok = arena_virtual_grow(a, mem_align_forward_ptr(new_size, a->commit_size));
				if(!ok){
					return false; /* Failed to grow arena */
				}
				return arena_resize(a, ptr, new_size);
			}
			else {
				return false;
			}
		}

		usize old_offset = a->offset;
		usize old_size = a->last_allocation_size;

		a->offset = (last_alloc + new_size) - base;
		a->last_allocation_size = new_size;

		if(new_size > old_size){
			usize diff = new_size - old_size;
			uintptr p = last_alloc + old_size;
			mem_zero((void*)p, diff);
		}
		return true;
	}

	return false;
}

Arena arena_from_buffer(u8* buf, usize bufsize){
	Arena a = {
		.data = (void*)buf,
		.offset = 0,
		.commited = bufsize,
		.reserved = bufsize,
		.type = Arena_Buffer,
		.last_allocation = NULL,
		.region_count = 0,
	};
	return a;
}

Arena arena_from_virtual(usize reserve_size, u32 commit_size, u32 initial_commit_size){
	if(commit_size == 0)
		commit_size = arena_default_commit_size;

	ensure(mem_valid_alignment(commit_size), "invalid commit size. expected a power of 2");
	ensure(commit_size >= virtual_page_size(), "commit size too small");

	void* data = virtual_reserve(reserve_size);
	ensure(data != NULL, "failed to reserve memory");
	Arena a = {
		.data = data,
		.offset = 0,
		.commited = 0,
		.reserved = reserve_size,
		.type = Arena_Virtual,
		.last_allocation = NULL,
		.region_count = 0,
		.commit_size = commit_size,
	};

	if(initial_commit_size != 0){
		arena_virtual_grow(&a, min(initial_commit_size, reserve_size));
	}

	return a;
}

void arena_destroy(Arena* a){
	arena_reset(a);
	if(a->type == Arena_Virtual){
		virtual_release(a->data, a->reserved);
	}
}

static
AllocatorResult arena_allocator_func(
	void* impl, u8 mode,
	void* ptr,
	usize old_size, usize old_align, /* Old layout */
	usize new_size, usize new_align /* New Layout */
){

    Arena* a = (Arena*)impl;
    AllocatorResult res = {.ptr = NULL};

    switch((AllocatorMode)mode){
    case Mem_Query:
        res.modes = Mem_Alloc | Mem_FreeAll | Mem_Realloc;
    break;

    case Mem_Alloc:
        res.ptr = arena_alloc(a, new_size, new_align);
    break;

    case Mem_Realloc:
        ensure(old_align == new_align, "unsupported");
        res.ptr = arena_realloc(a, ptr, old_size, new_size, new_align);
    break;

    case Mem_Free:
        /* Unsupported */
    break;

    case Mem_FreeAll:
        arena_reset(a);
    break;
    }

    return res;
}

Allocator arena_allocator(Arena* a){
    return (Allocator){
        .fn = arena_allocator_func,
        .impl = a,
    };
}




//// String
#define MASKX 0x3f /* 0011_1111 */
#define MASK2 0x1f /* 0001_1111 */
#define MASK3 0x0f /* 0000_1111 */
#define MASK4 0x07 /* 0000_0111 */

#define CONT_LO 0x80
#define CONT_HI 0xbf

struct UTF8AcceptRange { u8 lo, hi; };

static const
struct UTF8AcceptRange utf8_accept_ranges[5] = {
	{0x80, 0xbf},
	{0xa0, 0xbf},
	{0x80, 0x9f},
	{0x90, 0xbf},
	{0x80, 0x8f},
};

static const u8 utf8_accept_sizes[256] = {
	0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,
	0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,
	0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,
	0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,
	0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,
	0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,
	0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,
	0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,
	0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,
	0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,
	0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,
	0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,
	0xf1,0xf1,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
	0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
	0x13,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x23,0x03,0x03,
	0x34,0x04,0x04,0x04,0x44,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,0xf1,
};

RuneDecoded rune_decode(u8 const* buf, u32 buflen){
	RuneDecoded result = {};
	const RuneDecoded error = { .codepoint = RUNE_ERROR, .size = 1 };

	if(buflen < 1){
		return result;
	}

	u8 b0 = buf[0];
	u8 x = utf8_accept_sizes[b0];

	// ASCII or invalid
	if(x >= 0xf0){
		u32 mask = ((rune)(x) << 31) >> 31; // Either all 0's or all 1's to avoid branching
		result.codepoint = ((rune)(b0) & ~mask) | (RUNE_ERROR & mask);
		result.size = 1;
		return result;
	}

	u8 sz = x & 7;
	struct UTF8AcceptRange accept = utf8_accept_ranges[x >> 4];

	if(buflen < sz){
		return error;
	}

	u8 b1 = buf[1];
	if(b1 < accept.lo || accept.hi < b1){
		return error;
	}
	if(sz == 2){
		result.codepoint = ((rune)(b0 & MASK2) << 6) | ((rune)(b1 & MASKX));
		result.size = 2;
		return result;
	}

	u8 b2 = buf[2];
	if(b2 < CONT_LO || CONT_HI < b2){
		return error;
	}

	if(sz == 3){
		result.codepoint = ((rune)(b0 & MASK3) << 12) | ((rune)(b1 & MASKX) << 6) | (rune)(b2 & MASKX);
		result.size = 3;
		return result;
	}

	u8 b3 = buf[3];
	if(b3 < CONT_LO || CONT_HI < b3){
		return error;
	}

	result.codepoint = ((rune)(b0 & MASK4) << 18) | ((rune)(b1 & MASKX) << 12) | ((rune)(b2 & MASKX) << 6) | (rune)(b3 & MASKX);
	result.size = 4;
	return result;
}

RuneEncoded rune_encode(rune r){
	const u8 mask = 0x3f;
	RuneEncoded result = {};

	if(r <= 0x7f){ // 1-wide (ASCII)
		return (RuneEncoded){ .bytes = {(u8)r}, .size = 1 };
	}

	if(r <= 0x7ff){ // 2-wide
		result.bytes[0] = 0xc0 |  (u8)(r >> 6);
		result.bytes[1] = 0x80 | ((u8)(r) & mask);
		result.size = 2;
		return result;
	}

	// Surrogate or invalid -> Encode the error rune
	if((r > 0x10ffff) || ((0xd800 <= r) && (r <= 0xdfff))){
		r = 0xfffd;
	}

	if(r <= 0xffff){ // 3-wide
		result.bytes[0] = 0xe0 |  (u8)(r >> 12);
		result.bytes[1] = 0x80 | ((u8)(r >> 6) & mask);
		result.bytes[2] = 0x80 | ((u8)(r)      & mask);
		result.size = 3;
		return result;
	}
	else { // 4-wide
		result.bytes[0] = 0xf0 |  (u8)(r >> 18);
		result.bytes[1] = 0x80 | ((u8)(r >> 12) & mask);
		result.bytes[2] = 0x80 | ((u8)(r >> 6)  & mask);
		result.bytes[3] = 0x80 | ((u8)(r)       & mask);
		result.size = 4;
		return result;
	}
}

String buffer_vprintf(u8* buf, usize bufsize, char const* fmt, va_list args){
	int n = stbsp_vsnprintf((char*)buf, (int)bufsize, fmt, args);
	if(n > 0){
		return (String){.v = (u8 const*)buf, .len = n};
	}
	return (String){0};
}

String buffer_printf(u8* buf, usize bufsize, char const* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	String res = buffer_vprintf(buf, bufsize, fmt, args);
	va_end(args);
	return res;
}

// TODO: Ensure arena has at LEAST `fmt` bytes committed and preferrably `fmt * 2` bytes
String arena_vprintf(Arena* arena, char const* fmt, va_list args){
	u8* base = (u8*)((uintptr)arena->data + arena->offset);
	usize size = arena->commited - arena->offset;
	if(!size){
		return (String){0};
	}

	String res = buffer_vprintf(base, size, fmt, args);
	arena->offset += res.len;

	return res;
}

String arena_printf(Arena* arena, char const* fmt, ...){
	va_list args;
	va_start(args, fmt);
	String res = arena_vprintf(arena, fmt, args);
	va_end(args);
	return res;
}

//// Spinlock
void spin_lock(Spinlock* l){
	while(1){
		// Optimistically try an exchange, otherwhise spin on loading
		if(!atomic_exchange_explicit(&l->_state, true, memory_order_acquire)){
			break;
		}
		while(atomic_load_explicit(&l->_state, memory_order_relaxed)){}
	}
}

void spin_unlock(Spinlock* l){
	atomic_store_explicit(&l->_state, false, memory_order_release);
}

bool spin_try_lock(Spinlock* l){
    return !atomic_load_explicit(&l->_state, memory_order_relaxed)
		&& !atomic_exchange_explicit(&l->_state, true, memory_order_acquire);
}

// Platform specifics
#if defined(BUILD_PLATFORM_WINDOWS)
	#include "base_windows.c"
#elif defined(BUILD_PLATFORM_LINUX)
	#include "base_linux.c"
#endif
