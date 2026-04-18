#pragma once
#include "base.hpp"
#include "memory.hpp"
#include "dyn_array.hpp"

struct StringBuilder {
	DynArray<u8> buf;
};

// Create a new StringBuilder backed by allocator `a`
StringBuilder sb_create(Allocator a);

// Release underlying buffer
void sb_destroy(StringBuilder* sb);

// Reset to empty without releasing memory
void sb_reset(StringBuilder* sb);

// Ensure at least `n` additional bytes can be written without reallocation
bool sb_grow(StringBuilder* sb, usize n);

// Append a single byte
bool sb_write_byte(StringBuilder* sb, u8 b);

// Append raw bytes
bool sb_write_bytes(StringBuilder* sb, Slice<u8> data);

// Append a string
bool sb_write_string(StringBuilder* sb, String s);

// Append a UTF-8 encoded rune
bool sb_write_rune(StringBuilder* sb, rune r);

// Return a String view over the accumulated bytes. Valid until next write or destroy.
String sb_to_string(StringBuilder* sb);

// Number of bytes written so far
static attribute_force_inline
usize len(StringBuilder const& sb){
	return len(sb.buf);
}
