#pragma once
#include "stm32mp157cxx_ca7.h"

template<uint32_t BASE_ADDR>
class Uart {
	USART_TypeDef *const uart;

public:
	Uart()
		: uart{reinterpret_cast<USART_TypeDef *>(BASE_ADDR)}
	{}

	void write(const char *str)
	{
		while (*str) {
			uart->TDR = *str++;
			delay_for_write();
		}
	}

private:
	void delay_for_write(void)
	{
		while ((UART4->ISR & USART_ISR_TXFT) == 0)
			;
	}
};
