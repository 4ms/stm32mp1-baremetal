#include "delay.hh"
#include "leds.hh"
/* #include "mdrivlib/interrupt.hh" */
/* #include "mdrivlib/interrupt_control.hh" */
#include "firmware_m4.h"
#include "firmware_m4_vectors.h"
#include "stm32mp157cxx_ca7.h"
#include "uart.hh"
#include <stdint.h>

void cpy(uint32_t *dst, uint32_t *src, uint32_t length)
{
	for (uint32_t i = 0; i < length; i++) {
		*dst++ = *src++;
	}
}

void main()
{
	Uart<UART4_BASE> uart;

	Led<GPIOZ_BASE, 6, LedActive::Low> red_led1;
	Led<GPIOZ_BASE, 7, LedActive::Low> green_led1;
	red_led1.init();
	green_led1.init();

	uart.write("\r\n\r\nA7: Hello from Cortex-A7!\r\n");

	uart.write("A7: Resetting M4 core\r\n");

	// HOLD BOOT for MCU (prevents it from starting up after a reset)
	RCC->MP_GCR = RCC->MP_GCR & ~RCC_MP_GCR_BOOT_MCU;
	// Reset MCU
	RCC->MP_GRSTCSETR = RCC_MP_GRSTCSETR_MCURST;

	// copy m4 code
	cpy((uint32_t *)0x38000000, (uint32_t *)build_corem4_vectors_bin, build_corem4_vectors_bin_len / 4);
	cpy((uint32_t *)0x10000000, (uint32_t *)build_corem4_firmware_bin, build_corem4_firmware_bin_len / 4);
	L1C_CleanDCacheAll();
	__DSB();
	__ISB();

	// Wait a bit (todo: wait until RST flag is cleared?)
	Delay::cycles(10000000);

	green_led1.on();
	red_led1.on();
	uart.write("A7: Starting M4 core after LED1 goes off in 3 ");
	Delay::cycles(10000000);
	uart.write("2 ");
	red_led1.off();
	Delay::cycles(10000000);
	uart.write("1 ");
	green_led1.off();
	Delay::cycles(10000000);
	uart.write("now\r\n");

	// Release HOLD BOOT, allowing MCU to start
	RCC->MP_GCR = RCC_MP_GCR_BOOT_MCU;

	while (1) {
		red_led1.on();
		Delay::cycles(10000000);
		red_led1.off();

		green_led1.on();
		Delay::cycles(10000000);
		green_led1.off();
	};
}
