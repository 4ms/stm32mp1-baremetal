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
		uart.write("3) Triggering Inner ISR: Green LED 1 turning off (PI9 rising edge)\r\n");

		// Calculate Fibonacci sum
		uint32_t x0 = 1;
		uint32_t x1 = 1;
		uint32_t x2 = x0 + x1;
		uint32_t x3 = x1 + x2;
		uint32_t x4 = x2 + x3;
		uint32_t x5 = x3 + x4;
		uint32_t x6 = x4 + x5;
		uint32_t x7 = x5 + x6;
		uint32_t x8 = x6 + x7;
		uint32_t x9 = x7 + x8;
		uint32_t x10 = x8 + x9;
		uint32_t x11 = x9 + x10;
		uint32_t x12 = x10 + x11;
		uint32_t x13 = x11 + x12;
		uint32_t x14 = x12 + x13;
		uint32_t x15 = x13 + x14;
		uint32_t x16 = x14 + x15;
		double d0 = 1.25;
		double d1 = 1.5;
		double d2 = d0 + d1;
		double d3 = d1 + d2;
		double d4 = d2 + d3;
		double d5 = d3 + d4;
		double d6 = d4 + d5;
		double d7 = d5 + d6;
		double d8 = d6 + d7;
		double d9 = d7 + d8;
		double d10 = d8 + d9;
		double d11 = d9 + d10;
		double d12 = d10 + d11;
		double d13 = d11 + d12;
		double d14 = d12 + d13;
		double d15 = d13 + d14;
		double d16 = d14 + d15;
		asm("mov r5, #0x5555\n");
		asm("mov r6, #0x6666\n");
		asm("mov r7, #0x7777\n");
		green_led1.off();
		uint32_t x17 = x15 + x16;
		uint32_t x18 = x16 + x17;
		uint32_t x19 = x17 + x18;
		uint32_t x20 = x18 + x19;
		uint32_t x21 = x19 + x20;
		uint32_t x22 = x20 + x21;
		uint32_t x23 = x21 + x22;
		uint32_t x24 = x22 + x23;
		uint32_t x25 = x23 + x24;
		uint32_t x26 = x24 + x25;
		uint32_t x27 = x25 + x26;
		uint32_t x28 = x26 + x27;
		uint32_t x29 = x27 + x28;
		uint32_t x30 = x28 + x29;
		uint32_t x31 = x29 + x30;
		uint32_t x32 = x30 + x31;
		uint32_t x33 = x31 + x32;
		uint32_t x34 = x32 + x33;
		uint32_t x35 = x33 + x34;
		uint32_t x36 = x34 + x35;
		uint32_t x37 = x35 + x36;
		uint32_t x38 = x36 + x37;
		uint32_t x39 = x37 + x38;
		uint32_t x40 = x38 + x39;
		uint32_t x41 = x39 + x40;
		uint32_t x42 = x40 + x41;
		uint32_t x43 = x41 + x42;
		uint32_t x44 = x42 + x43;
		uint32_t x45 = x43 + x44;
		double d17 = d15 + d16;
		double d18 = d16 + d17;
		double d19 = d17 + d18;
		double d20 = d18 + d19;
		double d21 = d19 + d20;
		double d22 = d20 + d21;
		double d23 = d21 + d22;
		double d24 = d22 + d23;
		double d25 = d23 + d24;
		double d26 = d24 + d25;
		double d27 = d25 + d26;
		double d28 = d26 + d27;
		double d29 = d27 + d28;
		double d30 = d28 + d29;
		double d31 = d29 + d30;
		double d32 = d30 + d31;
		double d33 = d31 + d32;
		double d34 = d32 + d33;
		double d35 = d33 + d34;
		double d36 = d34 + d35;
		double d37 = d35 + d36;
		double d38 = d36 + d37;
		double d39 = d37 + d38;
		double d40 = d38 + d39;
		double d41 = d39 + d40;
		double d42 = d40 + d41;
		double d43 = d41 + d42;
		double d44 = d42 + d43;
		double d45 = d43 + d44;
		// Check r5 == 0x5555 << fail
		// Check r6 == 0x6666 << fail
		// Check r7 == 0x7777 << fail
		if (x45 != 0x6D73E55F)
			uart.write("6) --- Int sum is incorrect! ");
		else if (d45 != 2579115671.25)
			uart.write("6) --- Double sum is incorrect ");
		else
			uart.write("6) Sums are correct, ");
		uart.write("Exiting outer ISR (red LED 1)\r\n");
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
		uint32_t x0 = 1;
		uint32_t x1 = 1;
		uint32_t x2 = x0 + x1;
		uint32_t x3 = x1 + x2;
		uint32_t x4 = x2 + x3;
		uint32_t x5 = x3 + x4;
		uint32_t x6 = x4 + x5;
		uint32_t x7 = x5 + x6;
		uint32_t x8 = x6 + x7;
		uint32_t x9 = x7 + x8;
		uint32_t x10 = x8 + x9;
		uint32_t x11 = x9 + x10;
		uint32_t x12 = x10 + x11;
		uint32_t x13 = x11 + x12;
		uint32_t x14 = x12 + x13;
		uint32_t x15 = x13 + x14;
		uint32_t x16 = x14 + x15;
		double d0 = 1.25;
		double d1 = 1.5;
		double d2 = d0 + d1;
		double d3 = d1 + d2;
		double d4 = d2 + d3;
		double d5 = d3 + d4;
		double d6 = d4 + d5;
		double d7 = d5 + d6;
		double d8 = d6 + d7;
		double d9 = d7 + d8;
		double d10 = d8 + d9;
		double d11 = d9 + d10;
		double d12 = d10 + d11;
		double d13 = d11 + d12;
		double d14 = d12 + d13;
		double d15 = d13 + d14;
		double d16 = d14 + d15;
		asm("mov r5, #0x1111\n");
		asm("mov r6, #0x2222\n");
		asm("mov r7, #0x3333\n");
		green_led1.off();
		uint32_t x17 = x15 + x16;
		uint32_t x18 = x16 + x17;
		uint32_t x19 = x17 + x18;
		uint32_t x20 = x18 + x19;
		uint32_t x21 = x19 + x20;
		uint32_t x22 = x20 + x21;
		uint32_t x23 = x21 + x22;
		uint32_t x24 = x22 + x23;
		uint32_t x25 = x23 + x24;
		uint32_t x26 = x24 + x25;
		uint32_t x27 = x25 + x26;
		uint32_t x28 = x26 + x27;
		uint32_t x29 = x27 + x28;
		uint32_t x30 = x28 + x29;
		uint32_t x31 = x29 + x30;
		uint32_t x32 = x30 + x31;
		uint32_t x33 = x31 + x32;
		uint32_t x34 = x32 + x33;
		uint32_t x35 = x33 + x34;
		uint32_t x36 = x34 + x35;
		uint32_t x37 = x35 + x36;
		uint32_t x38 = x36 + x37;
		uint32_t x39 = x37 + x38;
		uint32_t x40 = x38 + x39;
		uint32_t x41 = x39 + x40;
		uint32_t x42 = x40 + x41;
		uint32_t x43 = x41 + x42;
		uint32_t x44 = x42 + x43;
		uint32_t x45 = x43 + x44;
		double d17 = d15 + d16;
		double d18 = d16 + d17;
		double d19 = d17 + d18;
		double d20 = d18 + d19;
		double d21 = d19 + d20;
		double d22 = d20 + d21;
		double d23 = d21 + d22;
		double d24 = d22 + d23;
		double d25 = d23 + d24;
		double d26 = d24 + d25;
		double d27 = d25 + d26;
		double d28 = d26 + d27;
		double d29 = d27 + d28;
		double d30 = d28 + d29;
		double d31 = d29 + d30;
		double d32 = d30 + d31;
		double d33 = d31 + d32;
		double d34 = d32 + d33;
		double d35 = d33 + d34;
		double d36 = d34 + d35;
		double d37 = d35 + d36;
		double d38 = d36 + d37;
		double d39 = d37 + d38;
		double d40 = d38 + d39;
		double d41 = d39 + d40;
		double d42 = d40 + d41;
		double d43 = d41 + d42;
		double d44 = d42 + d43;
		double d45 = d43 + d44;
		if (x45 != 0x6D73E55F)
			uart.write("5) --- Int sum is incorrect! ");
		else if (d45 != 2579115671.25)
			uart.write("5) --- Double sum is incorrect ");
		else
			uart.write("5) Sums are correct, ");
		uart.write("Exiting inner ISR (green LED 1)\r\n");
		green_led2.on();
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

	// Fill Pattern 1: skip R0-R3, R11 (we've verified separately that R0, R1 R2 are preserved, and if R11 wasn't, the
	// rest of the code would crash Haven't proven that R3 is preserved, but it seems likely
	asm volatile("mov r4, #0x4444\n movt r4, #0x5678\n");
	asm volatile("mov r5, #0x5555\n movt r5, #0x5678\n");
	asm volatile("mov r6, #0x6666\n movt r6, #0x5678\n");
	asm volatile("mov r7, #0x7777\n movt r7, #0x5678\n");
	asm volatile("mov r8, #0x8888\n movt r8, #0x5678\n");
	asm volatile("mov r9, #0x9999\n movt r9, #0x5678\n");
	asm volatile("mov r10, #0xaaaa\n movt r10, #0x5678\n");
	asm volatile("mov r12, #0xcccc\n movt r12, #0x5678\n");
	asm volatile("vmov s0, r10\n");
	asm volatile("vmov s1, r10\n");
	asm volatile("vmov s2, r12\n");
	asm volatile("vmov s3, r12\n");
	asm volatile("vmov s4, r4\n");
	asm volatile("vmov s14, r4\n");
	asm volatile("vmov s15, r5\n");
	asm volatile("vmov s16, r6\n");
	asm volatile("vmov s31, r12\n");

	red_led1.off();
	// asm volatile("mov r2, #0x0100\n movt r2, #0x0000\n");
	// asm volatile("mov r3, #0xA000\n movt r3, #0x5000\n");
	// asm volatile("str r2, [r3]\n");

	// Big assumption made, we only need R0-R3 and R11 after here:

	int i = 0;

	while (1) {
		i++;
		if (i == 50) {
			// check register pattern
			__BKPT();
		}
	};
}
