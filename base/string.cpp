#include "base.hpp"
#include "string.hpp"
#include "static_array.hpp"
#include "stb_sprintf.h"

//// String
constexpr u32 maskx = 0x3f; /* 0011_1111 */
constexpr u32 mask2 = 0x1f; /* 0001_1111 */
constexpr u32 mask3 = 0x0f; /* 0000_1111 */
constexpr u32 mask4 = 0x07; /* 0000_0111 */

constexpr u32 cont_lo = 0x80;
constexpr u32 cont_hi = 0xbf;

struct UTF8AcceptRange { u8 lo, hi; };

static const
struct UTF8AcceptRange utf8_accept_ranges[5] = {
	{0x80, 0xbf},
	{0xa0, 0xbf},
	{0x80, 0x9f},
	{0x90, 0xbf},
	{0x80, 0x8f},
};

constexpr
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
	const RuneDecoded error = { .codepoint = rune_error, .size = 1 };

	if(buflen < 1){
		return result;
	}

	u8 b0 = buf[0];
	u8 x = utf8_accept_sizes[b0];

	// ASCII or invalid
	if(x >= 0xf0){
		u32 mask = ((rune)(x) << 31) >> 31; // Either all 0's or all 1's to avoid branching
		result.codepoint = ((rune)(b0) & ~mask) | (rune_error & mask);
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
		result.codepoint = ((rune)(b0 & mask2) << 6) | ((rune)(b1 & maskx));
		result.size = 2;
		return result;
	}

	u8 b2 = buf[2];
	if(b2 < cont_lo || cont_hi < b2){
		return error;
	}

	if(sz == 3){
		result.codepoint = ((rune)(b0 & mask3) << 12) | ((rune)(b1 & maskx) << 6) | (rune)(b2 & maskx);
		result.size = 3;
		return result;
	}

	u8 b3 = buf[3];
	if(b3 < cont_lo || cont_hi < b3){
		return error;
	}

	result.codepoint = ((rune)(b0 & mask4) << 18) | ((rune)(b1 & maskx) << 12) | ((rune)(b2 & maskx) << 6) | (rune)(b3 & maskx);
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
		return String((char const*)buf, n);
	}
	return String{};
}

String buffer_printf(u8* buf, usize bufsize, char const* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	String res = buffer_vprintf(buf, bufsize, fmt, args);
	va_end(args);
	return res;
}

// // TODO: Ensure arena has at LEAST `fmt` bytes committed and preferrably `fmt * 2` bytes
// String arena_vprintf(Arena* arena, char const* fmt, va_list args){
// 	u8* base = (u8*)((uintptr)arena->data + arena->offset);
// 	usize size = arena->commited - arena->offset;
// 	if(!size){
// 		return String{};
// 	}

// 	String res = buffer_vprintf(base, size, fmt, args);
// 	arena->offset += res.len();

// 	return res;
// }

// String arena_printf(Arena* arena, char const* fmt, ...){
// 	va_list args;
// 	va_start(args, fmt);
// 	String res = arena_vprintf(arena, fmt, args);
// 	va_end(args);
// 	return res;
// }

//// String Builder

StringBuilder sb_create(Allocator a){
	return StringBuilder{ DynArray<u8>{a} };
}

void sb_destroy(StringBuilder* sb){
	destroy(&sb->buf);
}

void sb_reset(StringBuilder* sb){
	clear(&sb->buf);
}

bool sb_grow(StringBuilder* sb, usize n){
	usize needed = len(sb->buf) + n;
	if(needed <= cap(sb->buf)){
		return true;
	}
	return resize(&sb->buf, needed);
}

bool sb_write_byte(StringBuilder* sb, u8 b){
	return append(&sb->buf, b);
}

bool sb_write_bytes(StringBuilder* sb, Slice<u8> data){
	return append_slice(&sb->buf, data);
}

bool sb_write_string(StringBuilder* sb, String s){
	usize n = len(s);
	if(!sb_grow(sb, n)){ return false; }
	for(usize i = 0; i < n; i += 1){
		append(&sb->buf, (u8)raw_data(s)[i]);
	}
	return true;
}

bool sb_write_rune(StringBuilder* sb, rune r){
	RuneEncoded enc = rune_encode(r);
	for(u32 i = 0; i < enc.size; i += 1){
		if(!append(&sb->buf, enc.bytes[i])){ return false; }
	}
	return true;
}

String sb_to_string(StringBuilder* sb){
	return String{ (char const*)raw_data(sb->buf), len(sb->buf) };
}

//// String utilities

String str_clone(String s, Allocator a){
	usize n = len(s);
	u8* buf = (u8*)mem_alloc(a, n, alignof(u8));
	if(!buf){
		return String{};
	}

	mem_copy_no_overlap(buf, raw_data(s), n);
	return String{(char const*)buf, n};
}

i64 str_find(String haystack, String needle){
	usize hay_len = len(haystack);
	usize needle_len = len(needle);
	if(needle_len == 0){
		return 0;
	}

	if(needle_len > hay_len){
		return -1;
	}

	char const* hay_buf = raw_data(haystack);
	char const* needle_buf = raw_data(needle);

	for(usize i = 0; i <= (hay_len - needle_len); i += 1){
		if(mem_compare(&hay_buf[i], needle_buf, needle_len) == 0){
			return (i64)i;
		}
	}

	return -1;
}

String str_trim_prefix(String s, String prefix){
	usize plen = len(prefix);
	if(len(s) >= plen && mem_compare(raw_data(s), raw_data(prefix), plen) == 0){
		return String{raw_data(s) + plen, len(s) - plen};
	}
	return s;
}

String str_trim_suffix(String s, String suffix){
	usize slen  = len(s);
	usize sflen = len(suffix);
	if(slen >= sflen && mem_compare(raw_data(s) + slen - sflen, raw_data(suffix), sflen) == 0){
		return String{raw_data(s), slen - sflen};
	}
	return s;
}

using CutsetRunes = StaticArray<rune, max_cutset_rune_count>;

static CutsetRunes decode_cutset(String cutset){
	CutsetRunes cs;
	u8 const* d = (u8 const*)raw_data(cutset);
	usize n = len(cutset);
	usize i = 0;
	while(i < n){
		RuneDecoded rd = rune_decode(d + i, (u32)(n - i));
		if(!append(&cs, rd.codepoint)){ break; }
		i += rd.size ? rd.size : 1;
	}
	return cs;
}

static bool rune_in_cutset(rune r, CutsetRunes const& cs){
	for(usize i = 0; i < len(cs); i += 1){
		if(raw_data(cs)[i] == r){
			return true;
		}
	}
	return false;
}

String str_trim_start(String s, String cutset){
	CutsetRunes cs = decode_cutset(cutset);
	u8 const* d = (u8 const*)raw_data(s);
	usize n = len(s);
	usize i = 0;
	while(i < n){
		RuneDecoded rd = rune_decode(d + i, (u32)(n - i));
		if(!rune_in_cutset(rd.codepoint, cs)){ break; }
		i += rd.size;
	}
	return String{(char const*)d + i, n - i};
}

String str_trim_end(String s, String cutset){
	CutsetRunes cs = decode_cutset(cutset);
	u8 const* d = (u8 const*)raw_data(s);
	usize n = len(s);
	while(n > 0){
		usize start = n - 1;
		while(start > 0 && ((d[start] & 0xc0) == 0x80)){ start--; }
		RuneDecoded rd = rune_decode(d + start, (u32)(n - start));
		if(!rune_in_cutset(rd.codepoint, cs)){ break; }
		n = start;
	}
	return String{(char const*)d, n};
}

String str_trim(String s, String cutset){
	return str_trim_end(str_trim_start(s, cutset), cutset);
}

static bool is_whitespace(rune r){
	return r == ' ' || r == '\t' || r == '\n' || r == '\r' || r == '\f' || r == '\v';
}

String str_trim_spaces(String s){
	u8 const* d = (u8 const*)raw_data(s);
	usize n = len(s);
	usize start = 0;

	while(start < n){
		RuneDecoded rd = rune_decode(d + start, (u32)(n - start));
		if(!is_whitespace(rd.codepoint)){
			break;
		}
		start += rd.size;
	}
	while(n > start){
		usize rb = n - 1;
		while(rb > start && ((d[rb] & 0xc0) == 0x80)){
			rb--;
		}
		RuneDecoded rd = rune_decode(d + rb, (u32)(n - rb));
		if(!is_whitespace(rd.codepoint)){
			break;
		}
		n = rb;
	}
	return String{(char const*)d + start, n - start};
}

Slice<String> str_split(String target, String sep, Allocator a){
	DynArray<String> parts(a);
	usize sep_len = len(sep);

	if(sep_len == 0){
		append(&parts, target);
		return slice(parts);
	}

	char const* d = raw_data(target);
	usize target_len = len(target);
	usize start = 0;

	while(start <= target_len){
		String remaining{d + start, target_len - start};
		i64 pos = str_find(remaining, sep);
		if(pos < 0){
			append(&parts, remaining);
			break;
		}
		append(&parts, String{d + start, (usize)pos});
		start += (usize)pos + sep_len;
	}

	return slice(parts);
}

String str_replace(String s, String pattern, String replacement, Allocator a, usize count){
	StringBuilder sb = sb_create(a);

	if(len(pattern) == 0){
		sb_write_string(&sb, s);
		return sb_to_string(&sb);
	}

	String remaining = s;
	usize replaced = 0;

	while(len(remaining) > 0){
		if(count != 0 && replaced >= count){
			sb_write_string(&sb, remaining);
			break;
		}
		i64 pos = str_find(remaining, pattern);
		if(pos < 0){
			sb_write_string(&sb, remaining);
			break;
		}

		String matching =  take(remaining, (usize)pos);
		sb_write_string(&sb, matching);
		sb_write_string(&sb, replacement);
		remaining = skip(remaining, (usize)pos + len(pattern));
		replaced += 1;
	}

	return sb_to_string(&sb);
}

String sb_build(StringBuilder const& sb, Allocator a){
	auto buf = make_slice<u8>(a, len(sb) + 1);
	copy(buf, slice(sb.buf));
	return String{(char const*)raw_data(buf), len(buf)};
}
