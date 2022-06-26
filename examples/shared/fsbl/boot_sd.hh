#pragma once
#include "boot_image_def.hh"
// #include "norflash/qspi_norflash_read.h"
#include "print_messages.h"
#include <cstdint>

namespace BootSDLoader
{
inline BootImageDef::image_header read_image_header()
{
	BootImageDef::image_header header;
	// auto ok = QSPI_read_SIO((uint8_t *)(&header), BootImageDef::NorFlashSSBLAddr, BootImageDef::HeaderSize);
	// if (!ok)
	panic("SD not implemented\n");
	// return header;
}

inline bool load_image(uint32_t load_addr, uint32_t size)
{
	auto load_dst = reinterpret_cast<uint8_t *>(load_addr);
	// return QSPI_read_SIO(load_dst, BootImageDef::NorFlashSSBLAddr, size);
	return false;
}

}; // namespace BootSDLoader
