#pragma once
#include <array>
#include <type_traits>

template<typename KeyT, typename ValT, size_t N>
class SequentialMap {
public:
	//SequentialMap() {}
	struct PairT {
		KeyT key;
		ValT element;
		//const ValT& operator()() const { return element; }
	};
	using DataT = std::array<PairT, N>;
	DataT data;

	const ValT& operator[](const KeyT key) const {
		for (auto & pair : data) {
			if (pair.key == key)
				return pair.element;
		}
		return data[0].element;
	}
	ValT& operator[](const KeyT key) {
		for (auto & pair : data) {
			if (pair.key == key)
				return pair.element;
		}
		return data[0].element;
	}

	//const DataT& operator()() const { return data; }
	using const_seqmap_iterator = typename DataT::const_iterator;
    const const_seqmap_iterator begin() const { return data.begin(); }
    const const_seqmap_iterator end() const { return data.end(); }
};

