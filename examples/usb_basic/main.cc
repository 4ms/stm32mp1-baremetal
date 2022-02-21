#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "osd32brk_conf.hh"
#include "stm32mp1xx.h"
#include <cstdint>

using namespace OSD32BRK;

Uart<UART4_BASE> uart;

void main() {
	uart.write("\r\n\r\nBasic USB test\r\n");

	RedLED red1;
	RedLED2 red2;
	GreenLED green1;
	GreenLED2 green2;

	red1.off();
	red2.off();
	green1.on();
	green2.off();
	uint32_t last_tm = 0;
	bool led_state = false;
	while (1) {
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
