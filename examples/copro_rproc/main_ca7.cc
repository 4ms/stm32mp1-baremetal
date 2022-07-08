#include "drivers/delay.hh"
#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "stm32mp157cxx_ca7.h"
#include <stdint.h>

#include "osd32brk_conf.hh"
#include "stm32disco_conf.hh"

// Uncomment one of these to select your board:
namespace Board = OSD32BRK;
// namespace Board = STM32MP1Disco;

void main()
{
	Uart<Board::ConsoleUART> uart;

	Board::RedLED red_led1;
	Board::GreenLED green_led1;

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
