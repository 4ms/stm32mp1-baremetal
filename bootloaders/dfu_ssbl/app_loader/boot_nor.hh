#pragma once
#include "board_conf.hh"
#include "boot_image_def.hh"
#include "boot_loader.hh"
#include "drivers/pinconf.hh"
#include "norflash/qspi_norflash_read.h"
#include "print_messages.hh"

struct BootNorLoader : BootLoader {
	BootNorLoader()
	{
		Board::NORFlash::d2.init(PinMode::Alt);
		Board::NORFlash::d3.init(PinMode::Alt);

		QSPI_init();
	}

	BootImageDef::image_header read_image_header(ImageType image_type = ImageType::SSBL) override
	{
		BootImageDef::image_header header;
		auto image_addr =
			(image_type == ImageType::SSBL) ? BootImageDef::NorFlashSSBLAddr : BootImageDef::NorFlashKernelAddr;
		auto ok = QSPI_read_MM((uint8_t *)(&header), image_addr, BootImageDef::HeaderSize);
		if (!ok) {
			pr_err("Failed reading NOR Flash\n");
			return {};
		}

		return header;
	}

	bool load_image(uint32_t load_addr, uint32_t size, ImageType image_type = ImageType::SSBL) override
	{
		auto load_dst = reinterpret_cast<uint8_t *>(load_addr);
		auto image_addr =
			(image_type == ImageType::SSBL) ? BootImageDef::NorFlashSSBLAddr : BootImageDef::NorFlashKernelAddr;
		return QSPI_read_MM(load_dst, image_addr, size);
	}
};
