/**
 ******************************************************************************
 * @file    usbd_dfu_media_template.c
 * @author  MCD Application Team
 * @brief   Memory management layer
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                      www.st.com/SLA0044
 *
 ******************************************************************************
 */

#include "usbd_dfu_media.h"
#include "norflash/qspi_flash_driver.hh"
#include "print.hh"

#define NORFLASH_DESC_STR "@NOR Flash        /0x70080000/999*4Kg"
#define DDRRAM_DESC_STR "@DDR RAM          /0xC2000000/1*16Me"

constexpr uint32_t DDRAppAddrStart = 0xC2000000;
constexpr uint32_t DDRAppSizeBytes = 16 * 1024 * 1024;
constexpr uint32_t DDRAppAddrEnd = DDRAppAddrStart + DDRAppSizeBytes;

constexpr uint32_t NORAppAddrStart = 0x70080000;
constexpr uint32_t NORAppSizeBytes = 15 * 1024 * 1024;
constexpr uint32_t NORAppAddrEnd = NORAppAddrStart + NORAppSizeBytes;
constexpr uint32_t NORMediaOffset = 0x70000000;

constexpr inline uint32_t DDR_PROGRAM_TIME = 5;
constexpr inline uint32_t DDR_ERASE_TIME = 5;

constexpr inline uint32_t NOR_PROGRAM_TIME = 50;
constexpr inline uint32_t NOR_ERASE_TIME = 250;

// TODO: make this a class, but we need to expose the C callbacks using a static instance
using QSpiFlash = mdrivlib::QSpiFlash;
QSpiFlash *flash = nullptr;
void dfu_set_qspi_driver(mdrivlib::QSpiFlash *flash_driver) { flash = flash_driver; }
/////////////////

/**
 * @brief  MEM_If_Init
 *         Memory initialization routine.
 * @param  None
 * @retval 0 if operation is successful, MAL_FAIL else.
 */
static uint16_t MEM_If_Init()
{
	print("Init DFU\n");
	return 0;
}

/**
 * @brief  MEM_If_DeInit
 *         Memory deinitialization routine.
 * @param  None
 * @retval 0 if operation is successful, MAL_FAIL else.
 */
static uint16_t MEM_If_DeInit()
{
	print("DeInit DFU\n");
	return 0;
}

/**
 * @brief  MEM_If_Erase
 *         Erase sector.
 * @param  Add: Address of sector to be erased.
 * @retval 0 if operation is successful, MAL_FAIL else.
 */
static uint16_t MEM_If_Erase(uint32_t addr)
{
	print("Erasing 0x", Hex{addr}, "\n");

	if (addr >= DDRAppAddrStart && addr < DDRAppAddrEnd) {
		uint32_t *mem_ptr = reinterpret_cast<uint32_t *>(DDRAppAddrStart);
		for (uint32_t i = 0; i < DDRAppSizeBytes; i += 4)
			*mem_ptr++ = 0;
		return 0;
	}

	if (addr >= NORAppAddrStart && addr < NORAppAddrEnd) {
		addr -= NORMediaOffset;
		bool ok = flash->erase(QSpiFlash::SECTOR, addr);
		if (!ok)
			print("ERROR: Failed to erase NOR Flash\n");

		return ok ? 0 : 1;
	}

	return 2; // unknown address
}

/**
 * @brief  MEM_If_Write
 *         Memory write routine.
 * @param  Add: Address to be written to.
 * @param  Len: Number of data to be written (in bytes).
 * @retval 0 if operation is successful, MAL_FAIL else.
 */
static uint16_t MEM_If_Write(uint8_t *src, uint8_t *dest, uint32_t len)
{
	print("Writing ", len, " B from ", Hex{(uint32_t)&src}, " to ", Hex{(uint32_t)&dest});

	uint32_t addr = reinterpret_cast<uint32_t>(dest);

	if (addr >= DDRAppAddrStart && addr < DDRAppAddrEnd) {
		while (len--) {
			*dest++ = *src++;
		}
		return 0;
	}

	if (addr >= NORAppAddrStart && addr < NORAppAddrEnd) {
		addr -= NORMediaOffset;
		bool ok = flash->write(src, addr, len);
		if (!ok) {
			print("ERROR: Failed to write to NOR Flash\n");
			return 1;
		}
		print("First word is 0x", Hex{src[0]}, Hex{src[1]}, Hex{src[2]}, Hex{src[3]}, "\n");

		if (len > 1024) {
			print("Warning: can only verify first 1024B\n");
			len = 1024;
		}
		uint8_t check[1024];
		for (int i = 0; i < len; i++)
			check[i] = 0;

		flash->read(check, addr, len);
		for (int i = 0; i < len; i++) {
			if (check[i] != src[i]) {
				USBD_ErrLog("Not verified: ", Hex{check[i]}, " != ", Hex{src[i]}, " at ", dest + i, "\n");
			}
		}
		print("OK\n");
		return 0;
	}

	return 2; // unknown address
}

/**
 * @brief  MEM_If_Read
 *         Memory read routine.
 * @param  Add: Address to be read from.
 * @param  Len: Number of data to be read (in bytes).
 * @retval Pointer to the physical address where data should be read.
 */
static uint8_t *MEM_If_Read(uint8_t *src, uint8_t *dest, uint32_t len)
{
	print("Reading ", len, " B from ", Hex{(uint32_t)&src}, " to ", Hex{(uint32_t)&dest});

	uint32_t addr = reinterpret_cast<uint32_t>(src);

	if (addr >= DDRAppAddrStart && addr < DDRAppAddrEnd) {
		uint8_t *d = dest;
		while (len--) {
			*d++ = *src++;
		}
		/* Return a valid address to avoid HardFault */
		return (uint8_t *)(dest);
	}

	if (addr >= NORAppAddrStart && addr < NORAppAddrEnd) {
		addr -= NORMediaOffset;

		bool ok = flash->read(src, addr, len);
		return (uint8_t *)(dest);
	}

	return (uint8_t *)(DDRAppAddrStart); // unknown address, just return something safe
}

/**
 * @brief  Flash_If_GetStatus
 *         Memory read routine.
 * @param  Add: Address to be read from.
 * @param  cmd: Number of data to be read (in bytes).
 * @retval Pointer to the physical address where data should be read.
 */
static uint16_t MEM_If_GetStatus(uint32_t addr, uint8_t cmd, uint8_t *buffer)
{
	uint32_t prog_time;
	uint32_t erase_time;
	if (addr >= DDRAppAddrStart && addr < DDRAppAddrEnd) {
		prog_time = DDR_PROGRAM_TIME;
		erase_time = DDR_PROGRAM_TIME;
	}
	if (addr >= NORAppAddrStart && addr < NORAppAddrEnd) {
		prog_time = NOR_PROGRAM_TIME;
		erase_time = NOR_PROGRAM_TIME;
	}
	switch (cmd) {
		case DFU_MEDIA_PROGRAM:
			buffer[1] = (uint8_t)prog_time;
			buffer[2] = (uint8_t)(prog_time << 8);
			buffer[3] = 0;
			break;

		case DFU_MEDIA_ERASE:
			buffer[1] = (uint8_t)erase_time;
			buffer[2] = (uint8_t)(erase_time << 8);
			buffer[3] = 0;
			break;

		default:
			break;
	}
	return 0;
}

__ALIGN_BEGIN USBD_DFU_MediaTypeDef USBD_DFU_MEDIA_fops __ALIGN_END = {
	(uint8_t *)DDRRAM_DESC_STR NORFLASH_DESC_STR,
	MEM_If_Init,
	MEM_If_DeInit,
	MEM_If_Erase,
	MEM_If_Write,
	MEM_If_Read,
	MEM_If_GetStatus,
};
