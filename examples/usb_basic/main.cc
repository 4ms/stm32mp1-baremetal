#include "drivers/interrupt.hh"
#include "drivers/leds.hh"
//#include "drivers/pinchange.hh"
#include "drivers/uart.hh"
#include "osd32brk_conf.hh"
#include "rcc_setup.hh"
#include "stm32mp1xx.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc_storage.h"
#include <cstdint>

using namespace OSD32BRK;

// For debugging, it's handy to know these values:
// IRQn = 0x62 (98) | PRIORITY: Reg 24 bit 2 | CFGR: Reg 16, bit 2 | ENABLE Reg 3, bit2
// IRQn = 0x63 (99) | PRIORITY: Reg 24 bit 3 | CFGR: Reg 16, bit 3 | ENABLE Reg 3, bit3

extern PCD_HandleTypeDef hpcd;
void main() {
	Uart<UART4_BASE> uart;
	uart.write("\r\n\r\nBasic USB test\r\n");

	RedLED red1;
	RedLED2 red2;
	GreenLED green1;
	GreenLED2 green2;

	red1.off();
	red2.off();
	green1.on();
	green2.off();

	SystemClocks::init();

	InterruptManager::registerISR(OTG_IRQn, [&green2] {
		green2.on();
		HAL_PCD_IRQHandler(&hpcd);
	});

	USBD_HandleTypeDef USBD_Device;

	USBD_Init(&USBD_Device, &MSC_Desc, 0);

	USBD_RegisterClass(&USBD_Device, USBD_MSC_CLASS);

	USBD_MSC_RegisterStorage(&USBD_Device, &USBD_MSC_fops);

	USBD_Start(&USBD_Device);

	// Blink green1 light at 1Hz
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
