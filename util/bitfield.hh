#pragma once
#include <cstdint>

template<typename FieldT>
struct BitFieldBase {
	//casts an enum class as a bitfield
	template<typename T>
	FieldT constexpr bitfield(T const val)
	{
		//assert(sizeof(Rt)>=sizeof(T) || val>>sizeof(Rt) == 0);
		return (static_cast<FieldT>(val));
	}
	template<typename... Args>
	FieldT constexpr bitfield(const Args... args)
	{
		return (... | bitfield(args));
	}
};

template<typename T>
uint8_t constexpr bitfield8(T const val)
{
	return (static_cast<uint8_t>(val));
}
template<typename... Args>
uint8_t constexpr bitfield8(const Args... args)
{
	return (... | bitfield8(args));
}
