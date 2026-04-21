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

	// Create a static arena that owns a preallocated buffer
	static Arena from_buffer(u8* buf, usize bufsize);

	// Create a virtual arena that commits pages as required
	static Arena from_virtual(usize reserve_size, u32 commit_size = arena_default_commit_size, u32 initial_commit_size = arena_default_initial_commit_size);

	// Release arena and its resources
	void destroy();

	// Reset an arena
	void reset();

	// Check if pointer is within arena
	bool owns(void* p) const;

	// Allocate a region of arena. Returns NULL on failure
	void* alloc(usize size, usize align);

	// Attempt to resize an allocation to be `new_size` bytes long in-place. Returns if it was successful
	bool resize(void* ptr, usize new_size);

	// Reallocate a arena allocation, this always tries to optimistically resize
	// in-place and falls back to allocation + copy. Returns NULL on failure
	void* realloc(void* ptr, usize old_size, usize new_size, usize align);

	// Get arena as an allocator-compliant interface
	Allocator allocator();

private:
	bool virtual_grow(usize size);
	void virtual_decommit(usize size);
};
