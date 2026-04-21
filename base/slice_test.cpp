#pragma once
#include "testing.hpp"

void slice_test(Test* t){
	t->name = "Slice";

	// Basic construction
	{
		int buf[4] = {10, 20, 30, 40};
		Slice<int> s{buf, 4};

		t_expect(t, len(s) == 4,          "len of 4-element slice should be 4");
		t_expect(t, raw_data(s) == buf,    "raw_data should point to buf");
		t_expect(t, s[0] == 10,            "s[0] == 10");
		t_expect(t, s[3] == 40,            "s[3] == 40");
	}

	// Default construction
	{
		Slice<int> empty{};
		t_expect(t, len(empty) == 0,           "default slice len == 0");
		t_expect(t, raw_data(empty) == nullptr, "default slice data == nullptr");
	}

	// take
	{
		int buf[5] = {1, 2, 3, 4, 5};
		Slice<int> s{buf, 5};
		Slice<int> first3 = take(s, 3);

		t_expect(t, len(first3) == 3,  "take(3) len == 3");
		t_expect(t, first3[0] == 1,    "take first elem == 1");
		t_expect(t, first3[2] == 3,    "take last elem == 3");
	}

	// skip
	{
		int buf[5] = {1, 2, 3, 4, 5};
		Slice<int> s{buf, 5};
		Slice<int> last3 = skip(s, 2);

		t_expect(t, len(last3) == 3,  "skip(2) len == 3");
		t_expect(t, last3[0] == 3,    "skip first elem == 3");
		t_expect(t, last3[2] == 5,    "skip last elem == 5");
	}

	// slice(start, end)
	{
		int buf[6] = {0, 1, 2, 3, 4, 5};
		Slice<int> s{buf, 6};
		Slice<int> mid = slice(s, 2, 5);

		t_expect(t, len(mid) == 3,  "slice(2,5) len == 3");
		t_expect(t, mid[0] == 2,    "slice first elem == 2");
		t_expect(t, mid[2] == 4,    "slice last elem == 4");
	}

	// slice(whole) is idempotent
	{
		int buf[3] = {7, 8, 9};
		Slice<int> s{buf, 3};
		Slice<int> same = slice(s);

		t_expect(t, len(same) == 3,        "slice() len unchanged");
		t_expect(t, raw_data(same) == buf,  "slice() data unchanged");
	}

	// copy
	{
		int src_buf[4] = {1, 2, 3, 4};
		int dst_buf[4] = {0, 0, 0, 0};
		Slice<int> src{src_buf, 4};
		Slice<int> dst{dst_buf, 4};

		Slice<int> copied = copy(dst, src);
		t_expect(t, len(copied) == 4,  "copy returns 4-elem slice");
		t_expect(t, dst[0] == 1,       "copy dst[0] == 1");
		t_expect(t, dst[3] == 4,       "copy dst[3] == 4");
	}

	// copy partial (dst smaller than src)
	{
		int src_buf[4] = {10, 20, 30, 40};
		int dst_buf[2] = {0, 0};
		Slice<int> src{src_buf, 4};
		Slice<int> dst{dst_buf, 2};

		Slice<int> copied = copy(dst, src);
		t_expect(t, len(copied) == 2,  "partial copy len == 2");
		t_expect(t, dst[0] == 10,      "partial copy dst[0] == 10");
		t_expect(t, dst[1] == 20,      "partial copy dst[1] == 20");
	}
}
