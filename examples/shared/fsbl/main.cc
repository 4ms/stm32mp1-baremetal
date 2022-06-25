#include <cstdint>
#include <cstdio>

#include "clocks.hh"
#include "ddr/stm32mp1_ram.h"
#include "delay.h"
#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "mmp9_conf.hh"
#include "printf/printf.h"
#include "stm32mp157cxx_ca7.h"
#include <cstdint>

namespace Board = MMp9;

void delay(unsigned cycles);

void main()
{
	// TODO: verify clock speed
	auto sysclockerr = SystemClocks::init_pll1_pll2();

	Board::RedLED red_led;
	Board::GreenLED green_led;
	Board::BlueLED blue_led;

	Uart<Board::ConsoleUART> uart(Board::UartRX, Board::UartTX, 115200);
	uart.write("Starting FSBL\n");

	red_led.off();
	red_led.on();
	red_led.off();
	red_led.on();
	red_led.off();
	red_led.on();
	red_led.off();

	uart.write("Initializing RAM\n");
	int err = stm32mp1_ddr_setup();

	volatile uint32_t *addr = reinterpret_cast<uint32_t *>(0xC0000000);
	*addr = 0x12345678;
	printf_("Testing RAM: Wrote 0x12345678, read %08x", *addr);
	if (*addr == 0x12345678)
		printf_("... Success!\n");
	else
		printf_("... FAILED!\n");

	// Boot Detect
	uint32_t bootmode = BootDetect::read_bootmode();
	printf_("Booted from %s (%x)\n", BootDetect::bootmode_string(bootmode).data(), bootmode);
	// Inf loop for now
	constexpr uint32_t dlytime = 600000;
	while (1) {
		blue_led.on();
		udelay(dlytime);
		blue_led.off();
		udelay(dlytime);
	}
}

void delay(unsigned cycles)
{
	while (cycles--)
		asm("nop");
}

// Allows use of printf_(...)
extern "C" void putchar_(char c)
{
	Uart<Board::ConsoleUART>::putchar(c);
}
