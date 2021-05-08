#include "leds.hh"
#include "mdrivlib/interrupt.hh"
#include "mdrivlib/interrupt_control.hh"
#include "stm32mp157cxx_ca7.h"
#include "uart.hh"
#include <stdint.h>

void enable_pin_change_isr(uint8_t GPIOnum, uint8_t pin_num, bool rising, bool falling)
{
	if (GPIOnum > 12 || pin_num > 15)
		return;
	EXTI->EXTICR[pin_num / 4] = EXTI->EXTICR[pin_num / 4] | GPIOnum;

	if (rising)
		EXTI->RTSR1 = EXTI->RTSR1 | (1 << pin_num);
	else
		EXTI->RTSR1 = EXTI->RTSR1 & ~(1 << pin_num);

	if (falling)
		EXTI->FTSR1 = EXTI->FTSR1 | (1 << pin_num);
	else
		EXTI->FTSR1 = EXTI->FTSR1 & ~(1 << pin_num);
}

void clear_rising_pin_change_isr_flag(uint8_t pin_num)
{
	EXTI->RPR1 = (1 << pin_num); // clear on write
}

void clear_falling_pin_change_isr_flag(uint8_t pin_num)
{
	EXTI->FPR1 = (1 << pin_num); // clear on write
}

void pin_change_isr_start(uint8_t pin_num)
{
	EXTI->C1IMR1 = EXTI->C1IMR1 | (1 << pin_num);
}

void pin_change_isr_stop(uint8_t pin_num)
{
	EXTI->C1IMR1 = EXTI->C1IMR1 & ~(1 << pin_num);
}

int tmpvar;

void main()
{

	Uart<UART4_BASE> uart;
	const char *s = "HI";
	uart.write(s);

	Led<GPIOI_BASE, 8, LedActive::Low> red_led1;
	Led<GPIOI_BASE, 9, LedActive::Low> green_led1;
	Led<GPIOZ_BASE, 6, LedActive::Low> red_led2;
	Led<GPIOZ_BASE, 7, LedActive::Low> green_led2;

	red_led1.init();
	green_led1.init();
	red_led2.init();
	green_led2.init();

	uart.write("Testing nested interrupts\r\n");

	constexpr uint8_t GPIO_I = 8;

	// Initial conditions:
	red_led1.on();
	green_led1.on();
	red_led2.off();
	green_led2.off();

	// Outer interrupt
	// EXTI pin-change interrupt on rising edge of PI8 (red_led1 turning off)
	enable_pin_change_isr(GPIO_I, 8, true, false);
	InterruptManager::registerISR(EXTI8_IRQn, [&]() {
		// Spend a lot of time in here
		// because while it's running, the inner ISR
		// will triggered, be handled, and return
		uart.write("Entering red LED 1 ISR (Outer)\r\n");
		for (int i = 0; i < 800; i++) {
			if (i == 50) {
				// Trigger the inner ISR
				uart.write("Green LED 1 turning off (PI9 rising edge)");
				green_led1.off();
			}
			red_led2.on(); // This prevents optimizer from getting rid of the loop
		}
		clear_falling_pin_change_isr_flag(8);
		uart.write("Exiting red LED 1 ISR (Outer)\r\n");
	});

	// Inner interrupt
	// EXTI pin-change interrupt on rising edge of PI9 (green_led1 turning off)
	enable_pin_change_isr(GPIO_I, 9, true, false);
	InterruptManager::registerISR(EXTI9_IRQn, [&]() {
		uart.write("Entering green LED 1 ISR (Outer)\r\n");
		clear_falling_pin_change_isr_flag(9);
		uart.write("Exiting green LED 1 ISR (Outer)\r\n");
	});

	pin_change_isr_start(8);
	pin_change_isr_start(9);

	// Trigger the ISRs
	uart.write("Red LED 1 turning off (PI8 rising edge)");
	red_led1.off();

	while (1) {
	};
}
