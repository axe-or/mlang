#pragma once
#include "base.hpp"
#include "memory.hpp"
#include "dyn_array.hpp"

constexpr usize max_cutset_rune_count = 64;

struct StringBuilder {
	DynArray<u8> buf;

	StringBuilder() = default;
	explicit StringBuilder(Allocator a) : buf{a} {}

	// Release underlying buffer
	void destroy();

	// Reset to empty without releasing memory
	void reset();

	// Ensure at least `n` additional bytes can be written without reallocation
	bool grow(usize n);

	// Append a single byte
	bool write_byte(u8 b);

	// Append raw bytes
	bool write_bytes(Slice<u8> data);

	// Append a string
	bool write_string(String s);

	// Append a UTF-8 encoded rune
	bool write_rune(rune r);

	// Return a String view over the accumulated bytes. Valid until next write or destroy.
	String to_string() const;

	// Allocate a null-terminated copy of the built string using `a`
	String build(Allocator a) const;
};

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
