#pragma once
#include "base.hpp"
#include "memory.hpp"
#include "dyn_array.hpp"

constexpr usize max_cutset_rune_count = 64;

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

// Allocate a copy of `s` using `a`
String str_clone(String s, Allocator a);

// First occurrence of `needle` in `haystack`, -1 on failure
i64 str_find(String haystack, String needle);

// Strip `prefix` from start of `s` if present
String str_trim_prefix(String s, String prefix);

// Strip `suffix` from end of `s` if present
String str_trim_suffix(String s, String suffix);

// Strip leading runes found in `cutset`
String str_trim_start(String s, String cutset);

// Strip trailing runes found in `cutset`
String str_trim_end(String s, String cutset);

// Strip leading and trailing runes found in `cutset`
String str_trim(String s, String cutset);

// Strip leading and trailing whitespace
String str_trim_spaces(String s);

// Split `target` on each occurrence of `sep`. Strings are allocated with allocator
Slice<String> str_split(String target, String sep, Allocator a);

// Replace up to `count` occurrences of `pattern` with `replacement`; count=0 replaces all
String str_replace(String s, String pattern, String replacement, Allocator a, usize count = 0);

