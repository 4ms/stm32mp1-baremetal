#pragma once
#include "boot_image_def.hh"
#include "boot_loader.hh"
#include "drivers/pinconf.hh"
#include "norflash/qspi_norflash_read.h"
#include "print_messages.hh"
#include <cstdint>

struct BootNorLoader : BootLoader {
	BootNorLoader()
	{
		PinConf d2{GPIO::F, PinNum::_7, PinAF::AF_9};
		PinConf d3{GPIO::A, PinNum::_1, PinAF::AF_9};
		d2.init(PinMode::Alt);
		d3.init(PinMode::Alt);

		QSPI_init();
	}

	BootImageDef::image_header read_image_header() override
	{
		BootImageDef::image_header header;

		auto ok = QSPI_read_MM((uint8_t *)(&header), BootImageDef::NorFlashSSBLAddr, BootImageDef::HeaderSize);
		if (!ok) {
			pr_err("Failed reading NOR Flash\n");
			return {};
		}

		return header;
	}

	bool load_image(uint32_t load_addr, uint32_t size) override
	{
		auto load_dst = reinterpret_cast<uint8_t *>(load_addr);
		return QSPI_read_MM(load_dst, BootImageDef::NorFlashSSBLAddr, size);
	}
};
