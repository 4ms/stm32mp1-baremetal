#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "osd32brk_conf.hh"
#include "stm32mp157cxx_ca7.h"
#include <cstdint>

namespace Board = OSD32BRK;

void delay(unsigned cycles);

void main()
{
	Board::RedLED red_led;
	Board::GreenLED green_led;
	Board::OrangeLED blue_led;


	red_led.init();

	constexpr uint32_t dlytime = 600000;
	while (1) {
		red_led.on();
		delay(dlytime);
		red_led.off();

		green_led.on();
		delay(dlytime);
		green_led.off();

		blue_led.on();
		delay(dlytime);
		blue_led.off();
	}
}

void delay(unsigned cycles)
{
	while (cycles--) {
		asm("nop");
	}
}
