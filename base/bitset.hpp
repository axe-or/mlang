#include "base.hpp"

using BitSetSlot = u8;

template<u32 N>
struct BitSet {
	static constexpr u32 slot_width = sizeof(BitSetSlot) * 8;
	static constexpr u32 slot_count = ((N + u32(slot_width - 1)) & ~u32(slot_width - 1)) / u32(slot_width);

	BitSetSlot data[slot_count]{};

	bool get(usize idx, sourcelocation loc = sourcelocation::current()) const {
		ensure(idx < N, "out of bounds index", loc);
		BitSetSlot b = data[idx / slot_width];
		BitSetSlot n = idx % slot_width;
		return b & (1ull << n);
	}

	void set(usize idx, bool val, sourcelocation loc = sourcelocation::current()){
		ensure(idx < N, "out of bounds index", loc);
		BitSetSlot b = data[idx / slot_width];
		BitSetSlot n = idx % slot_width;
		b = (b & ~(1ull << n)) | (1ull << n);
		data[idx / slot_width] = b;
	}

	constexpr
	BitSet<N> operator~(){
		BitSet<N> res{};
		for(usize i = 0; i < slot_count; i += 1){
			res.data[i] = ~data[i];
		}
		return res;
	}

	constexpr
	BitSet<N> operator&(BitSet<N> const& s){
		BitSet<N> res{};
		for(usize i = 0; i < slot_count; i += 1){
			res.data[i] = data[i] & s.data[i];
		}
		return res;
	}

	constexpr
	BitSet<N> operator|(BitSet<N> const& s){
		BitSet<N> res{};
		for(usize i = 0; i < slot_count; i += 1){
			res.data[i] = data[i] | s.data[i];
		}
		return res;
	}

	constexpr
	BitSet<N> operator^(BitSet<N> const& s){
		BitSet<N> res{};
		for(usize i = 0; i < slot_count; i += 1){
			res.data[i] = data[i] ^ s.data[i];
		}
		return res;
	}
};

