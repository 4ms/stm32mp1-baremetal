#include "drivers/delay.hh"
#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "secondary_core.hh"
#include "stm32mp157cxx_ca7.h"
#include <cstdint>

namespace stm32mp1_baremetal_multicore
{
Uart<UART4_BASE> uart;
}

void main()
{
	using namespace stm32mp1_baremetal_multicore;

	uart.write("\r\n\r\n");
	uart.write("Core0: Flashing an LED is hard work! I'm going to wake up Core1 to help.\r\n");
	uart.write("Core0: I'll handle flashing LED1, and tell Core1 to flash LED2...\r\n");

	Led<GPIOZ_BASE, 6, LedActive::Low> red_led1;
	Led<GPIOZ_BASE, 7, LedActive::Low> green_led1;

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
	using namespace stm32mp1_baremetal_multicore;

	Led<GPIOI_BASE, 8, LedActive::Low> red_led2;
	Led<GPIOI_BASE, 9, LedActive::Low> green_led2;

	uart.write("Core1: Good morning, Core0! I'm happy to help, I can flash LED2 twice as fast as LED1!\r\n");
	while (1) {
		red_led2.on();
		Delay::cycles(5000000);
		red_led2.off();

		green_led2.on();
		Delay::cycles(5000000);
		green_led2.off();
	}
}
