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
	// CR[2] &= 0xFFFFFF00
	// CR[2] |= 0x00000008
	// CR[2] &= 0xFFFF00FF
	uint32_t tmp = EXTI->EXTICR[pin_num / 4];
	uint32_t shift = (pin_num % 4) * 8;
	uint32_t mask = 0xFFUL << shift;
	tmp &= ~mask;
	tmp |= (uint32_t)GPIOnum << shift;
	EXTI->EXTICR[pin_num / 4] = tmp;

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

void pin_change_isr_enable(uint8_t pin_num)
{
	EXTI->C1IMR1 = EXTI->C1IMR1 | (1 << pin_num);
}

void pin_change_isr_stop(uint8_t pin_num)
{
	EXTI->C1IMR1 = EXTI->C1IMR1 & ~(1 << pin_num);
}

constexpr uint8_t GPIO_I = 8; // GPIOA = 0, GPIOB = 1, etc..
constexpr uint8_t red_led1_gpio = GPIO_I;
constexpr uint8_t red_led1_pin = 8;
constexpr auto red_led1_irqnum = EXTI8_IRQn;
constexpr uint8_t green_led1_gpio = GPIO_I;
constexpr uint8_t green_led1_pin = 9;
constexpr auto green_led1_irqnum = EXTI9_IRQn;

void main()
{
	Uart<UART4_BASE> uart;
	uart.write("\r\n\r\nTesting nested interrupts\r\n");

	GIC_DisableIRQ(red_led1_irqnum);
	GIC_DisableIRQ(green_led1_irqnum);

	Led<GPIOI_BASE, 8, LedActive::Low> red_led1;
	Led<GPIOI_BASE, 9, LedActive::Low> green_led1;
	Led<GPIOZ_BASE, 6, LedActive::Low> red_led2;
	Led<GPIOZ_BASE, 7, LedActive::Low> green_led2;
	red_led1.init();
	green_led1.init();
	red_led2.init();
	green_led2.init();

	// Initial conditions:
	red_led1.on();
	green_led1.on();
	red_led2.off();
	green_led2.off();

	// Outer interrupt = red LED 1 turning off
	// EXTI pin-change interrupt on rising edge of PI8
	enable_pin_change_isr(red_led1_gpio, red_led1_pin, true, false);
	InterruptManager::registerISR(red_led1_irqnum, [&]() {
		// Spend a lot of time in here because while it's running,
		// the inner ISR needs to be triggered, be handled, and return
		uart.write("2) Entering red LED 1 ISR (Outer)\r\n");
		for (int i = 0; i < 800; i++) {
			if (i == 50) {
				// Trigger the inner ISR
				uart.write("3) Green LED 1 turning off (PI9 rising edge)\r\n");
				green_led1.off();
			}
			red_led2.on(); // This prevents optimizer from getting rid of the loop
		}
		clear_falling_pin_change_isr_flag(red_led1_pin);
		uart.write("6) Exiting red LED 1 ISR (Outer)\r\n");
	});

	// Set to low priority (so it can be interrupted by the inner interrupt)
	GIC_SetPriority(red_led1_irqnum, 0b11111000);
	GIC_SetConfiguration(red_led1_irqnum, 0b10); // Edge triggered

	// Inner interrupt = green LED 1 turning off
	// EXTI pin-change interrupt on rising edge of PI9 (green_led1 turning off)
	enable_pin_change_isr(green_led1_gpio, green_led1_pin, true, false);
	InterruptManager::registerISR(green_led1_irqnum, [&]() {
		uart.write("4"); //) Entering green LED 1 ISR (Outer)\r\n");
		clear_falling_pin_change_isr_flag(green_led1_pin);
		uart.write("5"); //) Exiting green LED 1 ISR (Outer)\r\n");
	});

	// Set to high priority so it can interrupt the outer interrupt
	GIC_SetPriority(green_led1_irqnum, 0b00000000);
	GIC_SetConfiguration(green_led1_irqnum, 0b10); // Edge triggered

	// Enable everything
	pin_change_isr_enable(red_led1_pin);
	pin_change_isr_enable(green_led1_pin);
	GIC_EnableIRQ(red_led1_irqnum);
	GIC_EnableIRQ(green_led1_irqnum);

	// Trigger the ISRs
	uart.write("1) Red LED 1 turning off (PI8 rising edge)\r\n");
	red_led1.off();

	while (1) {
	};
}
