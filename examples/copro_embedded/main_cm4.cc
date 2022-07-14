#include "delay.hh"
#include "drivers/uart.hh"
#include "leds.hh"
#include "stm32mp157cxx_cm4.h"
#include <stdint.h>

void main()
{
	Uart<UART4_BASE> uart;
	uart.write("M4: * yawn * \r\nM4: Hello from Cortex-M4!\r\n");

	Led<GPIOI_BASE, 8, LedActive::Low> red_led2;
	Led<GPIOI_BASE, 9, LedActive::Low> green_led2;
	red_led2.init();
	green_led2.init();

	red_led2.off();
	green_led2.off();

	uart.write("M4: I will be flashing LED2 slowly about 0.8Hz (if compiled at -O0)\r\n");
	while (1) {
		red_led2.on();
		Delay::cycles(10000000);
		red_led2.off();

		green_led2.on();
		Delay::cycles(10000000);
		green_led2.off();
	};
}
