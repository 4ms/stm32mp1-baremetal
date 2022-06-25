#include "boot_nor.hh"
#include "bootdetect.hh"
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

	Uart<Board::ConsoleUART> uart(Board::UartRX, Board::UartTX, 115200);
	uart.write("Starting FSBL\n");

	// Debugging LEDs
	Board::RedLED red_led;
	Board::GreenLED green_led;
	Board::BlueLED blue_led;
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

	if (bootmode == BootDetect::BOOT_FLASH_NOR_QSPI) {
		// uint8_t hdr[256];
		// BootNorLoader::read_header(hdr);
		// printf_("Header:\n");
		// for (unsigned i = 0; i < 256; i++)
		// 	printf_("%02x ", hdr[i]);

		BootImageDef::spl_image_info img;
		BootNorLoader::parse_header(img);
		printf_("name=%s\n", img.name);
		printf_("loadaddr=%x\n", img.load_addr);
		printf_("entry_point=%x\n", img.entry_point);
		printf_("size=%x\n", img.size);
		printf_("bootdevice=%x\n", img.boot_device);
		printf_("arg=%p\n", img.arg);
		printf_("flags=%x\n", img.flags);
		printf_("os=%x\n", img.os);
	}

	// Inf loop for now
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
