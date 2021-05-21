#include "leds.hh"
#include "mdrivlib/interrupt.hh"
#include "mdrivlib/interrupt_control.hh"
#include "pinchange.hh"
#include "stm32mp157cxx_ca7.h"
#include "uart.hh"
#include <cstdint>

constexpr uint8_t GPIO_I = 8; // GPIOA = 0, GPIOB = 1, etc..
// IRQn = 0x62 (98) | PRIORITY: Reg 24 bit 2 | CFGR: Reg 16, bit 2 | ENABLE Reg 3, bit2
// IRQn = 0x63 (99) | PRIORITY: Reg 24 bit 3 | CFGR: Reg 16, bit 3 | ENABLE Reg 3, bit3

double do_some_big_calculation(double x)
{
	double xx = x * x;
	double yy = xx + 1239.1234923994423;
	xx = yy * xx;
	for (int i = 1; i < 100; i++) {
		xx = xx / (11234.12341234 / (11234.5 - yy));
		yy += 19.4 - xx * xx;
	}
	return xx;
}

void main()
{
	// UART
	Uart<UART4_BASE> uart;
	uart.write("\r\n\r\nTesting nested interrupts\r\n");
	uart.write("You should see steps 1-6 occur in order:\r\n");

	// LEDs
	Led<GPIOI_BASE, 8, LedActive::Low> red_led1;
	Led<GPIOI_BASE, 9, LedActive::Low> green_led1;
	Led<GPIOZ_BASE, 6, LedActive::Low> red_led2;
	Led<GPIOZ_BASE, 7, LedActive::Low> green_led2;
	red_led1.init();
	green_led1.init();
	red_led2.init();
	green_led2.init();

	// Pin Change interrupts
	PinChangeISR red_led_pinchange{GPIO_I, 8};
	PinChangeISR green_led_pinchange{GPIO_I, 9};

	const auto red_led1_irqnum = red_led_pinchange.get_IRQ_num();
	const auto green_led1_irqnum = green_led_pinchange.get_IRQ_num();

	// Initial conditions:
	red_led1.on();
	green_led1.on();
	red_led2.off();
	green_led2.off();

	// Setup outer interrupt = red LED 1 turning off
	// Set to low priority (so it can be interrupted by the inner interrupt)
	GIC_DisableIRQ(red_led1_irqnum);
	red_led_pinchange.enable_isr_on_rising_falling_edges(true, false);

	InterruptManager::registerISR(red_led1_irqnum, [&]() {
		uart.write("2) Entering outer ISR (red LED 1)\r\n");
		red_led_pinchange.clear_falling_isr_flag();
		for (int i = 0; i < 80; i++) {

			double x = i * 123.49838;
			if (i == 5) {
				x = x - i * 12.;
				uart.write("3) Triggering Inner ISR: Green LED 1 turning off (PI9 rising edge)\r\n");
				auto y = do_some_big_calculation(x);
				green_led1.off();
				if (y > 4562.)
					red_led2.on();
			}
			red_led2.on(); // This prevents optimizer from getting rid of the loop
		}
		uart.write("6) Exiting outer ISR (red LED 1)\r\n");
	});

	GIC_SetTarget(red_led1_irqnum, 1);
	GIC_SetPriority(red_led1_irqnum, 0b01111000);
	GIC_SetConfiguration(red_led1_irqnum, 0b10); // Edge triggered

	// Setup inner interrupt = green LED 1 turning off
	// Set to high priority so it can interrupt the outer interrupt
	GIC_DisableIRQ(green_led1_irqnum);
	green_led_pinchange.enable_isr_on_rising_falling_edges(true, false);
	InterruptManager::registerISR(green_led1_irqnum, [&]() {
		green_led_pinchange.clear_falling_isr_flag();
		uart.write("4) Entering inner ISR (green LED 1)\r\n");
		green_led2.on();
		uart.write("5) Exiting inner ISR (green LED 1)\r\n");
	});

	GIC_SetTarget(green_led1_irqnum, 1);
	GIC_SetPriority(green_led1_irqnum, 0b00000000);
	GIC_SetConfiguration(green_led1_irqnum, 0b10); // Edge triggered

	// Enable everything
	red_led_pinchange.enable();
	green_led_pinchange.enable();
	GIC_EnableIRQ(red_led1_irqnum);
	GIC_EnableIRQ(green_led1_irqnum);

	// Trigger the outer ISR, which should trigger the inner ISR
	uart.write("1) Triggering outer ISR: Red LED 1 turning off (PI8 rising edge)\r\n");

	// Fill Pattern 1: skip R2, R3, R11
	asm volatile("mov r0, #0x0000\n movt r0, #0x5678\n");
	asm volatile("mov r1, #0x1111\n movt r1, #0x5678\n");
	asm volatile("mov r4, #0x4444\n movt r4, #0x5678\n");
	asm volatile("mov r5, #0x5555\n movt r5, #0x5678\n");
	asm volatile("mov r6, #0x6666\n movt r6, #0x5678\n");
	asm volatile("mov r7, #0x7777\n movt r7, #0x5678\n");
	asm volatile("mov r8, #0x8888\n movt r8, #0x5678\n");
	asm volatile("mov r9, #0x9999\n movt r9, #0x5678\n");
	asm volatile("mov r10, #0xaaaa\n movt r10, #0x5678\n");
	asm volatile("mov r12, #0xcccc\n movt r12, #0x5678\n");
	asm volatile("vmov s1, r1\n");
	asm volatile("vmov s4, r4\n");
	asm volatile("vmov s31, r12\n");

	// red_led1.off();
	asm volatile("mov r2, #0x0100\n movt r2, #0x0000\n");
	asm volatile("mov r3, #0xA000\n movt r3, #0x5000\n");
	asm volatile("str r2, [r3]\n");

	// Big assumption made, we only need R3 and R11 after here:

	int i = 0;

	while (1) {
		i++;
		if (i == 50) {
			// check register pattern
			__BKPT();
		}
	};
}
