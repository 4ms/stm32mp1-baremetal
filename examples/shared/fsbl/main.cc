#include <cstdint>
#include <cstdio>

#include "clocks.hh"
#include "ddr/stm32mp1_ram.h"
#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "osd32brk_conf.hh"
#include "printf/printf.h"
#include "stm32mp157cxx_ca7.h"

namespace Board = OSD32BRK;

void delay(unsigned cycles);

void main()
{
	auto sysclockerr = SystemClocks::init_pll1_pll2();
	int pll1 = ((__HAL_RCC_GET_MPU_SOURCE() == RCC_MPUSOURCE_PLL1) && __HAL_RCC_GET_FLAG(RCC_FLAG_MPUSRCRDY));
	int mpudiv = ((__HAL_RCC_GET_MPU_SOURCE() == RCC_MPUSOURCE_MPUDIV) && __HAL_RCC_GET_FLAG(RCC_FLAG_MPUSRCRDY));

	Board::RedLED red_led;
	Board::GreenLED green_led;
	Board::OrangeLED blue_led;

	Uart<Board::ConsoleUART> uart(Board::UartRX, Board::UartTX, 115200);
	uart.write("Starting FSBL...\n");
	printf_("Sysclock init returned %d, pll1mpu=%d, mpudiv=%d\n", sysclockerr, pll1, mpudiv);
	printf_("Printf works\n");
	printf_("1 + 2 = %d\n", (1 + 2));

	red_led.off();
	red_led.on();
	red_led.off();

	int err = stm32mp1_ddr_setup();
	printf_("DDR Setup Result = %d\n", err);

	red_led.on();
	if (err) {
		blue_led.on();
	}

	volatile uint32_t *addr = reinterpret_cast<uint32_t *>(0xC0000000);
	*addr = 0x12345678;

	printf_("Wrote 2, read %u", *addr);

	constexpr uint32_t dlytime = 600000;
	while (1) {
		// red_led.on();
		// delay(dlytime);
		red_led.off();

		green_led.on();
		delay(dlytime);
		green_led.off();
		delay(dlytime);

		// blue_led.on();
		// delay(dlytime);
		blue_led.off();
	}
}

void delay(unsigned cycles)
{
	while (cycles--) {
		asm("nop");
	}
}

// Allows use of printf_(...)
extern "C" void putchar_(char c)
{
	// Uart<Board::ConsoleUART>::putchar(c);
	Uart<Board::ConsoleUART> uart;
	uart.write(c);
}
