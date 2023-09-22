#pragma once
#include <cstdint>

struct Channel {
	uint16_t num;
	float phase;
};

unsigned make_number(Channel chan);
float perform_calculationA(float val);
unsigned perform_calculationB(float val);
