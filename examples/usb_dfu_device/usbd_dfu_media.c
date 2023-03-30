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

// @: To detect that this is a special mapping descriptor (to avoid decoding standard descriptor)
// ● /: for separator between zones
// ● Maximum 8 digits per address starting by “0x”
// ● /: for separator between zones
// ● Maximum of 2 digits for the number of sectors
// ● *: For separator between number of sectors and sector size
// ● Maximum 3 digits for sector size between 0 and 999
// ● 1 digit for the sector size multiplier. Valid entries are: B (byte), K (Kilo), M (Mega)
// ● 1 digit for the sector type as follows:
// – a (0x41): Readable
// – b (0x42): Erasable
// – c (0x43): Readable and Erasabled
// - d (0x44): Writeable
// – e (0x45): Readable and Writeable
// – f (0x46): Erasable and Writeable
// – g (0x47): Readable, Erasable and Writeable

// #define FLASH_DESC_STR      "@Internal Flash   /0x08000000/1*128Ka,7*128Kg"
#define FLASH_DESC_STR "@DDR RAM          /0xC2000000/1*512Kg"
const uint32_t AppAddrStart = 0xC2000000;

#define FLASH_ERASE_TIME (uint16_t)50
#define FLASH_PROGRAM_TIME (uint16_t)50

uint16_t MEM_If_Init(void);
uint16_t MEM_If_Erase(uint32_t Add);
uint16_t MEM_If_Write(uint8_t *src, uint8_t *dest, uint32_t Len);
uint8_t *MEM_If_Read(uint8_t *src, uint8_t *dest, uint32_t Len);
uint16_t MEM_If_DeInit(void);
uint16_t MEM_If_GetStatus(uint32_t Add, uint8_t Cmd, uint8_t *buffer);

USBD_DFU_MediaTypeDef USBD_DFU_MEDIA_fops = {
	(uint8_t *)"DFU MEDIA",
	MEM_If_Init,
	MEM_If_DeInit,
	MEM_If_Erase,
	MEM_If_Write,
	MEM_If_Read,
	MEM_If_GetStatus,

};
/**
 * @brief  MEM_If_Init
 *         Memory initialization routine.
 * @param  None
 * @retval 0 if operation is successful, MAL_FAIL else.
 */
uint16_t MEM_If_Init(void) { return 0; }

/**
 * @brief  MEM_If_DeInit
 *         Memory deinitialization routine.
 * @param  None
 * @retval 0 if operation is successful, MAL_FAIL else.
 */
uint16_t MEM_If_DeInit(void) { return 0; }

/**
 * @brief  MEM_If_Erase
 *         Erase sector.
 * @param  Add: Address of sector to be erased.
 * @retval 0 if operation is successful, MAL_FAIL else.
 */
uint16_t MEM_If_Erase(uint32_t Add) { return 0; }

/**
 * @brief  MEM_If_Write
 *         Memory write routine.
 * @param  Add: Address to be written to.
 * @param  Len: Number of data to be written (in bytes).
 * @retval 0 if operation is successful, MAL_FAIL else.
 */
uint16_t MEM_If_Write(uint8_t *src, uint8_t *dest, uint32_t Len) { return 0; }

/**
 * @brief  MEM_If_Read
 *         Memory read routine.
 * @param  Add: Address to be read from.
 * @param  Len: Number of data to be read (in bytes).
 * @retval Pointer to the physical address where data should be read.
 */
uint8_t *MEM_If_Read(uint8_t *src, uint8_t *dest, uint32_t Len)
{
	/* Return a valid address to avoid HardFault */
	return (uint8_t *)(0);
}

/**
 * @brief  Flash_If_GetStatus
 *         Memory read routine.
 * @param  Add: Address to be read from.
 * @param  cmd: Number of data to be read (in bytes).
 * @retval Pointer to the physical address where data should be read.
 */
uint16_t MEM_If_GetStatus(uint32_t Add, uint8_t Cmd, uint8_t *buffer)
{
	switch (Cmd) {
		case DFU_MEDIA_PROGRAM:

			break;

		case DFU_MEDIA_ERASE:
		default:

			break;
	}
	return (0);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
