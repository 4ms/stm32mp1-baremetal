#include "boot_media_loader.hh"
#include "clocks.hh"
#include "ddr/ram_tests.hh"
#include "ddr/stm32mp1_ram.h"
#include "delay.h"
#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "pmic.hh"
#include "print.hh"
#include "stm32mp157cxx_ca7.h"
#include "systeminit.h"

#include "osd32brk_conf.hh"
#include "stm32disco_conf.hh"

// Uncomment one of these to select your board:
namespace Board = OSD32BRK;
// namespace Board = STM32MP1Disco;

void main()
{
	Board::OrangeLED led;

	auto clockspeed = SystemClocks::init_core_clocks(Board::HSE_Clock_Hz, Board::MPU_MHz);
	security_init();

	Uart<Board::ConsoleUART> console(Board::UartRX, Board::UartTX, 115200);
	print("\n\nMP1-Boot\n\n");
	print("MPU clock: ", clockspeed, " Hz\n");

	if constexpr (Board::PMIC::HasSTPMIC) {
		STPMIC1 pmic{Board::PMIC::I2C_config};

		if (!pmic.setup_vddcore_pwr())
			panic("Could not setup PMIC VDDCORE\n");

		if (!pmic.setup_ddr3_pwr())
			panic("Could not setup PMIC DDR voltages\n");
	}

	print("Initializing RAM\n");
	stm32mp1_ddr_setup();

	print("Testing RAM.\n");
	RamTests::run_all(DRAM_MEM_BASE, stm32mp1_ddr_get_size());

	auto boot_method = BootDetect::read_boot_method();
	print("Booted from ", BootDetect::bootmethod_string(boot_method).data(), "\n");

	print("Loading app image...\n");

	BootMediaLoader loader{boot_method};

	bool image_ok = loader.load_image();

	if (image_ok) {
		print("Jumping to app\n");
		loader.boot_image();
	}

	// Should not reach here, but in case we do, blink LED rapidly
	print("FAILED!\n");
	constexpr uint32_t dlytime = 50000;
	while (1) {
		led.on();
		udelay(dlytime);
		led.off();
		udelay(dlytime);
	}
}

void putchar_s(const char c)
{
	Uart<Board::ConsoleUART>::putchar(c);
}
