#include "boot_media_loader.hh"
#include "clocks.hh"
#include "ddr/ram_tests.hh"
#include "ddr/stm32mp1_ram.h"
#include "delay.h"
#include "drivers/leds.hh"
#include "drivers/uart.hh"
// #include "mmp9_conf.hh"
#include "osd32brk_conf.hh"
#include "pmic.hh"
#include "printf/printf.h"
#include "stm32mp157cxx_ca7.h"

namespace Board = OSD32BRK;
void security_init();

void main()
{
	Board::BlueLED blue_led;
	blue_led.off();
	blue_led.on();

	SystemClocks::init_core_clocks();
	security_init();

	Uart<Board::ConsoleUART> uart(Board::UartRX, Board::UartTX, 115200);
	printf_("\n\n");

	if constexpr (Board::PMIC::HasSTPMIC) {
		STPMIC1 pmic{Board::PMIC::I2C_config};
	}

	printf_("Initializing RAM\n");
	stm32mp1_ddr_setup();

	printf_("Testing RAM.\n");
	const auto ram_size = stm32mp1_ddr_get_size();
	RamTests::run_all(DRAM_MEM_BASE, ram_size);

	auto boot_method = BootDetect::read_boot_method();
	printf_("Booted from %s\n", BootDetect::bootmethod_string(boot_method).data());

	printf_("Loading app image\n");
	BootMediaLoader loader{boot_method};
	bool image_ok = loader.load_image();

	if (image_ok) {
		printf_("Jumping to app\n");
		loader.boot_image();
	}

	// Should not reach here, but in case we do, blink LED rapidly
	printf_("FAILED! Did not find and load an app image\n");
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

void security_init()
{
	// Disable protection to RCC->BDCR, PWR->CR2, RTC, backup registers
	PWR->CR1 = PWR->CR1 | PWR_CR1_DBP;
	while (!(PWR->CR1 & PWR_CR1_DBP))
		;

	// Disable Trust Zone
	RCC->TZCR = 0;

	// Allow read/write for all securable peripherals (top 7 bits are reserved)
	TZPC->DECPROT0 = 0x01FFFFFF;

	// Allow non-secure access to SYSRAM
	TZPC->TZMA1_SIZE = 0;

	// Disable all tamper detection
	TAMP->CR1 = 0;

	// Allow non-secure access to GPIOZ
	RCC->MP_AHB5ENSETR = RCC_MP_AHB5ENSETR_GPIOZEN;
	GPIOZ->SECCFGR = 0;
}
