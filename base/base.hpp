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
	#define attribute_force_inline __attribute__((always_inline)) inline
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

//// Core includes

#include <stddef.h>
#include <stdarg.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdbool.h>
#include <source_location>

//// Basic types & Utilities
using i8 = int8_t;
using u8 = uint8_t;

using i16 = int16_t;
using u16 = uint16_t;

using i32 = int32_t;
using u32 = uint32_t;

using i64 = int64_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

using rune = int32_t;
using uintptr = uintptr_t;

using usize = size_t;
using isize = ptrdiff_t;

using cstring = const char*;

using sourcelocation = std::source_location;

template<typename T>
constexpr attribute_force_inline
T min(T x, T y){
	return x < y ? x : y;
}

template<typename T>
constexpr attribute_force_inline
T max(T x, T y){
	return x > y ? x : y;
}

template<typename T>
constexpr attribute_force_inline
T clamp(T lo, T x, T hi){
	return min(max((lo), (x)), (hi));
}

// Helpers that use preprocessor expansion tricks to "glue" identifiers
#define ident_concat0(x, y) x##y
#define ident_concat1(x, y) ident_concat0(x, y)
#define ident_concat2(x, y) ident_concat1(x, y)
#define ident_concat(x, y)  ident_concat2(x, y)
#define ident_counter(x)    ident_concat(x, __COUNTER__)

//// Assertions

// Exit the program fatally
[[noreturn]]
void trap();

[[noreturn]]
void panic(cstring msg, sourcelocation loc = sourcelocation::current());

void ensure(bool predicate, cstring msg, sourcelocation loc = sourcelocation::current());

[[noreturn]]
static inline
void unimplemented(sourcelocation loc = sourcelocation::current()){
	panic("unimplemented code", loc);
}

// Length of a C-style string
constexpr static inline
usize cstring_len(cstring cs) {
	usize n = 0;
	while(cs[n] != 0){
		n += 1;
	}
	return n;
}

struct Nat {};

void* operator new (size_t, void*, Nat);

//// Slice

template<typename T>
struct Slice{
private:
	T* _data;
	usize _len;

public:
	attribute_force_inline
	Slice<T> take(usize n){
		ensure(n <= _len, "cannot take more than length");
		return Slice<T>{
			._data = _data,
			._len = n,
		};
	}

	attribute_force_inline
	Slice<T> skip(usize n){
		ensure(n <= _len, "cannot skip more than length");
		return Slice<T>{
			._data = &_data[n],
			._len = _len - n,
		};
	}

	attribute_force_inline
	Slice<T> slice(usize start, usize end){
		ensure(end <= _len && start <= end, "invalid slice indexes");
		return Slice<T>{
			._data = &_data[start],
			._len = end - start,
		};
	}

	T& operator[](usize idx){
		ensure(idx < _len, "index out of bounds");
		return _data[idx];
	}

	T const& operator[](usize idx) const {
		ensure(idx < _len, "index out of bounds");
		return _data[idx];
	}

	static Slice<T> from(T* p, usize len){
		return Slice<T>{
			._data = p,
			._len = len,
		};
	}

	attribute_force_inline constexpr auto len() const { return _len; }
	attribute_force_inline constexpr auto raw_data() const { return _data; }
};

//// String
struct String {
private:
	char const* _data;
	usize _len;

public:
	constexpr
	String() : _data{nullptr}, _len{0} {}

	constexpr
	String(char const* s) : _data{s}, _len{cstring_len(s)} {}

	explicit constexpr
	String(char const* s, usize n) : _data{s}, _len{n} {}

	attribute_force_inline constexpr auto len() const { return _len; }
	attribute_force_inline constexpr auto raw_data() const { return _data; }
};

// The error unicode codepoint
constexpr rune rune_error = 0xfffd;

// Decoded form of a unicode codepoint
struct RuneDecoded {
	rune codepoint;
	u32  size;
};

// Encoded form of a unicode codepoint
struct RuneEncoded {
	u8  bytes[4];
	u32 size;
};

// Encode a codepoint `r` to UTF-8
RuneEncoded rune_encode(rune r);

// Decode the first rune of a UTF-8 encoded buffer
RuneDecoded rune_decode(u8 const* buf, u32 buflen);


