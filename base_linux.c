#include "base.h"
#include <sys/unistd.h>
#include <sys/mman.h>
#include <semaphore.h>

//// Virtual Memory

static usize _virtual_page_size = 4096;
static bool _virtual_initialized = false;

usize virtual_page_size(){
	ensure(_virtual_initialized, "virtual system info is not initialized, initialize it with virtual_init()");
	return _virtual_page_size;
}

void virtual_init(){
    usize page_size = sysconf(_SC_PAGESIZE);
	if(!mem_valid_alignment(page_size)){
		panic("virtual page size is not valid");
	}

	_virtual_page_size = page_size;
	_virtual_initialized = true;
}

bool virtual_protect(void* ptr, usize size, u8 prot){
	u32 flags = 0;

	if(prot & MemProt_Read)
		flags |= PROT_READ;

	if(prot & MemProt_Write)
		flags |= PROT_WRITE;

	if(prot & MemProt_Exec)
		flags |= PROT_EXEC;
	
	int res = mprotect(ptr, size, flags);

	return res == 0;
}

bool virtual_commit(void* base, usize size){
	return virtual_protect(base, size, MemProt_Read | MemProt_Write);
}

void virtual_decommit(void* base, usize size){
	virtual_protect(base, size, MemProt_None);
	madvise(base, size, MADV_FREE)
}

void* virtual_reserve(usize size){
	return mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void virtual_release(void* base, usize size){
	munmap(base, size);
}

//// Threads
typedef struct {
	char x;
} ThreadPlatformSpecifc;

Thread* thread_create(ThreadFunc func, void* arg){
	(void)func; (void)arg;
	TODO();
}

void thread_start(Thread* t){
	TODO();
}

void thread_join(Thread* t){
	TODO();
}

void thread_cancel(Thread* t){
	TODO();
}

void thread_destroy(Thread* t){
	TODO();
}

_Static_assert(sizeof(ThreadPlatformSpecific) <= sizeof(ThreadPlatformSpecificData), "invalid thread platform specific size");
_Static_assert(alignof(ThreadPlatformSpecific) <= alignof(ThreadPlatformSpecificData), "invalid thread platform specific align");

