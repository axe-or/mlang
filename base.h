#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define clamp(lo, x, hi) min(max(lo, x), hi)

extern void* memset(void* p, int val, size_t nbytes);
extern void* memmove(void* dst, void const* src, size_t nbytes);
extern void* memcpy(void* dst, void const* src, size_t nbytes);
extern int   memcmp(void const* lhs, void const* rhs, size_t nbytes);

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t  i8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef uintptr_t uintptr;
typedef int32_t rune;
typedef size_t usize;
typedef ptrdiff_t isize;

extern void* calloc(size_t count, size_t size);
extern void* realloc(void* p, size_t nbytes);

extern _Noreturn void abort();
extern int printf(char const*, ...);

#define ensure(pred, msg) do { if(!(pred)) { \
    printf("%s:%d Assertion failed: %s", __FILE__, __LINE__, msg); \
    abort(); \
} } while(0)
