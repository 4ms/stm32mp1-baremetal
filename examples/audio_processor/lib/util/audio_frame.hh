#pragma once
#include "math.hh"
#include <array>
#include <stdint.h>

template<typename SampleType, int UsedBits = sizeof(SampleType) * 8, int NumChannels = 2>
struct AudioFrame {
	std::array<SampleType, NumChannels> chan;

public:
	static constexpr inline unsigned kSampleTypeBits = sizeof(SampleType) * 8;
	static constexpr inline unsigned kMaxValue = MathTools::ipow(2, UsedBits - 1);
	static constexpr inline float kOutScaling = static_cast<float>(kMaxValue);

	static inline constexpr float scaleInput(SampleType val)
	{
		return sign_extend(val) / kOutScaling;
	}
	static inline constexpr SampleType scaleOutput(const float val)
	{
		const float v = MathTools::constrain(val, -1.f, (kOutScaling - 1.f) / kOutScaling);
		return static_cast<SampleType>(v * kOutScaling);
	}

	static inline constexpr SampleType sign_extend(const SampleType &v) noexcept
	{
		static_assert((sizeof(SampleType) * 8u) >= UsedBits, "SampleType is smaller than the specified width");
		if constexpr ((sizeof(SampleType) * 8u) == UsedBits)
			return v;
		else {
			using S = struct {
				signed Val : UsedBits;
			};
			return reinterpret_cast<const S *>(&v)->Val;
		}
	}
};
