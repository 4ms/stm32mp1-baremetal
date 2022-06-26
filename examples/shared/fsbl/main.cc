#include "stm32mp157cxx_ca7.h"

#include "clocks.hh"
#include "delay.h"
#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "mmp9_conf.hh"

#include "ddr/ram_tests.hh"
#include "ddr/stm32mp1_ram.h"

#include "boot_media_loader.hh"
#include "boot_nor.hh"
#include "bootdetect.hh"

#include "printf/printf.h"

namespace Board = MMp9;

void delay(unsigned cycles);

void main()
{
	SystemClocks::init_pll1_pll2();

	Uart<Board::ConsoleUART> uart(Board::UartRX, Board::UartTX, 115200);

	printf_("Initializing RAM\n");
	stm32mp1_ddr_setup();

	printf_("Testing RAM\n");
	const uint32_t ram_start = DRAM_MEM_BASE;
	const auto ram_size = stm32mp1_ddr_get_size();
	const uint32_t ram_end = ram_start + ram_size;
	RamTests::run_all(ram_start, ram_size);

	BootMediaLoader loader{BootDetect::read_boot_method()};

	printf_("Reading app image header\n");
	auto app_memory_area = loader.read_app_image_header();
	if (app_memory_area.start < ram_start || app_memory_area.end > ram_end)
		panic("App will not fit in RAM: app %x to %x exceeds RAM %x to %x\n",
			  app_memory_area.start,
			  app_memory_area.end,
			  ram_start,
			  ram_end);

	printf_("Loading app image\n");
	loader.load_image();

	printf_("Jumping to app\n");
	loader.boot_image();

	// Should not reach here, but in case we do, blink LED rapidly
	printf_("FAILED!\n");
	Board::BlueLED blue_led;
	constexpr uint32_t dlytime = 100000;
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
