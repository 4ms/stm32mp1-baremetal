#include "delay.hh"
#include "leds.hh"
#include "stm32mp157cxx_cm4.h"
#include "uart.hh"
#include <stdint.h>

void main()
{
	Uart<UART4_BASE> uart;
	uart.write("Hello from M4!\r\n");

	Led<GPIOI_BASE, 8, LedActive::Low> red_led2;
	Led<GPIOI_BASE, 9, LedActive::Low> green_led2;
	red_led2.init();
	green_led2.init();

	red_led2.off();
	green_led2.off();

	while (1) {
		red_led2.on();
		Delay::cycles(10000000);
		red_led2.off();

		green_led2.on();
		Delay::cycles(10000000);
		green_led2.off();
	};
}
