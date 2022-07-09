#pragma once
#include "drivers/pinconf.hh"
#include "stm32mp1xx.h"
#include "stm32mp1xx_ll_usart.h"
#include <cstdint>

template<uint32_t BASE_ADDR>
class Uart {
	USART_TypeDef *const uart{reinterpret_cast<USART_TypeDef *>(BASE_ADDR)};

public:
	// Use only if UART has already been init by a previous stage (i.e. FSBL init the UART, so it's safe to use in app
	Uart() = default;

	// If you just construct with a baudrate, it defaults to 8N1
	Uart(PinConf txpin, PinConf rxpin, uint32_t baudrate)
	{
		txpin.init(PinMode::Alt);
		rxpin.init(PinMode::Alt);
		init(baudrate);
	}

	Uart(PinConf txpin, PinConf rxpin, const LL_USART_InitTypeDef &conf)
	{
		txpin.init(PinMode::Alt);
		rxpin.init(PinMode::Alt);
		init(conf);
	}

	void init(uint32_t baudrate)
	{
		LL_USART_InitTypeDef conf{
			.PrescalerValue = LL_USART_PRESCALER_DIV1,
			.BaudRate = baudrate,
			.DataWidth = LL_USART_DATAWIDTH_8B,
			.StopBits = LL_USART_STOPBITS_1,
			.Parity = LL_USART_PARITY_NONE,
			.TransferDirection = LL_USART_DIRECTION_TX_RX,
			.HardwareFlowControl = LL_USART_HWCONTROL_NONE,
			.OverSampling = LL_USART_OVERSAMPLING_16,
		};

		init(&conf);
	}

	void init(LL_USART_InitTypeDef *conf)
	{
		_enable_rcc();
		LL_USART_Disable(uart);
		LL_USART_Init(uart, conf);
		LL_USART_EnableFIFO(uart);
		LL_USART_Enable(uart);
	}

	void write(const char *str)
	{
		while (*str) {
			uart->TDR = *str++;
			delay_for_write();
		}
	}

	static void putchar(const char c)
	{
		while ((reinterpret_cast<USART_TypeDef *>(BASE_ADDR)->ISR & USART_ISR_TXFT) == 0)
			;
		reinterpret_cast<USART_TypeDef *>(BASE_ADDR)->TDR = c;
	}

	void write(const char c)
	{
		delay_for_write();
		uart->TDR = c;
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
	void delay_for_write()
	{
		while ((uart->ISR & USART_ISR_TXFT) == 0)
			;
	}

	void _enable_rcc()
	{
		if constexpr (BASE_ADDR == UART4_BASE)
			RCC->MP_APB1ENSETR = RCC->MP_APB1ENSETR | RCC_MP_APB1ENSETR_UART4EN;
		if constexpr (BASE_ADDR == USART6_BASE)
			RCC->MP_APB2ENSETR = RCC->MP_APB2ENSETR | RCC_MP_APB2ENSETR_USART6EN;
		// TODO: add the rest
	}
};
