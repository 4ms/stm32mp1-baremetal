// spl_parse_image_header() turnsinto this:
// void spl_set_header_raw_uboot(struct spl_image_info *spl_image)
// {
// 	ulong u_boot_pos = binman_sym(ulong, u_boot_any, image_pos);

// 	spl_image->size = CONFIG_SYS_MONITOR_LEN;

// 	/*
// 	 * Binman error cases: address of the end of the previous region or the
// 	 * start of the image's entry area (usually 0) if there is no previous
// 	 * region.
// 	 */
// 	if (u_boot_pos && u_boot_pos != BINMAN_SYM_MISSING) {
// 		/* Binman does not support separated entry addresses */
// 		spl_image->entry_point = u_boot_pos;
// 		spl_image->load_addr = u_boot_pos;
// 	} else {
// 		spl_image->entry_point = CONFIG_SYS_UBOOT_START;
// 		spl_image->load_addr = CONFIG_SYS_TEXT_BASE;
// 	}
// 	spl_image->os = IH_OS_U_BOOT;
// 	spl_image->name = "U-Boot";
// }
// spl_nor_get_uboot_base() returns CONFIG_SYS_UBOOT_BASE

// static int spl_nor_load_image(struct spl_image_info *spl_image, struct spl_boot_device *bootdev)
// {
// 	ret = spl_parse_image_header(spl_image, (const struct image_header *)spl_nor_get_uboot_base());
// 	if (ret)
// 		return ret;

// 	memcpy((void *)(unsigned long)spl_image->load_addr,
// 	       (void *)(spl_nor_get_uboot_base() + sizeof(struct image_header)),
// 	       spl_image->size);

// 	return 0;
// }

#pragma once
#include "image_def.hh"
#include "norflash/qspi_nor.h"
#include "print_messages.h"
#include <cstdint>
#include <span>

struct BootNorLoader {
	using image_header = BootImageDef::image_header;
	using image_info = BootImageDef::image_info;
	using spl_image_info = BootImageDef::spl_image_info;

	static bool read_header(std::span<uint8_t, 256> data)
	{
		return QSPI_read(data.data(), BootImageDef::NorFlashSSBLAddr, BootImageDef::HeaderSize);
	}

	static bool parse_header(spl_image_info &spl_image)
	{
		image_header header;
		pr_err("read to %p\n", &header);
		QSPI_read((uint8_t *)(&header), BootImageDef::NorFlashSSBLAddr, BootImageDef::HeaderSize);
		pr_err("done read %x\n", header.ih_magic);
		// Wrong endian!
		if (header.ih_magic == BootImageDef::IH_MAGIC) {
			uint32_t header_size = sizeof(image_header);

			if (spl_image.flags & BootImageDef::SPL_COPY_PAYLOAD_ONLY) {
				/*
				 * On some system (e.g. powerpc), the load-address and
				 * entry-point is located at address 0. We can't load
				 * to 0-0x40. So skip header in this case.
				 */
				spl_image.load_addr = header.ih_load;
				spl_image.entry_point = header.ih_ep;
				spl_image.size = header.ih_size;
			} else {
				spl_image.entry_point = header.ih_load;
				/* Load including the header */
				spl_image.load_addr = spl_image.entry_point - header_size;
				spl_image.size = header.ih_size + header_size;
			}

			spl_image.os = header.ih_os;
			spl_image.name = (char *)header.ih_name;
			debug(
				"payload image: %32s load addr: 0x%lx size: %d\n", spl_image.name, spl_image.load_addr, spl_image.size);
			return true;
		};

		// Handle other image types?
		return false;
	};
};
