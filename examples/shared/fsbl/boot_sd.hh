#pragma once
#include "boot_image_def.hh"
#include "print_messages.h"
#include "stm32mp1xx_hal_sd.h"
#include <cstdint>
// #include <cstring>

struct BootSDLoader {
	static inline SD_HandleTypeDef hsd;

	static BootImageDef::image_header read_image_header()
	{
		BootImageDef::image_header header;
		hsd.Instance = SDMMC1;
		hsd.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
		hsd.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
		hsd.Init.BusWide = SDMMC_BUS_WIDE_1B;
		hsd.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
		hsd.Init.ClockDiv = SDMMC_INIT_CLK_DIV;

		auto ok = HAL_SD_Init(&hsd);
		if (ok != HAL_OK)
			panic("SDInit not ok");

		uint8_t data[512];
		ok = HAL_SD_ReadBlocks(&hsd, data, BootImageDef::SDCardSSBLBlock, 1, 0xFFFF);
		if (ok != HAL_OK)
			panic("HAL Read SD not ok");

		// auto ok = QSPI_read_SIO((uint8_t *)(&header), BootImageDef::NorFlashSSBLAddr, BootImageDef::HeaderSize);
		// if (!ok)
		// panic("SD not implemented\n");

		// memcpy((void *)&header, data, sizeof(header));
		uint32_t *dst = (uint32_t *)(&header);
		uint32_t *src = (uint32_t *)(&data);
		uint32_t sz = sizeof(header) / 4;
		while (sz--)
			*dst++ = *src++;

		return header;
	}

	static bool load_image(uint32_t load_addr, uint32_t size)
	{
		auto load_dst = reinterpret_cast<uint8_t *>(load_addr);
		// return QSPI_read_SIO(load_dst, BootImageDef::NorFlashSSBLAddr, size);
		return false;
	}
};
