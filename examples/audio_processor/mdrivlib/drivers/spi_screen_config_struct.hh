#pragma once
#include "interrupt.hh"
#include "pin.hh"
#include "spi_config_struct.hh"
#include "stm32xx.h"

struct DefaultSpiScreenConf {
	struct ScreenSpiConf : DefaultSpiConf {
		static constexpr uint16_t NumChips = 1;
		static constexpr bool use_hardware_ss = true;
		static constexpr SpiDataDir data_dir = SpiDataDir::TXOnly;
	};
	using DCPin = FPin<GPIO::A, 0>;

	static constexpr uint32_t width = 240;
	static constexpr uint32_t height = 240;
	static constexpr uint32_t rowstart = 80;
	static constexpr uint32_t colstart = 0;
};
