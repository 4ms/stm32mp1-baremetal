#pragma once
#include <stdint.h>

// actual freq = update_rate(Hz) * increment(#) / 2^32
// increment = 2^32 * freq / update_rate
template<int UpdateRateHz>
struct TriangleOscillator {
	TriangleOscillator(uint32_t freq)
		: increment_(freq * freq_units)
	{}
	TriangleOscillator()
		: increment_(0)
	{}

	void update()
	{
		phase_ += increment_;
	}
	uint32_t val()
	{
		return (phase_ > max_ / 2) ? (max_ - phase_ * 2) : (phase_ * 2);
	}
	uint32_t Process()
	{
		update();
		return val();
	}

	void set_period_ms(uint32_t period_ms)
	{
		increment_ = freq_units / (period_ms / 1000);
	}
	void set_period_sec(uint32_t period_sec)
	{
		increment_ = freq_units / period_sec;
	}

	// This doesn't handle freq < 1 (e.g. slow LFO)
	void set_frequency(uint32_t freq)
	{
		increment_ = freq * freq_units;
	}

	void set_frequency(int freq)
	{
		increment_ = freq * freq_units;
	}

	void set_frequency(float freq)
	{
		increment_ = static_cast<uint32_t>(freq * static_cast<float>(freq_units));
	}

	void set_phase(uint32_t phase)
	{
		phase_ = phase;
	}

private:
	static constexpr uint32_t max_ = 0xFFFFFFFF;
	static constexpr uint32_t freq_units = max_ / UpdateRateHz;
	uint32_t phase_ = 0;
	uint32_t increment_;
};

template<int UpdateRateHz>
struct PhaseAccum {
	PhaseAccum(uint32_t freq)
		: increment_(freq * freq_units)
	{}
	PhaseAccum()
		: increment_(0)
	{}
	uint32_t Process()
	{
		phase_ += increment_;
		return phase_;
	}

	void set_frequency(uint32_t freq)
	{
		increment_ = freq * freq_units;
	}
	void set_phase(uint32_t phase)
	{
		phase_ = phase;
	}

private:
	static constexpr uint32_t max_ = 0xFFFFFFFF;
	static constexpr uint32_t freq_units = max_ / UpdateRateHz;
	uint32_t phase_ = 0;
	uint32_t increment_;
};

