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
	// Test Interrupts
	// PinChangeISR<GPIOI_BASE, 8> red_led2_pinchange;
	// const auto red_led2_irqnum = red_led2_pinchange.get_IRQ_num();
	// GIC_DisableIRQ(red_led2_irqnum);
	// red_led2_pinchange.enable_isr_on_rising_falling_edges(true, false);

	// InterruptManager::registerISR(red_led2_irqnum, [&]() {
	// 	red2.on();
	// 	// uart.write(" 2) Entering ISR (red LED)\r\n");
	// 	red_led2_pinchange.clear_falling_isr_flag();
	// });
	// GIC_SetTarget(red_led2_irqnum, 1);
	// GIC_SetPriority(red_led2_irqnum, 0b01111000);
	// GIC_SetConfiguration(red_led2_irqnum, 0b10); // Edge triggered
	// red_led2_pinchange.enable();
	// GIC_EnableIRQ(red_led2_irqnum);
	// HAL_Delay(5);
	// uart.write("Triggering ISR: Red LED 2 turning off (PI8 rising edge)\r\n");
	// HAL_Delay(5);
	// red2.on();
	// HAL_Delay(5);
	// red2.off();
	// HAL_Delay(5);

	// End test Interrupts

	// Start USB:
	// PCD_HandleTypeDef hpcd;

	InterruptManager::registerISR(OTG_IRQn, [/*&hpcd, &green2*/] {
		// green2.on();
		HAL_PCD_IRQHandler(&hpcd);
	});

	USBD_HandleTypeDef USBD_Device;

	/* Init Device Library */
	USBD_Init(&USBD_Device, &MSC_Desc, 0);

	/* Add Supported Class */
	USBD_RegisterClass(&USBD_Device, USBD_MSC_CLASS);

	/* Add Storage callbacks for MSC Class */
	USBD_MSC_RegisterStorage(&USBD_Device, &USBD_MSC_fops);

	/* Start Device Process */
	USBD_Start(&USBD_Device);

	// Blink green1 light at 1Hz
	uint32_t last_tm = 0;
	bool led_state = false;
	while (1) {
		uint32_t tm = HAL_GetTick();
		if (tm > (last_tm + 500)) {
			last_tm = tm;
			if (led_state) {
				// green1.off();
			} else {
				// green1.on();
			}
			led_state = !led_state;
		}
	}
}
