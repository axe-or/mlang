require 'prelude'

local templ = file_read('dynamic_array.txt')

local gen = exec_template(templ, {
    T = 'int',
    PointerT = '#(T)*',
    ArrayType = 'IntArray',
    Prefix = 'int_array',
}, 2)

gen = R[[
    #include <stdint.h>
    #include <stddef.h>

    extern void* calloc(size_t count, size_t size);
    extern void* realloc(void* p, size_t nbytes);
    extern void* memset(void* p, int val, size_t nbytes);

]] .. '\n\n' .. gen


print(gen)
file_write('dynamic_array.h', gen, true)
