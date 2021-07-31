#pragma once
#include "conf/qspi_flash_conf.hh"
#include "math.hh"
#include "qspi_flash_registers.h"
#include "qspi_flash_struct.hh"
#include "stm32xx.h"

namespace mdrivlib
{

class QSpiFlash {

public:
	enum ErasableSizes : uint32_t {
		SECTOR = QSPI_SECTOR_SIZE,
		BLOCK_32K = QSPI_32KBLOCK_SIZE,
		BLOCK_64K = QSPI_64KBLOCK_SIZE,
		ENTIRE_CHIP = QSPI_FLASH_SIZE_BYTES
	};

	enum FlashStatus { STATUS_READY, STATUS_WIP, STATUS_RXING, STATUS_RX_COMPLETE, STATUS_TXING, STATUS_TX_COMPLETE };

	enum UseInterruptFlags { EXECUTE_FOREGROUND, EXECUTE_BACKGROUND };

	// public for use in callbacks and IRQ
	volatile enum FlashStatus QSPI_status = STATUS_READY;

private:
	QSPI_HandleTypeDef handle;
	QSPI_CommandTypeDef s_command;

	HAL_StatusTypeDef WriteEnable(void);
	void GPIO_Init_IO0_IO1(const QSPIFlashConfig &defs);
	void GPIO_Init_IO2_IO3_AF(const QSPIFlashConfig &defs);
	HAL_StatusTypeDef AutoPollingMemReady(uint32_t Timeout);
	HAL_StatusTypeDef AutoPollingMemReady_IT(void);
	HAL_StatusTypeDef EnterMemory_QPI(void);
	void init_command(QSPI_CommandTypeDef *s_command);

	bool done_TXing(void) {
		return QSPI_status == STATUS_TX_COMPLETE;
	}

	uint8_t test_encode_num(uint32_t num);

public:
	QSpiFlash(const QSPIFlashConfig &defs);

	bool is_ready(void) {
		return QSPI_status == STATUS_READY;
	}

	bool Test(void);
	bool Test_Sector(uint8_t sector_num);

	static constexpr uint32_t get_64kblock_addr(uint8_t block64k_num);
	static constexpr uint32_t get_32kblock_addr(uint8_t block32k_num);
	static constexpr uint32_t get_sector_addr(uint8_t sector_num);

	HAL_StatusTypeDef Reset(void);

	bool Read(uint8_t *pData, uint32_t read_addr, uint32_t num_bytes, UseInterruptFlags use_interrupt);
	bool Read_Background(uint8_t *pData, uint32_t read_addr, uint32_t num_bytes) {
		return Read(pData, read_addr, num_bytes, EXECUTE_BACKGROUND);
	}

	bool Write(uint8_t *pData, uint32_t write_addr, uint32_t num_bytes);
	bool Write_Page(uint8_t *pData, uint32_t write_addr, uint32_t num_bytes, UseInterruptFlags use_interrupt);

	bool Erase(uint32_t size, uint32_t BaseAddress, UseInterruptFlags use_interrupt);
	bool Erase_Background(ErasableSizes size, uint32_t BaseAddress) {
		return Erase(size, BaseAddress, EXECUTE_FOREGROUND);
	}
	bool Erase_Block_Background(uint32_t BaseAddress) {
		return Erase(BLOCK_32K, BaseAddress, EXECUTE_BACKGROUND);
	}

	static QSpiFlash *instance_;
};

} // namespace mdrivlib
