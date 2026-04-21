#pragma once
#include "testing.hpp"
#include "string.hpp"

void string_test(Test* t){
	t->name = "String";

	// len
	{
		String s = "hello";
		t_expect(t, len(s) == 5,  "len(\"hello\") == 5");

		String empty = "";
		t_expect(t, len(empty) == 0,  "len(\"\") == 0");

		String def{};
		t_expect(t, len(def) == 0,  "default String len == 0");
	}

	// equality
	{
		String a = "foo";
		String b = "foo";
		String c = "bar";
		t_expect(t, a == b,       "\"foo\" == \"foo\"");
		t_expect(t, !(a == c),    "\"foo\" != \"bar\"");
	}

	// raw_data
	{
		String s = "xyz";
		t_expect(t, raw_data(s)[0] == 'x',  "raw_data[0] == 'x'");
		t_expect(t, raw_data(s)[2] == 'z',  "raw_data[2] == 'z'");
	}

	// take / skip / slice
	{
		String s = "abcdef";
		t_expect(t, take(s, 3) == "abc",    "take(3) == \"abc\"");
		t_expect(t, skip(s, 3) == "def",    "skip(3) == \"def\"");
		t_expect(t, slice(s, 1, 4) == "bcd","slice(1,4) == \"bcd\"");
		t_expect(t, take(s, 0) == "",        "take(0) == \"\"");
		t_expect(t, skip(s, 6) == "",        "skip(6) == \"\"");
	}

	// str_find
	{
		String hay = "hello world";
		t_expect(t, str_find(hay, "world") == 6,   "find \"world\" at 6");
		t_expect(t, str_find(hay, "hello") == 0,   "find \"hello\" at 0");
		t_expect(t, str_find(hay, "xyz")   == -1,  "find absent == -1");
		t_expect(t, str_find(hay, "")      == 0,   "find \"\" == 0");
	}

	// str_trim_prefix / str_trim_suffix
	{
		String s = "foobar";
		t_expect(t, str_trim_prefix(s, "foo") == "bar",    "trim_prefix present");
		t_expect(t, str_trim_prefix(s, "baz") == "foobar", "trim_prefix absent noop");
		t_expect(t, str_trim_suffix(s, "bar") == "foo",    "trim_suffix present");
		t_expect(t, str_trim_suffix(s, "baz") == "foobar", "trim_suffix absent noop");
	}

	// str_trim_spaces
	{
		t_expect(t, str_trim_spaces("  hi  ")    == "hi",        "trim_spaces both sides");
		t_expect(t, str_trim_spaces("no space")  == "no space",  "trim_spaces noop");
		t_expect(t, str_trim_spaces("   ")       == "",          "trim_spaces all whitespace");
	}

	// str_trim_start / str_trim_end / str_trim
	{
		String s = "xxxhelloxxx";
		t_expect(t, str_trim_start(s, "x") == "helloxxx",  "trim_start 'x'");
		t_expect(t, str_trim_end(s, "x")   == "xxxhello",  "trim_end 'x'");
		t_expect(t, str_trim(s, "x")       == "hello",     "trim both 'x'");
	}

	// str_clone
	{
		Allocator a = heap_allocator();
		String original = "cloned";
		String cloned = str_clone(original, a);
		t_expect(t, cloned == original,                        "cloned equals original");
		t_expect(t, raw_data(cloned) != raw_data(original),   "cloned has distinct buffer");
	}

	// str_replace
	{
		Allocator a = heap_allocator();
		String s  = "aabbaabb";
		String r  = str_replace(s, "aa", "X", a);
		t_expect(t, r == "XbbXbb",   "replace all \"aa\" -> \"X\"");

		String r1 = str_replace(s, "aa", "X", a, 1);
		t_expect(t, r1 == "Xbbaabb", "replace first \"aa\" -> \"X\"");
	}

	// str_split
	{
		Allocator a = heap_allocator();
		Slice<String> parts = str_split("a,b,c", ",", a);
		t_expect(t, len(parts) == 3,   "split produces 3 parts");
		t_expect(t, parts[0] == "a",   "split[0] == \"a\"");
		t_expect(t, parts[1] == "b",   "split[1] == \"b\"");
		t_expect(t, parts[2] == "c",   "split[2] == \"c\"");
	}

	// UTF-8 iteration
	{
		String s = "Hi";
		rune expected[] = {'H', 'i'};
		usize i = 0;
		for(rune r : s){
			if(i < 2){
				t_expect(t, r == expected[i], "UTF-8 iter rune matches");
			}
			i++;
		}
		t_expect(t, i == 2, "UTF-8 iter visits 2 runes");
	}
}
