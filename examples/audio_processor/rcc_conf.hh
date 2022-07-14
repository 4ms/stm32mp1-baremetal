#pragma once

#include "drivers/stm32xx.h"

RCC_PeriphCLKInitTypeDef rcc_periph_clk_conf = {
	.PeriphClockSelection = RCC_PERIPHCLK_I2C12 | RCC_PERIPHCLK_SAI2,
	.PLL3 =
		{
			.PLLState = RCC_PLL_ON,
			.PLLSource = RCC_PLL3SOURCE_HSE,
			.PLLM = 2,
			.PLLN = 34,
			.PLLP = 2,	// 208.896MHZ for MCU
			.PLLQ = 34, //=>2*12.288014MHz for SAI
			.PLLR = 37, // 208.89624MHz for ...?
			.PLLRGE = RCC_PLL4IFRANGE_1,
			.PLLFRACV = 6685,
			.PLLMODE = RCC_PLL_FRACTIONAL,
		},
	.I2c12ClockSelection = RCC_I2C12CLKSOURCE_PCLK1,
	.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLL3_Q,
};
