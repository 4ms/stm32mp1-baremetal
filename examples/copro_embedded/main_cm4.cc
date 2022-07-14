#include "delay.hh"
#include "drivers/uart.hh"
#include "stm32mp157cxx_cm4.h"
#include <stdint.h>

#include "osd32brk_conf.hh"
#include "stm32disco_conf.hh"

// Uncomment one of these to select your board:
namespace Board = OSD32BRK;
// namespace Board = STM32MP1Disco;

void main()
{
	Uart<Board::ConsoleUART> uart;
	uart.write("M4: * yawn * \r\nM4: Hello from Cortex-M4!\r\n");

	Board::RedLED2 red_led2;
	Board::GreenLED2 green_led2;

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
