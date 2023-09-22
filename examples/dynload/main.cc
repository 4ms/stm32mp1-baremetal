//
// dynload/main.cc
// dynamic loading experiment
//
// By Dan Green
// Copyright (c) 2023 Dan Green
// Licensed under MIT License, see LICENSE file
//

#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "stm32mp1xx.h"
#include "system_clk.hh"

#include "osd32brk_conf.hh"
#include "stm32disco_conf.hh"

// Uncomment one of these to select your board:
// namespace Board = OSD32BRK;
namespace Board = STM32MP1Disco;

void main()
{
	Uart<Board::ConsoleUART> uart;
	uart.write("\n\nDynamic Loading Example\n");

	Board::GreenLED green_led;
	Board::RedLED red_led;
	red_led.off();
	green_led.off();

	SystemClocks::init();

	// Blink green LED
	uint32_t last_tm = 0;
	bool led_state = false;

	while (true) {

		uint32_t tm = HAL_GetTick();
		if (tm > (last_tm + 500)) {
			last_tm = tm;
			green_led.set(led_state);
			led_state = !led_state;
		}
	}
}
