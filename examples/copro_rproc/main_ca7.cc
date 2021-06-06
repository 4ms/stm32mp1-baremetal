#include "delay.hh"
#include "leds.hh"
#include "stm32mp157cxx_ca7.h"
#include "uart.hh"
#include <stdint.h>

void main()
{
	Uart<UART4_BASE> uart;

	Led<GPIOZ_BASE, 6, LedActive::Low> red_led1;
	Led<GPIOZ_BASE, 7, LedActive::Low> green_led1;
	red_led1.init();
	green_led1.init();

	uart.write("\r\n\r\nA7 core is running...\r\n");

	green_led1.on();
	red_led1.on();
	uart.write("Starting M4 core after LED1 goes off...\r\n");
	Delay::cycles(10000000);
	red_led1.off();
	Delay::cycles(10000000);
	green_led1.off();

	RCC->MP_GCR = RCC_MP_GCR_BOOT_MCU;

	red_led1.off();
	green_led1.off();

	while (1) {
		red_led1.on();
		Delay::cycles(10000000);
		red_led1.off();

		green_led1.on();
		Delay::cycles(10000000);
		green_led1.off();
	};
}
