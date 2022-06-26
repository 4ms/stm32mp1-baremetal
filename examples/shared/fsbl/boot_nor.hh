#pragma once
#include "compiler.h"
#include "image_def.hh"
#include "norflash/qspi_norflash_read.h"
#include "print_messages.h"
#include <cstdint>
#include <optional>
#include <span>

class BootNorLoader {
	BootImageDef::spl_image_info spl_image;
	bool valid_header_found = false;

public:
	bool read_image_info()
	{
		BootImageDef::image_header header;

		auto ok = QSPI_read_SIO((uint8_t *)(&header), BootImageDef::NorFlashSSBLAddr, BootImageDef::HeaderSize);
		if (!ok)
			panic("Failed reading NOR Flash\n");

		uint32_t magic = be32_to_cpu(header.ih_magic);
		if (magic == BootImageDef::IH_MAGIC) {
			uint32_t header_size = sizeof(BootImageDef::image_header);

			if (spl_image.flags & BootImageDef::SPL_COPY_PAYLOAD_ONLY) {
				// On some system (e.g. powerpc), the load-address and
				// entry-point is located at address 0. We can't load
				// to 0-0x40. So skip header in this case.
				spl_image.load_addr = be32_to_cpu(header.ih_load);
				spl_image.entry_point = be32_to_cpu(header.ih_ep);
				spl_image.size = be32_to_cpu(header.ih_size);
			} else {
				spl_image.entry_point = be32_to_cpu(header.ih_load);
				// Load including the header
				spl_image.load_addr = spl_image.entry_point - header_size;
				spl_image.size = be32_to_cpu(header.ih_size) + header_size;
			}

			spl_image.os = be32_to_cpu(header.ih_os);
			spl_image.name = (char *)header.ih_name;
			debug(
				"payload image: %32s load addr: 0x%lx size: %d\n", spl_image.name, spl_image.load_addr, spl_image.size);

			// TODO: check values are sane
			valid_header_found = true;
		};

		// TODO: Handle other image types

		panic("Failed to read a valid header: magic was %x, expected %x\n", magic, BootImageDef::IH_MAGIC);

		return valid_header_found;
	}

	bool is_valid_header() const
	{
		return valid_header_found;
	}

	bool load_image()
	{
		if (!valid_header_found)
			return false;

		auto load_dst = reinterpret_cast<uint8_t *>(spl_image.load_addr);
		auto ok = QSPI_read_SIO(load_dst, BootImageDef::NorFlashSSBLAddr, spl_image.size);
		if (!ok)
			panic("Failed reading NOR Flash\n");

		return true;
	}

	typedef void __attribute__((noreturn)) (*image_entry_noargs_t)(void);
	void boot_image() const
	{
		if (!valid_header_found)
			return;

		uint32_t entry_addr = spl_image.entry_point;
		image_entry_noargs_t image_entry = (image_entry_noargs_t)spl_image.entry_point;
		debug("image entry point: 0x%lx\n", spl_image->entry_point);
		image_entry();
	}

	///////////////
	// Debugging:

	static bool raw_read_header(std::span<uint8_t, BootImageDef::HeaderSize> data)
	{
		return QSPI_read_SIO(data.data(), BootImageDef::NorFlashSSBLAddr, BootImageDef::HeaderSize);
	}

	std::optional<const BootImageDef::spl_image_info> get_image_info() const
	{
		if (valid_header_found)
			return spl_image;
		else
			return std::nullopt;
	}
};
