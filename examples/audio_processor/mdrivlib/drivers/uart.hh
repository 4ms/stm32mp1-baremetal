#pragma once
#include "pin.hh"
#include "stm32xx.h"

class Uart {
public:
	Uart() {
		__HAL_RCC_USART6_CLK_ENABLE();
		Pin tx{GPIO::G, 14, PinMode::Alt, LL_GPIO_AF_8, PinPull::None, PinPolarity::Normal, PinSpeed::VeryHigh};
		Pin rx{GPIO::G, 9, PinMode::Alt, LL_GPIO_AF_8, PinPull::None, PinPolarity::Normal, PinSpeed::VeryHigh};

		hal_h.Instance = USART6;
		hal_h.Init.BaudRate = 115200;
		hal_h.Init.WordLength = UART_WORDLENGTH_8B;
		hal_h.Init.StopBits = UART_STOPBITS_1;
		hal_h.Init.Parity = UART_PARITY_NONE;
		hal_h.Init.Mode = UART_MODE_TX_RX;
		hal_h.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		hal_h.Init.OverSampling = UART_OVERSAMPLING_16;
		hal_h.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
		hal_h.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

		auto err = HAL_UART_Init(&hal_h);
	}

	void send(uint8_t *data, uint32_t len) {
		auto err = HAL_UART_Transmit(&hal_h, data, len, 10000);
		if (err != HAL_OK) {
			while (1)
				;
		}
	}

private:
	UART_HandleTypeDef hal_h;
};
