#pragma once
#include "sys/alloc_buffer.hh"
#include <cstddef>
#include <type_traits>

// BigBuffer is an RAII class that allocates a given type in the "big" heap.
// The "big" heap is platofrom-specific and is defined in sys/alloc_buffer.hh
// It requires that the class it wraps has the [] operator (i.e., a std::array or InterpArray, etc)
template<typename T>
struct BigBuffer : BigHeapAllocation {
	T *data;
	using ElementType = typename std::decay_t<decltype((*data)[0])>;

	BigBuffer()
	{
		data = new T;
	}
	~BigBuffer()
	{
		delete data;
	}
	T &get()
	{
		return *data;
	}

	// mybuf[x] = something;
	ElementType &operator[](const size_t x)
	{
		return (*data)[x];
	}

	// something = mybuf[x];
	const ElementType operator[](size_t x) const
	{
		return (*data)[x];
	}
};

