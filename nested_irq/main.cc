#include "leds.hh"
#include "mdrivlib/interrupt.hh"
#include "mdrivlib/interrupt_control.hh"
#include "stm32mp157cxx_ca7.h"
#include "uart.hh"
#include <stdint.h>

void run_nesting_isr_test();

void main()
{
	Uart<UART4_BASE> uart;
	Led<GPIOI_BASE, 8, LedActive::Low> red_led1;
	Led<GPIOI_BASE, 9, LedActive::Low> green_led1;
	Led<GPIOZ_BASE, 6, LedActive::Low> red_led2;
	Led<GPIOZ_BASE, 7, LedActive::Low> green_led2;

	red_led1.init();
	green_led1.init();
	red_led2.init();
	green_led2.init();

	uart.write("Testing nested interrupts\r\n");

	constexpr uint32_t TestSemaphoreID = 1;
	constexpr IRQn_Type TestSemaphoreIRQn = HSEM_IT1_IRQn;

	InterruptManager::registerISR(TestSemaphoreIRQn, [&]() {
		if (HWSemaphore<TestSemaphoreID>::is_ISR_triggered_and_enabled()) {
			for (int i = 0; i < 800; i++) {
				if (i == 50)
					red_led2.on();

				red_led1.off();
				green_led1.on();
			}
			red_led1.on();
			green_led1.off();
			HWSemaphore<TestSemaphoreID>::clear_ISR();
		}
	});

	while (1) {
	};
}
