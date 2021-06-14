#pragma once
#include "adc_builtin_driver.hh"
namespace mdrivlib
{
template<AdcPeriphNum p, AdcChanNum c, typename FILTER>
struct AnalogInBuiltin : AdcChan<p, c> {
	AnalogInBuiltin(GPIO port, uint8_t pin_num) {
		Pin pin(port, pin_num, PinMode::Analog);
	}
	void read() {
		oversampler_.add_val(this->get_val());
	}
	auto get() {
		return oversampler_.val();
	}

private:
	FILTER oversampler_;
};

// Todo: is this worth getting working? ~12% faster than AnalogIn
template<typename ADCIN, uint16_t *const DMABUFFER, typename FILTER>
struct AnalogInPtr {
	AnalogInPtr(GPIO port, uint8_t pin_num)
		: offset_(ADCIN::get_rank()) {
		Pin pin(port, pin_num, PinMode::Analog);
	}

	void read() {
		oversampler_.add_val(*(DMABUFFER + offset_));
	}
	auto get() {
		return oversampler_.val();
	}

private:
	unsigned offset_;
	FILTER oversampler_;
};
// Usage
// AnalogInPtr<AdcChan<AdcPeriphNum::_1, AdcChanNum::_0>, adc_buffer, Oversampler<16>> freq_cv1 = {GPIO::A, 0};
} // namespace mdrivlib
