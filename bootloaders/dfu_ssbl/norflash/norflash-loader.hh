#pragma once
#include "drivers/interrupt.hh"
#include "drivers/interrupt_control.hh"
#include "print.hh"
#include "qspi_flash_conf.hh"
#include "qspi_flash_driver.hh"
#include "stm32mp1xx.h"
#include "usbd_desc.h"
#include "usbd_dfu_media.h"

struct NorFlashDFULoader {
	mdrivlib::QSpiFlash flash;

	NorFlashDFULoader()
		: flash{qspi_flash_conf}
	{
		print("QSPI is initialized.\n\r");
	}

	NorFlashDFULoader(const mdrivlib::QSPIFlashConfig &conf)
		: flash{conf}
	{
		print("QSPI is initialized.\n\r");
	}

	void start()
	{
		extern PCD_HandleTypeDef hpcd;

		USBD_HandleTypeDef USBD_Device;
		// ST USB library assumes handle is cleared to 0's:
		memset(&USBD_Device, 0, sizeof(USBD_Device));

		// TODO: DFUdevice dfu{flash};
		dfu_set_qspi_driver(&flash);

		auto init_ok = USBD_Init(&USBD_Device, &DFU_Desc, 0);
		if (init_ok != USBD_OK) {
			print("USB Device failed to initialize!\n");
			print("Error code: ", static_cast<uint32_t>(init_ok));
		}
		InterruptControl::disable_irq(OTG_IRQn);
		InterruptManager::registerISR(OTG_IRQn, [] { HAL_PCD_IRQHandler(&hpcd); });
		InterruptControl::set_irq_priority(OTG_IRQn, 0, 0);
		InterruptControl::enable_irq(OTG_IRQn);

		USBD_RegisterClass(&USBD_Device, USBD_DFU_CLASS);
		USBD_DFU_RegisterMedia(&USBD_Device, &USBD_DFU_MEDIA_fops);
		USBD_Start(&USBD_Device);
	}
};
