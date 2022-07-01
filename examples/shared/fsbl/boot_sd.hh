#pragma once
#include "boot_image_def.hh"
#include "gpt/gpt.hh"
#include "print_messages.h"
#include "stm32mp1xx_hal_sd.h"
#include <array>
#include <cstdint>
#include <optional>
// #include <cstring>

struct BootSDLoader {
	SD_HandleTypeDef hsd;

	BootSDLoader()
	{
		HAL_SD_DeInit(&hsd);
		hsd.Instance = SDMMC1;
		hsd.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
		hsd.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
		hsd.Init.BusWide = SDMMC_BUS_WIDE_1B;
		hsd.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
		hsd.Init.ClockDiv = SDMMC_INIT_CLK_DIV;

		auto ok = HAL_SD_Init(&hsd);
		if (ok != HAL_OK)
			panic("SDInit not ok");
	}

	BootImageDef::image_header read_image_header()
	{
		BootImageDef::image_header header;

		__le64 ssbl_blockaddr = 0;

		// TODO: get_next_gpt_header(&gpt_hdr)

		// get_valid_gpt_header()
		{
			gpt_header gpt_hdr;
			const uint32_t last_block = hsd.SdCard.BlockNbr;
			const uint32_t gpt_addrs[2] = {1, last_block - 1};

			for (auto blockaddr : gpt_addrs) {
				read(gpt_hdr, blockaddr, 1);
				if (validate_gpt_header(&gpt_hdr, blockaddr, last_block)) {
					const uint32_t part_num = BootImageDef::SDCardSSBLPartition - 1;

					// get_gpt_partition_startaddr(ptes[part_num])
					std::array<gpt_entry, 4> ptes;
					// Make sure we're loading 512B into a variable that's 512B
					static_assert(sizeof(ptes) == 512);
					if (hsd.SdCard.BlockSize != 512)
						panic("SD Card block size is not 512!");

					uint32_t part_lba = gpt_hdr.partition_entry_lba + (part_num / 4);
					read(ptes, gpt_hdr.partition_entry_lba, 1);
					if (validate_partition_entry(ptes[part_num])) {
						ssbl_blockaddr = ptes[part_num].starting_lba;
						break;
					}
				}
			}
			if (!ssbl_blockaddr)
				panic("No valid GPT header found\n");
		}

		log("Found 3rd partition at %llu\n", ssbl_blockaddr - 1);

		read(header, ssbl_blockaddr, 1);
		// uint8_t data[512];
		// auto ok = HAL_SD_ReadBlocks(&hsd, data, ssbl_blockaddr, 1, 0xFFFFFF);
		// if (ok != HAL_OK)
		// 	panic("HAL Read SD not ok");

		// // memcpy((void *)&header, data, sizeof(header));
		// uint32_t *dst = (uint32_t *)(&header);
		// uint32_t *src = (uint32_t *)(&data);
		// uint32_t sz = sizeof(header) / 4;
		// while (sz--)
		// 	*dst++ = *src++;

		return header;
	}

	static bool load_image(uint32_t load_addr, uint32_t size)
	{
		auto load_dst = reinterpret_cast<uint8_t *>(load_addr);
		// return QSPI_read_SIO(load_dst, BootImageDef::NorFlashSSBLAddr, size);
		return false;
	}

	void read(auto &data, uint32_t block, uint32_t numblocks)
	{
		log(">>Reading block %d\n", block);
		if constexpr (sizeof data == 512) {
			// Size mathes block size: read directly into data
			auto ok = HAL_SD_ReadBlocks(&hsd, (uint8_t *)&data, block, numblocks, 0xFFFFFF);
			if (ok != HAL_OK)
				read_error();
		} else if (sizeof data < 512) {
			uint8_t _data[512];
			auto ok = HAL_SD_ReadBlocks(&hsd, _data, block, numblocks, 0xFFFFFF);
			if (ok != HAL_OK)
				read_error();

			auto *dst = (uint8_t *)(&data);
			auto *src = (uint8_t *)_data;
			auto sz = sizeof(data);
			while (sz--)
				*dst++ = *src++;
		} else {
			static_assert(sizeof data <= 512, "Multiblock reads not yet supported");
		}
	}

	void read_error()
	{
		panic("HAL Read SD not ok");
	}
};
