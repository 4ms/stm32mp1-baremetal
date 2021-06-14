#pragma once

#include "math.hh"

template<int Size, class T = unsigned int>
struct Oversampler {
	static_assert(MathTools::is_power_of_2(Size) > 0, "Oversampler<Size, T> requires Size to be a power of 2");

public:
	Oversampler() {}
	void add_val(T newval)
	{
		buff_ += newval;
		if (++idx_ >= Size) {
			val_ = buff_ >> oversample_shift_;
			idx_ = 0;
			buff_ = 0;
		}
	}
	T val()
	{
		return val_;
	}

private:
	constexpr static auto oversample_shift_ = MathTools::Log2<Size>::val;
	T buff_ = 0;
	T val_ = 0;
	unsigned int idx_ = 0;
};

struct NoFilter {
public:
	NoFilter() {}
	void add_val(unsigned newval)
	{
		val_ = newval;
	}
	unsigned val()
	{
		return val_;
	}

private:
	unsigned val_ = 0;
};

