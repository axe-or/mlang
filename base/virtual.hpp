#pragma once

#include "base.hpp"

//// Virtual Memory
enum MemoryProtection : u8 {
	MemProt_None  = 0,
	MemProt_Read  = (1 << 0),
	MemProt_Write = (1 << 1),
	MemProt_Exec  = (1 << 2),
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
