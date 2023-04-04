#include "app_loader/boot_detect.hh"
#include "app_loader/boot_media_loader.hh"
#include "board_conf.hh"
#include "drivers/interrupt.hh"
#include "drivers/interrupt_control.hh"
#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "norflash/norflash-loader.hh"
#include "norflash/qspi_flash_conf.hh"
#include "print.hh"
#include "stm32mp1xx.h"
#include "system_clk.hh"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_dfu_media.h"
#include <cstdint>

namespace
{
Uart<Board::ConsoleUART> uart;
} // namespace

void main()
{
	// auto boot_method = BootDetect::read_boot_method();
	// print("Booted from ", BootDetect::bootmethod_string(boot_method).data(), "\n");
	// print("Loading app image...\n");

	// constexpr uint32_t AppFlashSectorAddr = 0x100000;
	// BootMediaLoader loader{boot_method};
	// bool image_ok = loader.load_image(BootLoader::ImageType::Kernel);

	// if (image_ok) {
	// 	print("Jumping to app\n");
	// 	loader.boot_image();
	// }

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
