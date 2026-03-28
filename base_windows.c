#include "base.h"
#include <windows.h>

extern void* _aligned_malloc(size_t, size_t);
extern void _aligned_free(void*);

//// Virtual Memory

static usize _virtual_page_size = 4096;
static bool _virtual_initialized = false;

usize virtual_page_size(){
	ensure_debug(_virtual_initialized, "virtual system info is not initialized, initialize it with virtual_init()");
	return _virtual_page_size;
}

void virtual_init(){
	static SYSTEM_INFO info = {0};
	GetSystemInfo(&info);
	if(!mem_valid_alignment(info.dwPageSize)){
		panic("virtual page size is not valid");
	}

	_virtual_page_size = info.dwPageSize;
	_virtual_initialized = true;
}

bool virtual_protect(void* ptr, usize size, u8 protection){
	DWORD flags = 0;

	switch(protection){
	// Read
	case MemProt_Read:
		flags = PAGE_READONLY; break;

	case MemProt_Read | MemProt_Write:
		flags = PAGE_READWRITE; break;

	// Write
	case MemProt_Write:
		flags = PAGE_WRITECOPY; break;

	// Exec
	case MemProt_Exec:
		flags = PAGE_EXECUTE; break;

	case MemProt_Exec | MemProt_Write:
		flags = PAGE_EXECUTE_WRITECOPY; break;

	case MemProt_Exec | MemProt_Read | MemProt_Write:
		flags = PAGE_EXECUTE_READWRITE; break;

	case MemProt_Exec | MemProt_Read:
		flags = PAGE_EXECUTE_READ; break;

	}

	if(!flags){
		return false;
	}

	DWORD old = 0;
	return VirtualProtect(ptr, size, flags, &old);
}

bool virtual_commit(void* ptr, usize size){
	return VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != NULL;
}

void virtual_decommit(void* ptr, usize size){
	VirtualFree(ptr, size, MEM_DECOMMIT);
}

void* virtual_reserve(usize size){
	return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);
}

void virtual_release(void* ptr, usize size){
	VirtualFree(ptr, size, MEM_RELEASE);
}

void* heap_alloc(usize size, usize align){
	void* p = _aligned_malloc(size, align);
	if(p){
		mem_zero(p, size);
	}
	return p;
}

void heap_free(void* ptr){
	_aligned_free(ptr);
}

//// Threads

typedef struct {
	HANDLE handle;
} ThreadPlatformSpecific;

static inline
ThreadPlatformSpecific thread_load_platform_specific(Thread* t){
	ThreadPlatformSpecific spec = {0};
	mem_copy_no_overlap(&spec, &t->_specific, sizeof(spec));
	return spec;
}

static inline
void thread_store_platform_specific(Thread* t, ThreadPlatformSpecific spec){
	mem_copy_no_overlap(&t->_specific, &spec, sizeof(spec));
}

static
DWORD _thread_platform_wrapper(void* data){
	Thread* thread = (Thread*)data;
	thread->func(thread->arg);
	atomic_fetch_or(&thread->flags, Thread_Done);
	return 0;
}

#define THREAD_STACK_SIZE (2 * Mem_Megabyte)

void thread_yield(){
	SwitchToThread();
}

Thread* thread_create(ThreadFunc func, void* arg){
	Thread* t = heap_alloc(sizeof(Thread), alignof(Thread));

	// NOTE: Cast because TCC complains about it for some reason
	ThreadPlatformSpecific spec = {
		.handle = CreateThread(NULL, THREAD_STACK_SIZE, (LPTHREAD_START_ROUTINE)_thread_platform_wrapper, (void*)t, CREATE_SUSPENDED, NULL),
	};

	if(spec.handle == NULL){
		heap_free(t);
		return NULL;
	}

	thread_store_platform_specific(t, spec);
	t->func = func;
	t->arg = arg;
	atomic_fetch_or(&t->flags, Thread_Initialized);

	return t;
}

bool thread_is_done(Thread* t){
	return atomic_load(&t->flags) & Thread_Done;
}

void thread_start(Thread* t){
	spin_lock(&t->mtx);

	ThreadPlatformSpecific spec = thread_load_platform_specific(t);
	atomic_fetch_or(&t->flags, Thread_Started);
	ResumeThread(spec.handle);

	spin_unlock(&t->mtx);
}

void thread_join(Thread* t){
	spin_lock(&t->mtx);

	ThreadPlatformSpecific spec = thread_load_platform_specific(t);
	int flags = atomic_load(&t->flags);

	bool cannot_join = (flags & Thread_Joined)
		|| !(flags & Thread_Started)
		|| (spec.handle == INVALID_HANDLE_VALUE);

	if(cannot_join){ return; }

	WaitForSingleObject(spec.handle, INFINITE);
	CloseHandle(spec.handle);

	atomic_fetch_or(&t->flags, Thread_Joined);
	spec.handle = INVALID_HANDLE_VALUE;

	thread_store_platform_specific(t, spec);

	spin_unlock(&t->mtx);
}

void thread_cancel(Thread* t){
	(void)t;
	TODO();
	// spin_lock(&t->mtx);

	// TerminateThread
	// atomic_fetch_or(&t->flags, Thread_Cancelled);

	// spin_unlock(&t->mtx);
}

void thread_destroy(Thread* t){
	thread_join(t);
	heap_free(t);
}

