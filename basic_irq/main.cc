#include "delay.hh"
#include "leds.hh"
#include "mdrivlib/interrupt.hh"
#include "mdrivlib/interrupt_control.hh"
#include "pinchange.hh"
#include "stm32mp157cxx_ca7.h"
#include "uart.hh"
#include <cstdint>

constexpr uint8_t GPIO_I = 8; // GPIOA = 0, GPIOB = 1, etc..
// IRQn = 0x62 (98) | PRIORITY: Reg 24 bit 2 | CFGR: Reg 16, bit 2 | ENABLE Reg 3, bit2

void main()
{
	uint32_t num_times_irq_called = 0;

	// UART
	Uart<UART4_BASE> uart;
	uart.write("\r\n\r\nTesting interrupts\r\n");

	// LEDs
	Led<GPIOI_BASE, 8, LedActive::Low> red_led1;
	Led<GPIOI_BASE, 9, LedActive::Low> green_led1;
	Led<GPIOE_BASE, 15, LedActive::High> debug_pin2;
	Led<GPIOG_BASE, 15, LedActive::High> debug_pin3;
	red_led1.init();
	green_led1.init();
	red_led1.off();
	green_led1.off();

	debug_pin2.init();
	debug_pin3.init();

	// Pin Change interrupt for red led
	PinChangeISR red_led_pinchange{GPIO_I, 8};
	const auto red_led1_irqnum = red_led_pinchange.get_IRQ_num();

	GIC_DisableIRQ(red_led1_irqnum);
	GIC_SetTarget(red_led1_irqnum, 1);
	GIC_SetPriority(red_led1_irqnum, 0b00000000);
	GIC_SetConfiguration(red_led1_irqnum, 0b10);
	// 0b10 Edge triggered (reads as 0b11)
	// 0b00 Level triggered (reads as 0b01)

	// Setup interrupt for red LED 1 turning off

	InterruptManager::registerISR(red_led1_irqnum, [&]() {
		red_led_pinchange.clear_rising_isr_flag();
		uart.write(">>> Entering IRQ handler\r\n");
		num_times_irq_called++;
		uart.write("<<< Exiting IRQ handler\r\n");
	});

	// Toggle the LED just to prove the ISR is not getting called
	red_led1.on();
	red_led1.off();
	red_led1.on();

	// Enable the ISR
	red_led_pinchange.enable_isr_on_rising_falling_edges(true, false);
	red_led_pinchange.enable();
	red_led_pinchange.clear_rising_isr_flag();
	GIC_ClearPendingIRQ(red_led1_irqnum);
	GIC_EnableIRQ(red_led1_irqnum);

	// Trigger ISR
	uart.write("Triggering ISR: Red LED 1 turning off...\r\n");
	Delay::cycles(0x1000000);
	uart.write("5 ");
	Delay::cycles(0x2000000);
	uart.write("\r4 ");
	Delay::cycles(0x2000000);
	uart.write("\r3 ");
	Delay::cycles(0x2000000);
	uart.write("\r2 ");
	Delay::cycles(0x2000000);
	uart.write("\r1 ");
	Delay::cycles(0x2000000);
	uart.write("\r0 \r");
	red_led1.off();

	int i = 0;
	while (1) {
		i++;
		if (i == 10000) { // plenty of time for ISR to complete
			if (num_times_irq_called == 1)
				uart.write("ISR was called once, hooray!\r\n");
			else if (num_times_irq_called == 0)
				uart.write("ISR was not called, boo.\r\n");
			else
				uart.write("ISR was called more than once.. hmmmrr.\r\n");

			while (1)
				;
		}
	};
}
