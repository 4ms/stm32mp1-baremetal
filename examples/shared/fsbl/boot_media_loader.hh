#pragma once
#include "boot_detect.hh"
#include "boot_image_def.hh"
#include "boot_nor.hh"
#include "boot_sd.hh"
#include "compiler.h"
#include "print_messages.h"

struct AppImageInfo {
	uint32_t load_addr = 0;
	uint32_t entry_point = 0;
	uint32_t size = 0;
};

class BootMediaLoader {

public:
	BootMediaLoader(BootDetect::BootMethod boot_method)
	{
		set_bootmethod(boot_method);
		if (!_loader)
			pr_err("BootMediaLoader(): Unknown boot method\n");
	}

	bool load_image()
	{
		if (!_loader) {
			pr_err("BootMediaLoader::load_image(): Unknown boot method\n");
			return false;
		}

		BootImageDef::image_header header;
		static_assert(sizeof(header) == BootImageDef::HeaderSize);

		header = _loader->read_image_header();

		if (!_parse_header(header)) {
			pr_err("No valid img header found\n");
			return false;
		}

		bool ok = _loader->load_image(_image_info.load_addr, _image_info.size);
		if (!ok) {
			pr_err("Failed reading boot media when loading app img\n");
			return false;
		}

		_image_loaded = true;
		return true;
	}

	typedef void __attribute__((noreturn)) (*image_entry_noargs_t)(void);
	void boot_image()
	{
		if (!_image_loaded) {
			if (!load_image()) {
				pr_err("Failed to jump to app because of error loading image\n");
				return;
			}
		}

		auto image_entry = reinterpret_cast<image_entry_noargs_t>(_image_info.entry_point);
		log("image entry point: 0x%08x\n", _image_info.entry_point);
		image_entry();
	}

	// You may call this to change boot methods. For example
	// if load_image() fails, you can try a different boot method
	bool set_bootmethod(BootDetect::BootMethod new_boot_method)
	{
		_loader = _get_boot_loader(new_boot_method);
		return (_loader != nullptr);
	}

private:
	bool _image_loaded = false;
	AppImageInfo _image_info;
	// We don't have dynamic memory, so instead of having a static copy of each
	// type of loader we use placement new.
	uint8_t loader_storage[std::max(sizeof(BootSDLoader), sizeof(BootNorLoader))];
	BootLoader *_loader;

	// To support a new boot media (such as NAND Flash),
	// associate its class to the enum value BOOTROM uses for that media:
	BootLoader *_get_boot_loader(BootDetect::BootMethod bootmethod)
	{
		return bootmethod == BootDetect::BOOT_NOR	 ? new (loader_storage) BootNorLoader :
			   bootmethod == BootDetect::BOOT_SDCARD ? new (loader_storage) BootSDLoader :
														 static_cast<BootLoader *>(nullptr);
	}

	bool _parse_header(BootImageDef::image_header &header)
	{
		log("Raw header (big-endian):\n");
		log("  ih_magic: %x\n", header.ih_magic);
		log("  ih_hcrc: %x\n", header.ih_hcrc);
		log("  ih_time: %x\n", header.ih_time);
		log("  ih_size: %x\n", header.ih_size);
		log("  ih_load: %x\n", header.ih_load);
		log("  ih_ep: %x\n", header.ih_ep);
		log("  ih_dcrc: %x\n", header.ih_dcrc);
		log("  ih_os: %x\n", header.ih_os);
		log("  ih_arch: %x\n", header.ih_arch);
		log("  ih_type: %x\n", header.ih_type);
		log("  ih_comp: %x\n", header.ih_comp);
		log("  ih_name: %32s\n", header.ih_name);

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

			log("Image load addr: 0x%08x entry_addr: %08x size: %08x\n",
				_image_info.load_addr,
				_image_info.entry_point,
				_image_info.size);

			return true;

		} else
			// TODO: Handle raw images
			pr_err("Failed to read a valid header: magic was %x, expected %x\n", magic, BootImageDef::IH_MAGIC);

		return false;
	}
};
