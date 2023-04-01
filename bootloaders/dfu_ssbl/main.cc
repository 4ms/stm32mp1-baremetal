#include "drivers/interrupt.hh"
#include "drivers/interrupt_control.hh"
#include "drivers/leds.hh"
#include "drivers/uart.hh"
#include "norflash/qspi_flash_conf.hh"
#include "stm32mp1xx.h"
#include "system_clk.hh"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_dfu_media.h"
#include <cstdint>

#include "brainboard_p3_conf.hh"
#include "mmp9_conf.hh"
#include "osd32brk_conf.hh"
#include "stm32disco_conf.hh"

// Uncomment one of these to select your board:
// namespace Board = OSD32BRK;
// namespace Board = STM32MP1Disco;
// namespace Board = MMp9;
namespace Board = Brainboard_p3;

extern PCD_HandleTypeDef hpcd;

namespace
{
Uart<Board::ConsoleUART> uart;
bool dfu_mode_requested();
} // namespace

void main()
{
	// Check DFU pin:
	// If it's open (high) -->  Try to Jump to App or else fall-through to DFU mode
	// If it's jumpered shut (low) --> run DFU mode

	if (!dfu_mode_requested()) {
		constexpr uint32_t AppFlashSectorAddr = 0x100000;
		// Load image
		auto image_entry = reinterpret_cast<void (*)()>(AppFlashSectorAddr);
		image_entry();
	}

	uart.write("\n\nUSB DFU Device\n");
	uart.write("Connect a USB cable to the computer\n");
	uart.write("Run `dfu-util --list` in a terminal and you should see this device.\n");
	uart.write("Note: This DFU loader only works with DDR RAM and NOR Flash. TODO: SDMMC\n");

	Board::GreenLED green1;
	green1.off();

	SystemClocks::init();

	USBD_HandleTypeDef USBD_Device;
	// ST USB library assumes handle is cleared to 0's:
	memset(&USBD_Device, 0, sizeof(USBD_Device));

	mdrivlib::QSpiFlash flash{qspi_flash_conf};
	// TODO: DFUdevice dfu{flash};
	dfu_set_qspi_driver(&flash);

	auto init_ok = USBD_Init(&USBD_Device, &DFU_Desc, 0);
	if (init_ok != USBD_OK) {
		uart.write("USB Device failed to initialize!\n");
		uart.write("Error code: ");
		uart.write(static_cast<uint32_t>(init_ok));
	}
	InterruptControl::disable_irq(OTG_IRQn);
	InterruptManager::registerISR(OTG_IRQn, [] { HAL_PCD_IRQHandler(&hpcd); });
	InterruptControl::set_irq_priority(OTG_IRQn, 0, 0);
	InterruptControl::enable_irq(OTG_IRQn);

	USBD_RegisterClass(&USBD_Device, USBD_DFU_CLASS);
	USBD_DFU_RegisterMedia(&USBD_Device, &USBD_DFU_MEDIA_fops);
	USBD_Start(&USBD_Device);

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

namespace
{
bool dfu_mode_requested()
{
	Board::DFUBootPin.init(PinMode::Input, PinPull::Up, PinPolarity::Inverted);
	uint32_t pin_read = 10000;
	uint32_t dfu_pin_active = 0;

	while (pin_read--) {
		if (Board::DFUBootPin.read())
			dfu_pin_active++;
	}

	return (dfu_pin_active > 5000);
}
} // namespace

extern "C" int __io_putchar(int ch)
{
	uart.putchar(ch);
	return ch;
}
