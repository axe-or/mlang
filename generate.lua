require 'prelude'

local templ = file_read('dynamic_array.txt')

function gen_dynamic_array(inner_type, array_type, prefix)
    array_type = array_type or ('%sArray'):format(snake_to_pascal(inner_type))
    prefix = prefix or pascal_to_snake(array_type)

    return exec_template(templ, {
        type = inner_type,
        arrayType = array_type,
        prefix = prefix,
    }, 2)
end

local gen = R[[
    #include <stdint.h>
    #include <stddef.h>
    #include <stdbool.h>

    #define max(a, b) (((a) > (b)) ? (a) : (b))
    #define min(a, b) (((a) < (b)) ? (a) : (b))
    #define clamp(lo, x, hi) min(max(lo, x), hi)
    #define ensure(pred, msg) do { if(!(pred)) { \
        printf("%s:%d Assertion failed: %s", __FILE__, __LINE__, msg); \
        __builtin_trap(); \
    } } while(0)

    extern void* calloc(size_t count, size_t size);
    extern void* realloc(void* p, size_t nbytes);
    extern void* memset(void* p, int val, size_t nbytes);
    extern void* memmove(void* dst, void const* src, size_t nbytes);

]] .. '\n\n' .. gen_dynamic_array('int')

print(gen)
file_write('dynamic_array.h', gen, true)
