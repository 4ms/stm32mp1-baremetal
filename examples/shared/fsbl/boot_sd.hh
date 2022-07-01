#pragma once
#include "boot_image_def.hh"
#include "boot_loader.hh"
#include "gpt/gpt.hh"
#include "print_messages.h"
#include "stm32mp1xx_hal_sd.h"
#include <array>
#include <optional>

struct BootSDLoader : BootLoader {

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

	BootImageDef::image_header read_image_header() override
	{
		BootImageDef::image_header header;

		// TODO: get_next_gpt_header(&gpt_hdr)
		// get_valid_gpt_header()
		{
			gpt_header gpt_hdr;
			const uint32_t last_block = hsd.SdCard.BlockNbr;
			const uint32_t gpt_addrs[2] = {1, last_block - 1};

			for (auto blockaddr : gpt_addrs) {
				read(gpt_hdr, blockaddr);
				if (validate_gpt_header(&gpt_hdr, blockaddr, last_block)) {
					const uint32_t part_num = BootImageDef::SDCardSSBLPartition - 1;

					// get_gpt_partition_startaddr(ptes[part_num])
					std::array<gpt_entry, 4> ptes;
					// Make sure we're loading 512B into a variable that's 512B
					static_assert(sizeof(ptes) == 512);
					if (hsd.SdCard.BlockSize != 512)
						panic("SD Card block size is not 512!");

					uint32_t part_lba = gpt_hdr.partition_entry_lba + (part_num / 4);
					read(ptes, gpt_hdr.partition_entry_lba);
					if (validate_partition_entry(ptes[part_num])) {
						ssbl_blockaddr = ptes[part_num].starting_lba;
						break;
					}
				}
			}
			if (!ssbl_blockaddr)
				panic("No valid GPT header found\n");
		}
		log("GPT partition header says 3rd partition is at %llu. Reading\n", ssbl_blockaddr);
		read(header, ssbl_blockaddr);
		return header;
	}

	bool load_image(uint32_t load_addr, uint32_t size) override
	{
		auto load_dst = reinterpret_cast<uint8_t *>(load_addr);
		auto err = HAL_SD_ReadBlocks(&hsd, load_dst, ssbl_blockaddr, size / hsd.SdCard.BlockSize, 0xFFFFFF);
		return (err == HAL_OK);
	}

private:
	SD_HandleTypeDef hsd;
	uint64_t ssbl_blockaddr = 0;

	void read(auto &data, uint32_t block)
	{
		constexpr uint32_t numblocks = 1;
		constexpr uint32_t timeout = 0xFFFFFF;

		log(">>Reading block %d\n", block);
		if constexpr (sizeof data == 512) {
			// Size mathes block size: read directly into data
			auto ok = HAL_SD_ReadBlocks(&hsd, (uint8_t *)&data, block, numblocks, timeout);
			if (ok != HAL_OK)
				read_error();
		} else if (sizeof data < 512) {
			uint8_t _data[512];
			auto ok = HAL_SD_ReadBlocks(&hsd, _data, block, numblocks, timeout);
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
