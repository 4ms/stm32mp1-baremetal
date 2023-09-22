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
#include "plugin_loader.hh"
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

	PluginLoader loader;

	// In a real usage, we would scan a filesystem for plugin files,
	// And then load the raw binary data into RAM, then pass it to PluginLoader.
	// Here, we will just start with the raw data already in RAM.

	std::array<char, 100> raw_plugin_A{
		0x12, //...
	};
	std::array<char, 100> raw_plugin_B{
		0x12, //...
	};

	PluginBase *plugins[2];
	plugins[0] = loader.load_plugin(raw_plugin_A);
	plugins[1] = loader.load_plugin(raw_plugin_B);

	if (plugins[0] == nullptr) {
		uart.write("Error: Plugin A Not Valid\n");
		while (true)
			;
	}
	if (plugins[1] == nullptr) {
		uart.write("Error: Plugin B Not Valid\n");
		while (true)
			;
	}

	// Use the plugins
	plugins[0]->init();
	plugins[1]->init();

	uint32_t last_tm = 0;
	bool led_state = false;

	Channel chanA{1, 0.f};
	Channel chanB{2, 0.f};

	while (true) {
		{
			auto val = plugins[0]->get_val(chanA);
			plugins[1]->set_val(chanA, val);
		}

		{
			auto val = plugins[1]->get_val(chanB);
			plugins[0]->set_val(chanB, val);
		}

		plugins[0]->update();
		plugins[1]->update();

		// Blink green LED
		uint32_t tm = HAL_GetTick();
		if (tm > (last_tm + 500)) {
			last_tm = tm;
			green_led.set(led_state);
			led_state = !led_state;
		}
	}
}
