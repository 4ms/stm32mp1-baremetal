#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "stm32mp1xx.h"
#include "system_clk.hh"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc_storage.h"
#include <cstdint>

// Uncomment these to use the OSD32BRK board:
//#include "osd32brk_conf.hh"
// using namespace OSD32BRK;

// Uncomment these to use the STM32MP1Disco board:
#include "stm32disco_conf.hh"
using namespace STM32MP1Disco;

extern PCD_HandleTypeDef hpcd;
void main()
{
	Uart<UART4_BASE> uart;
	uart.write("\r\n\r\nSimple USB MSC Device test\r\n");
	uart.write("Connect a USB cable to a computer\r\n");
	uart.write("You should see a 128MB unformatted drive appear.\r\n");

	GreenLED green1;

	green1.off();

	SystemClocks::init();

	USBD_HandleTypeDef USBD_Device;

	auto init_ok = USBD_Init(&USBD_Device, &MSC_Desc, 0);
	if (init_ok != USBD_OK) {
		uart.write("USB Device failed to initialize!\r\n");
		uart.write("Error code: ");
		uart.write(static_cast<int>(init_ok));
	}

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

extern "C" void IRQ_Initialize(void)
{
	// do nothing, just a stub
}
