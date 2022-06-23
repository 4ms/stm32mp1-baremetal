#pragma once
#include "stm32mp1xx.h"
#include <cstdint>

// No init is needed because U-boot initializes it
template<uint32_t BASE_ADDR>
class Uart {
	USART_TypeDef *const uart;

public:
	Uart()
		: uart{reinterpret_cast<USART_TypeDef *>(BASE_ADDR)}
	{}

	// Defaults to 8N1, txrx
	void init(uint32_t baudrate)
	{
		uint32_t txrx = (USART_CR1_TE | USART_CR1_RE);
		uart->CR1 = txrx;
	}

	void write(const char *str)
	{
		while (*str) {
			uart->TDR = *str++;
			delay_for_write();
		}
	}

	void write(uint32_t value)
	{
		if (!value) {
			write("0");
			return;
		}

		constexpr int MAX_DIGITS = 10;
		char buf[MAX_DIGITS + 1];
		int len = 0;
		do {
			const char digit = (char)(value % 10);
			buf[len++] = '0' + digit;
			value /= 10;
		} while (value && (len < MAX_DIGITS));
		buf[len] = '\0';

		for (int i = 0; i < len / 2; i++) {
			auto tmp = buf[i];
			buf[i] = buf[len - i - 1];
			buf[len - i - 1] = tmp;
		}

		write(buf);
	}

private:
	void delay_for_write(void)
	{
		while ((uart->ISR & USART_ISR_TXFT) == 0)
			;
	}
};
