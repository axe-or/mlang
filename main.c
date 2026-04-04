#include "base.h"
#include <stdio.h>

#include "testing.h"

void test_foo(Test* t){
	t_expect(t, 2 + 2 == 3);
}

int main(){
    virtual_init();
    Arena arena = arena_from_virtual(8 * Mem_Gigabyte, 1 * Mem_Megabyte, 4 * Mem_Megabyte);
    // static u8 buf[1 * Mem_Megabyte];
    // Arena arena = arena_from_buffer(&buf[0], sizeof(buf));

    u64* p = arena_make(&arena, u64, 16 * Mem_Megabyte);
    printf("%p\n", p);

    Allocator a = arena_allocator(&arena);
    Array(i32) arr = array_make(a);

    for(i32 i = 0; i < 100; i++){
        printf("[%d] %zu/%zu\n", i, array_len(arr), array_cap(arr));
        array_push(&arr, i);
    }
    printf("%zu %zu\n", array_len(arr), array_cap(arr));

	test_run("Foo", test_foo);

    return 0;
}

#include "base.c"
