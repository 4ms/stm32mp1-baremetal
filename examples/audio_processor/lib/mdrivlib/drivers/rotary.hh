#pragma once
#include "pin.hh"
#include <cstdint>

namespace mdrivlib
{

class RotaryBase {
public:
	RotaryBase(GPIO portA, uint8_t pin_numA, GPIO portB, uint8_t pin_numB);
	void update(const uint32_t table_select);
	int32_t read_position();

private:
	Pin pinA;
	Pin pinB;
	uint8_t state_;
	int32_t position_;
	static const uint8_t StateTable[13][4];
};

enum RotaryStepSize { RotaryHalfStep = 0, RotaryFullStep = 6 };

template<RotaryStepSize StepSize>
class RotaryEncoder {
public:
	RotaryEncoder(GPIO portA, uint8_t pin_numA, GPIO portB, uint8_t pin_numB)
		: rotary{portA, pin_numA, portB, pin_numB} {
	}
	void update() {
		rotary.update(StepSize);
	}
	int32_t read() {
		return rotary.read_position();
	}

private:
	RotaryBase rotary;
};

} // namespace mdrivlib
