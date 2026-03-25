// File automatically generated at 2026-03-24 22:45:14. DO NOT EDIT.

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
#include "base.h"
#define DYN_ARRAY_MIN_CAP 16

typedef struct {
    u8* v;
    size_t len;
    size_t cap;
} U8Array;

static inline
void u8array_init(U8Array* arr, size_t len, size_t cap){
    arr->len = len;
    arr->cap = cap;
    arr->v = (u8*)calloc(cap, sizeof(u8));
}

static inline
void u8array_resize(U8Array* arr, size_t new_cap){
    bool requires_zeroing_excess = new_cap > arr->cap;

    arr->v = (u8*)realloc(arr->v, sizeof(u8) * new_cap);
    ensure(arr->v, "failed to reallocate");

    arr->len = min(arr->len, new_cap);
    arr->cap = new_cap;

    if(requires_zeroing_excess){
        memset(&arr->v[arr->len], 0, sizeof(u8) * (arr->cap - arr->len));
    }
}

static inline
void u8array_clear(U8Array* arr){
    arr->len = 0;
}

static inline
void u8array_append(U8Array* arr, u8 value){
    if(arr->len >= arr->cap){
        size_t new_cap = max(arr->cap * 2, DYN_ARRAY_MIN_CAP);
        u8array_resize(arr, new_cap);
    }

    arr->v[arr->len] = value;
    arr->len += 1;
}

static inline
bool u8array_pop(U8Array* arr){
    if(arr->len == 0){
        return false;
    }

    arr->len -= 1;
    return true;
}

static inline
bool u8array_pop_into(U8Array* arr, u8* out){
    if(arr->len == 0){
        return false;
    }

    arr->len -= 1;
    *out = arr->v[arr->len];
    return true;
}

static inline
void u8array_insert(U8Array* arr, size_t idx, u8 val){
    if(arr->len >= arr->cap){
        size_t new_cap = max(arr->cap * 2, DYN_ARRAY_MIN_CAP);
        u8array_resize(arr, new_cap);
    }

    memmove(&arr->v[idx + 1], &arr->v[idx], sizeof(u8) * (arr->len - 0));
    arr->v[idx] = val;
}
#include "base.h"
#define DYN_ARRAY_MIN_CAP 16

typedef struct {
    String* v;
    size_t len;
    size_t cap;
} StringArray;

static inline
void string_array_init(StringArray* arr, size_t len, size_t cap){
    arr->len = len;
    arr->cap = cap;
    arr->v = (String*)calloc(cap, sizeof(String));
}

static inline
void string_array_resize(StringArray* arr, size_t new_cap){
    bool requires_zeroing_excess = new_cap > arr->cap;

    arr->v = (String*)realloc(arr->v, sizeof(String) * new_cap);
    ensure(arr->v, "failed to reallocate");

    arr->len = min(arr->len, new_cap);
    arr->cap = new_cap;

    if(requires_zeroing_excess){
        memset(&arr->v[arr->len], 0, sizeof(String) * (arr->cap - arr->len));
    }
}

static inline
void string_array_clear(StringArray* arr){
    arr->len = 0;
}

static inline
void string_array_append(StringArray* arr, String value){
    if(arr->len >= arr->cap){
        size_t new_cap = max(arr->cap * 2, DYN_ARRAY_MIN_CAP);
        string_array_resize(arr, new_cap);
    }

    arr->v[arr->len] = value;
    arr->len += 1;
}

static inline
bool string_array_pop(StringArray* arr){
    if(arr->len == 0){
        return false;
    }

    arr->len -= 1;
    return true;
}

static inline
bool string_array_pop_into(StringArray* arr, String* out){
    if(arr->len == 0){
        return false;
    }

    arr->len -= 1;
    *out = arr->v[arr->len];
    return true;
}

static inline
void string_array_insert(StringArray* arr, size_t idx, String val){
    if(arr->len >= arr->cap){
        size_t new_cap = max(arr->cap * 2, DYN_ARRAY_MIN_CAP);
        string_array_resize(arr, new_cap);
    }

    memmove(&arr->v[idx + 1], &arr->v[idx], sizeof(String) * (arr->len - 0));
    arr->v[idx] = val;
}
#include "base.h"
#define DYN_ARRAY_MIN_CAP 16

typedef struct {
    StringArray* v;
    size_t len;
    size_t cap;
} StringArrayArray;

static inline
void string_array_array_init(StringArrayArray* arr, size_t len, size_t cap){
    arr->len = len;
    arr->cap = cap;
    arr->v = (StringArray*)calloc(cap, sizeof(StringArray));
}

static inline
void string_array_array_resize(StringArrayArray* arr, size_t new_cap){
    bool requires_zeroing_excess = new_cap > arr->cap;

    arr->v = (StringArray*)realloc(arr->v, sizeof(StringArray) * new_cap);
    ensure(arr->v, "failed to reallocate");

    arr->len = min(arr->len, new_cap);
    arr->cap = new_cap;

    if(requires_zeroing_excess){
        memset(&arr->v[arr->len], 0, sizeof(StringArray) * (arr->cap - arr->len));
    }
}

static inline
void string_array_array_clear(StringArrayArray* arr){
    arr->len = 0;
}

static inline
void string_array_array_append(StringArrayArray* arr, StringArray value){
    if(arr->len >= arr->cap){
        size_t new_cap = max(arr->cap * 2, DYN_ARRAY_MIN_CAP);
        string_array_array_resize(arr, new_cap);
    }

    arr->v[arr->len] = value;
    arr->len += 1;
}

static inline
bool string_array_array_pop(StringArrayArray* arr){
    if(arr->len == 0){
        return false;
    }

    arr->len -= 1;
    return true;
}

static inline
bool string_array_array_pop_into(StringArrayArray* arr, StringArray* out){
    if(arr->len == 0){
        return false;
    }

    arr->len -= 1;
    *out = arr->v[arr->len];
    return true;
}

static inline
void string_array_array_insert(StringArrayArray* arr, size_t idx, StringArray val){
    if(arr->len >= arr->cap){
        size_t new_cap = max(arr->cap * 2, DYN_ARRAY_MIN_CAP);
        string_array_array_resize(arr, new_cap);
    }

    memmove(&arr->v[idx + 1], &arr->v[idx], sizeof(StringArray) * (arr->len - 0));
    arr->v[idx] = val;
}
