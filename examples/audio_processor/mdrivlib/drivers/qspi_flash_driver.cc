#include "qspi_flash_driver.hh"
#include "interrupt.hh"
#include "qspi_flash_registers.h"

// #define QSPI_DO_TESTS

namespace mdrivlib
{

constexpr uint32_t QSpiFlash::get_64kblock_addr(uint8_t block64k_num) {
	if (block64k_num >= QSPI_NUM_64KBLOCKS)
		return 0;

	return block64k_num * QSPI_64KBLOCK_SIZE;
}

constexpr uint32_t QSpiFlash::get_32kblock_addr(uint8_t block32k_num) {
	if (block32k_num >= QSPI_NUM_32KBLOCKS)
		return 0;

	return block32k_num * QSPI_32KBLOCK_SIZE;
}

constexpr uint32_t QSpiFlash::get_sector_addr(uint8_t sector_num) {
	if (sector_num >= QSPI_NUM_SECTORS)
		return 0;

	return sector_num * QSPI_SECTOR_SIZE;
}

QSpiFlash::QSpiFlash(const QSPIFlashConfig &defs) {
	instance_ = this;

	handle.Instance = QUADSPI;

	HAL_QSPI_DeInit(&handle);

	// FixMe: Why isn't this unlocked by HAL?
	handle.Lock = HAL_UNLOCKED;

	__HAL_RCC_QSPI_CLK_ENABLE();
	__HAL_RCC_QSPI_FORCE_RESET();
	__HAL_RCC_QSPI_RELEASE_RESET();

	// Initialize chip pins in single IO mode
	GPIO_Init_IO0_IO1(defs);

	HAL_NVIC_SetPriority(QUADSPI_IRQn, defs.IRQ_pri, defs.IRQ_subpri);
	HAL_NVIC_EnableIRQ(QUADSPI_IRQn);
	InterruptManager::registerISR(QUADSPI_IRQn, [hal_handle_ptr = &handle]() {
		// Todo: use our own handler, so can get rid of the status instance_ and extern "C" functions
		HAL_QSPI_IRQHandler(hal_handle_ptr);
	});

	handle.Init.ClockPrescaler = defs.clock_division;
	handle.Init.FifoThreshold = 1;
	handle.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
	handle.Init.FlashSize = QSPI_FLASH_SIZE_ADDRESSBITS - 1;
	handle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
	handle.Init.ClockMode = QSPI_CLOCK_MODE_0;
	handle.Init.FlashID = QSPI_FLASH_ID_1;
	handle.Init.DualFlash = QSPI_DUALFLASH_DISABLE;

	HAL_QSPI_Init(&handle);

	init_command(&s_command);

	QSPI_status = STATUS_READY;

	Reset();
	EnterMemory_QPI();

	// Now that chip is in QSPI mode, IO2 and IO3 can be initialized
	GPIO_Init_IO2_IO3_AF(defs);

#ifdef QSPI_DO_TESTS
	// Erase(ENTIRE_CHIP, 0, EXECUTE_FOREGROUND);
	if (!Test()) {
		asm("nop");
	}
#endif
}

void QSpiFlash::GPIO_Init_IO0_IO1(const QSPIFlashConfig &defs) {
	Pin cs{defs.cs.gpio, defs.cs.pin, PinMode::Alt, defs.cs.af, PinPull::Up};
	Pin clk{defs.clk.gpio, defs.clk.pin, PinMode::Alt, defs.clk.af, PinPull::None};
	Pin io0{defs.io0.gpio, defs.io0.pin, PinMode::Alt, defs.io0.af, PinPull::None};
	Pin io1{defs.io1.gpio, defs.io1.pin, PinMode::Alt, defs.io1.af, PinPull::None};
	Pin wp{defs.io2.gpio, defs.io2.pin, PinMode::Output, PinAF::AFNone, PinPull::None};
	Pin hold{defs.io3.gpio, defs.io3.pin, PinMode::Output, PinAF::AFNone, PinPull::None};
	// Set /HOLD and /WP pins high to disable holding and write protection, until we enter QPI mode
	hold.high();
	wp.high();
}

void QSpiFlash::GPIO_Init_IO2_IO3_AF(const QSPIFlashConfig &defs) {
	Pin io2{defs.io2.gpio, defs.io2.pin, PinMode::Alt, defs.io2.af, PinPull::None};
	Pin io3{defs.io3.gpio, defs.io3.pin, PinMode::Alt, defs.io3.af, PinPull::None};
}

void QSpiFlash::init_command(QSPI_CommandTypeDef *s_command) {
	s_command->InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command->AddressSize = QSPI_ADDRESS_24_BITS;
	s_command->DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command->DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command->SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
}

HAL_StatusTypeDef QSpiFlash::Reset(void) {
	// Enable Reset
	s_command.Instruction = RESET_ENABLE_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0;

	if (HAL_QSPI_Command(&handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return HAL_ERROR;

	// Perform Reset
	s_command.Instruction = RESET_CMD;

	if (HAL_QSPI_Command(&handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return HAL_ERROR;

	return HAL_OK;
}

// Tests entire chip sector-by-sector
// Returns 1 if passed, 0 if failed
bool QSpiFlash::Test() {
	uint8_t sector;
	for (sector = 0; sector < QSPI_NUM_SECTORS; sector++) {
		if (!Test_Sector(sector))
			return false; // fail
	}
	return true; // pass
}

// Tests one sector
// Returns true if passed, false if failed
bool QSpiFlash::Test_Sector(uint8_t sector_num) {
	uint32_t i;
	uint8_t test_buffer[QSPI_SECTOR_SIZE];
	uint32_t test_addr = get_sector_addr(sector_num);

	for (i = 0; i < QSPI_SECTOR_SIZE; i++)
		test_buffer[i] = (test_encode_num(i) + sector_num) & 0xFF;

	while (!is_ready())
		;

	// Benchmark: ~38ms/sector
	if (!Erase(SECTOR, test_addr, EXECUTE_BACKGROUND))
		return false;

	while (!is_ready())
		;

	for (i = 0; i < (QSPI_SECTOR_SIZE / QSPI_PAGE_SIZE); i++) {
		// Benchmark: ~380us/page
		if (!Write_Page(
				&(test_buffer[i * QSPI_PAGE_SIZE]), test_addr + i * QSPI_PAGE_SIZE, QSPI_PAGE_SIZE, EXECUTE_BACKGROUND))
			return false;
		while (!is_ready())
			;
	}

	for (i = 0; i < QSPI_SECTOR_SIZE; i++)
		test_buffer[i] = 0;

	// Benchmark: ~680-850us/sector
	if (!Read(test_buffer, test_addr, QSPI_SECTOR_SIZE, EXECUTE_BACKGROUND))
		return false;

	while (!is_ready())
		;

	for (i = 0; i < (QSPI_SECTOR_SIZE - 1); i++) {
		if (test_buffer[i] != ((test_encode_num(i) + sector_num) & 0xFF))
			return false;
	}

	return true;
}

uint8_t QSpiFlash::test_encode_num(uint32_t num) {
	return (num * 7) + (num >> 7);
}

bool QSpiFlash::Erase(uint32_t size, uint32_t BaseAddress, UseInterruptFlags use_interrupt) {
	uint8_t status;
	uint32_t timeout;

	if (WriteEnable() != HAL_OK)
		return false;

	if (size == SECTOR) {
		s_command.Instruction = SECTOR_ERASE_CMD;
		s_command.Address = BaseAddress;
		s_command.AddressMode = QSPI_ADDRESS_1_LINE;
		timeout = QSPI_SECTOR_ERASE_MAX_TIME_SYSTICKS;
	} else if (size == BLOCK_32K) {
		s_command.Instruction = BLOCK_ERASE_32K_CMD;
		s_command.Address = BaseAddress;
		s_command.AddressMode = QSPI_ADDRESS_1_LINE;
		timeout = QSPI_32KBLOCK_ERASE_MAX_TIME_SYSTICKS;
	} else if (size == BLOCK_64K) {
		s_command.Instruction = BLOCK_ERASE_64K_CMD;
		s_command.Address = BaseAddress;
		s_command.AddressMode = QSPI_ADDRESS_1_LINE;
		timeout = QSPI_64KBLOCK_ERASE_MAX_TIME_SYSTICKS;
	} else if (size == ENTIRE_CHIP) {
		s_command.Instruction = BULK_ERASE_CMD;
		s_command.Address = QSPI_ADDRESS_NONE;
		s_command.AddressMode = QSPI_ADDRESS_NONE;
		timeout = QSPI_CHIP_ERASE_MAX_TIME_SYSTICKS;
	} else
		return false; // invalid size

	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0;

	if (HAL_QSPI_Command(&handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return false;

	if (use_interrupt == EXECUTE_BACKGROUND)
		status = AutoPollingMemReady_IT();
	else
		status = AutoPollingMemReady(timeout);

	if (status != HAL_OK)
		return false;

	return true;
}

bool QSpiFlash::Write(uint8_t *pData, uint32_t write_addr, uint32_t num_bytes) {
	uint32_t end_addr, current_size, current_addr;

	if (write_addr >= QSPI_FLASH_SIZE_BYTES)
		return false;

	if (write_addr + num_bytes >= QSPI_FLASH_SIZE_BYTES)
		return false;

	// Calculation of the size between the write address and the end of the page
	current_addr = 0;

	while (current_addr <= write_addr)
		current_addr += QSPI_PAGE_SIZE;

	current_size = current_addr - write_addr;

	// Check if the size of the data is less than the remaining in the page
	if (current_size > num_bytes)
		current_size = num_bytes;

	current_addr = write_addr;
	end_addr = write_addr + num_bytes;

	// Perform the write page by page
	do {
		if (WriteEnable() != HAL_OK)
			return false;

		s_command.Instruction = QUAD_IN_FAST_PROG_CMD;
		s_command.AddressMode = QSPI_ADDRESS_1_LINE;
		s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
		s_command.DataMode = QSPI_DATA_4_LINES;
		s_command.DummyCycles = 0;
		s_command.Address = current_addr;
		s_command.NbData = current_size;

		if (HAL_QSPI_Command(&handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
			return false;

		if (HAL_QSPI_Transmit(&handle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
			return false;

		if (AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
			return false;

		current_addr += current_size;
		pData += current_size;
		current_size = ((current_addr + QSPI_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : QSPI_PAGE_SIZE;
	} while (current_addr < end_addr);

	return true;
}

// Writes within a page (256 Bytes)
// Data to be written must not cross page boundaries.
// Setting use_interrupt to 1 means HAL_QSPI_TxCpltCallback() interrupt will be called when TX is
// done, but you must still check the chip status before accessing it again.
//
bool QSpiFlash::Write_Page(uint8_t *pData, uint32_t write_addr, uint32_t num_bytes, UseInterruptFlags use_interrupt) {
	// Cannot write more than a page
	if (num_bytes > QSPI_PAGE_SIZE)
		return false;

	uint32_t start_page = write_addr >> QSPI_PAGE_ADDRESS_BITS;
	uint32_t end_page = (write_addr + num_bytes - 1) >> QSPI_PAGE_ADDRESS_BITS;

	// Cannot cross page boundaries
	if (start_page != end_page)
		return false;

	if (WriteEnable() != HAL_OK)
		return false;

	// Initialize the program command
	s_command.Instruction = QUAD_IN_FAST_PROG_CMD;
	s_command.AddressMode = QSPI_ADDRESS_1_LINE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_4_LINES;
	s_command.DummyCycles = 0;
	s_command.Address = write_addr;
	s_command.NbData = num_bytes;

	if (HAL_QSPI_Command(&handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return false;

	if (use_interrupt == EXECUTE_BACKGROUND) {
		QSPI_status = STATUS_TXING;

		if (HAL_QSPI_Transmit_IT(&handle, pData) != HAL_OK)
			return false;

		while (!done_TXing()) {
			;
		}

		// Set-up auto polling, which periodically queries the chip in the background
		AutoPollingMemReady_IT();
	} else {
		if (HAL_QSPI_Transmit(&handle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
			return false;

		if (AutoPollingMemReady_IT() != HAL_OK)
			return false;
	}

	return true;
}

bool QSpiFlash::Read(uint8_t *pData, uint32_t read_addr, uint32_t num_bytes, UseInterruptFlags use_interrupt) {
	uint8_t status;

	// Todo: take advantage of AX Read mode (see datasheet)
	// by setting:
	// s_command.SIOOMode			= QSPI_SIOO_INST_ONLY_FIRST_CMD;
	// s_command.AlternateByteMode 	= QSPI_ALTERNATE_BYTES_4_LINES;
	// s_command.AlternateBytesSize 	= QSPI_ALTERNATE_BYTES_8_BITS;
	// s_command.AlternateBytes 		= 0xA0;

	s_command.Instruction = QUAD_INOUT_FAST_READ_CMD;
	s_command.AddressMode = QSPI_ADDRESS_4_LINES;
	s_command.Address = read_addr;
	s_command.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_4_LINES;
	s_command.AlternateBytes = 0x00;
	s_command.DataMode = QSPI_DATA_4_LINES;
	s_command.DummyCycles = QSPI_DUMMY_CYCLES_READ_QUAD_IO;
	s_command.NbData = num_bytes;

	if (HAL_QSPI_Command(&handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return HAL_ERROR;

	if (use_interrupt == EXECUTE_BACKGROUND) {
		QSPI_status = STATUS_RXING;

		status = HAL_QSPI_Receive_IT(&handle, pData);
	} else
		status = HAL_QSPI_Receive(&handle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);

	if (status != HAL_OK)
		return false;

	return true;
}

HAL_StatusTypeDef QSpiFlash::WriteEnable(void) {
	QSPI_AutoPollingTypeDef s_config;

	/* Enable write operations */
	s_command.Instruction = WRITE_ENABLE_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0;

	if (HAL_QSPI_Command(&handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return HAL_ERROR;

	/* Configure automatic polling mode to wait for write enabling */
	s_config.Match = QSPI_SR_WREN;
	s_config.Mask = QSPI_SR_WREN;
	s_config.MatchMode = QSPI_MATCH_MODE_AND;
	s_config.StatusBytesSize = 1;
	s_config.Interval = 0x10;
	s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

	s_command.Instruction = READ_STATUS_REG_CMD;
	s_command.DataMode = QSPI_DATA_1_LINE;

	if (HAL_QSPI_AutoPolling(&handle, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return HAL_ERROR;

	return HAL_OK;
}

/**
 * @brief	Repeatedly reads the status register of the chip and waits until it indicates Write In
 * Progress is complete.
 * @param	Timeout
 * @retval None
 */
HAL_StatusTypeDef QSpiFlash::AutoPollingMemReady(uint32_t Timeout) {
	QSPI_AutoPollingTypeDef s_config;

	/* Configure automatic polling mode to wait for memory ready */
	s_command.Instruction = READ_STATUS_REG_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.DummyCycles = 0;

	s_config.Match = 0;
	s_config.Mask = QSPI_SR_WIP;
	s_config.MatchMode = QSPI_MATCH_MODE_AND;
	s_config.StatusBytesSize = 1;
	s_config.Interval = 0x10;
	s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

	if (HAL_QSPI_AutoPolling(&handle, &s_command, &s_config, Timeout) != HAL_OK)
		return HAL_ERROR;

	return HAL_OK;
}

/**
 * @brief	Sets up auto-polling to call the HAL_QSPI_StatusMatchCallback() when the status register
 * indicates Write In Progress is cleared.
 * @param	None
 * @retval None
 */

HAL_StatusTypeDef QSpiFlash::AutoPollingMemReady_IT(void) {
	QSPI_AutoPollingTypeDef s_config;

	/* Configure automatic polling mode to wait for memory ready */
	s_command.Instruction = READ_STATUS_REG_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.DummyCycles = 0;

	s_config.Match = 0;
	s_config.Mask = QSPI_SR_WIP;
	s_config.MatchMode = QSPI_MATCH_MODE_AND;
	s_config.StatusBytesSize = 1;
	s_config.Interval = 0x10;
	s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

	QSPI_status = STATUS_WIP;

	if (HAL_QSPI_AutoPolling_IT(&handle, &s_command, &s_config) != HAL_OK)
		return HAL_ERROR;

	return HAL_OK;
}

/**
 * @brief	This function put QSPI memory in QPI mode (quad I/O).
 * @retval None
 */
HAL_StatusTypeDef QSpiFlash::EnterMemory_QPI(void) {
	QSPI_AutoPollingTypeDef s_config;
	uint8_t reg;

	reg = QSPI_SR_QUADEN;

	s_command.Instruction = WRITE_ENABLE_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0;

	if (HAL_QSPI_Command(&handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return HAL_ERROR;

	s_command.Instruction = WRITE_STATUS_REG_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.DummyCycles = 0;
	s_command.NbData = 1;

	if (HAL_QSPI_Command(&handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return HAL_ERROR;

	if (HAL_QSPI_Transmit(&handle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return HAL_ERROR;

	/* 40ms	Write Status/Configuration Register Cycle Time */
	HAL_Delay(8);

	/* Configure automatic polling mode to wait the QUADEN bit=1 and WIP bit=0 */
	s_config.Match = QSPI_SR_QUADEN;
	s_config.Mask = QSPI_SR_QUADEN /*|QSPI_SR_WIP*/;
	s_config.MatchMode = QSPI_MATCH_MODE_AND;
	s_config.StatusBytesSize = 1;
	s_config.Interval = 0x10;
	s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

	s_command.Instruction = READ_STATUS_REG_CMD;
	s_command.DataMode = QSPI_DATA_1_LINE;

	if (HAL_QSPI_AutoPolling(&handle, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return HAL_ERROR;

	return HAL_OK;
}

QSpiFlash *QSpiFlash::instance_;

// Callbacks & IRQ handlers

extern "C" void HAL_QSPI_StatusMatchCallback(QSPI_HandleTypeDef *hqspi) {
	QSpiFlash::instance_->QSPI_status = QSpiFlash::STATUS_READY;
}

extern "C" void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi) {
	if (QSpiFlash::instance_->QSPI_status == QSpiFlash::STATUS_RXING)
		QSpiFlash::instance_->QSPI_status = QSpiFlash::STATUS_READY;
}

extern "C" void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi) {
	if (QSpiFlash::instance_->QSPI_status == QSpiFlash::STATUS_TXING)
		QSpiFlash::instance_->QSPI_status = QSpiFlash::STATUS_TX_COMPLETE;
}

} // namespace mdrivlib
