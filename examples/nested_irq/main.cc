#include "drivers/interrupt.hh"
#include "drivers/leds.hh"
#include "drivers/pinchange.hh"
#include "drivers/uart.hh"
#include "osd32brk_conf.hh"
#include "stm32mp157cxx_ca7.h"
#include <cstdint>

#include "osd32brk_conf.hh"
#include "stm32disco_conf.hh"
// Uncomment one of these to select your board:
namespace Board = OSD32BRK;
// namespace Board = STM32MP1Disco;

// For debugging, it's handy to know these values:
// IRQn = 0x62 (98) | PRIORITY: Reg 24 bit 2 | CFGR: Reg 16, bit 2 | ENABLE Reg 3, bit2
// IRQn = 0x63 (99) | PRIORITY: Reg 24 bit 3 | CFGR: Reg 16, bit 3 | ENABLE Reg 3, bit3

using namespace OSD32BRK;

void main()
{
	// UART
	Uart<Board::ConsoleUART> uart;
	uart.write("\r\n\r\nTesting nested interrupts\r\n");
	uart.write("Make sure this is compiled with -O1 to test register clobbering (Makefile line 4, OPTFLAG)\r\n");
	uart.write("To see the floating-point register test fail, ");
	uart.write("comment out the #define STASH_FPU_REGS in irq_handler.cc\r\n");
	uart.write("\r\n");
	uart.write("You should see steps 1-6 occur in order:\r\n");

	// LEDs
	RedLED2 red_led;
	GreenLED2 green_led;
	Led<GPIO::E, PinNum::_15, LedActive::Low> debug_pin2;
	Led<GPIO::G, PinNum::_15, LedActive::Low> debug_pin3;

	// Pin Change interrupts
	PinChangeISR<GPIOI_BASE, 8> red_led_pinchange;
	PinChangeISR<GPIOI_BASE, 9> green_led_pinchange;

	const auto red_led1_irqnum = red_led_pinchange.get_IRQ_num();
	const auto green_led1_irqnum = green_led_pinchange.get_IRQ_num();

	// Initial conditions:
	red_led.on();
	green_led.on();

	debug_pin2.off();
	debug_pin2.on();
	debug_pin2.off();

	debug_pin3.off();
	debug_pin3.on();
	debug_pin3.off();

	// Setup outer interrupt = red LED 1 turning off
	// Set to low priority (so it can be interrupted by the inner interrupt)
	GIC_DisableIRQ(red_led1_irqnum);
	red_led_pinchange.enable_isr_on_rising_falling_edges(true, false);

	InterruptManager::registerISR(red_led1_irqnum, [&]() {
		uart.write(" 2) Entering outer ISR (red LED 1)\r\n");
		red_led_pinchange.clear_falling_isr_flag();

		volatile uint32_t x0 = 1;
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
		volatile double d0 = 1.25 * x0;
		volatile double d1 = 1.5;
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

		if (d31 > d1 * x1) { // always true, but the compiler's optimizer doesn't know that (since d1 is volatile)
			uart.write("  3) Triggering Inner ISR: Green LED 1 turning off (PI9 rising edge)\r\n");
			green_led.off(); // trigger the inner ISR
			// Kill some time to ensure the GIC has time to propagate the request to the inner ISR
			long i = 0x1000;
			while (i--)
				;
		} else {
			d31 -= 1.; // this won't be called, but the compiler doesn't know that, so it won't do the calculations at
					   // compile time
		}
		// These use all available registers (FPU and normal)
		// If the inner ISR clobbered the registers, then we'll get an incorrect sum
		double d32 = d31 + d30 + d29 + d28 + d27 + d26 + d25 + d24 + d23 + d22 + d21 + d20 + d19 + d18 + d17 + d16 +
					 d15 + d14 + d13 + d12 + d11 + d10 + d9 + d8 + d7 + d6 + d5 + d4 + d3 + d2 + d1 + d0;
		uint32_t x17 = x16 + x15 + x14 + x13 + x12 + x11 + x10 + x9 + x8 + x7 + x6 + x5 + x4 + x3 + x2 + x1 + x0;

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
		double d46 = d44 + d45;
		double d47 = d45 + d46;
		double d48 = d46 + d47;
		double d49 = d47 + d48;
		double d50 = d48 + d49;
		double d51 = d49 + d50;
		double d52 = d50 + d51;
		double d53 = d51 + d52;
		double d54 = d52 + d53;
		double d55 = d53 + d54;
		double d56 = d54 + d55;
		double d57 = d55 + d56;
		double d58 = d56 + d57;
		double d59 = d57 + d58;
		double d60 = d58 + d59;
		double d61 = d59 + d60;
		double d62 = d60 + d61;
		double d63 = d61 + d62;
		double d64 = d62 + d63;
		double d65 = d63 + d64;
		double d66 = d64 + d65;
		double d67 = d65 + d66;
		double d68 = d66 + d67;
		double d69 = d67 + d68;
		double d70 = d68 + d69;
		double d71 = d69 + d70;
		double d72 = d70 + d71;
		double d73 = d71 + d72;
		double d74 = d72 + d73;
		double d75 = d73 + d74;
		double d76 = d74 + d75;
		double d77 = d75 + d76;
		double d78 = d76 + d77;
		double d79 = d77 + d78;
		double d80 = d78 + d79;
		double d81 = d79 + d80;
		double d82 = d80 + d81;
		double d83 = d81 + d82;
		double d84 = d82 + d83;
		double d85 = d83 + d84;
		double d86 = d84 + d85;
		double d87 = d85 + d86;
		double d88 = d86 + d87;
		double d89 = d87 + d88;
		double d90 = d88 + d89;
		double d91 = d89 + d90;
		double d92 = d90 + d91;
		double d93 = d91 + d92;
		double d94 = d92 + d93;
		double d95 = d93 + d94;
		double d96 = d94 + d95;

		bool int_sum_ok = true;
		bool float_sum_ok = true;
		uart.write("     6) Outer ISR: ");
		if (x40 != 0x0E47C0ED) {
			int_sum_ok = false;
			uart.write("--- integer sum is incorrect! ");
		}
		if (d96 < 1.69e+20 || d96 > 1.71e+20) {
			float_sum_ok = false;
			uart.write("--- floating-point sum is incorrect! ");
		}
		if (int_sum_ok && float_sum_ok)
			uart.write("both sums are OK. ");

		uart.write("...Exiting ISR\r\n");
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
		uart.write("   4) Entering inner ISR (green LED 1)\r\n");
		volatile uint32_t x0 = 1;
		volatile uint32_t x1 = 1;
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
		uint32_t x17 = x16 + x15 + x14 + x13 + x12 + x11 + x10 + x9 + x8 + x7 + x6 + x5 + x4 + x3 + x2 + x1 + x0;
		volatile double d0 = 1.25;
		volatile double d1 = 1.5;
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
		double d32 = d30 + d31 + d29 + d28 + d27 + d26 + d25 + d24 + d23 + d22 + d21 + d20 + d19 + d18 + d17 + d16 +
					 d15 + d14 + d13 + d12 + d11 + d10 + d9 + d8 + d7 + d6 + d5 + d4 + d3 + d2 + d1 + d0;
		uart.write("    5) Inner ISR: ");

		bool int_sum_ok = true;
		bool float_sum_ok = true;
		if (x17 != 4180) {
			int_sum_ok = false;
			uart.write("--- integer sum is incorrect! ");
		}
		if (d32 < 8009751. || d32 > 8009752.) {
			float_sum_ok = false;
			uart.write("--- floating-point sum is incorrect! ");
		}
		if (int_sum_ok && float_sum_ok)
			uart.write("both sums are OK. ");

		uart.write("...Exiting inner ISR\r\n");
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

	volatile uint32_t x0 = 2;
	volatile uint32_t x1 = 3;
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
	volatile double d0 = 1.75;
	volatile double d1 = 1.55;
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

	red_led.off();

	int i = 0;
	while (1) {
		i++;
		if (i == 1000) { // plenty of time for ISRs to complete
			uart.write("Reached main context: ");
			uint32_t x17 = x16 + x15 + x14 + x13 + x12 + x11 + x10 + x9 + x8 + x7 + x6 + x5 + x4 + x3 + x2 + x1 + x0;
			double d32 = d30 + d31 + d29 + d28 + d27 + d26 + d25 + d24 + d23 + d22 + d21 + d20 + d19 + d18 + d17 + d16 +
						 d15 + d14 + d13 + d12 + d11 + d10 + d9 + d8 + d7 + d6 + d5 + d4 + d3 + d2 + d1 + d0;

			bool int_sum_ok = true;
			bool float_sum_ok = true;
			if (x17 != 10943) {
				int_sum_ok = false;
				uart.write("--- Main loop integer sum is incorrect! ");
			}
			if (d32 < 9275135. || d32 > 9275136.) {
				float_sum_ok = false;
				uart.write("--- Main loop floating-point sum is incorrect! ");
			}
			if (int_sum_ok && float_sum_ok)
				uart.write(" both sums are OK ");

			while (1)
				;
		}
	};
}
