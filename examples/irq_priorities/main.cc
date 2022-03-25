#include "drivers/interrupt.hh"
#include "drivers/interrupt_control.hh"
#include "drivers/leds.hh"
#include "drivers/pinchange.hh"
#include "drivers/uart.hh"
#include "osd32brk_conf.hh"
#include "stm32mp157cxx_ca7.h"
#include <cstdint>

// For debugging, it's handy to know these values:
// IRQn = 0x62 (98) | PRIORITY: Reg 24 bit 2 | CFGR: Reg 16, bit 2 | ENABLE Reg 3, bit2
// IRQn = 0x63 (99) | PRIORITY: Reg 24 bit 3 | CFGR: Reg 16, bit 3 | ENABLE Reg 3, bit3

using namespace OSD32BRK;

void main()
{
	InterruptControl::global_disable_irq();

	// UART
	Uart<UART4_BASE> uart;

	// Pins to be used in pin-change interrupts:
	Led<GPIOI_BASE, 8, LedActive::Low> pin0;
	Led<GPIOI_BASE, 9, LedActive::Low> pin1;
	Led<GPIOE_BASE, 15, LedActive::Low> pin2;
	Led<GPIOG_BASE, 15, LedActive::Low> pin3;
	pin0.init();
	pin1.init();
	pin2.init();
	pin3.init();

	// Pin Change interrupts
	PinChangeISR<GPIOI_BASE, 8> pin0_pinchange;
	PinChangeISR<GPIOI_BASE, 9> pin1_pinchange;
	PinChangeISR<GPIOE_BASE, 15> pin2_pinchange;
	PinChangeISR<GPIOG_BASE, 15> pin3_pinchange;
	const auto pin0_irqnum = pin0_pinchange.get_IRQ_num();
	const auto pin1_irqnum = pin1_pinchange.get_IRQ_num();
	const auto pin2_irqnum = pin2_pinchange.get_IRQ_num();
	const auto pin3_irqnum = pin3_pinchange.get_IRQ_num();
	pin0_pinchange.enable_isr_on_rising_falling_edges(true, false);
	pin1_pinchange.enable_isr_on_rising_falling_edges(true, false);
	pin2_pinchange.enable_isr_on_rising_falling_edges(true, false);
	pin3_pinchange.enable_isr_on_rising_falling_edges(true, false);

	uart.write("\r\n\r\nTesting IRQ sub-priorities\r\n");
	uart.write("Pending ISRs with equal priority but unequal sub-priority are handled in order of sub-priority\r\n");
	// uart.write("Pending ISRs with unequal priority are handled in order of priority, ignoring sub-priority\r\n");
	{
		// All IRQs off while we set everything up
		InterruptControl::global_disable_irq();

		// Initial conditions:
		pin0.off();
		pin1.off();
		pin2.off();

		// Pin0 IRQ: Priority 1, Sub-pri 0
		InterruptControl::disable_irq(pin0_irqnum);
		InterruptControl::set_irq_priority(pin0_irqnum, 1, 0);
		InterruptManager::registerISR(pin0_irqnum, [&]() {
			uart.write(">> 0  in\r\n");
			pin0_pinchange.clear_rising_isr_flag();
			uart.write("<< 0 out\r\n");
		});

		// Pin1 IRQ: Priority 1, Sub-pri 1
		InterruptControl::disable_irq(pin1_irqnum);
		InterruptControl::set_irq_priority(pin1_irqnum, 1, 1);
		InterruptManager::registerISR(pin1_irqnum, [&]() {
			uart.write(">> 1  in\r\n");
			pin1_pinchange.clear_rising_isr_flag();
			uart.write("<< 1  out\r\n");
		});

		// Pin2 IRQ: Priority 1, Sub-pri 2
		InterruptControl::disable_irq(pin2_irqnum);
		InterruptControl::set_irq_priority(pin2_irqnum, 1, 2);
		InterruptManager::registerISR(pin2_irqnum, [&]() {
			uart.write(">> 2  in\r\n");
			pin2_pinchange.clear_rising_isr_flag();
			uart.write("<< 2  out\r\n");
		});

		// Enable everything
		pin0_pinchange.enable();
		pin1_pinchange.enable();
		pin2_pinchange.enable();
		InterruptControl::enable_irq(pin0_irqnum);
		InterruptControl::enable_irq(pin1_irqnum);
		InterruptControl::enable_irq(pin2_irqnum);

		// Turn on pins in a different order than their sub-priorities
		pin1.on();
		pin2.on();
		pin0.on();

		// Enable IRQs globally.
		uart.write("You should see the ISR handlers run in order: 0, 1, 2\r\n");
		InterruptControl::global_enable_irq();

		// Wait around a bit
		int i = 1000;
		while (i--)
			__NOP();
	}

	uart.write("\r\n\r\nTesting IRQ pre-emption\r\n");
	uart.write("Higher priority pending ISRs will interrupt a lower priority ISR, but not vice-versa (nor if "
			   "priorities are equal, regardless of sub-priority).\r\n");
	{
		// All IRQs off while we set everything up
		InterruptControl::global_disable_irq();

		// Initial conditions:
		pin0.off();
		pin1.off();
		pin2.off();
		pin3.off();

		// Test that Priority 1 can interrupt Priority 2, but Priority

		// Pin0 IRQ: Priority 0, Sub-pri 2
		InterruptControl::disable_irq(pin0_irqnum);
		InterruptControl::set_irq_priority(pin0_irqnum, 0, 2);
		InterruptManager::registerISR(pin0_irqnum, [&]() {
			uart.write(">> 0:2  in\r\n");
			pin0_pinchange.clear_rising_isr_flag();
			uart.write("<< 0:2 out\r\n");
		});

		// Pin1 IRQ: Priority 1, Sub-pri 1
		InterruptControl::disable_irq(pin1_irqnum);
		InterruptControl::set_irq_priority(pin1_irqnum, 1, 1);
		InterruptManager::registerISR(pin1_irqnum, [&]() {
			uart.write(">> 1:1  in\r\n");
			pin1_pinchange.clear_rising_isr_flag();
			uart.write("   1:1 triggering 0:2\r\n");
			pin0.on();
			uart.write("   1:1 triggering 2:0\r\n");
			pin2.on();
			uart.write("   1:1 triggering 1:0\r\n");
			pin3.on();
			uart.write("<< 1:1  out\r\n");
		});

		// Pin2 IRQ: Priority 2, Sub-pri 0
		InterruptControl::disable_irq(pin2_irqnum);
		InterruptControl::set_irq_priority(pin2_irqnum, 2, 0);
		InterruptManager::registerISR(pin2_irqnum, [&]() {
			uart.write(">> 2:0  in\r\n");
			pin2_pinchange.clear_rising_isr_flag();
			uart.write("<< 2:0  out\r\n");
		});

		// Pin3 IRQ: Priority 1, Sub-pri 0
		InterruptControl::disable_irq(pin3_irqnum);
		InterruptControl::set_irq_priority(pin3_irqnum, 1, 0);
		InterruptManager::registerISR(pin3_irqnum, [&]() {
			uart.write(">> 1:0  in\r\n");
			pin3_pinchange.clear_rising_isr_flag();
			uart.write("<< 1:0  out\r\n");
		});

		// Enable everything
		pin0_pinchange.enable();
		pin1_pinchange.enable();
		pin2_pinchange.enable();
		pin3_pinchange.enable();
		InterruptControl::enable_irq(pin0_irqnum);
		InterruptControl::enable_irq(pin1_irqnum);
		InterruptControl::enable_irq(pin2_irqnum);
		InterruptControl::enable_irq(pin3_irqnum);

		// Enable IRQs globally.
		InterruptControl::global_enable_irq();

		uart.write("\r\n\r\nYou should see the ISR handlers run in order: [1:1 [0:2]] [1:0] [2:0]\r\n");
		// Start the chain of events:
		pin1.on();

		// Wait around a bit
		int i = 1000;
		while (i--)
			__NOP();

		InterruptControl::global_disable_irq();
	}

	while (1) {
	};
}
