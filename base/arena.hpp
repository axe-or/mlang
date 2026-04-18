#pragma once

//// Arena
enum ArenaType : u8 {
	Arena_Buffer = 0,
	Arena_Virtual = 2,
};

constexpr u32 arena_default_commit_size = 64 * mem_kilobyte;

constexpr u32 arena_default_initial_commit_size = 128 * mem_kilobyte;

// A contigous allocator that can use virtual memory. An arena represents one
// lifetime of allocations that can be efficiently allocated or deallocated all
// at once
struct Arena {
	void* data;
	usize offset;
	usize commited;
	usize reserved;

	void* last_allocation;
	usize last_allocation_size;

	u32 commit_size;
	u16 region_count;
	u8  type;
};

// Create a static arena that owns a preallocated buffer
Arena arena_from_buffer(u8* buf, usize bufsize);

// Create a virtual arena that commits pages as required
Arena arena_from_virtual(usize reserve_size, u32 commit_size = arena_default_commit_size, u32 initial_commit_size = arena_default_initial_commit_size);

// Release arena and its resources
void arena_destroy(Arena* a);

// Reset an arena
void arena_reset(Arena* a);

// Check if pointer is within arena
bool arena_owns(Arena* a, void* p);

// Allocate a region of arena. Returns NULL on failure
void* arena_alloc(Arena* a, usize size, usize align);

// Attempt to resize an allocation to be `new_size` bytes long in-place. Returns if it was successful
bool arena_resize(Arena* a, void* ptr, usize new_size);

// Reallocate a arena allocation, this always tries to optimistically resize
// in-place and falls back to allocation + copy. Returns NULL on failure
void* arena_realloc(Arena* a, void* ptr, usize old_size, usize new_size, usize align);

// Get arena as an allocator-compliant interface
Allocator arena_allocator(Arena* a);
