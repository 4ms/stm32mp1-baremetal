#include "boot_media_loader.hh"
#include "clocks.hh"
#include "ddr/ram_tests.hh"
#include "ddr/stm32mp1_ram.h"
#include "delay.h"
#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "mmp9_conf.hh"
#include "printf/printf.h"
#include "stm32mp157cxx_ca7.h"

namespace Board = MMp9;

void main()
{
	SystemClocks::init_pll1_pll2();

	Uart<Board::ConsoleUART> uart(Board::UartRX, Board::UartTX, 115200);
	printf_("\n\nInitializing RAM\n");
	stm32mp1_ddr_setup();

	printf_("Testing RAM\n");
	const uint32_t ram_start = DRAM_MEM_BASE;
	const auto ram_size = stm32mp1_ddr_get_size();
	const uint32_t ram_end = ram_start + ram_size;
	RamTests::run_all(ram_start, ram_size);

	auto boot_method = BootDetect::read_boot_method();
	BootMediaLoader loader{boot_method};

	printf_("Reading app image header\n");
	loader.read_app_image_header();

	printf_("Loading app image\n");
	loader.load_image();

	printf_("Jumping to app\n");
	loader.boot_image();

	// Should not reach here, but in case we do, blink LED rapidly
	printf_("FAILED!\n");
	Board::BlueLED blue_led;
	constexpr uint32_t dlytime = 50000;
	while (1) {
		blue_led.on();
		udelay(dlytime);
		blue_led.off();
		udelay(dlytime);
	}
}

// Allows use of printf_(...)
extern "C" void putchar_(char c)
{
	Uart<Board::ConsoleUART>::putchar(c);
}
