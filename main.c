#include "base.h"
#include <stdio.h>

int main(){
    Arena arena = arena_from_virtual(256 * Mem_Gigabyte, 1 * Mem_Megabyte, 4 * Mem_Megabyte);

    u64* p = arena_make(&arena, u64, 730034);
    printf("%p\n", p);

    return 0;
}

#include "base.c"
