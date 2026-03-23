// File automatically generated at 2026-03-22 21:30:50. DO NOT EDIT.

#include <stdint.h>
#include <stddef.h>

extern void* calloc(size_t count, size_t size);
extern void* realloc(void* p, size_t nbytes);
extern void* memset(void* p, int val, size_t nbytes);

#include <stdbool.h>
#define DYN_ARRAY_MIN_CAP 16

typedef struct {
    int* v;
    size_t len;
    size_t cap;
} IntArray;

static inline
void int_array_init(IntArray* arr, size_t len, size_t cap){
    arr->len = len;
    arr->cap = cap;
    arr->v = (int*)calloc(len, sizeof(int));
}

static inline
void int_array_resize(IntArray* arr, size_t new_cap){
    bool requires_zeroing_excess = new_cap > arr->cap;

    arr->len = new_cap < arr->len ? new_cap : arr->len;
    arr->cap = new_cap;
    arr->v   = (int*)realloc(arr->v, sizeof(int) * new_cap);

    if(requires_zeroing_excess){
        memset(&arr->v[arr->len], 0, sizeof(int) * (arr->cap - arr->len));
    }
}

static inline
void int_array_clear(IntArray* arr){
    arr->len = 0;
}

static inline
void int_array_append(IntArray* arr, int value){
    if(arr->len >= arr->cap){
        size_t new_cap = arr->cap * 2;
        if(new_cap < DYN_ARRAY_MIN_CAP) new_cap = DYN_ARRAY_MIN_CAP;
        int_array_resize(arr, new_cap);
    }

    arr->v[arr->len] = value;
    arr->len += 1;
}

static inline
bool int_array_pop(IntArray* arr){
    if(arr->len == 0){
        return false;
    }

    arr->len -= 1;
    return true;
}

static inline
bool int_array_pop_into(IntArray* arr, int* out){
    if(arr->len == 0){
        return false;
    }

    arr->len -= 1;
    *out = arr->v[arr->len];
    return true;
}
