//
// usb_midi_audio_host/main.cc
// A USB MIDI Host example project for the STM32MP15x
//
// By Dan Green
// Copyright (c) 2022 Dan Green
// Licensed under MIT License, see LICENSE file
//

#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "midi_host.hh"
#include "midi_message.hh"
#include "stm32mp1xx.h"
#include "system_clk.hh"

#include "osd32brk_conf.hh"
#include "stm32disco_conf.hh"

// Uncomment one of these to select your board:
namespace Board = OSD32BRK;
// namespace Board = STM32MP1Disco;

void main()
{
	Uart<Board::ConsoleUART> uart;
	uart.write("\r\n\r\nUSB MIDI Host test\r\n");
	uart.write("Connect a USB cable to a MIDI device (keyboard, etc)\r\n");

	Board::GreenLED green1;
	green1.off();

	SystemClocks::init();

	MidiHost midi_host;

	if (!midi_host.init()) {
		printf("USB Host failed to initialize!\r\n");
	}

	midi_host.set_rx_callback([&](std::span<uint8_t> rx_buf) {
		if (rx_buf.size() == 4) {
			auto msg = MidiMessage(rx_buf[1], rx_buf[2], rx_buf[3]);
			msg.print();
		} else {
			printf("RX %d bytes\n", rx_buf.size());
		}

		// Start listening for more data:
		midi_host.receive();
	});

	if (!midi_host.start()) {
		printf("MIDI Host failed to start!\r\n");
	}

	// Blink green1 light at 1Hz
	uint32_t last_tm = 0;
	bool led_state = false;

	while (1) {
		midi_host.process();

		uint32_t tm = HAL_GetTick();
		if (tm > (last_tm + 500)) {
			last_tm = tm;
			if (led_state) {
				green1.off();
			} else {
				green1.on();
			}
			led_state = !led_state;
		}
	}
}

// required for printf()
extern "C" int __io_putchar(int ch)
{
	Uart<Board::ConsoleUART>::putchar(ch);
	return ch;
}
