#pragma once
#include "util/math.hh"

struct float32x4_t {
	float v[4];
	float &operator[](unsigned i)
	{
		return v[i];
	}
	float operator[](unsigned i) const
	{
		return v[i];
	}
};

struct float32x2_t {
	float v[2];
	float &operator[](unsigned i)
	{
		return v[i];
	}
	float operator[](unsigned i) const
	{
		return v[i];
	}
};

inline float32x4_t vdupq_n_f32(float a)
{
	// out[0] = out[1] = out[2] = out[3] = a;
	return {a, a, a, a};
}

inline float32x4_t vld1q_f32(const float a[4])
{
	return {a[0], a[1], a[2], a[3]};
}

inline float32x4_t vmulq_f32(float32x4_t a, float32x4_t b)
{
	return {
		a.v[0] * b.v[0],
		a.v[1] * b.v[1],
		a.v[2] * b.v[2],
		a.v[3] * b.v[3],
	};
}

inline float32x4_t vmlaq_f32(float32x4_t &dst, float32x4_t a, float32x4_t b)
{
	dst.v[0] += a.v[0] * b.v[0];
	dst.v[1] += a.v[1] * b.v[1];
	dst.v[2] += a.v[2] * b.v[2];
	dst.v[3] += a.v[3] * b.v[3];

	return {
		dst.v[0],
		dst.v[1],
		dst.v[2],
		dst.v[3],
	};
}

inline float32x4_t vsubq_f32(float32x4_t a, float32x4_t b)
{
	return {a.v[0] - b.v[0], a.v[1] - b.v[1], a.v[2] - b.v[2], a.v[3] - b.v[3]};
}

inline float32x2_t vpadd_f32(float32x2_t a, float32x2_t b)
{
	return {
		a.v[0] + a.v[1],
		b.v[0] + b.v[1],
	};
}

inline float32x2_t vget_low_f32(float32x4_t a)
{
	return {a.v[0], a.v[1]};
}

inline float32x2_t vget_high_f32(float32x4_t a)
{
	return {a.v[2], a.v[3]};
}

inline float vget_lane_f32(float32x2_t a, unsigned int lane)
{
	return a.v[lane];
}

inline float32x4_t vmaxq_f32(float32x4_t a, float32x4_t b)
{
	return {
		MathTools::max(a.v[0], b.v[0]),
		MathTools::max(a.v[1], b.v[1]),
		MathTools::max(a.v[2], b.v[2]),
		MathTools::max(a.v[3], b.v[3]),
	};
}
