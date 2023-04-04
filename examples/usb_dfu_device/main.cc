#include "board_conf/qspi_flash_conf.hh"
#include "drivers/interrupt.hh"
#include "drivers/interrupt_control.hh"
#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "norflash/norflash-loader.hh"
#include "print.hh"
#include "stm32mp1xx.h"
#include "system_clk.hh"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_dfu_media.h"
#include <cstdint>

// Uncomment one of these to select your board:
// #include "osd32brk_conf.hh"
// namespace Board = OSD32BRK;

// #include "stm32disco_conf.hh"
// namespace Board = STM32MP1Disco;

#include "brainboard_p3_conf.hh"
namespace Board = Brainboard_p3;

namespace
{
Uart<Board::ConsoleUART> uart;
} // namespace

void main()
{
	print("\n\nUSB DFU Loader Starting...\n");

	Board::GreenLED green1;
	green1.off();

	SystemClocks::init();

	NorFlashDFULoader dfu_loader{qspi_flash_conf}; // TODO: Board::QspiFlashConf
	dfu_loader.start();

	print("Connect a USB cable to the computer\n");
	print("Run `dfu-util --list` in a terminal and you should see this device.\n");
	print("Note: This DFU loader only works with NOR Flash. TODO: SDMMC and DDR RAM\n");
	print("You must reboot after loading. TODO: auto-jump to app after detaching\n");

	// Blink green1 light at 1Hz
	uint32_t last_tm = 0;
	bool led_state = false;
	while (1) {
		uint32_t tm = HAL_GetTick();
		if (tm > (last_tm + 500)) {
			last_tm = tm;
			if (led_state) {
				green1.off();
			} else {
				green1.on();
			}
			led_state = !led_state;
		}
	}
}

void putchar_s(const char c) { uart.putchar(c); }
