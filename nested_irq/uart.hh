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
	// Todo: poll registers to see if TX is possible, instead of hard delay
	void delay_for_write(void)
	{
		int i = 255;
		while (i--)
			;
	}
};
