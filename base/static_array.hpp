#pragma once
#include "base.hpp"
#include "memory.hpp"

template<typename T, usize N>
struct StaticArray {
private:
	T     _data[N];
	usize _len;

public:
	StaticArray<T, N>() : _data{}, _len{0} {}

	T& operator[](usize idx){
		ensure(idx < _len, "index out of bounds");
		return _data[idx];
	}

	T const& operator[](usize idx) const {
		ensure(idx < _len, "index out of bounds");
		return _data[idx];
	}

	friend attribute_force_inline constexpr usize len(StaticArray<T, N> const& a)      { return a._len; }
	friend attribute_force_inline constexpr usize cap(StaticArray<T, N> const&)        { return N; }
	friend attribute_force_inline constexpr T*    raw_data(StaticArray<T, N> const& a) { return const_cast<T*>(a._data); }

	friend attribute_force_inline
	void clear(StaticArray<T, N>* a){ a->_len = 0; }

	friend attribute_force_inline
	void destroy(StaticArray<T, N>* a){ a->_len = 0; }

	friend bool append(StaticArray<T, N>* a, T const& v){
		if(a->_len >= N){ return false; }
		a->_data[a->_len] = v;
		a->_len += 1;
		return true;
	}

	friend bool append_slice(StaticArray<T, N>* a, Slice<T> vals){
		if(a->_len + len(vals) > N){ return false; }
		mem_copy_no_overlap(&a->_data[a->_len], raw_data(vals), len(vals) * sizeof(T));
		a->_len += len(vals);
		return true;
	}

	friend void remove(StaticArray<T, N>* a, usize idx, sourcelocation loc = sourcelocation::current()){
		ensure(idx < a->_len, "out of bounds remove", loc);
		void* dst      = (void*)&a->_data[idx];
		void* src      = (void*)&a->_data[idx + 1];
		usize shift_by = (a->_len - idx - 1) * sizeof(T);
		mem_copy(dst, src, shift_by);
		a->_len -= 1;
	}

	friend bool insert(StaticArray<T, N>* a, usize idx, T const& elem, sourcelocation loc = sourcelocation::current()){
		ensure(idx <= a->_len, "out of bounds insert", loc);
		if(a->_len >= N){ return false; }
		void* dst      = (void*)&a->_data[idx + 1];
		void* src      = (void*)&a->_data[idx];
		usize shift_by = (a->_len - idx) * sizeof(T);
		mem_copy(dst, src, shift_by);
		a->_data[idx] = elem;
		a->_len += 1;
		return true;
	}

	[[nodiscard]] friend attribute_force_inline
	Slice<T> take(StaticArray<T, N> const& a, usize n, sourcelocation loc = sourcelocation::current()){
		ensure(n <= a._len, "cannot take more than length", loc);
		return Slice<T>{ const_cast<T*>(a._data), n };
	}

	[[nodiscard]] friend attribute_force_inline
	Slice<T> skip(StaticArray<T, N> const& a, usize n, sourcelocation loc = sourcelocation::current()){
		ensure(n <= a._len, "cannot skip more than length", loc);
		return Slice<T>{ const_cast<T*>(&a._data[n]), a._len - n };
	}

	[[nodiscard]] friend attribute_force_inline
	Slice<T> slice(StaticArray<T, N> const& a, usize start, usize end, sourcelocation loc = sourcelocation::current()){
		ensure(end <= a._len && start <= end, "invalid slice indexes", loc);
		return Slice<T>{ const_cast<T*>(&a._data[start]), end - start };
	}

	[[nodiscard]] friend attribute_force_inline
	Slice<T> slice(StaticArray<T, N> const& a){
		return Slice<T>{ const_cast<T*>(a._data), a._len };
	}
};
