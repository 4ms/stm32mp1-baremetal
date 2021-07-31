#pragma once
#include "drivers/stm32xx.h"
// #include "periph.hh"
// #include "rcc.hh"

// Todo: refactor to use CMSIS header intead of HAL (RCC_OscInitTypeDef, etc)
namespace mdrivlib
{
struct System {
	System() = delete;

	static void SetVectorTable(uint32_t reset_address) {
		SCB->VTOR = reset_address & (uint32_t)0x1FFFFF80;
	}
};

} // namespace mdrivlib
