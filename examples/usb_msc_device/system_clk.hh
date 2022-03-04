#include "stm32mp1xx.h"

struct SystemClocks {
	static inline RCC_ClkInitTypeDef rcc_clk_conf = {
		.ClockType = RCC_CLOCKTYPE_MPU | RCC_CLOCKTYPE_ACLK | RCC_CLOCKTYPE_HCLK /* <-- MCU Clock */ |
					 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK3 | RCC_CLOCKTYPE_PCLK4 |
					 RCC_CLOCKTYPE_PCLK5,
		.MPUInit =
			{
				.MPU_Clock = RCC_MPUSOURCE_PLL1,
				.MPU_Div = RCC_MPU_DIV2,
			},
		.AXISSInit =
			{
				.AXI_Clock = RCC_AXISSOURCE_PLL2,
				.AXI_Div = RCC_MPU_DIV_OFF,
			},
		.MCUInit =
			{
				.MCU_Clock = RCC_MCUSSOURCE_PLL3,
				.MCU_Div = RCC_MCU_DIV1,
			},
		.APB4_Div = RCC_APB4_DIV2,
		.APB5_Div = RCC_APB5_DIV4,
		.APB1_Div = RCC_APB1_DIV2,
		.APB2_Div = RCC_APB2_DIV2,
		.APB3_Div = RCC_APB3_DIV2,
	};

	static inline RCC_PeriphCLKInitTypeDef rcc_periph_clk_conf = {
		.PeriphClockSelection = RCC_PERIPHCLK_USBO | RCC_PERIPHCLK_USBPHY,
		.PLL4 =
			{
				.PLLState = RCC_PLL_ON,
				.PLLSource = RCC_PLL4SOURCE_HSE,
				.PLLM = 4,
				.PLLN = 99,
				.PLLP = 6,
				.PLLQ = 8, // 112,
				.PLLR = 8,
				.PLLRGE = RCC_PLL4IFRANGE_0, // was 1
				.PLLFRACV = 0x1000,
				.PLLMODE = RCC_PLL_INTEGER,
			},

		.UsbphyClockSelection = RCC_USBPHYCLKSOURCE_HSE, // HSE = 00, PLL4 = 01, HSE/2 = 0b10
		.UsboClockSelection = RCC_USBOCLKSOURCE_PHY,	 // PHY = 1, PLL4 = 0

	};

	static bool init() {
		if (HAL_RCC_ClockConfig(&rcc_clk_conf) != HAL_OK)
			return false;
		if (HAL_RCCEx_PeriphCLKConfig(&rcc_periph_clk_conf) != HAL_OK)
			return false;
		return true;
	}
};
