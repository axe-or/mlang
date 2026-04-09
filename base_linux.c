#include "base.h"
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/futex.h>

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
	madvise(base, size, MADV_FREE);
}

void* virtual_reserve(usize size){
	return mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void virtual_release(void* base, usize size){
	munmap(base, size);
}

static
int futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3) {
	return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}


typedef struct {
	AtomicInt _v;
} LinuxSemaphore;

bool sema_wait(LinuxSemaphore* s){
	while(1){
		int expect = 1;
		if(atomic_compare_exchange_strong_explicit(
			&s->_v,
			&expect,
			0,
			memory_order_relaxed, memory_order_relaxed)
		){
			return true;
		}

		int err = futex((int*)(&s->_v), FUTEX_WAIT, 0, NULL, NULL, 0);

		switch(err){
		case ETIMEDOUT:
			return false;

		case 0: case EINTR: case EAGAIN:
			return true;

		default:
			panic("bad wait on futex");
		}
	}
}

void sema_post(LinuxSemaphore* s){
	int expect = 0;
	if(atomic_compare_exchange_strong_explicit(
		&s->_v,
		&expect,
		1,
		memory_order_relaxed, memory_order_relaxed)
	){
		int err = futex((int*)(&s->_v), FUTEX_WAKE, 1, NULL, NULL, 0);
		if(err < 0){
			panic("bad wake on futex");
		}
	}

}


//// Threads
typedef struct {
} ThreadPlatformSpecific;

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


// futex_broadcast
// futex_wait_with_timeout
// futex_signal
// futex_wait

_Static_assert(sizeof(ThreadPlatformSpecific) <= sizeof(ThreadPlatformSpecificData), "invalid thread platform specific size");
_Static_assert(alignof(ThreadPlatformSpecific) <= alignof(ThreadPlatformSpecificData), "invalid thread platform specific align");

