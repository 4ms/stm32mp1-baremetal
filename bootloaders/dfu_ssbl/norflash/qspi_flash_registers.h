#pragma once

// S25FL127/128: 16MByte
// IS25LQ040B: 512kBytes
// IS25LQ020B: 256kBytes

#define QSPI_64KBLOCK_SIZE 0x10000 // 64 KByte Blocks
#define QSPI_32KBLOCK_SIZE 0x8000  // 32 KByte Half-blocks
#define QSPI_SECTOR_SIZE 0x1000	   // 4 KBytes sectors
#define QSPI_PAGE_SIZE 0x100	   // 256 Byte pages
#define QSPI_PAGE_ADDRESS_BITS 8   // 8 bits = 256 addresses per page

#define QSPI_DUMMY_CYCLES_READ 0
#define QSPI_DUMMY_CYCLES_FAST_READ 8
#define QSPI_DUMMY_CYCLES_READ_QUAD 8

//TODO: have a struct for each chip type with its commands and constants
#define IS25LQ0x0B_QSPI_DUMMY_CYCLES_READ_QUAD_IO 4
#define S25FLxxxL_QSPI_DUMMY_CYCLES_READ_QUAD_IO 8
#define W25Q128JV_QSPI_DUMMY_CYCLES_READ_QUAD_IO 4

#define QSPI_CHIP_ERASE_MAX_TIME_SYSTICKS 200000
#define QSPI_64KBLOCK_ERASE_MAX_TIME_SYSTICKS 2000
#define QSPI_32KBLOCK_ERASE_MAX_TIME_SYSTICKS 500
#define QSPI_SECTOR_ERASE_MAX_TIME_SYSTICKS 300

/**
 * @brief  QSPI Commands
 */
/* Reset Operations */
#define RESET_ENABLE_CMD 0x66
#define RESET_CMD 0x99
#define DEEP_POWER_DOWN_CMD 0xB9

/* Identification Operations */
#define READ_ID_CMD 0x9F
#define READ_MANUF_RELEASE_DP 0xAB
#define READ_UID 0x4B
#define READ_SERIAL_FLASH_DISCO_PARAM_CMD 0x5A

/* Read Operations */
#define READ_CMD 0x03
#define FAST_READ_CMD 0x0B
#define DUAL_OUT_FAST_READ_CMD 0x3B
#define DUAL_INOUT_FAST_READ_CMD 0xBB
#define QUAD_OUT_FAST_READ_CMD 0x6B
#define QUAD_INOUT_FAST_READ_CMD 0xEB

/* Register Operations */
#define READ_STATUS_REG_CMD 0x05
#define WRITE_STATUS_REG_CMD 0x01
#define READ_FUNCTION_REG_CMD 0x48
#define WRITE_FUNCTION_REG_CMD 0x42
#define READ_INFO_ROW 0x62
#define PROG_INFO_ROW 0x68
#define SECTOR_UNLOCK 0x26
#define SECTOR_LOCK 0x24
#define WRITE_ENABLE_CMD 0x06

/* Program Operations */
#define PAGE_PROG_CMD 0x02

#define IS25LQ0x0B_QUAD_IN_FAST_PROG_CMD 0x38
#define S25FLxxxL_QUAD_IN_FAST_PROG_CMD 0x32
#define W25Q128JV_QUAD_IN_FAST_PROG_CMD 0x32

/* Erase Operations */
#define SECTOR_ERASE_CMD 0x20

#define BLOCK_ERASE_32K_CMD 0x52
#define BLOCK_ERASE_64K_CMD 0xD8
#define BULK_ERASE_CMD 0xC7
#define PROG_ERASE_RESUME_CMD 0x30
#define PROG_ERASE_SUSPEND_CMD 0xB0

/* Status Register */
#define QSPI_SR_WIP ((uint8_t)0x01)		 /*!< Write in progress */
#define QSPI_SR_WREN ((uint8_t)0x02)	 /*!< Write enable latch */
#define QSPI_SR_BLOCKPR ((uint8_t)0x5C)	 /*!< Block protected against program and erase operations */
#define QSPI_SR_PRBOTTOM ((uint8_t)0x20) /*!< Protected memory area defined by BLOCKPR starts from top or bottom */
#define QSPI_SR_QUADEN ((uint8_t)0x40)	 /*!< Quad IO mode enabled if =1 */
#define QSPI_SR_SRWREN ((uint8_t)0x80)	 /*!< Status register write enable/disable */
