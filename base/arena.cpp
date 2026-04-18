#include "arena.hpp"
#include "virtual.hpp"

//// Arena

void arena_reset(Arena* a){
	ensure(a->region_count == 0, "Arena has dangling regions");
	a->offset = 0;
	a->last_allocation = NULL;
}

bool arena_owns(Arena* a, void* p){
	uintptr ptr = (uintptr)p;
	uintptr lo = (uintptr)a->data;
	uintptr hi = lo + a->reserved;

	return (ptr >= lo) && (ptr <= hi);
}

static
bool arena_virtual_grow(Arena* a, usize size){
	ensure(a->type == Arena_Virtual, "not a virtual arena");
	ensure(a->commit_size >= virtual_page_size(), "invalid arena");

	if(a->commited >= a->reserved){
		return false;
	}
	size = mem_align_forward_ptr(size, virtual_page_size());

	uintptr base = (uintptr)a->data;
	uintptr start = base + a->commited;
	uintptr end = base + a->reserved;
	uintptr remaining = end - start;

	if(size > remaining){
    	return false;
	}

	bool ok = virtual_commit((void*)start, size);
	if(!ok){
	    return false;
	}

	a->commited += size;

	return ok;
}

static
void arena_virtual_decommit(Arena* a, usize size){
	ensure(a->type == Arena_Virtual, "Not a virtual arena");

	size = min(size, a->commited);
	virtual_decommit(a->data, size);
	a->commited = a->commited - size;
}

void* arena_alloc(Arena* a, usize size, usize align){
	if(size == 0){ return NULL; }
	uintptr base = (uintptr)a->data;
	uintptr current = base + (uintptr)a->offset;

	usize available = a->commited - (current - base);

	uintptr aligned  = mem_align_forward_ptr(current, align);
	uintptr padding  = aligned - current;
	uintptr required = padding + size;

	if(required > available){
		if(a->type == Arena_Buffer){
			return NULL;
		}

		usize to_commit = max(size, (usize)a->commit_size);
		if(!arena_virtual_grow(a, to_commit)){
			return NULL;
		}
		return arena_alloc(a, size, align);
	}

	a->offset += required;
	void* allocation = (void*)aligned;

	mem_zero(allocation, size);

	a->last_allocation = allocation;
	a->last_allocation_size = size;

	return allocation;
}

bool arena_resize(Arena* a, void* ptr, usize new_size){
	if(ptr == NULL){
    	return false;
	}
	ensure(arena_owns(a, ptr), "Pointer not owned by arena");

	uintptr base = (uintptr)a->data;

	if(ptr == a->last_allocation){
		uintptr last_alloc = (uintptr)a->last_allocation;
		uintptr can_commit = a->reserved - a->commited;

		if((last_alloc + new_size) > (base + a->commited)){
			if(a->type == Arena_Buffer){
				return false; /* No space left, cannot grow */
			}
			else if(new_size <= can_commit) {
				bool ok = arena_virtual_grow(a, mem_align_forward_ptr(new_size, a->commit_size));
				if(!ok){
					return false; /* Failed to grow arena */
				}
				return arena_resize(a, ptr, new_size);
			}
			else {
				return false;
			}
		}

		usize old_size = a->last_allocation_size;

		a->offset = (last_alloc + new_size) - base;
		a->last_allocation_size = new_size;

		if(new_size > old_size){
			usize diff = new_size - old_size;
			uintptr p = last_alloc + old_size;
			mem_zero((void*)p, diff);
		}
		return true;
	}

	return false;
}

void* arena_realloc(Arena* a, void* ptr, usize old_size, usize new_size, usize align){
	if(ptr == NULL){
    	return arena_alloc(a, new_size, align);
	}
	ensure(arena_owns(a, ptr), "Pointer not owned by arena");

	bool in_place = arena_resize(a, ptr, new_size);
	if(in_place){
		return ptr;
	}
	else {
		void* new_data = arena_alloc(a, new_size, align);
		if(new_data == NULL){ return NULL; } /* Out of memory */
		mem_copy(new_data, ptr, min(old_size, new_size));

		if(new_size > old_size){
			usize diff = new_size - old_size;
			mem_zero((u8*)new_data + old_size, diff);
		}

		return new_data;
	}
}

Arena arena_from_buffer(u8* buf, usize bufsize){
	Arena a = {
		.data = (void*)buf,
		.offset = 0,
		.commited = bufsize,
		.reserved = bufsize,
		.last_allocation = NULL,
		.last_allocation_size = 0,

		.commit_size = 0,
		.region_count = 0,
		.type = Arena_Buffer,
	};
	return a;
}

Arena arena_from_virtual(usize reserve_size, u32 commit_size, u32 initial_commit_size){
	if(commit_size == 0)
		commit_size = arena_default_commit_size;

	ensure(mem_valid_alignment(commit_size), "invalid commit size. expected a power of 2");
	ensure(commit_size >= virtual_page_size(), "commit size too small");

	void* data = virtual_reserve(reserve_size);
	ensure(data != NULL, "failed to reserve memory");
	Arena a = {
		.data = data,
		.offset = 0,
		.commited = 0,
		.reserved = reserve_size,
		.last_allocation = NULL,
		.last_allocation_size = 0,

		.commit_size = commit_size,
		.region_count = 0,
		.type = Arena_Virtual,
	};

	if(initial_commit_size != 0){
		arena_virtual_grow(&a, min((usize)initial_commit_size, reserve_size));
	}

	return a;
}

void arena_destroy(Arena* a){
	arena_reset(a);
	if(a->type == Arena_Virtual){
		virtual_release(a->data, a->reserved);
	}
}

static
AllocatorResult arena_allocator_func(
	void* impl,
	u8 mode,
	void* ptr,
	usize old_size, usize old_align, /* Old layout */
	usize new_size, usize new_align /* New Layout */
){
    Arena* a = (Arena*)impl;
    AllocatorResult res = {.ptr = NULL};

    switch((AllocatorMode)mode){
    case Mem_Query:
        res.modes = Mem_Alloc | Mem_FreeAll | Mem_Realloc;
    break;

    case Mem_Alloc:
        res.ptr = arena_alloc(a, new_size, new_align);
    break;

    case Mem_Realloc:
        ensure(old_align == new_align, "unsupported");
        res.ptr = arena_realloc(a, ptr, old_size, new_size, new_align);
    break;

    case Mem_Free:
        /* Unsupported */
    break;

    case Mem_FreeAll:
		arena_reset(a);
    break;
    }

    return res;
}

Allocator arena_allocator(Arena* a){
    return Allocator{
        ._impl = a,
        ._fn = arena_allocator_func,
    };
}
