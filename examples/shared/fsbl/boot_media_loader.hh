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

struct AppImageInfo {
	uint32_t load_addr = 0;
	uint32_t entry_point = 0;
	uint32_t size = 0;
};

class BootMediaLoader {

public:
	BootMediaLoader(BootDetect::BootMethod boot_method)
		: _bootmethod{boot_method}
	{
		printf_("Booted from %s (%x)\n", BootDetect::bootmethod_string(boot_method).data(), boot_method);
	}

	AppImageInfo read_app_image_header()
	{
		BootImageDef::image_header header;
		static_assert(sizeof(header) == BootImageDef::HeaderSize);

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

		_parse_header(header);

		return _image_info;
	}

	bool load_image()
	{
		if (!_header_parsed)
			read_app_image_header();

		bool ok;
		switch (_bootmethod) {
			case BootDetect::BOOT_NOR:
				ok = BootNorLoader::load_image(_image_info.load_addr, _image_info.size);
				break;

			case BootDetect::BOOT_SDCARD:
				ok = BootSDLoader::load_image(_image_info.load_addr, _image_info.size);
				break;

			default:
				panic("Unknown boot method");
				break;
		}

		if (!ok)
			panic("Failed reading Boot media\n");

		_image_loaded = true;
		return ok;
	}

	typedef void __attribute__((noreturn)) (*image_entry_noargs_t)(void);
	void boot_image()
	{
		if (!_header_parsed)
			read_app_image_header();

		if (!_image_loaded)
			load_image();

		auto image_entry = reinterpret_cast<image_entry_noargs_t>(_image_info.entry_point);
		debug("image entry point: 0x%lx\n", _image_info.entry_point);
		image_entry();
	}

private:
	BootDetect::BootMethod _bootmethod;
	bool _header_parsed = false;
	bool _image_loaded = false;
	AppImageInfo _image_info;

	void _parse_header(BootImageDef::image_header &header)
	{
		uint32_t magic = be32_to_cpu(header.ih_magic);
		if (magic == BootImageDef::IH_MAGIC) {
			if (header.ih_load == 0) {
				printf_("ih_load is 0\n");
				// On some system (e.g. powerpc), the load-address and
				// entry-point is located at address 0. We can't load
				// to 0-0x40. So skip header in this case.
				_image_info.load_addr = be32_to_cpu(header.ih_load);
				_image_info.entry_point = be32_to_cpu(header.ih_ep);
				_image_info.size = be32_to_cpu(header.ih_size);
			} else {
				constexpr uint32_t header_size = sizeof(BootImageDef::image_header);
				_image_info.entry_point = be32_to_cpu(header.ih_load);
				_image_info.load_addr = _image_info.entry_point - header_size;
				_image_info.size = be32_to_cpu(header.ih_size) + header_size;
			}
			debug("Raw header (big-endian):\n");
			debug("  ih_magic: %x\n", header.ih_magic);
			debug("  ih_hcrc: %x\n", header.ih_hcrc);
			debug("  ih_time: %x\n", header.ih_time);
			debug("  ih_size: %x\n", header.ih_size);
			debug("  ih_load: %x\n", header.ih_load);
			debug("  ih_ep: %x\n", header.ih_ep);
			debug("  ih_dcrc: %x\n", header.ih_dcrc);
			debug("  ih_os: %x\n", header.ih_os);
			debug("  ih_arch: %x\n", header.ih_arch);
			debug("  ih_type: %x\n", header.ih_type);
			debug("  ih_comp: %x\n", header.ih_comp);
			debug("  ih_name: %32s\n", header.ih_name);

			debug("Image load addr: 0x%08x entry_addr: %08x size: %08x\n",
				  _image_info.load_addr,
				  _image_info.entry_point,
				  _image_info.size);

			_header_parsed = true;
		} else
			// TODO: Handle raw images
			panic("Failed to read a valid header: magic was %x, expected %x\n", magic, BootImageDef::IH_MAGIC);
	}
};
