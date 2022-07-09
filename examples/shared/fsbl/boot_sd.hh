#pragma once
#include "boot_image_def.hh"
#include "boot_loader.hh"
#include "drivers/pinconf.hh"
#include "gpt/gpt.hh"
#include "stm32mp1xx_hal_sd.h"
#include <array>

struct BootSDLoader : BootLoader {
	BootSDLoader()
	{
		RCC->SDMMC12CKSELR = 3; // HSI = 64MHz. Default value (just showing it here for educational purposes)

		HAL_SD_DeInit(&hsd);
		hsd.Instance = SDMMC1;
		hsd.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
		hsd.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
		hsd.Init.BusWide = SDMMC_BUS_WIDE_4B;
		hsd.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
		hsd.Init.ClockDiv = 2; // 64MHz/2 / 2 = 16MHz, seems to be the max OSD32-BRK can handle reliably

		// These pins are not board-specific, they are required by BOOTROM
		// for booting with SDMMC1
		// D1 - D3 are not used by BOOTROM, so need to be init by FSBL
		PinConf{GPIO::C, PinNum::_9, PinAF::AF_12}.init(PinMode::Alt);
		PinConf{GPIO::C, PinNum::_10, PinAF::AF_12}.init(PinMode::Alt);
		PinConf{GPIO::C, PinNum::_11, PinAF::AF_12}.init(PinMode::Alt);

		// D0, CK, CMD are used by BOOTROM and should already be init. We re-init them just in case...
		PinConf{GPIO::C, PinNum::_8, PinAF::AF_12}.init(PinMode::Alt);
		PinConf{GPIO::C, PinNum::_12, PinAF::AF_12}.init(PinMode::Alt);
		PinConf{GPIO::D, PinNum::_2, PinAF::AF_12}.init(PinMode::Alt);

		auto ok = HAL_SD_Init(&hsd);
		if (ok != HAL_OK)
			init_error();
	}

	BootImageDef::image_header read_image_header() override
	{
		BootImageDef::image_header header{};

		// TODO: get_next_gpt_header(&gpt_hdr)
		gpt_header gpt_hdr;
		const uint32_t last_block = hsd.SdCard.BlockNbr;
		const uint32_t gpt_addrs[2] = {1, last_block - 1};

		for (auto blockaddr : gpt_addrs) {
			read(gpt_hdr, blockaddr);
			if (validate_gpt_header(&gpt_hdr, blockaddr, last_block)) {

				ssbl_blockaddr = get_gpt_partition_startaddr(gpt_hdr);
				if (ssbl_blockaddr != InvalidPartitionNum)
					break;
			}
		}
		if (ssbl_blockaddr == InvalidPartitionNum) {
			// pr_err("No valid GPT header found\n");
			return {};
		}

		// log("GPT partition header says partition %d is at %llu. Reading\n", ssbl_part_num, ssbl_blockaddr);
		read(header, ssbl_blockaddr);
		return header;
	}

	bool load_image(uint32_t load_addr, uint32_t size) override
	{
		auto load_dst = reinterpret_cast<uint8_t *>(load_addr);
		uint32_t num_blocks = (size + hsd.SdCard.BlockSize - 1) / hsd.SdCard.BlockSize;
		// log("Reading %d blocks starting with block %llu from SD Card\n", num_blocks, ssbl_blockaddr);
		auto err = HAL_SD_ReadBlocks(&hsd, load_dst, ssbl_blockaddr, num_blocks, 0xFFFFFF);
		return (err == HAL_OK);
	}

	bool has_error()
	{
		return _has_error;
	}

private:
	static constexpr uint32_t ssbl_part_num = BootImageDef::SDCardSSBLPartition - 1;
	static constexpr uint32_t InvalidPartitionNum = 0xFFFFFFFF;

	SD_HandleTypeDef hsd;
	uint64_t ssbl_blockaddr = 0;
	bool _has_error = false;

	// Given a gpt_header, find the starting address (LBA) of the SSBL partition
	// Validate the gpt partition entry, too.
	uint64_t get_gpt_partition_startaddr(gpt_header &gpt_hdr)
	{
		std::array<gpt_entry, 4> ptes;

		// Make sure we're loading 512B into a variable that's 512B
		static_assert(sizeof(ptes) == 512, "GPT Entry must be 128 Bytes");
		if (hsd.SdCard.BlockSize != 512)
			sdcard_error();

		uint32_t part_lba = gpt_hdr.partition_entry_lba + (ssbl_part_num / 4);
		read(ptes, part_lba);
		if (validate_partition_entry(ptes[ssbl_part_num % 4])) {
			return ptes[ssbl_part_num % 4].starting_lba;
		}

		return InvalidPartitionNum;
	}

	// Read from SD card into a generic data structure. Max one block (512B)
	void read(auto &data, uint32_t block)
	{
		constexpr uint32_t numblocks = 1;
		constexpr uint32_t timeout = 0xFFFFFF;

		if constexpr (sizeof data == 512) {
			// Size matches block size: read directly into data
			auto err = HAL_SD_ReadBlocks(&hsd, (uint8_t *)&data, block, numblocks, timeout);
			if (err != HAL_OK)
				read_error();
		} else if (sizeof data < 512) {
			uint8_t _data[512];
			auto err = HAL_SD_ReadBlocks(&hsd, _data, block, numblocks, timeout);
			if (err != HAL_OK)
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

	void init_error()
	{
		_has_error = true;
		// 	panic("SDInit not ok");
	}

	void sdcard_error()
	{
		_has_error = true;
		// panic("SD Card block size is not 512!");
	}

	void read_error()
	{
		_has_error = true;
		// panic("HAL Read SD not ok");
	}
};
