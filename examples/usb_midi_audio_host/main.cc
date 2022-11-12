#include "drivers/interrupt.hh"
#include "drivers/interrupt_control.hh"
#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "stm32mp1xx.h"
#include "system_clk.hh"
#include "usbh_core.h"
#include <cstdint>

#include "osd32brk_conf.hh"
#include "stm32disco_conf.hh"

// Uncomment one of these to select your board:
namespace Board = OSD32BRK;
// namespace Board = STM32MP1Disco;

extern HCD_HandleTypeDef hhcd;

void main()
{
	Uart<Board::ConsoleUART> uart;
	uart.write("\r\n\r\nUSB MIDI Host test\r\n");
	uart.write("Connect a USB cable to a MIDI device (keyboard, etc)\r\n");

	Board::GreenLED green1;
	green1.off();

	SystemClocks::init();

	USBH_HandleTypeDef USBH_Host;

	// auto init_ok = USBH_Init(&USBH_Host, state_change_callback, 0);
	// if (init_ok != USBH_OK) {
	// 	printf("USB Host failed to initialize! Error code: %d\n", static_cast<uint32_t>(init_ok));
	// }
	InterruptControl::disable_irq(OTG_IRQn);
	InterruptManager::registerISR(OTG_IRQn, [] { HAL_HCD_IRQHandler(&hhcd); });
	InterruptControl::set_irq_priority(OTG_IRQn, 0, 0);
	InterruptControl::enable_irq(OTG_IRQn);

	// USBH_RegisterClass(&USBH_Host, HOST_MIDI_CLASS);
	// USBH_Start(&USBH_Host);

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

// required for print()
void putchar_s(const char c) { Uart<Board::ConsoleUART>::putchar(c); }
