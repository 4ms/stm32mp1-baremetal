#include "stm32mp157cxx_ca7.h"

#include "clocks.hh"
#include "delay.h"
#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "mmp9_conf.hh"

#include "ddr/ram_tests.hh"
#include "ddr/stm32mp1_ram.h"

#include "boot_nor.hh"
#include "bootdetect.hh"

#include "printf/printf.h"

namespace Board = MMp9;

void delay(unsigned cycles);

void main()
{
	// TODO: verify clock speed
	auto sysclockerr = SystemClocks::init_pll1_pll2();

	Uart<Board::ConsoleUART> uart(Board::UartRX, Board::UartTX, 115200);
	printf_("Starting FSBL\n");

	printf_("Initializing RAM\n");
	stm32mp1_ddr_setup();
	const uint32_t ram_start = DRAM_MEM_BASE;
	auto ram_size = stm32mp1_ddr_get_size();

	printf_("Testing RAM\n");
	RamTests::run_all(ram_start, ram_size);

	// Boot Detect
	uint32_t bootmode = BootDetect::read_bootmode();
	printf_("Booted from %s (%x)\n", BootDetect::bootmode_string(bootmode).data(), bootmode);

	if (bootmode == BootDetect::BOOT_FLASH_NOR_QSPI) {
		BootNorLoader nor_loader;
		nor_loader.read_image_info();
		// Debug info
		if (auto result = nor_loader.get_image_info()) {
			auto img = *result;
			printf_("loadaddr=%x\n", img.load_addr);
			printf_("entry_point=%x\n", img.entry_point);
			printf_("size=%x\n", img.size);
			printf_("bootdevice=%x\n", img.boot_device);
			printf_("arg=%p\n", img.arg);
			printf_("flags=%x\n", img.flags);
			printf_("os=%x\n", img.os);
			printf_("name=%s\n", img.name);
		} else {
			panic("No valid boot image found");
		}

		nor_loader.boot_image();
	}

	// Inf loop for now
	// Debugging LEDs
	Board::RedLED red_led;
	Board::GreenLED green_led;
	Board::BlueLED blue_led;
	constexpr uint32_t dlytime = 600000;
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
