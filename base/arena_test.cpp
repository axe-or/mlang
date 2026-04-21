#pragma once
#include "testing.hpp"
#include "arena.hpp"
#include "virtual.hpp"

void arena_test(Test* t){
	t->name = "Arena";

	//// Buffer arena

	// Basic alloc and zero-init
	{
		u8 buf[256];
		Arena a = arena_from_buffer(buf, sizeof(buf));

		i32* p = (i32*)arena_alloc(&a, sizeof(i32), alignof(i32));
		t_expect(t, p != nullptr,  "buffer arena: alloc returns non-null");
		t_expect(t, *p == 0,       "buffer arena: allocation is zero-initialized");
		t_expect(t, a.offset > 0,  "buffer arena: offset advances after alloc");
	}

	// Alignment
	{
		u8 buf[512];
		Arena a = arena_from_buffer(buf, sizeof(buf));

		// Force an odd offset then alloc with stricter alignment
		arena_alloc(&a, 1, 1);
		void* p = arena_alloc(&a, sizeof(u64), alignof(u64));
		t_expect(t, p != nullptr,                           "buffer arena: aligned alloc non-null");
		t_expect(t, ((uintptr)p % alignof(u64)) == 0,      "buffer arena: pointer is 8-byte aligned");
	}

	// arena_owns
	{
		u8 buf[128];
		Arena a = arena_from_buffer(buf, sizeof(buf));

		void* inside = arena_alloc(&a, 16, 1);
		t_expect(t, arena_owns(&a, inside),    "buffer arena: owns allocated pointer");
		t_expect(t, !arena_owns(&a, (void*)&a),"buffer arena: does not own external pointer");
	}

	// OOM returns null
	{
		u8 buf[32];
		Arena a = arena_from_buffer(buf, sizeof(buf));

		void* p = arena_alloc(&a, 64, 1);
		t_expect(t, p == nullptr, "buffer arena: OOM returns null");
	}

	// arena_resize (in-place, last allocation)
	{
		u8 buf[256];
		Arena a = arena_from_buffer(buf, sizeof(buf));

		i32* p = (i32*)arena_alloc(&a, sizeof(i32), alignof(i32));
		usize offset_before = a.offset;
		bool ok = arena_resize(&a, p, sizeof(i32) * 4);
		t_expect(t, ok,                         "buffer arena: resize last alloc ok");
		t_expect(t, a.offset > offset_before,   "buffer arena: offset grows after resize");
	}

	// arena_resize fails for non-last allocation
	{
		u8 buf[256];
		Arena a = arena_from_buffer(buf, sizeof(buf));

		void* first  = arena_alloc(&a, 16, 1);
		arena_alloc(&a, 16, 1); // second alloc displaces last_allocation
		bool ok = arena_resize(&a, first, 32);
		t_expect(t, !ok, "buffer arena: resize non-last alloc fails");
	}

	// arena_realloc in-place
	{
		u8 buf[256];
		Arena a = arena_from_buffer(buf, sizeof(buf));

		u8* p = (u8*)arena_alloc(&a, 8, 1);
		p[0] = 0xab;
		void* q = arena_realloc(&a, p, 8, 16, 1);
		t_expect(t, q == (void*)p,       "buffer arena: realloc in-place returns same ptr");
		t_expect(t, ((u8*)q)[0] == 0xab, "buffer arena: realloc preserves data");
	}

	// arena_realloc copy fallback
	{
		u8 buf[256];
		Arena a = arena_from_buffer(buf, sizeof(buf));

		u8* first = (u8*)arena_alloc(&a, 8, 1);
		first[0] = 0xcd;
		arena_alloc(&a, 8, 1); // displace last_allocation

		void* q = arena_realloc(&a, first, 8, 16, 1);
		t_expect(t, q != nullptr,         "buffer arena: realloc copy fallback non-null");
		t_expect(t, q != (void*)first,    "buffer arena: realloc copy fallback new ptr");
		t_expect(t, ((u8*)q)[0] == 0xcd,  "buffer arena: realloc copy preserves data");
	}

	// arena_reset
	{
		u8 buf[128];
		Arena a = arena_from_buffer(buf, sizeof(buf));

		arena_alloc(&a, 64, 1);
		arena_reset(&a);
		t_expect(t, a.offset == 0,              "buffer arena: offset 0 after reset");
		t_expect(t, a.last_allocation == nullptr,"buffer arena: last_allocation null after reset");

		// Can allocate again from the start
		void* p = arena_alloc(&a, 64, 1);
		t_expect(t, p != nullptr, "buffer arena: alloc after reset ok");
	}

	//// Virtual arena

	// Basic alloc
	{
		Arena a = arena_from_virtual(4 * mem_megabyte);

		i32* p = (i32*)arena_alloc(&a, sizeof(i32), alignof(i32));
		t_expect(t, p != nullptr, "virtual arena: alloc non-null");
		t_expect(t, *p == 0,      "virtual arena: allocation is zero-initialized");

		arena_destroy(&a);
	}

	// Auto-grow across commit boundary
	{
		Arena a = arena_from_virtual(4 * mem_megabyte, (u32)virtual_page_size(), (u32)virtual_page_size());

		usize first_commit = a.commited;
		// Allocate more than the initial commit to force a grow
		void* p = arena_alloc(&a, first_commit + 1, 1);
		t_expect(t, p != nullptr,           "virtual arena: alloc triggers grow");
		t_expect(t, a.commited > first_commit, "virtual arena: commited increases after grow");

		arena_destroy(&a);
	}

	// arena_owns
	{
		Arena a = arena_from_virtual(4 * mem_megabyte);

		void* p = arena_alloc(&a, 64, 1);
		t_expect(t, arena_owns(&a, p),          "virtual arena: owns allocated pointer");
		t_expect(t, !arena_owns(&a, (void*)&a), "virtual arena: does not own external pointer");

		arena_destroy(&a);
	}

	// arena_reset and reuse
	{
		Arena a = arena_from_virtual(4 * mem_megabyte);

		void* p1 = arena_alloc(&a, 1024, 1);
		arena_reset(&a);
		t_expect(t, a.offset == 0, "virtual arena: offset 0 after reset");

		void* p2 = arena_alloc(&a, 1024, 1);
		t_expect(t, p2 != nullptr, "virtual arena: alloc after reset ok");
		t_expect(t, p1 == p2,      "virtual arena: same base address after reset");

		arena_destroy(&a);
	}

	// DynArray backed by arena allocator
	{
		Arena a = arena_from_virtual(1 * mem_megabyte);
		Allocator alloc = arena_allocator(&a);

		DynArray<i32> arr = make_dyn_array<i32>(alloc);
		for(i32 i = 0; i < 64; i++){
			t_expect(t, append(&arr, i), "virtual arena: DynArray append ok");
		}
		t_expect(t, len(arr) == 64,  "virtual arena: DynArray len == 64");
		t_expect(t, arr[63] == 63,   "virtual arena: DynArray last elem correct");

		arena_destroy(&a);
	}
}

