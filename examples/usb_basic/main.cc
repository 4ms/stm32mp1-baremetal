#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "osd32brk_conf.hh"
#include "rcc_setup.hh"
#include "stm32mp1xx.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc_storage.h"
#include <cstdint>

using namespace OSD32BRK;


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

	USBD_HandleTypeDef USBD_Device;

	USBD_Init(&USBD_Device, &MSC_Desc, 0);

	USBD_RegisterClass(&USBD_Device, USBD_MSC_CLASS);

	USBD_MSC_RegisterStorage(&USBD_Device, &USBD_MSC_fops);

	USBD_Start(&USBD_Device);

	// Blink green1 light at 1Hz
	uint32_t last_tm = 0;
	bool led_state = false;
	while (1) {
		
		// Check for new USB events
		HAL_PCD_IRQHandler(&hpcd);

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

extern "C" void IRQ_Initialize(void) {
	// do nothing, just a stub
}
