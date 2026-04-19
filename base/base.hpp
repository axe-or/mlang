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

template<typename T, typename U = T>
constexpr attribute_force_inline
T min(T x, U y){
	return x < static_cast<T>(y) ? x : static_cast<T>(y);
}

template<typename T, typename U = T>
constexpr attribute_force_inline
T max(T x, U y){
	return x > static_cast<T>(y) ? x : static_cast<T>(y);
}

template<typename T>
constexpr attribute_force_inline
T clamp(T lo, T x, T hi){
	return min(max((lo), (x)), (hi));
}

template<typename A, typename B = A>
struct Pair {
	A a;
	B b;
};

struct Nat {};

void* operator new (size_t, void*, Nat);

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

//// Slice

template<typename T>
struct Slice{
private:
	T* _data;
	usize _len;

public:
	T& operator[](usize idx){
		ensure(idx < _len, "slice index out of bounds");
		return _data[idx];
	}

	T const& operator[](usize idx) const {
		ensure(idx < _len, "slice index out of bounds");
		return _data[idx];
	}

	Slice<T>(): _data{nullptr}, _len{0} {}

	Slice<T>(T* data, usize len): _data{data}, _len{len} {}

	friend attribute_force_inline constexpr
	usize len(Slice<T> s){ return s._len; }

	friend attribute_force_inline constexpr
	T* raw_data(Slice<T> s){ return s._data; }

	[[nodiscard]] friend attribute_force_inline
	Slice<T> take(Slice<T> s, usize n, sourcelocation loc = sourcelocation::current()){
		ensure(n <= s._len, "cannot take more than length", loc);
		return Slice<T>{ s._data, n };
	}

	[[nodiscard]] friend attribute_force_inline
	Slice<T> skip(Slice<T> s, usize n, sourcelocation loc = sourcelocation::current()){
		ensure(n <= s._len, "cannot skip more than length", loc);
		return Slice<T>{ &s._data[n], s._len - n };
	}

	[[nodiscard]] friend attribute_force_inline
	Slice<T> slice(Slice<T> s, usize start, usize end, sourcelocation loc = sourcelocation::current()){
		ensure(end <= s._len && start <= end, "invalid slice indexes", loc);
		return Slice<T>{ &s._data[start], end - start };
	}

	[[nodiscard]] friend attribute_force_inline
	Slice<T> slice(Slice<T> s){
		// NOTE: Idempotent on purpose, mostly for symmetry with DynArray
		return s;
	}
};

extern "C" void* memmove(void*, void const*, usize);

template<typename T>
Slice<T> copy(Slice<T> dst, Slice<T> src){
	usize n = min(len(dst), len(src));
	memmove(raw_data(dst), raw_data(src), n * sizeof(T));
	return Slice<T>{raw_data(dst), n};
}

//// UTF-8
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



//// String

// Simple UTF-8 iterator. This is ugly but required to use for(rune c : s){} syntax
struct UTF8Iterator {
	u8 const* _data;
	usize _len;
	usize _pos;

	rune _current;
	u32 _size;

	rune operator*() const { return _current; }

	UTF8Iterator& operator++(){
		_pos += _size;
		if(_pos < _len){
			RuneDecoded rd = rune_decode(_data + _pos, (u32)(_len - _pos));
			_current = rd.codepoint;
			_size    = rd.size;
		} else {
			_current = 0;
			_size    = 0;
		}
		return *this;
	}

	bool operator!=(UTF8Iterator const& o) const { return _pos != o._pos; }
};

Pair<rune, bool> next(UTF8Iterator* it){
	if(it->_pos >= it->_len){
		return {0, false};
	}
	Pair<rune, bool> result = {it->_current, true};
	++(*it);
	return result;
}

void utf8_iter_init(UTF8Iterator* it, u8 const* data, usize len){
	it->_data = data;
	it->_len  = len;
	it->_pos  = 0;

	if(it->_len > 0){
		RuneDecoded rd = rune_decode(it->_data, (u32)it->_len);
		it->_current = rd.codepoint;
		it->_size    = rd.size;
	} else {
		it->_current = 0;
		it->_size    = 0;
	}
}

struct UTF8RevIterator {
	u8 const* _data;
	usize     _pos;
};

void str_rev_iter_init(UTF8RevIterator* it, u8 const* data, usize len){
	it->_data = data;
	it->_pos  = len;
}

Pair<rune, bool> next(UTF8RevIterator* it){
	if(it->_pos == 0){
		return {0, false};
	}

	usize start = it->_pos - 1;
	while(start > 0 && ((it->_data[start] & 0xc0) == 0x80)){
		start -= 1;
	}

	RuneDecoded rd = rune_decode(it->_data + start, (u32)(it->_pos - start));
	it->_pos = start;

	return {rd.codepoint, true};
}

extern "C" int memcmp(void const*, void const*, size_t);

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

	constexpr
	bool operator==(String o){
		if(o._len != _len){
			return false;
		}
		return memcmp(_data, o._data, _len) == 0;
	}

	constexpr
	bool operator!=(String o){
		if(o._len != _len){
			return false;
		}

		for(usize i = 0; i < _len; i += 1){
			if(o._data[i] != _data[i]){
				return false;
			}
		}

		return true;
	}

	UTF8Iterator begin(){
		UTF8Iterator it = {};
		utf8_iter_init(&it, (u8 const*)_data, _len);
		return it;
	}

	UTF8Iterator end(){
		// TODO: Use Init here
		return UTF8Iterator{ (u8 const*)_data, _len, _len, 0, 0 };
	}

	friend attribute_force_inline constexpr usize       len(String s)      { return s._len; }
	friend attribute_force_inline constexpr char const* raw_data(String s) { return s._data; }

	[[nodiscard]] friend attribute_force_inline
	String take(String s, usize n, sourcelocation loc = sourcelocation::current()){
		ensure(n <= s._len, "cannot take more than length", loc);
		return String{s._data, n};
	}

	[[nodiscard]] friend attribute_force_inline
	String skip(String s, usize n, sourcelocation loc = sourcelocation::current()){
		ensure(n <= s._len, "cannot skip more than length", loc);
		return String{s._data + n, s._len - n};
	}

	[[nodiscard]] friend attribute_force_inline
	String slice(String s, usize start, usize end, sourcelocation loc = sourcelocation::current()){
		ensure(end <= s._len && start <= end, "invalid slice indexes", loc);
		return String{s._data + start, end - start};
	}
};

inline UTF8Iterator str_iterator(String s){
	return s.begin();
}

inline UTF8RevIterator str_rev_iterator(String s){
	UTF8RevIterator it;
	str_rev_iter_init(&it, (u8 const*)raw_data(s), len(s));
	return it;
}

