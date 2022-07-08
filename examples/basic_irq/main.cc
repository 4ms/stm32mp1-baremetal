#include "drivers/delay.hh"
#include "drivers/leds.hh"
#include "drivers/pinchange.hh"
#include "drivers/uart.hh"
#include "stm32mp157cxx_ca7.h"
#include <cstdint>

#include "osd32brk_conf.hh"
#include "stm32disco_conf.hh"

// Uncomment one of these to select your board:
namespace Board = OSD32BRK;
// namespace Board = STM32MP1Disco;

namespace BasicIRQ
{
Uart<UART4_BASE> uart;

PinChangeISR<Board::RedLED2::gpio, Board::RedLED2::pin_num> red_led_pinchange;
const auto red_led_irqnum = red_led_pinchange.get_IRQ_num();

PinChangeISR<Board::GreenLED2::gpio, Board::RedLED2::pin_num> green_led_pinchange;
const auto green_led_irqnum = green_led_pinchange.get_IRQ_num();

uint32_t num_times_red_led_irq_called = 0;
uint32_t num_times_green_led_irq_called = 0;

} // namespace BasicIRQ

void main()
{
	using namespace BasicIRQ;
	uart.write("\r\n\r\nTesting interrupts\r\n");

	Board::RedLED2 red_led;
	Board::GreenLED2 green_led;
	red_led.off();
	green_led.off();

	GIC_DisableIRQ(red_led_irqnum);
	GIC_SetTarget(red_led_irqnum, 1);
	GIC_SetPriority(red_led_irqnum, 0b00000000);
	GIC_SetConfiguration(red_led_irqnum, 0b10);

	// Toggle the LEDs just to prove the ISR is not getting called
	red_led.on();
	red_led.off();
	red_led.on();

	green_led.on();
	green_led.off();
	green_led.on();

	// Enable the ISRs
	red_led_pinchange.enable_isr_on_rising_falling_edges(true, false);
	red_led_pinchange.enable();
	red_led_pinchange.clear_rising_isr_flag();
	GIC_ClearPendingIRQ(red_led_irqnum);
	GIC_EnableIRQ(red_led_irqnum);

	green_led_pinchange.enable_isr_on_rising_falling_edges(true, false);
	green_led_pinchange.enable();
	green_led_pinchange.clear_rising_isr_flag();
	GIC_ClearPendingIRQ(green_led_irqnum);
	GIC_EnableIRQ(green_led_irqnum);

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
	// Flip the red LED pin, triggering the pin-change interrupt
	red_led.off();

	Delay::cycles(0x2000000);

	uart.write("Triggering ISR: Green LED 1 turning off...\r\n");
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

	// Flip the green LED pin, triggering the pin-change interrupt
	green_led.off();

	int i = 0;
	while (1) {
		i++;
		if (i == 10000) { // plenty of time for ISR to complete
			if (num_times_red_led_irq_called == 1)
				uart.write("Red LED ISR was called once, hooray!\r\n");
			else if (num_times_red_led_irq_called == 0)
				uart.write("Red LED ISR was not called, boo.\r\n");
			else
				uart.write("Red LED ISR was called more than once.. hmmmrr.\r\n");

			if (num_times_green_led_irq_called == 1)
				uart.write("Green LED ISR was called once, hooray!\r\n");
			else if (num_times_green_led_irq_called == 0)
				uart.write("Green LED ISR was not called, boo.\r\n");
			else
				uart.write("Green LED ISR was called more than once.. hmmmrr.\r\n");

			while (1)
				;
		}
	};
}

// The vector table says to jump here when the processor receives an IRQ from the GIC
extern "C" void __attribute__((interrupt("IRQ"))) IRQ_Handler()
{
	using namespace BasicIRQ;

	// Read from the GIC IAR register to get the interrupt number
	// By reading this register, we signal to the GIC that we're
	// about to handle the interrupt.
	auto irqnum = GIC_AcknowledgePending();

	if (irqnum == red_led_irqnum) {
		red_led_pinchange.clear_rising_isr_flag();
		uart.write(">>> Entering red LED IRQ handler\r\n");
		num_times_red_led_irq_called++;
		uart.write("<<< Exiting red LED IRQ handler\r\n");
	}

	if (irqnum == green_led_irqnum) {
		green_led_pinchange.clear_rising_isr_flag();
		uart.write(">>> Entering green LED IRQ handler\r\n");
		num_times_green_led_irq_called++;
		uart.write("<<< Exiting green LED IRQ handler\r\n");
	}

	// Write back the interrupt number into the GIC's EOIR register.
	// This tells the GIC we're done handling the interrupt.
	GIC_EndInterrupt(irqnum);
}
