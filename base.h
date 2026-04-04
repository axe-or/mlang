#pragma once

//// Attributes and Compiler specifics
#if __STDC_VERSION__ >= 202311L
	/* Nice, we have native typeof support */
#else
	#if defined(__clang__) || defined(__GNUC__) || defined(_MSC_VER) || defined(__TINYC__)
		#define typeof __typeof__
	#else
		#error "Could not define typeof macro"
	#endif
#endif

#if defined(_MSC_VER)
	#define attribute_force_inline __forceinline
#elif defined(__clang__) || defined(__GNUC__)
	#define attribute_force_inline __attribute__((always_inline))
#else
	#define attribute_force_inline
#endif

#if defined(__clang__) || defined(__GNUC__)
	#define attribute_format(fmt_pos, args_pos) __attribute__((format (printf, fmt_pos, args_pos)))
#else
	#define attribute_format(fmt, args)
#endif

//// Auto platform detection
#if !defined(BUILD_PLATFORM_WINDOWS) && !defined(BUILD_PLATFORM_LINUX)
	#if defined(_WIN32) || defined(_WIN64)
		#define BUILD_PLATFORM_WINDOWS
	#elif defined(__linux__)
		#define BUILD_PLATFORM_LINUX
	#endif
#endif

#if defined(BUILD_PLATFORM_WINDOWS)
	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS
	#endif

	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
#elif defined(BUILD_PLATFORM_LINUX)
	#ifndef _DEFAULT_SOURCE
		#define _DEFAULT_SOURCE
	#endif
#else
	#error "Undefined platform macro"
#endif

#include <stddef.h>
#include <stdarg.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

//// Basic types & Utilities
typedef int8_t i8;
typedef uint8_t u8;

typedef int16_t i16;
typedef uint16_t u16;

typedef int32_t i32;
typedef uint32_t u32;

typedef int64_t i64;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef int32_t rune;
typedef uintptr_t uintptr;

typedef size_t usize;
typedef ptrdiff_t isize;

typedef const char* cstring;

typedef _Atomic(int) AtomicInt;
typedef _Atomic(bool) AtomicBool;

#define min(x, y) (((x) < (y)) ? (x) : (y))

#define max(x, y) (((x) > (y)) ? (x) : (y))

#define clamp(lo, x, hi) min(max((lo), (x)), (hi))

// Helpers that use preprocessor expansion tricks to "glue" identifiers
#define ident_concat0(x, y) x##y
#define ident_concat1(x, y) ident_concat0(x, y)
#define ident_concat2(x, y) ident_concat1(x, y)
#define ident_concat(x, y)  ident_concat2(x, y)
#define ident_counter(x)    ident_concat(x, __COUNTER__)

//// Assertions

// Exit the program fatally
_Noreturn void trap();

_Noreturn void panic_ex(cstring msg, cstring filename, int line);

void ensure_ex(bool pred, cstring msg, cstring filename, int line);

// Exit the program fatally with a message
#define panic(msg) panic_ex((msg), __FILE__, __LINE__)

// Exit the program fatally with a message if a predicate fails
#define ensure(pred, msg) ensure_ex((pred), (msg), __FILE__, __LINE__)

// Same as `ensure` but only when BUILD_DEBUG is defined
#if defined(BUILD_DEBUG)
	#define ensure_debug(pred, msg) ensure_ex((pred), (msg), __FILE__, __LINE__)
#else
	#define ensure_debug(pred, msg)
#endif

// Helper for unimplemented sections of code
#define TODO() panic_ex("TODO", __FILE__, __LINE__)

//// String

// UTF-8 encoded slice of bytes
typedef struct {
	u8 const* v;
	usize len;
} String;

// Helper macro to turn usual C-strings into sized strings
#define $str(s) ((String){.v = (u8 const*)("" s ""), .len = (sizeof(s) - 1)})

// Helper macro to use sized string with printf's "%.*s"
#define $strfmt(S) ((int)((S).len)), ((u8 const*)((S).v))

// Length of a C-style string
static inline
usize cstring_len(cstring cs) {
	usize n = 0;
	while(cs[n] != 0){
		n += 1;
	}
	return n;
}

// The error unicode codepoint
#define RUNE_ERROR ((rune)0xfffd)

// Decoded form of a unicode codepoint
typedef struct {
	rune codepoint;
	u32  size;
} RuneDecoded;

// Encoded form of a unicode codepoint
typedef struct {
	u8  bytes[4];
	u32 size;
} RuneEncoded;

// Encode a codepoint `r` to UTF-8
RuneEncoded rune_encode(rune r);

// Decode the first rune of a UTF-8 encoded buffer
RuneDecoded rune_decode(u8 const* buf, u32 buflen);

// Call vprintf into a buffer returning a UTF-8 string
String buffer_vprintf(u8* buf, usize bufsize, char const* fmt, va_list args);

// Call printf into a buffer returning a UTF-8 string
String buffer_printf(u8* buf, usize bufsize, char const* fmt, ...);

//// Memory

// LibC declarations
extern void* memmove(void* dest, void const* src, size_t n);
extern void* memcpy(void* dest, void const* src, size_t n);
extern void* memset(void* dest, int v, size_t n);
extern int memcmp(void const* lhs, void const* rhs, size_t n);

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

// Common power of 2 prefixes for dealing with memory
#define Mem_Kilobyte (1ll << 10ll)
#define Mem_Megabyte (1ll << 20ll)
#define Mem_Gigabyte (1ll << 30ll)

//// Virtual Memory
enum MemoryProtection {
	MemProt_None = 0,
	MemProt_Read =  (1 << 0),
	MemProt_Write = (1 << 1),
	MemProt_Exec =  (1 << 2),
};

// Protect a piece of vritual memory with a combination of `MemoryProtection` flags
bool virtual_protect(void* ptr, usize size, u8 protection);

// Commits `size` bytes with (Read | Write)
bool virtual_commit(void* base, usize size);

// Decommits `size` bytes of memory from buffer
void virtual_decommit(void* base, usize size);

// Request `size` bytes of memory to be reserved memory
void* virtual_reserve(usize size);

// Release a portion of virtual memory, allowing it to be reused by the OS
void virtual_release(void* base, usize size);

// Init system specific info such as page size
void virtual_init();

// Get system page size
usize virtual_page_size();

//// Allocator
typedef enum {
	Mem_Query   = 0,
	Mem_Alloc   = 1 << 0,
	Mem_Realloc = 1 << 1,
	Mem_Free    = 1 << 2,
	Mem_FreeAll = 1 << 3,
} AllocatorMode;

typedef union {
	void* ptr;
	u8    modes;
} AllocatorResult;

typedef AllocatorResult (*AllocatorFunc)(
	void* impl, u8 mode,
	void* ptr,
	usize old_size, usize old_align, /* Old layout */
	usize new_size, usize new_align /* New Layout */
);

typedef struct {
	void* impl;
	AllocatorFunc fn;
} Allocator;

static inline
void* mem_alloc(Allocator a, usize size, usize align){
	return a.fn(a.impl, Mem_Alloc, NULL, 0, 0, size, align).ptr;
}

static inline
void* mem_realloc(Allocator a, void* ptr, usize old_size, usize old_align, usize new_size, usize new_align){
	return a.fn(a.impl, Mem_Realloc, ptr, old_size, old_align, new_size, new_align).ptr;
}

static inline
void mem_free(Allocator a, void* ptr, usize size, usize align){
	a.fn(a.impl, Mem_Free, ptr, size, align, 0, 0);
}

static inline
void mem_free_all(Allocator a){
	a.fn(a.impl, Mem_FreeAll, NULL, 0, 0, 0, 0);
}

static inline
u8 mem_query(Allocator a){
	return a.fn(a.impl, Mem_FreeAll, NULL, 0, 0, 0, 0).modes;
}

#define mem_make(alloc, type, count) \
    ((type *)mem_alloc((alloc), (count) * sizeof(type), alignof(type)))

#define mem_destroy(alloc, ptr, count) \
    (mem_free((alloc), (ptr), (count) * sizeof(ptr[0]), alignof(typeof(ptr[0]))))

//// Arena
enum ArenaType {
	Arena_Buffer = 0,
	Arena_Virtual = 2,
};

// A contigous allocator that can use virtual memory. An arena represents one
// lifetime of allocations that can be efficiently allocated or deallocated all
// at once
typedef struct {
	void* data;
	usize offset;
	usize commited;
	usize reserved;

	void* last_allocation;
	usize last_allocation_size;

	u32 commit_size;
	u16 region_count;
	u8  type;
} Arena;

const static u32 arena_default_commit_size = 64 * Mem_Kilobyte;

const static u32 arena_default_initial_commit_size = 128 * Mem_Kilobyte;

// Check if pointer is within arena
bool arena_owns(Arena* a, void* p);

// Attempt to resize an allocation to be `new_size` bytes long in-place. Returns if it was successful
bool arena_resize(Arena* a, void* ptr, usize new_size);

// Reallocate a arena allocation, this always tries to optimistically resize
// in-place and falls back to allocation + copy. Returns NULL on failure
void* arena_realloc(Arena* a, void* ptr, usize old_size, usize new_size, usize align);

// Allocate a region of arena. Returns NULL on failure
void* arena_alloc(Arena* a, usize size, usize align);

// Reset an arena
void arena_reset(Arena* a);

// Transform arena into allocator
Allocator arena_allocator(Arena* arena);

// Create a static arena that owns a preallocated buffer
Arena arena_from_buffer(u8* buf, usize bufsize);

// Create a virtual arena that commits pages as required
Arena arena_from_virtual(usize reserve_size, u32 commit_size, u32 initial_commit_size);

// Print using vprintf directly to arena and return result
String arena_vprintf(Arena* arena, char const* fmt, va_list args);

// Print using printf directly to arena and return result
String arena_printf(Arena* arena, char const* fmt, ...);

#define arena_make(arena, type, count) (type *)arena_alloc((arena), sizeof(type) * (count), alignof(type))

// //// Slice (raw)
// typedef struct {
// 	void* data;
// 	usize len;
// } RawSlice;

// static inline
// RawSlice raw_slice_take(RawSlice s, usize count, usize elem_size, cstring file, int line){
// 	ensure_ex(count <= s.len, "cannot take more than slice length", file, line);
// 	return (RawSlice){
// 		.data = s.data,
// 		.len = count,
// 	};
// }

// static inline
// RawSlice raw_slice_skip(RawSlice s, usize count, usize elem_size, cstring file, int line){
// 	ensure_ex(count < s.len, "cannot skip more than slice length", file, line);
// 	return (RawSlice){
// 		.data = (void*)((uintptr)s.data + (count * elem_size)),
// 		.len = s.len - count,
// 	};
// }

// //// Slice (typed)
// #define Slice(T) union { RawSlice slice; T* _tag; }

// #define slice_take(S, N) (typeof(S)){ .slice = raw_slice_take((S).slice, (N), sizeof(*(S)._tag), __FILE__, __LINE__), ._tag = (S)._tag }

// #define slice_skip(S, N) (typeof(S)){ .slice = raw_slice_skip((S).slice, (N), sizeof(*(S)._tag), __FILE__, __LINE__), ._tag = (S)._tag }

//// Array (raw)
typedef struct {
	void*  data;
	usize  len;
	usize  cap;
	Allocator allocator;
} RawArray;

static inline
bool raw_array_resize(RawArray* arr, usize new_cap, usize elem_size, usize elem_align) {
	void* new_data = mem_realloc(arr->allocator,
	    arr->data,
		elem_size * arr->cap, elem_align,
		elem_size * new_cap, elem_align
	);

	if(!new_data){
		return false;
	}
	arr->data = new_data;
	arr->cap = new_cap;
	arr->len = min(arr->len, new_cap);
	return true;
}

static inline
bool raw_array_push(RawArray* arr, void const* elem_data, usize elem_size, usize elem_align){
	if(arr->len >= arr->cap){
		if(!raw_array_resize(arr, max(16, arr->cap * 2), elem_size, elem_align)){
			return false;
		}
	}
	uintptr target = (uintptr)arr->data + (elem_size * arr->len);
	mem_copy_no_overlap((void*)target, elem_data, elem_size);
	arr->len += 1;
	return true;
}

static inline
void* raw_array_at(RawArray arr, usize idx, usize elem_size, cstring file, int line){
	ensure_ex(idx < arr.len, "index out of bounds", file, line);
	return (void*)((uintptr)arr.data + (idx * elem_size));
}

static inline
void* raw_array_at_unchecked(RawArray arr, usize idx, usize elem_size){
	return (void*)((uintptr)arr.data + (idx * elem_size));
}

static inline
void raw_array_remove(RawArray* arr, usize idx, usize elem_size){
	ensure(idx < arr->len, "out of bounds remove");
	if(!arr->len){ return; }

	void* remove = raw_array_at_unchecked(*arr, idx, elem_size);
	void* rest = raw_array_at_unchecked(*arr, idx + 1, elem_size);
	usize shift_elems = (arr->len - idx);

	mem_copy(remove, rest, shift_elems * elem_size);
	arr->len -= 1;
}

static inline
bool raw_array_insert(RawArray* arr, usize idx, void const * elem, usize elem_size, usize elem_align){
	ensure(idx <= arr->len, "out of bounds insert");
	if(arr->len >= arr->cap){
		if(!raw_array_resize(arr, max(16, arr->cap * 2), elem_size, elem_align)){
			return false;
		}
	}

	void* insert = raw_array_at_unchecked(*arr, idx, elem_size);
	void* next = raw_array_at_unchecked(*arr, idx + 1, elem_size);
	usize shift_elems = (arr->len - idx);

	mem_copy(next, insert, shift_elems * elem_size);
	arr->len += 1;
	mem_copy(insert, elem, elem_size);

	return true;
}

static inline
RawArray raw_array_make(Allocator a){
	RawArray array = {
		.data = NULL,
		.len = 0,
		.cap = 0,
		.allocator = a,
	};
	return array;
}

static inline
void raw_array_destroy(RawArray* arr, usize size, usize align){
    if(!arr) return;
    mem_free(arr->allocator, arr->data, size, align);
    arr->cap = 0;
}

//// Array (typed)
#define Array(T) union { RawArray array; T* _tag; }

#define array_len(arr) ((arr).array.len)

#define array_cap(arr) ((arr).array.cap)

#define array_init(arr, allocator) do { \
	(arr)->array = raw_array_make((allocator)); \
} while(0)

#define array_make(allocator) { .array = raw_array_make((allocator)) }

#define array_at(arr, idx) \
	(typeof(arr._tag))(raw_array_at( \
		(arr).array, (idx), sizeof(*(arr)._tag), __FILE__, __LINE__))

#define array_resize(arr, cap) \
	raw_array_resize((arr).array, (cap), sizeof(arr), usize elem_align)

#define array_remove(arr, idx) \
	raw_array_remove(&(arr)->array, idx, sizeof(*(arr)->_tag))

#define array_insert(arr, idx, elem) do { \
	ensure_ex( \
		raw_array_insert( \
			&(arr)->array, (idx), &(typeof((elem))){ (elem) }, \
			sizeof((elem)), alignof(typeof((elem))) \
		), \
		"failed to insert to array", __FILE__, __LINE__ \
	); \
} while(0)

#define array_pop(arr) do { \
	if((arr)->array.len) \
		(arr)->array.len -= 1; \
} while(0)

#define array_push(arr, elem) do { \
	ensure_ex( \
		raw_array_push( \
			&(arr)->array, &(typeof((elem))){ (elem) }, \
			sizeof((elem)), alignof(typeof((elem))) \
		), \
		"failed to push array", __FILE__, __LINE__ \
	); \
} while(0)

//// Spinlock
typedef struct {
	AtomicBool _state;
} Spinlock;

void spin_lock(Spinlock* l);

void spin_unlock(Spinlock* l);

bool spin_try_lock(Spinlock* l);

//// Threads
typedef struct Thread Thread;

enum ThreadFlags {
	Thread_Unitialized = 0,
	Thread_Initialized = (1 << 0),
	Thread_Started     = (1 << 1),
	Thread_Joined      = (1 << 2),
	Thread_Done        = (1 << 3),
	Thread_Cancelled   = (1 << 4),
};

typedef union {
	uintptr _align;
	u8 data[16];
} ThreadPlatformSpecificData;

typedef void (*ThreadFunc)(Thread* t);

struct Thread {
	AtomicInt flags;
	ThreadFunc func;
	void* arg;
	void* result;
	usize result_size;
	Spinlock mtx;

	ThreadPlatformSpecificData _specific;
};

Thread* thread_create(ThreadFunc func, void* arg);

void thread_start(Thread* t);

void thread_join(Thread* t);

void thread_cancel(Thread* t);

void thread_destroy(Thread* t);

bool thread_is_done(Thread* t);

void thread_yield();
