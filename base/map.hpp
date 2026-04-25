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
u64 map_default_hash(T const* k) = delete;

template<>
inline u64 map_default_hash<String>(String const* k){
	return fnv64a(Slice<u8>{(u8*)raw_data(*k), len(*k)});
}

template<typename K, typename V>
struct MapSlot {
	K key;
	V val;
	MapSlot<K, V>* next;
};

template<typename K, typename V, MapHash<K> H = map_default_hash<K>>
struct Map {
	// Slice<MapSlot<K, V>> slots;
	// Allocator allocator;
	// usize elem_count;

	// MapSlot<K, V>* get(K const& key){
	// 	return nullptr;
	// }

	// static
	// Map<K,V,H> create(Allocator a, usize n){
	// 	auto slot_data = make_slice<MapSlot<K, V>>(a, n);
	// 	auto slots = make_slice<MapSlot<K, V>*>(a, n);
	// }
};


