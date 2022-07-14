#include "delay.h"
#include "qspi_ll.h"
#include "stm32mp1xx.h"

// TODO: Config file for QSPI chip:
//  struct Cmd {
//   u32 cmd;
//   NumLines addr_lines;
//   NumLines data_lines;
//   NumLines altbyte_lines;
//   u32 num_alt_bytes;
//   u32 num_dummy_cycles;
//   u32 alt_byte_value;
// };
// struct QSPIFlashChip {
//   u32 chip_size_bytes;
//   Cmd read_sio_cmd;
//   Cmd read_quad_cmd;
//   etc..
// };

#define QSPI_DUMMY_CYCLES_READ 0

void QSPI_init()
{
	// Select ACLK 266MHz
	RCC->QSPICKSELR = 0;

	// Disable and reset QUADSPI
	QUADSPI->CR = 0;
	RCC->AHB2RSTSETR = RCC_AHB6RSTSETR_QSPIRST;
	RCC->AHB2RSTCLRR = RCC_AHB6RSTCLRR_QSPIRST;

	// Prescale 3 = /4 --> 67MHz
	// FIFO Threshold = 2 of 16
	// Sample shift
	QUADSPI->CR = (3 << QUADSPI_CR_PRESCALER_Pos) | (2 << QUADSPI_CR_FTHRES_Pos) | (QUADSPI_CR_SSHIFT) | QUADSPI_CR_EN;

	// RAM size 24-bits (16MByte), CS hold time of 2
	QUADSPI->DCR = (23 << QUADSPI_DCR_FSIZE_Pos) | (2 << QUADSPI_DCR_CSHT_Pos);

	// Enable MM mode:
	const uint32_t dummy_cycles = 6;
	LL_QSPI_SetCommConfig(QSPI_SIOO_INST_EVERY_CMD | QSPI_INSTRUCTION_1_LINE | QSPI_DATA_4_LINES |
						  (dummy_cycles << QUADSPI_CCR_DCYC_Pos) | QSPI_ALTERNATE_BYTES_NONE |
						  QSPI_ALTERNATE_BYTES_4_LINES | QSPI_ADDRESS_24_BITS | QSPI_ADDRESS_1_LINE |
						  QUAD_OUT_FAST_READ_CMD | QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED);
	LL_QPSI_SetAltBytes(0x00);
}

uint32_t QSPI_read_MM(uint8_t *pData, uint32_t read_addr, uint32_t num_bytes)
{
	uint8_t *src = (uint8_t *)(0x70000000U + read_addr);
	while (num_bytes--)
		*pData++ = *src++;

	return 1;
}

uint32_t QSPI_read_SIO(uint8_t *pData, uint32_t read_addr, uint32_t num_bytes)
{
	LL_QSPI_WaitNotBusy();

	LL_QPSI_SetDataLength(num_bytes);
	LL_QSPI_SetCommConfig(QSPI_DDR_MODE_DISABLE | QSPI_DDR_HHC_ANALOG_DELAY | QSPI_SIOO_INST_EVERY_CMD |
						  QSPI_INSTRUCTION_1_LINE | QSPI_DATA_1_LINE |
						  (QSPI_DUMMY_CYCLES_READ << QUADSPI_CCR_DCYC_Pos) | QSPI_ALTERNATE_BYTES_8_BITS |
						  QSPI_ALTERNATE_BYTES_1_LINE | QSPI_ADDRESS_24_BITS | QSPI_ADDRESS_1_LINE | FAST_READ_CMD |
						  QSPI_FUNCTIONAL_MODE_INDIRECT_READ);
	LL_QPSI_SetAltBytes(0);
	LL_QSPI_SetAddress(read_addr);

	return LL_QSPI_Receive(pData);
}
