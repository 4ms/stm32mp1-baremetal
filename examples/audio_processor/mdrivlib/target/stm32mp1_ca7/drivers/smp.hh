#pragma once
#include "drivers/secondary_core_control.hh"
#include "stm32xx.h"
#include <cstdint>

struct SMPControl {
	static void notify(uint32_t channel) {
		if (channel == 1)
			SecondaryCore::send_sgi(SGI1_IRQn);
		if (channel == 2)
			SecondaryCore::send_sgi(SGI2_IRQn);
	}

	static uint32_t read() {
		return TAMP->BKP31R;
	}

	static void write(uint32_t value) {
		TAMP->BKP31R = value;
	}
};
