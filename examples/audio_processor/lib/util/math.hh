#pragma once
#include "math_tables.hh"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace MathTools
{

#ifndef M_PI
	#define M_PI 3.14159265358979323846264338327950288f
#endif

template<typename Tval, typename Tin, typename Tout>
static constexpr Tout
map_value(const Tval x, const Tin in_min, const Tin in_max, const Tout out_min, const Tout out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

template<typename T>
static constexpr T constrain(const T val, const T min, const T max)
{
	return val < min ? min : val > max ? max : val;
}

template<typename T>
static constexpr T max(const T val1, const T val2)
{
	if (val1 > val2) {
		return val1;
	} else {
		return val2;
	}
}

template<typename T>
static constexpr T min(const T val1, const T val2)
{
	if (val1 < val2) {
		return val1;
	} else {
		return val2;
	}
}

inline float interpolate(float in1, float in2, float phase)
{
	return (in2 * phase) + in1 * (1.0f - phase);
}

template<class T>
T randomNumber(T minNum, T maxNum)
{
	return map_value<int, long, T>(std::rand(), 0, RAND_MAX, minNum, maxNum);
}

template<unsigned int N>
struct Log2 {
	enum { val = Log2<N / 2>::val + 1 };
};

template<>
struct Log2<1> {
	enum { val = 0 };
};

constexpr bool is_power_of_2(unsigned int v)
{
	return v && ((v & (v - 1)) == 0);
}

// Todo: log2_ceiling()

constexpr unsigned int log2_floor(const unsigned int x)
{
	int i = 32;
	while (i--) {
		if (x & (1UL << i))
			return i;
	}
	return 0;
}

constexpr unsigned int ipow(unsigned int a, unsigned int b)
{
	return b == 0 ? 1 : a * ipow(a, b - 1);
}

// Todo: this needs a better name
template<typename T>
constexpr unsigned int bipolar_type_range(T val)
{
	return ipow(2, (sizeof(val) * 8) - 1) - 1;
}

template<uint32_t Max, typename T = uint32_t>
T wrap(T val)
{
	while (val >= Max)
		val -= Max;
	return val;
}

constexpr float f_abs(float x)
{
	return (x >= 0.f) ? x : -x;
}

// [0..1] --> [-1..1]
// 0.00 => 0
// 0.25 => -1
// 0.50 => 0
// 0.75 => 1
// 1.00 => 0
constexpr float faster_sine(float x)
{
	x = (x * 2.f) - 1.f;
	return 4.f * (x - x * f_abs(x));
}

constexpr uint16_t swap_bytes16(uint16_t halfword)
{
	return ((halfword & 0xFF) << 8) | (halfword >> 8);
}

constexpr uint32_t swap_bytes32(uint32_t word)
{
	return ((word & 0x000000FF) << 24) | ((word & 0x0000FF00) << 8) | ((word & 0x00FF0000) >> 8) | (word >> 24);
}
constexpr uint32_t swap_bytes_and_combine(uint16_t halfword1, uint16_t halfword2)
{
	return ((halfword1 & 0xFF) << 24) | ((halfword1 & 0xFF00) << 8) | ((halfword2 & 0x00FF) << 8) | (halfword2 >> 8);
}

constexpr float setPitchMultiple(float val)
{
	float pitchMultiple = 1;
	if (val >= 0)
		pitchMultiple = exp5Table.interp(constrain(val, 0.0f, 1.0f));
	else {
		float invertPitch = val * -1.0f;
		pitchMultiple = 1.0f / exp5Table.interp(constrain(invertPitch, 0.0f, 1.0f));
	}

	return pitchMultiple;
}

static inline float audioFreqToNorm(float input) // normalized filter frequency conversion
{
	float output = 0;
	input = constrain(input, 20.0f, 20000.0f);
	float temp1 = logTable.interp(map_value(input, 20.0f, 20000.0f, 0.0f, 1.0f));
	output = (temp1 - 2.99573f) / (6.90776f);
	return output;
}

// Returns 2^x
static inline float pow2(float x)
{
	x = x / 5.0f;
	float res = 1.f;
	for (;;) {
		if (x < 1.f) {
			// Note: exp5Table.interp(x) = 2^(x*5), where 0 <= x <= 1
			res *= exp5Table.interp(x);
			break;
		} else {
			res *= 32.f;
			x -= 1.f;
		}
	}
	return res;
}

static inline float sin(float x)
{
	return sinTable.interp_wrap(x / (2.f * M_PI));
}

//
static inline float sin01(float x)
{
	return sinTable.interp_wrap(x);
}

static inline float cos(float x)
{
	return sinTable.interp_wrap((x / (2.f * M_PI)) + 0.25f);
}

static inline float cos_close(float x)
{
	return sinTable.closest_wrap((x / (2.f * M_PI)) + 0.25f);
}

static inline float tan(float x)
{
	return tanTable.interp_wrap(x / M_PI);
}

static inline float tan_close(float x)
{
	return tanTable.closest_wrap(x / M_PI);
}

}; // namespace MathTools
