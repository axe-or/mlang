#include "base.hpp"
#include "memory.hpp"

template<typename K>
using MapHash = u64 (*)(K const*);

static inline
u64 fnv64a(Slice<u8> b){
	constexpr u64 offset_basis = 0xcbf29ce484222325ull;
	constexpr u64 prime = 0x00000100000001b3ull;

	u64 h = offset_basis;
	u8 const* data = raw_data(b);
	for(usize i = 0; i < len(b); i++){
		h = (h ^ (u64)data[i]) * prime;
	}
	return h;
}

template<typename T>
u64 map_default_hash(T const* k){
	return fnv64a(Slice<u8>{(u8*)k, sizeof(T)});
}

template<>
inline u64 map_default_hash<String>(String const* k){
	return fnv64a(Slice<u8>{(u8*)raw_data(*k), len(*k)});
}

template<typename K, typename V>
struct MapSlot {
	K key;
	V val;
	MapSlot<K, V>* next; // Pointed to itself do indicate end of list. Points to NULL to indicate slot vacancy
};

template<typename K, typename V, MapHash H = map_default_hash<K>>
struct Map {
	Slice<MapSlot<K, V>> slots;
	Allocator allocator;
	usize elem_count;

	MapSlot<K, V>* get(K const& key){
		auto& m = *this;

		u64 pos = H(&key) % len(map->slots);
		
		auto start_slot = &map->slots[pos];

		if(start_slot->next == nullptr){
			return nullptr;
		}

		for(auto slot = start_slot; slot->next != start_slot; slot = slot->next){
			if(slot->key == key){
				return slot;
			}
		}

		return nullptr;
	}
};


