#pragma once
#include "base.hpp"
#include "memory.hpp"

template<typename T>
struct DynArray {
private:
	T*     _data;
	usize  _len;
	usize  _cap;
	Allocator _allocator;

public:
	bool resize(usize new_cap) {
		void* new_data = _allocator.realloc(
			_data,
			sizeof(T) * _cap, alignof(T),
			sizeof(T) * new_cap, alignof(T)
		);

		if(!new_data){
			return false;
		}

		_data = (T*)new_data;
		_cap = new_cap;
		_len = min(_len, new_cap);
		return true;
	}

	bool push(T const& v){
		if(_len >= _cap){
			if(!this->resize(max<usize>(_cap * 2, 16))){
				return false;
			}
		}

		_data[_len] = v;
		_len += 1;
		return true;
	}

	void remove(usize idx, sourcelocation loc = sourcelocation::current()){
		ensure(idx < _len, "out of bounds remove", loc);
		if(!_len){ return; }

		void* remove = (void*)&_data[idx];
		void* rest = (void*)&_data[idx + 1];
		usize shift_by = (_len - idx) * sizeof(T);

		mem_copy(remove, rest, shift_by);
		_len -= 1;
	}

	bool insert(usize idx, T const& elem, sourcelocation loc = sourcelocation::current()){
		ensure(idx <= _len, "out of bounds insert", loc);
		if(_len >= _cap){
			if(!this->resize(max<usize>(_cap * 2, 16), sizeof(T), alignof(T))){
				return false;
			}
		}

		void* insert = (void*)&_data[idx];
		void* next = (void*)&_data[idx + 1];
		usize shift_by = (_len - idx) * sizeof(T);

		mem_copy(next, insert, shift_by);

		_len += 1;
		_data[idx] = elem;

		return true;
	}

	void destroy(){
		_allocator.free(_data, sizeof(T) * _cap, alignof(T));
		_cap = 0; _len = 0;
	}

	attribute_force_inline
	Slice<T> take(usize n, sourcelocation loc = sourcelocation::current()){
		ensure(n <= _len, "cannot take more than length", loc);
		return Slice<T>{ _data, n };
	}

	attribute_force_inline
	Slice<T> skip(usize n, sourcelocation loc = sourcelocation::current()){
		ensure(n <= _len, "cannot skip more than length", loc);
		return Slice<T>{ &_data[n], _len - n };
	}

	attribute_force_inline
	Slice<T> slice(usize start, usize end, sourcelocation loc = sourcelocation::current()){
		ensure(end <= _len && start <= end, "invalid slice indexes", loc);
		return Slice<T>{ &_data[start], end - start };
	}

	T& operator[](usize idx){
		ensure(idx < _len, "slice index out of bounds");
		return _data[idx];
	}

	T const& operator[](usize idx) const {
		ensure(idx < _len, "slice index out of bounds");
		return _data[idx];
	}

	static DynArray<T> from(Allocator a){
		DynArray arr = {};
		arr._allocator = a;
		return arr;
	}
};

template<typename T>
DynArray<T> make_dyn_array(Allocator a){
	return DynArray<T>::from(a);
}
