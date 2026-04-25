#include "base.hpp"

template<u32 N, typename UInt = u8>
struct BitSet {
	static constexpr u32 slot_width = sizeof(UInt) * 8;
	static constexpr u32 slot_count = ((N + u32(slot_width - 1)) & ~u32(slot_width - 1)) / u32(slot_width);

	UInt data[slot_count]{};

	bool get(usize idx, sourcelocation loc = sourcelocation::current()) const {
		ensure(idx < N, "out of bounds index", loc);
		UInt b = data[idx / slot_width];
		UInt n = idx % slot_width;
		return b & (1ull << n);
	}

	void set(usize idx, bool val, sourcelocation loc = sourcelocation::current()){
		ensure(idx < N, "out of bounds index", loc);
		UInt b = data[idx / slot_width];
		UInt n = idx % slot_width;
		b = (b & ~(1ull << n)) | (usize(val) << n);
		data[idx / slot_width] = b;
	}

	constexpr
	bool operator==(BitSet<N, UInt> const& other){
		for(usize i = 0; i < slot_count; i += 1){
			if(data[i] != other.data[i]){
				return false;
			}
		}
		return true;
	}

	constexpr
	bool operator!=(BitSet<N, UInt> const& other){
		return !(*this == other);
	}

	constexpr
	BitSet<N, UInt> operator~(){
		BitSet<N, UInt> res{};
		for(usize i = 0; i < slot_count; i += 1){
			res.data[i] = ~data[i];
		}
		return res;
	}

	constexpr
	BitSet<N, UInt> operator&(BitSet<N, UInt> const& s){
		BitSet<N, UInt> res{};
		for(usize i = 0; i < slot_count; i += 1){
			res.data[i] = data[i] & s.data[i];
		}
		return res;
	}

	constexpr
	BitSet<N, UInt> operator|(BitSet<N, UInt> const& s){
		BitSet<N, UInt> res{};
		for(usize i = 0; i < slot_count; i += 1){
			res.data[i] = data[i] | s.data[i];
		}
		return res;
	}

	constexpr
	BitSet<N, UInt> operator^(BitSet<N, UInt> const& s){
		BitSet<N, UInt> res{};
		for(usize i = 0; i < slot_count; i += 1){
			res.data[i] = data[i] ^ s.data[i];
		}
		return res;
	}
};

