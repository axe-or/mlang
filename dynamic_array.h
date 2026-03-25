// File automatically generated at 2026-03-24 22:37:05. DO NOT EDIT.

#include "base.h"
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
    arr->v = (int*)calloc(cap, sizeof(int));
}

static inline
void int_array_resize(IntArray* arr, size_t new_cap){
    bool requires_zeroing_excess = new_cap > arr->cap;

    arr->v = (int*)realloc(arr->v, sizeof(int) * new_cap);
    ensure(arr->v, "failed to reallocate");

    arr->len = min(arr->len, new_cap);
    arr->cap = new_cap;

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
        size_t new_cap = max(arr->cap * 2, DYN_ARRAY_MIN_CAP);
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

static inline
void int_array_insert(IntArray* arr, size_t idx, int val){
    if(arr->len >= arr->cap){
        size_t new_cap = max(arr->cap * 2, DYN_ARRAY_MIN_CAP);
        int_array_resize(arr, new_cap);
    }

    memmove(&arr->v[idx + 1], &arr->v[idx], sizeof(int) * (arr->len - 0));
    arr->v[idx] = val;
}
