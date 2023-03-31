#include "qspi_flash_driver.hh"
#include "qspi_flash_registers.h"
#include "stm32mp1xx.h"

// #define QSPI_DO_TESTS

namespace mdrivlib
{

uint32_t QSpiFlash::get_64kblock_addr(unsigned block64k_num)
{
	if (block64k_num >= defs.flash_size_bytes / QSPI_64KBLOCK_SIZE)
		return 0;

	return block64k_num * QSPI_64KBLOCK_SIZE;
}

uint32_t QSpiFlash::get_32kblock_addr(unsigned block32k_num)
{
	if (block32k_num >= defs.flash_size_bytes / QSPI_32KBLOCK_SIZE)
		return 0;

	return block32k_num * QSPI_32KBLOCK_SIZE;
}

uint32_t QSpiFlash::get_sector_addr(unsigned sector_num)
{
	if (sector_num >= defs.flash_size_bytes / QSPI_SECTOR_SIZE)
		return 0;

	return sector_num * QSPI_SECTOR_SIZE;
}

QSpiFlash::QSpiFlash(const QSPIFlashConfig &config_defs)
	: defs{config_defs}
	, data_mode{config_defs.io_mode == QSPIFlashConfig::QuadSPI ? QSPI_DATA_4_LINES :
				config_defs.io_mode == QSPIFlashConfig::DualSPI ? QSPI_DATA_2_LINES :
																  QSPI_DATA_1_LINE}
	, quad_write_cmd{config_defs.chip_id == QSPIFlashConfig::IS25L	   ? IS25LQ0x0B_QUAD_IN_FAST_PROG_CMD :
					 config_defs.chip_id == QSPIFlashConfig::W25Q128JV ? W25Q128JV_QUAD_IN_FAST_PROG_CMD :
																		 S25FLxxxL_QUAD_IN_FAST_PROG_CMD}
	, quad_read_dummy_cycles{config_defs.chip_id == QSPIFlashConfig::IS25L ? IS25LQ0x0B_QSPI_DUMMY_CYCLES_READ_QUAD_IO :
							 config_defs.chip_id == QSPIFlashConfig::W25Q128JV ?
																			 W25Q128JV_QSPI_DUMMY_CYCLES_READ_QUAD_IO :
																			 S25FLxxxL_QSPI_DUMMY_CYCLES_READ_QUAD_IO}
{
	instance_ = this;

	handle.Instance = QUADSPI;

	HAL_QSPI_DeInit(&handle);

	// FixMe: Why isn't this unlocked by HAL?
	handle.Lock = HAL_UNLOCKED;

	__HAL_RCC_QSPI_CLK_ENABLE();
	__HAL_RCC_QSPI_FORCE_RESET();
	__HAL_RCC_QSPI_RELEASE_RESET();

	// Initialize chip pins in single IO mode
	GPIO_init_IO0_IO1();

	handle.Init.ClockPrescaler = defs.clock_division;
	handle.Init.FifoThreshold = 1;
	handle.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
	handle.Init.FlashSize = defs.flash_size_address_bits - 1;
	handle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
	handle.Init.ClockMode = QSPI_CLOCK_MODE_0;
	handle.Init.FlashID = defs.bank == QSPIFlashConfig::Bank1 ? QSPI_FLASH_ID_1 : QSPI_FLASH_ID_2;
	handle.Init.DualFlash = QSPI_DUALFLASH_DISABLE;

	HAL_QSPI_Init(&handle);

	init_command(&s_command);

	QSPI_status = STATUS_READY; // NOLINT

	Reset();

	if (defs.io_mode == QSPIFlashConfig::QuadSPI) {
		// Now that chip is in QSPI mode, IO2 and IO3 can be initialized
		if (defs.chip_id == QSPIFlashConfig::IS25L)
			enter_memory_QPI(); // S25FL doesn't seem to need this?
		GPIO_init_IO2_IO3_AF();
	}

#ifdef QSPI_DO_TESTS
	// Erase(ENTIRE_CHIP, 0, EXECUTE_FOREGROUND);
	if (!test()) {
		__BKPT();
	}
#endif
}

void QSpiFlash::GPIO_init_IO0_IO1()
{
	// Disable all possible QSPI IO2 and IO3, since the FSBL may have used incorrect alt pins for QSPI quad mode
	// This only would happen if we're using a debugger to bootstrap the bootloader into DDR RAM
	// in order to overwrite the faulty FSBL
	PinConf{GPIO::E, PinNum::_2, PinAF::AFNone}.init(PinMode::Analog, PinPull::None);
	PinConf{GPIO::F, PinNum::_7, PinAF::AFNone}.init(PinMode::Analog, PinPull::None);
	PinConf{GPIO::A, PinNum::_1, PinAF::AFNone}.init(PinMode::Analog, PinPull::None);
	PinConf{GPIO::F, PinNum::_6, PinAF::AFNone}.init(PinMode::Analog, PinPull::None);
	PinConf{GPIO::D, PinNum::_13, PinAF::AFNone}.init(PinMode::Analog, PinPull::None);

	// Setup required IO pins
	PinConf{defs.cs.gpio, defs.cs.pin, defs.cs.af}.init(PinMode::Alt, PinPull::Up);
	PinConf{defs.clk.gpio, defs.clk.pin, defs.clk.af}.init(PinMode::Alt);
	PinConf{defs.io0.gpio, defs.io0.pin, defs.io0.af}.init(PinMode::Alt);
	PinConf{defs.io1.gpio, defs.io1.pin, defs.io1.af}.init(PinMode::Alt);

	// Set /HOLD and /WP pins high to disable holding and write protection, until we enter QPI mode
	PinConf wp{defs.io2.gpio, defs.io2.pin, PinAF::AFNone};
	wp.init(PinMode::Output, PinPull::None);
	wp.high();

	PinConf hold{defs.io3.gpio, defs.io3.pin, PinAF::AFNone};
	hold.init(PinMode::Output, PinPull::None);
	hold.high();
}

void QSpiFlash::GPIO_init_IO2_IO3_AF()
{

	PinConf io2{defs.io2.gpio, defs.io2.pin, defs.io2.af};
	io2.init(PinMode::Alt, PinPull::None);

	PinConf io3{defs.io3.gpio, defs.io3.pin, defs.io3.af};
	io3.init(PinMode::Alt, PinPull::None);
}

void QSpiFlash::init_command(QSPI_CommandTypeDef *s_command)
{
	s_command->InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command->AddressSize = QSPI_ADDRESS_24_BITS; // TODO: make this 32 if defs.AddrBits == 32?
	s_command->DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command->DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command->SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
}

HAL_StatusTypeDef QSpiFlash::Reset()
{
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
bool QSpiFlash::test()
{
	unsigned sector;
	for (sector = 0; sector < defs.flash_size_bytes / QSPI_SECTOR_SIZE; sector++) {
		if (!test_sector(sector))
			return false; // fail
	}
	return true; // pass
}

// Tests one sector
// Returns true if passed, false if failed
bool QSpiFlash::test_sector(uint8_t sector_num)
{
	uint32_t i;
	uint8_t test_buffer[QSPI_SECTOR_SIZE];
	uint32_t test_addr = get_sector_addr(sector_num);

	read(test_buffer, test_addr, QSPI_SECTOR_SIZE, EXECUTE_FOREGROUND);

	for (i = 0; i < QSPI_SECTOR_SIZE; i++)
		test_buffer[i] = test_encode_num(i, sector_num);

	while (!is_ready())
		;

	if (!erase(SECTOR, test_addr, EXECUTE_FOREGROUND))
		return false;

	while (!is_ready())
		;

	for (i = 0; i < (QSPI_SECTOR_SIZE / QSPI_PAGE_SIZE); i++) {
		// Benchmark: ~380us/page
		if (!write_page(
				&(test_buffer[i * QSPI_PAGE_SIZE]), test_addr + i * QSPI_PAGE_SIZE, QSPI_PAGE_SIZE, EXECUTE_FOREGROUND))
			return false;
		while (!is_ready())
			;
	}

	for (i = 0; i < QSPI_SECTOR_SIZE; i++)
		test_buffer[i] = 0;

	if (!read(test_buffer, test_addr, QSPI_SECTOR_SIZE, EXECUTE_FOREGROUND))
		return false;

	while (!is_ready())
		;

	for (i = 0; i < (QSPI_SECTOR_SIZE - 1); i++) {
		if (test_buffer[i] != test_encode_num(i, sector_num))
			return false;
	}

	return true;
}

uint8_t QSpiFlash::test_encode_num(uint32_t num, uint32_t sector_num)
{
	return (((num * 9) + (num >> 7)) + sector_num) & 0xFF;
}

bool QSpiFlash::erase(uint32_t size, uint32_t base_addr, UseInterruptFlags use_interrupt)
{
	uint8_t status;
	uint32_t timeout;

	if (write_enable() != HAL_OK)
		return false;

	if (size == SECTOR) {
		s_command.Instruction = SECTOR_ERASE_CMD;
		s_command.Address = base_addr;
		s_command.AddressMode = QSPI_ADDRESS_1_LINE;
		timeout = QSPI_SECTOR_ERASE_MAX_TIME_SYSTICKS;
	} else if (size == BLOCK_32K) {
		s_command.Instruction = BLOCK_ERASE_32K_CMD;
		s_command.Address = base_addr;
		s_command.AddressMode = QSPI_ADDRESS_1_LINE;
		timeout = QSPI_32KBLOCK_ERASE_MAX_TIME_SYSTICKS;
	} else if (size == BLOCK_64K) {
		s_command.Instruction = BLOCK_ERASE_64K_CMD;
		s_command.Address = base_addr;
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

	if (HAL_QSPI_Command(&handle, &s_command, 200 /*HAL_QPSI_TIMEOUT_DEFAULT_VALUE*/) != HAL_OK)
		return false;

	if (use_interrupt == EXECUTE_BACKGROUND)
		status = auto_polling_mem_ready_it();
	else
		status = auto_polling_mem_ready(timeout);

	if (status != HAL_OK)
		return false;

	return true;
}

bool QSpiFlash::write(const uint8_t *pData, uint32_t write_addr, uint32_t num_bytes)
{
	uint32_t end_addr, current_size, current_addr;

	if (write_addr >= defs.flash_size_bytes)
		return false;

	if (write_addr + num_bytes >= defs.flash_size_bytes)
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
		if (write_enable() != HAL_OK)
			return false;

		if (defs.io_mode == QSPIFlashConfig::QuadSPI) {
			s_command.Instruction = quad_write_cmd;
		} else {
			// This is for DDR mode also, on S25FL128L/256L
			s_command.Instruction = PAGE_PROG_CMD;
		}
		s_command.AddressMode = QSPI_ADDRESS_1_LINE;
		s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
		s_command.DataMode = data_mode;
		s_command.DummyCycles = 0;
		s_command.Address = current_addr;
		s_command.NbData = current_size;

		if (HAL_QSPI_Command(&handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
			return false;

		if (HAL_QSPI_Transmit(&handle, const_cast<uint8_t *>(pData), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
			return false;

		if (auto_polling_mem_ready(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
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
bool QSpiFlash::write_page(const uint8_t *pData,
						   uint32_t write_addr,
						   uint32_t num_bytes,
						   UseInterruptFlags use_interrupt)
{
	// Cannot write more than a page
	if (num_bytes > QSPI_PAGE_SIZE)
		return false;

	uint32_t start_page = write_addr >> QSPI_PAGE_ADDRESS_BITS;
	uint32_t end_page = (write_addr + num_bytes - 1) >> QSPI_PAGE_ADDRESS_BITS;

	// Cannot cross page boundaries
	if (start_page != end_page)
		return false;

	if (write_enable() != HAL_OK)
		return false;

	if (defs.io_mode == QSPIFlashConfig::QuadSPI) {
		s_command.Instruction = quad_write_cmd;
	} else {
		// This is for DDR mode also, on S25FL128L/256L
		s_command.Instruction = PAGE_PROG_CMD;
	}
	s_command.AddressMode = QSPI_ADDRESS_1_LINE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = data_mode;
	s_command.DummyCycles = 0;
	s_command.Address = write_addr;
	s_command.NbData = num_bytes;

	if (HAL_QSPI_Command(&handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return false;

	if (use_interrupt == EXECUTE_BACKGROUND) {
		QSPI_status = STATUS_TXING;

		if (HAL_QSPI_Transmit_IT(&handle, const_cast<uint8_t *>(pData)) != HAL_OK)
			return false;

		while (!done_TXing()) {
			;
		}

		// Set-up auto polling, which periodically queries the chip in the background
		auto_polling_mem_ready_it();
	} else {
		if (HAL_QSPI_Transmit(&handle, const_cast<uint8_t *>(pData), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
			return false;

		if (auto_polling_mem_ready_it() != HAL_OK)
			return false;
	}

	return true;
}

bool QSpiFlash::read(uint8_t *pData, uint32_t read_addr, uint32_t num_bytes, UseInterruptFlags use_interrupt)
{
	uint8_t status;

	// Todo: take advantage of AX Read mode (see datasheet)
	// by setting:
	// s_command.SIOOMode			= QSPI_SIOO_INST_ONLY_FIRST_CMD;
	// s_command.AlternateByteMode 	= QSPI_ALTERNATE_BYTES_4_LINES;
	// s_command.AlternateBytesSize 	= QSPI_ALTERNATE_BYTES_8_BITS;
	// s_command.AlternateBytes 		= 0xA0;

	if (defs.io_mode == QSPIFlashConfig::QuadSPI) {
		s_command.Instruction = QUAD_INOUT_FAST_READ_CMD;
		s_command.AddressMode = QSPI_ADDRESS_4_LINES;
		s_command.DummyCycles = quad_read_dummy_cycles;
		s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_4_LINES;
		s_command.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
		s_command.AlternateBytes = 0x00;

		// TODO:: DDR Mode: DUAL_OUT_FAST_READ_CMD
	} else {
		s_command.Instruction = FAST_READ_CMD;
		s_command.AddressMode = QSPI_ADDRESS_1_LINE;
		s_command.DummyCycles = QSPI_DUMMY_CYCLES_FAST_READ;
		s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	}
	s_command.Address = read_addr;
	s_command.DataMode = data_mode;
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

bool QSpiFlash::read_chip_id(uint32_t *chip_id_ptr)
{
	s_command.Instruction = READ_ID_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.DummyCycles = 0;
	s_command.NbData = 3;

	if (HAL_QSPI_Command(&handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return false;

	if (HAL_QSPI_Receive(&handle, (uint8_t *)chip_id_ptr, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return false;

	return true;
}
bool QSpiFlash::check_chip_id(uint32_t expected_id, uint32_t mask)
{
	uint32_t timeout = 100;
	uint32_t id = 0;
	do {
		HAL_Delay(1);
		read_chip_id(&id);
		id &= mask;
		if (id == expected_id)
			return true;
	} while (timeout--);
	return false;
}

bool QSpiFlash::read_config(uint32_t *data)
{
	s_command.Instruction = READ_STATUS_REG_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.DummyCycles = 0;
	s_command.NbData = 4;

	if (HAL_QSPI_Command(&handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return false;

	if (HAL_QSPI_Receive(&handle, (uint8_t *)data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return false;

	return true;
}

HAL_StatusTypeDef QSpiFlash::write_enable()
{
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
 * @brief	This function put QSPI memory in QPI mode (quad I/O).
 * @retval None
 */
HAL_StatusTypeDef QSpiFlash::enter_memory_QPI()
{
	QSPI_AutoPollingTypeDef s_config;

	s_command.Instruction = WRITE_ENABLE_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0;

	if (HAL_QSPI_Command(&handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return HAL_ERROR;

	// FIXME: Some chips need this: some use 0x38 enter QSPI mode
	s_command.Instruction = WRITE_STATUS_REG_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_1_LINE;
	s_command.DummyCycles = 0;
	s_command.NbData = 1;

	if (HAL_QSPI_Command(&handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return HAL_ERROR;

	uint8_t reg = QSPI_SR_QUADEN;
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

/**
 * @brief	Repeatedly reads the status register of the chip and waits until it indicates Write In
 * Progress is complete.
 * @param	Timeout
 * @retval None
 */
HAL_StatusTypeDef QSpiFlash::auto_polling_mem_ready(uint32_t Timeout)
{
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

HAL_StatusTypeDef QSpiFlash::auto_polling_mem_ready_it()
{
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

QSpiFlash *QSpiFlash::instance_;

// Callbacks & IRQ handlers

extern "C" void HAL_QSPI_StatusMatchCallback(QSPI_HandleTypeDef *hqspi)
{
	QSpiFlash::instance_->QSPI_status = QSpiFlash::STATUS_READY;
}

extern "C" void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
	if (QSpiFlash::instance_->QSPI_status == QSpiFlash::STATUS_RXING)
		QSpiFlash::instance_->QSPI_status = QSpiFlash::STATUS_READY;
}

extern "C" void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
	if (QSpiFlash::instance_->QSPI_status == QSpiFlash::STATUS_TXING)
		QSpiFlash::instance_->QSPI_status = QSpiFlash::STATUS_TX_COMPLETE;
}

} // namespace mdrivlib
