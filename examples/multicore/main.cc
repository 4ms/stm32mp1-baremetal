#include "delay.hh"
#include "leds.hh"
#include "mdrivlib/interrupt.hh"
#include "mdrivlib/interrupt_control.hh"
#include "secondary_core.hh"
#include "stm32mp157cxx_ca7.h"
#include "uart.hh"
#include <cstdint>

Uart<UART4_BASE> uart;

void main()
{
	// Core 1 flashes LED1
	uart.write("\r\n\r\nCore0: Running: flashing LED1\r\n");

	// LED setup
	Led<GPIOZ_BASE, 6, LedActive::Low> red_led1;
	Led<GPIOZ_BASE, 7, LedActive::Low> green_led1;
	red_led1.init();
	green_led1.init();

	uart.write("Core0: Starting Core1\r\n");
	Delay::cycles(10000000);

	SecondaryCore::start();

	while (1) {
		red_led1.on();
		Delay::cycles(10000000);
		red_led1.off();

		green_led1.on();
		Delay::cycles(10000000);
		green_led1.off();
	}
}

extern "C" void aux_core_main()
{
	// Core 2 flashes LED2 twice the rate of the other LED
	Led<GPIOI_BASE, 8, LedActive::Low> red_led2;
	Led<GPIOI_BASE, 9, LedActive::Low> green_led2;
	red_led2.init();
	green_led2.init();

	uart.write("Core1: Running: flashing LED2\r\n");
	uart.write("You should see LED2 flashing twice as fast as LED1.\r\n");
	while (1) {
		red_led2.on();
		Delay::cycles(5000000);
		red_led2.off();

		green_led2.on();
		Delay::cycles(5000000);
		green_led2.off();
	}
}
