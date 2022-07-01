#include "stm32mp1xx_hal_rcc.h"

struct SystemClocks {
	static unsigned init_core_clocks(uint32_t MPU_MHz = 800)
	{
		RCC_OscInitTypeDef rcc_osc_conf = {
			.OscillatorType = RCC_OSCILLATORTYPE_HSE,
			.HSEState = RCC_HSE_BYPASS,
			.PLL =
				{
					.PLLState = RCC_PLL_ON,
					.PLLSource = RCC_PLL12SOURCE_HSE,
					.PLLM = 3,
					.PLLN = MPU_MHz == 650 ? 81U : 100U,
					.PLLP = 1,
					.PLLQ = 2,
					.PLLR = 2,
					.PLLFRACV = MPU_MHz == 650 ? 2048U : 0U,
					.PLLMODE = MPU_MHz == 650 ? RCC_PLL_FRACTIONAL : RCC_PLL_INTEGER,
				},
			.PLL2 =
				{
					.PLLState = RCC_PLL_ON,
					.PLLSource = RCC_PLL12SOURCE_HSE,
					.PLLM = 3,
					.PLLN = 66,
					.PLLP = 2,
					.PLLQ = 1,
					.PLLR = 1,
					.PLLFRACV = 5120,
					.PLLMODE = RCC_PLL_FRACTIONAL,
				},
		};

		auto err_deinit = HAL_RCC_DeInit();

		auto err = HAL_RCC_OscConfig(&rcc_osc_conf);

		__HAL_RCC_MPU_SOURCE(RCC_MPUSOURCE_PLL1);
		while (!RCC_FLAG_MPUSRCRDY)
			;

		RCC_ClkInitTypeDef rcc_mpuclk_conf = {
			.ClockType = RCC_CLOCKTYPE_MPU | RCC_CLOCKTYPE_ACLK /*| RCC_CLOCKTYPE_HCLK*/,
			.MPUInit =
				{
					.MPU_Clock = RCC_MPUSOURCE_PLL1,
					.MPU_Div = RCC_MPU_DIV_OFF,
				},
			.AXISSInit =
				{
					.AXI_Clock = RCC_AXISSOURCE_PLL2,
					.AXI_Div = RCC_MPU_DIV_OFF,
				},
		};

		auto clk_err = HAL_RCC_ClockConfig(&rcc_mpuclk_conf);
		return static_cast<unsigned>(err + 1000 * err_deinit + 10000 * clk_err);
	}
};
