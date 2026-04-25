#include <stddef.h>

extern "C" {
void* malloc(size_t);
void free(void* p);
}

void* operator new(size_t size) { return malloc(size); }
void* operator new[](size_t size) { return malloc(size); }
void operator delete(void* p) noexcept { free(p); }
void operator delete(void* p, size_t) noexcept { free(p); }
void operator delete[](void* p) noexcept { free(p); }
void operator delete[](void* p, size_t){ free(p); }


// void* __dso_handle = nullptr;

extern "C" void __cxa_pure_virtual() { while(1); }

