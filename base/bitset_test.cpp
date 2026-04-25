#pragma once
#include "testing.hpp"
#include "bitset.hpp"

template<typename UInt>
void bitset_test_impl(Test* t){
	constexpr u32 W = sizeof(UInt) * 8; // bits per slot

	// Default init: all bits zero (two slots)
	{
		BitSet<W*2, UInt> bs{};
		bool all_zero = true;
		for(usize i = 0; i < W*2; i++){
			if(bs.get(i)){ all_zero = false; break; }
		}
		t_expect(t, all_zero, "default init: all bits zero");
	}

	// set/get: first, mid, cross-slot boundary, last
	{
		BitSet<W*2, UInt> bs{};
		bs.set(0,     true);
		bs.set(W/2,   true);
		bs.set(W-1,   true); // last bit slot 0
		bs.set(W,     true); // first bit slot 1
		bs.set(W*2-1, true); // last bit overall
		t_expect(t, bs.get(0), "set/get bit 0");
		t_expect(t, bs.get(W/2), "set/get bit W/2");
		t_expect(t, bs.get(W-1), "set/get last bit slot 0");
		t_expect(t, bs.get(W), "set/get first bit slot 1");
		t_expect(t, bs.get(W*2-1), "set/get last bit");
		t_expect(t, !bs.get(1), "bit 1 unset");
		t_expect(t, !bs.get(W+1), "bit W+1 unset");
	}

	// set false clears bit
	{
		BitSet<W, UInt> bs{};
		bs.set(W/2, true);
		t_expect(t, bs.get(W/2),  "bit W/2 set");
		bs.set(W/2, false);
		t_expect(t, !bs.get(W/2), "bit W/2 cleared");
	}

	// operator~
	{
		BitSet<W, UInt> bs{};
		bs.set(0, true);
		bs.set(2, true);
		BitSet<W, UInt> inv = ~bs;
		t_expect(t, !inv.get(0), "~: bit 0 flipped off");
		t_expect(t, !inv.get(2), "~: bit 2 flipped off");
		t_expect(t, inv.get(1), "~: bit 1 flipped on");
		t_expect(t, inv.get(3), "~: bit 3 flipped on");
	}

	// operator&
	{
		BitSet<W, UInt> a{}, b{};
		a.set(0, true); a.set(1, true);
		b.set(1, true); b.set(2, true);
		BitSet<W, UInt> r = a & b;
		t_expect(t, !r.get(0), "AND: bit 0 off");
		t_expect(t, r.get(1), "AND: bit 1 on");
		t_expect(t, !r.get(2), "AND: bit 2 off");
	}

	// operator|
	{
		BitSet<W, UInt> a{}, b{};
		a.set(0, true); a.set(1, true);
		b.set(1, true); b.set(2, true);
		BitSet<W, UInt> r = a | b;
		t_expect(t, r.get(0), "OR: bit 0 on");
		t_expect(t, r.get(1), "OR: bit 1 on");
		t_expect(t, r.get(2), "OR: bit 2 on");
		t_expect(t, !r.get(3), "OR: bit 3 off");
	}

	// operator^
	{
		BitSet<W, UInt> a{}, b{};
		a.set(0, true); a.set(1, true);
		b.set(1, true); b.set(2, true);
		BitSet<W, UInt> r = a ^ b;
		t_expect(t, r.get(0), "XOR: bit 0 on");
		t_expect(t, !r.get(1), "XOR: bit 1 off (both set)");
		t_expect(t, r.get(2), "XOR: bit 2 on");
		t_expect(t, !r.get(3), "XOR: bit 3 off");
	}

	// operator== and !=
	{
		BitSet<W*2, UInt> a{}, b{};
		t_expect(t, a == b, "== empty sets equal");
		a.set(W, true);
		t_expect(t, a != b, "!= after set");
		b.set(W, true);
		t_expect(t, a == b, "== after matching set");
	}
}

void bitset_test(Test* t){
	t->name = "BitSet";
	bitset_test_impl<u8>(t);
	bitset_test_impl<u16>(t);
	bitset_test_impl<u32>(t);
	bitset_test_impl<u64>(t);
}
