#include "stm32mp1xx_hal_rcc.h"

struct SystemClocks {
	static unsigned init_pll1_pll2()
	{
		RCC_OscInitTypeDef rcc_osc_conf = {
			.OscillatorType = RCC_OSCILLATORTYPE_HSE,
			.HSEState = RCC_HSE_ON,
			.PLL =
				{
					.PLLState = RCC_PLL_ON,
					.PLLSource = RCC_PLL12SOURCE_HSE,
					.PLLM = 3,
					.PLLN = 100,
					.PLLP = 1,
					.PLLQ = 2,
					.PLLR = 2,
					.PLLFRACV = 0,
					.PLLMODE = RCC_PLL_INTEGER,
				},
			.PLL2 =
				{
					.PLLState = RCC_PLL_ON,
					.PLLSource = RCC_PLL12SOURCE_HSE,
					.PLLM = 3,
					.PLLN = 66,
					.PLLP = 2,
					.PLLQ = 2,
					.PLLR = 1,
					.PLLFRACV = 4096,
					.PLLMODE = RCC_PLL_FRACTIONAL,
				},
		};

		// __HAL_RCC_MPU_SOURCE(RCC_MPUSOURCE_HSI);
		// while (!RCC_FLAG_MPUSRCRDY)
		// 	;

		// __HAL_RCC_MPU_DIV(RCC_MPU_DIV_OFF);
		// for (int i = 0; i < 100000; i++)
		// 	;

		auto err_deinit = HAL_RCC_DeInit();
		auto err = HAL_RCC_OscConfig(&rcc_osc_conf);

		// __HAL_RCC_MPU_SOURCE(RCC_MPUSOURCE_PLL1);
		// while (!RCC_FLAG_MPUSRCRDY)
		// 	;
		return static_cast<unsigned>(err + 1000 * err_deinit);
	}
};
