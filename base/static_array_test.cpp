#pragma once
#include "testing.hpp"
#include "static_array.hpp"

void static_array_test(Test* t){
	t->name = "StaticArray";

	// Default construction
	{
		StaticArray<int, 8> a{};
		t_expect(t, len(a) == 0, "default len == 0");
		t_expect(t, cap(a) == 8, "cap == N");
		t_expect(t, raw_data(a) != nullptr, "data != nullptr");
	}

	// append / len
	{
		StaticArray<int, 8> a{};
		t_expect(t, append(&a, 1), "append 1 ok");
		t_expect(t, append(&a, 2), "append 2 ok");
		t_expect(t, append(&a, 3), "append 3 ok");
		t_expect(t, len(a) == 3, "len == 3 after 3 appends");
		t_expect(t, a[0] == 1, "a[0] == 1");
		t_expect(t, a[1] == 2, "a[1] == 2");
		t_expect(t, a[2] == 3, "a[2] == 3");
	}

	// append returns false when full
	{
		StaticArray<int, 2> a{};
		t_expect(t, append(&a, 1),  "append 1 ok");
		t_expect(t, append(&a, 2),  "append 2 ok");
		t_expect(t, !append(&a, 3), "append 3 fails (full)");
		t_expect(t, len(a) == 2,    "len stays 2 when full");
	}

	// append_slice
	{
		StaticArray<int, 8> a{};
		int buf[3] = {10, 20, 30};
		Slice<int> s{buf, 3};
		t_expect(t, append_slice(&a, s), "append_slice ok");
		t_expect(t, len(a) == 3,         "len == 3 after append_slice");
		t_expect(t, a[0] == 10,          "a[0] == 10");
		t_expect(t, a[2] == 30,          "a[2] == 30");
	}

	// append_slice returns false when would overflow
	{
		StaticArray<int, 4> a{};
		int buf[3] = {1, 2, 3};
		Slice<int> s{buf, 3};
		append_slice(&a, s);
		t_expect(t, !append_slice(&a, s), "append_slice fails when not enough room");
		t_expect(t, len(a) == 3,          "len unchanged after failed append_slice");
	}

	// clear
	{
		StaticArray<int, 8> a{};
		append(&a, 1);
		append(&a, 2);
		clear(&a);
		t_expect(t, len(a) == 0, "len == 0 after clear");
		t_expect(t, cap(a) == 8, "cap unchanged after clear");
	}

	// remove (order-preserving)
	{
		StaticArray<int, 8> a{};
		for(int i = 1; i <= 5; i++){ append(&a, i); }
		remove(&a, 2); // remove value 3
		t_expect(t, len(a) == 4,  "len == 4 after remove");
		t_expect(t, a[0] == 1,    "a[0] == 1");
		t_expect(t, a[1] == 2,    "a[1] == 2");
		t_expect(t, a[2] == 4,    "a[2] == 4 (shifted)");
		t_expect(t, a[3] == 5,    "a[3] == 5 (shifted)");
	}

	// insert
	{
		StaticArray<int, 8> a{};
		append(&a, 1);
		append(&a, 3);
		t_expect(t, insert(&a, 1, 2), "insert at 1 ok");
		t_expect(t, len(a) == 3,      "len == 3 after insert");
		t_expect(t, a[0] == 1,        "a[0] == 1");
		t_expect(t, a[1] == 2,        "a[1] == 2 (inserted)");
		t_expect(t, a[2] == 3,        "a[2] == 3 (shifted)");
	}

	// insert returns false when full
	{
		StaticArray<int, 2> a{};
		append(&a, 1);
		append(&a, 2);
		t_expect(t, !insert(&a, 0, 0), "insert fails when full");
		t_expect(t, len(a) == 2,       "len unchanged after failed insert");
	}

	// take / skip / slice
	{
		StaticArray<int, 8> a{};
		for(int i = 1; i <= 6; i++){ append(&a, i); }

		Slice<int> t3 = take(a, 3);
		t_expect(t, len(t3) == 3, "take(3) len == 3");
		t_expect(t, t3[0] == 1,   "take first == 1");
		t_expect(t, t3[2] == 3,   "take last == 3");

		Slice<int> s3 = skip(a, 3);
		t_expect(t, len(s3) == 3, "skip(3) len == 3");
		t_expect(t, s3[0] == 4,   "skip first == 4");
		t_expect(t, s3[2] == 6,   "skip last == 6");

		Slice<int> mid = slice(a, 1, 4);
		t_expect(t, len(mid) == 3, "slice(1,4) len == 3");
		t_expect(t, mid[0] == 2,   "slice first == 2");
		t_expect(t, mid[2] == 4,   "slice last == 4");

		Slice<int> all = slice(a);
		t_expect(t, len(all) == 6, "slice() len == 6");
	}
}
