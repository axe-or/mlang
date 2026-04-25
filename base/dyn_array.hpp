#pragma once
#include "base.hpp"
#include "memory.hpp"

template<typename T>
struct DynArray {
private:
	T*        _data;
	usize     _len;
	usize     _cap;
	Allocator _allocator;

public:
	DynArray() : _data{nullptr}, _len{0}, _cap{0}, _allocator{} {}
	explicit DynArray(Allocator a) : _data{nullptr}, _len{0}, _cap{0}, _allocator{a} {}

	T& operator[](usize idx){
		ensure(idx < _len, "index out of bounds");
		return _data[idx];
	}

	T const& operator[](usize idx) const {
		ensure(idx < _len, "index out of bounds");
		return _data[idx];
	}

	friend attribute_force_inline constexpr usize len(DynArray<T> const& a)     { return a._len; }
	friend attribute_force_inline constexpr usize cap(DynArray<T> const& a)     { return a._cap; }
	friend attribute_force_inline constexpr T*    raw_data(DynArray<T> const& a){ return a._data; }

	friend attribute_force_inline
	void clear(DynArray<T>* a){ a->_len = 0; }

	friend bool resize(DynArray<T>* a, usize new_cap){
		void* new_data = mem_realloc(
			a->_allocator,
			a->_data,
			sizeof(T) * a->_cap, alignof(T),
			sizeof(T) * new_cap, alignof(T)
		);

		if(!new_data){ return false; }

		a->_data = (T*)new_data;
		a->_cap  = new_cap;
		a->_len  = min(a->_len, new_cap);
		return true;
	}

	friend bool append(DynArray<T>* a, T const& v){
		if(a->_len >= a->_cap){
			if(!resize(a, max(a->_cap * 2, 16))){ return false; }
		}
		a->_data[a->_len] = v;
		a->_len += 1;
		return true;
	}

	friend bool append_slice(DynArray<T>* a, Slice<T> vals){
		if((a->_len + len(vals)) >= a->_cap){
			if(!resize(a, max(len(vals) + a->_cap, a->_cap * 2))){
				return false;
			}
		}

		mem_copy(&a->_data[a->_len], raw_data(vals), len(vals) * sizeof(T));
		a->_len += len(vals);
		return true;
	}

	friend void remove(DynArray<T>* a, usize idx, sourcelocation loc = sourcelocation::current()){
		ensure(idx < a->_len, "out of bounds remove", loc);
		if(!a->_len){ return; }

		void* dst      = (void*)&a->_data[idx];
		void* src      = (void*)&a->_data[idx + 1];
		usize shift_by = (a->_len - idx) * sizeof(T);

		mem_copy(dst, src, shift_by);
		a->_len -= 1;
	}

	friend bool insert(DynArray<T>* a, usize idx, T const& elem, sourcelocation loc = sourcelocation::current()){
		ensure(idx <= a->_len, "out of bounds insert", loc);
		if(a->_len >= a->_cap){
			if(!resize(a, max<usize>(a->_cap * 2, 16))){ return false; }
		}

		void* dst      = (void*)&a->_data[idx + 1];
		void* src      = (void*)&a->_data[idx];
		usize shift_by = (a->_len - idx) * sizeof(T);

		mem_copy(dst, src, shift_by);
		a->_len += 1;
		a->_data[idx] = elem;
		return true;
	}

	friend void destroy(DynArray<T>* a){
		mem_free(a->_allocator, a->_data, sizeof(T) * a->_cap, alignof(T));
		a->_cap = 0; a->_len = 0;
	}

	[[nodiscard]] friend attribute_force_inline
	Slice<T> take(DynArray<T> const& a, usize n, sourcelocation loc = sourcelocation::current()){
		ensure(n <= a._len, "cannot take more than length", loc);
		return Slice<T>{ a._data, n };
	}

	[[nodiscard]] friend attribute_force_inline
	Slice<T> skip(DynArray<T> const& a, usize n, sourcelocation loc = sourcelocation::current()){
		ensure(n <= a._len, "cannot skip more than length", loc);
		return Slice<T>{ &a._data[n], a._len - n };
	}

	[[nodiscard]] friend attribute_force_inline
	Slice<T> slice(DynArray<T> const& a, usize start, usize end, sourcelocation loc = sourcelocation::current()){
		ensure(end <= a._len && start <= end, "invalid slice indexes", loc);
		return Slice<T>{ &a._data[start], end - start };
	}

	[[nodiscard]] friend attribute_force_inline
	Slice<T> slice(DynArray<T> const& a){
		return Slice<T>{ a._data, a._len };
	}
};

template<typename T> [[nodiscard]]
DynArray<T> make_dyn_array(Allocator a){
	return DynArray<T>{a};
}
