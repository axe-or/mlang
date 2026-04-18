#pragma once
#include "base.hpp"
#include "memory.hpp"

template<typename T>
struct DynArray {
private:
	T*        data;
	usize     len;
	usize     cap;
	Allocator allocator;

public:
	DynArray<T>() : data{nullptr}, len{0}, cap{0}, allocator{} {}
	explicit DynArray<T>(Allocator a) : data{nullptr}, len{0}, cap{0}, allocator{a} {}

	T& operator[](usize idx){
		ensure(idx < len, "index out of bounds");
		return data[idx];
	}

	T const& operator[](usize idx) const {
		ensure(idx < len, "index out of bounds");
		return data[idx];
	}

	friend attribute_force_inline constexpr usize len(DynArray<T> const& a) { return a->len; }
	friend attribute_force_inline constexpr usize cap(DynArray<T> const& a) { return a->cap; }
	friend attribute_force_inline constexpr T*    raw_data(DynArray<T> a)  { return a->data; }

	friend bool resize(DynArray<T>* a, usize new_cap){
		void* new_data = a->allocator.realloc(
			a->data,
			sizeof(T) * a->cap, alignof(T),
			sizeof(T) * new_cap, alignof(T)
		);

		if(!new_data){ return false; }

		a->data = (T*)new_data;
		a->cap  = new_cap;
		a->len  = min(a->len, new_cap);
		return true;
	}

	friend bool append(DynArray<T>* a, T const& v){
		if(a->len >= a->cap){
			if(!resize(a, max<usize>(a->cap * 2, 16))){ return false; }
		}
		a->data[a->len] = v;
		a->len += 1;
		return true;
	}

	friend void remove(DynArray<T>* a, usize idx, sourcelocation loc = sourcelocation::current()){
		ensure(idx < a->len, "out of bounds remove", loc);
		if(!a->len){ return; }

		void* dst      = (void*)&a->data[idx];
		void* src      = (void*)&a->data[idx + 1];
		usize shift_by = (a->len - idx) * sizeof(T);

		mem_copy(dst, src, shift_by);
		a->len -= 1;
	}

	friend bool insert(DynArray<T>* a, usize idx, T const& elem, sourcelocation loc = sourcelocation::current()){
		ensure(idx <= a->len, "out of bounds insert", loc);
		if(a->len >= a->cap){
			if(!resize(a, max<usize>(a->cap * 2, 16))){ return false; }
		}

		void* dst      = (void*)&a->data[idx + 1];
		void* src      = (void*)&a->data[idx];
		usize shift_by = (a->len - idx) * sizeof(T);

		mem_copy(dst, src, shift_by);
		a->len += 1;
		a->data[idx] = elem;
		return true;
	}

	friend void destroy(DynArray<T>* a){
		a->allocator.free(a->data, sizeof(T) * a->cap, alignof(T));
		a->cap = 0; a->len = 0;
	}

	[[nodiscard]] friend attribute_force_inline
	Slice<T> take(DynArray<T> const& a, usize n, sourcelocation loc = sourcelocation::current()){
		ensure(n <= a.len, "cannot take more than length", loc);
		return Slice<T>{ a.data, n };
	}

	[[nodiscard]] friend attribute_force_inline
	Slice<T> skip(DynArray<T> const& a, usize n, sourcelocation loc = sourcelocation::current()){
		ensure(n <= a.len, "cannot skip more than length", loc);
		return Slice<T>{ &a.data[n], a.len - n };
	}

	[[nodiscard]] friend attribute_force_inline
	Slice<T> slice(DynArray<T> const& a, usize start, usize end, sourcelocation loc = sourcelocation::current()){
		ensure(end <= a.len && start <= end, "invalid slice indexes", loc);
		return Slice<T>{ &a.data[start], end - start };
	}
};

template<typename T> [[nodiscard]]
DynArray<T> make_dyn_array(Allocator a){
	return DynArray<T>{a};
}
