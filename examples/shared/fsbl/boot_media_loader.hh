#pragma once
#include "boot_detect.hh"
#include "boot_image_def.hh"
#include "boot_nor.hh"
#include "boot_sd.hh"
#include "compiler.h"
#include "print_messages.h"
#include <cstdint>
#include <optional>
#include <span>

struct MemArea {
	uint32_t start = 0;
	uint32_t end = 0;
};

class BootMediaLoader {

public:
	BootMediaLoader(BootDetect::BootMethod boot_method)
		: _bootmethod{boot_method}
	{
		printf_("Booted from %s (%x)\n", BootDetect::bootmethod_string(boot_method).data(), boot_method);
	}

	MemArea read_app_image_header()
	{
		BootImageDef::image_header header;

		switch (_bootmethod) {
			case BootDetect::BOOT_NOR:
				header = BootNorLoader::read_image_header();
				break;

			case BootDetect::BOOT_SDCARD:
				header = BootSDLoader::read_image_header();
				break;

			default:
				panic("Unknown boot method");
				break;
		}
		return _parse_header(header);
	}

	bool load_image()
	{
		if (!header_parsed)
			read_app_image_header();

		switch (_bootmethod) {
			case BootDetect::BOOT_NOR: {
				auto ok = BootNorLoader::load_image(_spl_image.load_addr, _spl_image.size);
				if (!ok)
					panic("Failed reading NOR Flash\n");
				return ok;
				break;
			}

			case BootDetect::BOOT_SDCARD: {
				auto ok = BootSDLoader::load_image(_spl_image.load_addr, _spl_image.size);
				if (!ok)
					panic("Failed reading SD Card\n");
				return ok;
				break;
			}

			default:
				panic("Unknown boot method");
				return false;
				break;
		}
	}

	typedef void __attribute__((noreturn)) (*image_entry_noargs_t)(void);
	void boot_image()
	{
		if (!header_parsed)
			read_app_image_header();

		uint32_t entry_addr = _spl_image.entry_point;
		auto image_entry = reinterpret_cast<image_entry_noargs_t>(_spl_image.entry_point);
		debug("image entry point: 0x%lx\n", _spl_image->entry_point);
		image_entry();
	}

private:
	BootDetect::BootMethod _bootmethod;
	BootImageDef::spl_image_info _spl_image;
	bool header_parsed = false;

	MemArea _parse_header(BootImageDef::image_header &header)
	{
		uint32_t magic = be32_to_cpu(header.ih_magic);
		if (magic == BootImageDef::IH_MAGIC) {
			uint32_t header_size = sizeof(BootImageDef::image_header);

			if (_spl_image.flags & BootImageDef::SPL_COPY_PAYLOAD_ONLY) {
				// On some system (e.g. powerpc), the load-address and
				// entry-point is located at address 0. We can't load
				// to 0-0x40. So skip header in this case.
				_spl_image.load_addr = be32_to_cpu(header.ih_load);
				_spl_image.entry_point = be32_to_cpu(header.ih_ep);
				_spl_image.size = be32_to_cpu(header.ih_size);
			} else {
				_spl_image.entry_point = be32_to_cpu(header.ih_load);
				// Load including the header
				_spl_image.load_addr = _spl_image.entry_point - header_size;
				_spl_image.size = be32_to_cpu(header.ih_size) + header_size;
			}

			_spl_image.os = be32_to_cpu(header.ih_os);
			_spl_image.name = (char *)header.ih_name;
			debug(
				"payload image: %32s load addr: 0x%lx size: %d\n", spl_image.name, spl_image.load_addr, spl_image.size);

			header_parsed = true;
			return {_spl_image.load_addr, _spl_image.size};
		};
		// TODO: Handle raw images

		panic("Failed to read a valid header: magic was %x, expected %x\n", magic, BootImageDef::IH_MAGIC);
		return {0, 0};
	}
};
