// #ifndef STM32MP1BAREMETAL
// #define STM32MP1BAREMETAL
// #endif

#include "drivers/interrupt.hh"
#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "osd32brk_conf.hh"
#include "printf/printf.h"
#include "rcc_setup.hh"
#include "stm32mp1xx.h"

#include "usb_mass_storage.h"

using namespace OSD32BRK;

Uart<UART4_BASE> uart;

void main()
{
	HAL_Init();

	uart.write("\r\n\r\nU-boot USB Gadget test\r\n");
	printf("Hello World!\r\n");

	RedLED red1;
	RedLED2 red2;
	GreenLED green1;
	GreenLED2 green2;

	red1.off();
	red1.on();
	red1.off();
	red2.off();
	red2.on();
	red2.off();
	green1.off();
	green1.on();
	green1.off();
	green2.off();
	green2.on();
	green2.off();

	SystemClocks::init();

	do_usb_mass_storage(3, "ums", "0");
	// InterruptManager::registerISR(OTG_IRQn, [&green2] {
	// 	green2.on();
	// 	HAL_PCD_IRQHandler(&hpcd);
	// 	green2.off();
	// });

	// USBD_HandleTypeDef USBD_Device;

	// USBD_Init(&USBD_Device, &MSC_Desc, 0);

	// USBD_RegisterClass(&USBD_Device, USBD_MSC_CLASS);

	// USBD_MSC_RegisterStorage(&USBD_Device, &USBD_MSC_fops);

	// USBD_Start(&USBD_Device);

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

// mpaland/printf wants to see _putchar(char)
extern "C" void _putchar(char c)
{
	uart.write(c);
}

// CubeMX-generated syscalls.c with stdlibc wants to see __io_putchar(int)
extern "C" int __io_putchar(int ch)
{
	const char c = static_cast<char>(ch);
	uart.write(c);
	return ch;
}
