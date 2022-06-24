extern "C" {
#include "ddr/stm32mp1_ram.h"
}
#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "osd32brk_conf.hh"
#include "printf/printf.h"
#include "stm32mp157cxx_ca7.h"
#include <cstdint>
#include <cstdio>

namespace Board = OSD32BRK;

void delay(unsigned cycles);

void main()
{
	Board::RedLED red_led;
	Board::GreenLED green_led;
	Board::OrangeLED blue_led;

	Uart<Board::ConsoleUART> uart(Board::UartRX, Board::UartTX, 115200);
	uart.write("Starting FSBL...\n");
	printf_("Printf works\n");
	printf_("1 + 2 = %d\n", (1 + 2));

	red_led.off();
	red_led.on();
	red_led.off();

	int err = stm32mp1_ddr_setup();
	printf_("Result = %d\n", err);

	red_led.on();
	if (err) {
		blue_led.on();
	}

	constexpr uint32_t dlytime = 600000;
	while (1) {
		// red_led.on();
		// delay(dlytime);
		// red_led.off();

		green_led.on();
		delay(dlytime);
		green_led.off();

		// blue_led.on();
		// delay(dlytime);
		// blue_led.off();
	}
}

void delay(unsigned cycles)
{
	while (cycles--) {
		asm("nop");
	}
}

extern "C" void putchar_(char c)
{
	Uart<Board::ConsoleUART> uart;
	uart.write(c);
}
