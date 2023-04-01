#pragma once
#include "boot_image_def.hh"

struct BootLoader {
	enum class ImageType { SSBL, Kernel };
	virtual BootImageDef::image_header read_image_header(ImageType image_type = ImageType::SSBL) = 0;
	virtual bool load_image(uint32_t load_addr, uint32_t size, ImageType image_type = ImageType::SSBL) = 0;
};
