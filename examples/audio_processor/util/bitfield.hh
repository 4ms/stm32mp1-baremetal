#pragma once

template <typename FieldT>
struct BitFieldBase {
	//casts an enum class as a bitfield
	template <typename T>
	FieldT constexpr bitfield(T const val) {
		//assert(sizeof(Rt)>=sizeof(T) || val>>sizeof(Rt) == 0);
		return(static_cast<FieldT>(val));
	}
	template<typename... Args>
	FieldT constexpr bitfield(const Args... args) {
		return ( ... | bitfield(args));
	}
};

