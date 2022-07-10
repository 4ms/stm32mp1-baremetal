#pragma once
#include "boot_image_def.hh"

struct BootLoader {
	virtual BootImageDef::image_header read_image_header() = 0;
	virtual bool load_image(uint32_t load_addr, uint32_t size) = 0;
};
