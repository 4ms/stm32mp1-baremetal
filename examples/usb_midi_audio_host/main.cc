#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "midi_host.hh"
#include "stm32mp1xx.h"
#include "system_clk.hh"
#include "usbh_core.h"
#include "usbh_midi.hh"
#include <cstdint>

#include "osd32brk_conf.hh"
#include "stm32disco_conf.hh"

// Uncomment one of these to select your board:
namespace Board = OSD32BRK;
// namespace Board = STM32MP1Disco;

static void usbh_state_change_callback(USBH_HandleTypeDef *phost, uint8_t id);

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

	midi_host.set_rx_callback([&](uint8_t *buf, uint32_t sz) { 
		printf("RX %d bytes: ", sz);
		if (sz >= 4)
			printf("0x%x 0x%x 0x%x 0x%x\n", buf[0], buf[1], buf[2], buf[3]);

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
