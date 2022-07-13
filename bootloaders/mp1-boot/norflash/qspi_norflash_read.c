#include "delay.h"
#include "qspi_ll.h"
#include "stm32mp1xx.h"

#define QSPI_DUMMY_CYCLES_READ 0
#define QSPI_DUMMY_CYCLES_QUAD_READ 8	 // Datasheet says 4 bytes: 2 clks/byte * 4 = 8 clocks
#define QSPI_DUMMY_CYCLES_QUAD_IO_READ 4 // Datasheet says 2 bytes: 2 clks/byte * 2 = 4 clocks
#define QSPI_DUMMY_CYCLES_DUAL_READ 8

void QSPI_init()
{
	// Select ACLK 266MHz
	RCC->QSPICKSELR = 0;

	QUADSPI->CR = 0;
	RCC->AHB2RSTSETR = RCC_AHB6RSTSETR_QSPIRST;
	RCC->AHB2RSTCLRR = RCC_AHB6RSTCLRR_QSPIRST;
	(void)RCC->AHB2RSTCLRR;
	QUADSPI->CR =
		(0b010 << QUADSPI_CR_PRESCALER_Pos) | (2 << QUADSPI_CR_FTHRES_Pos) | (QUADSPI_CR_SSHIFT) | QUADSPI_CR_EN;

	QUADSPI->DCR = (23 << QUADSPI_DCR_FSIZE_Pos) | (2 << QUADSPI_DCR_CSHT_Pos);

	// LL_QSPI_SendInstructionNoDataNoAddress(RESET_ENABLE_CMD);
	// LL_QSPI_SendInstructionNoDataNoAddress(RESET_CMD);

	udelay(30);

	// Statis register 2; Quad Enable bit
	// LL_QSPI_WaitNotBusy();
	// LL_QPSI_SetDataLength(1);
	// LL_QSPI_SetCommConfig(QSPI_DDR_MODE_DISABLE | QSPI_DDR_HHC_ANALOG_DELAY | QSPI_SIOO_INST_EVERY_CMD |
	// 					  QSPI_INSTRUCTION_1_LINE | QSPI_DATA_1_LINE | (0 << 18) | QSPI_ALTERNATE_BYTES_8_BITS |
	// 					  QSPI_ALTERNATE_BYTES_NONE | QSPI_ADDRESS_24_BITS | QSPI_ADDRESS_NONE | 0x35 |
	// 					  QSPI_FUNCTIONAL_MODE_INDIRECT_READ);
	// LL_QPSI_SetAltBytes(0);
	// LL_QSPI_SetAddress(0x35);

	// uint8_t statusreg;
	// uint8_t ok = LL_QSPI_Receive(&statusreg);
	// if (!(statusreg & (1 << 1))) {
	// 	// Debug breakpoint: QE is not set on the chip
	// 	__BKPT();
	// }

	// Enable MM mode:
	LL_QSPI_SetCommConfig(QSPI_DDR_MODE_DISABLE | QSPI_DDR_HHC_ANALOG_DELAY | QSPI_SIOO_INST_EVERY_CMD |
						  QSPI_INSTRUCTION_1_LINE | QSPI_DATA_4_LINES | (6 << 18) | QSPI_ALTERNATE_BYTES_NONE |
						  QSPI_ALTERNATE_BYTES_4_LINES | QSPI_ADDRESS_24_BITS | QSPI_ADDRESS_1_LINE |
						  QUAD_OUT_FAST_READ_CMD | QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED);
	LL_QPSI_SetAltBytes(0x00);

	// Prescale 0b11 = /4 --> 67MHz
	// FIFO Threshold = 8 of 16
	//
	// Dual flash disabled
	// Flash bank 1 selected
	// all interrupts disabled
	// no DMA
	// No sample shift
	// Polling Mode = AND
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
						  QSPI_INSTRUCTION_1_LINE | QSPI_DATA_1_LINE | (QSPI_DUMMY_CYCLES_READ << 18) |
						  QSPI_ALTERNATE_BYTES_8_BITS | QSPI_ALTERNATE_BYTES_1_LINE | QSPI_ADDRESS_24_BITS |
						  QSPI_ADDRESS_1_LINE | FAST_READ_CMD | QSPI_FUNCTIONAL_MODE_INDIRECT_READ);
	LL_QPSI_SetAltBytes(0);
	LL_QSPI_SetAddress(read_addr);

	return LL_QSPI_Receive(pData);
}

uint32_t QSPI_read_quad(uint8_t *pData, uint32_t read_addr, uint32_t num_bytes)
{
	LL_QSPI_WaitNotBusy();

	LL_QPSI_SetDataLength(num_bytes);

	// LL_QSPI_SetCommConfig(QSPI_DDR_MODE_DISABLE | QSPI_DDR_HHC_ANALOG_DELAY | QSPI_SIOO_INST_ONLY_FIRST_CMD |
	// 					  QSPI_INSTRUCTION_1_LINE | QSPI_DATA_4_LINES | (QSPI_DUMMY_CYCLES_QUAD_READ << 18) |
	// 					  QSPI_ALTERNATE_BYTES_8_BITS | QSPI_ALTERNATE_BYTES_NONE | QSPI_ADDRESS_24_BITS |
	// 					  QSPI_ADDRESS_1_LINE | QUAD_OUT_FAST_READ_CMD | QSPI_FUNCTIONAL_MODE_INDIRECT_READ);
	// LL_QPSI_SetAltBytes(0);

	// LL_QSPI_SetCommConfig(QSPI_DDR_MODE_DISABLE | QSPI_DDR_HHC_ANALOG_DELAY | QSPI_SIOO_INST_EVERY_CMD |
	// 					  QSPI_INSTRUCTION_1_LINE | QSPI_DATA_4_LINES | (QSPI_DUMMY_CYCLES_QUAD_IO_READ << 18) |
	// 					  QSPI_ALTERNATE_BYTES_8_BITS | QSPI_ALTERNATE_BYTES_4_LINES | QSPI_ADDRESS_24_BITS |
	// 					  QSPI_ADDRESS_4_LINES | QUAD_INOUT_FAST_READ_CMD | QSPI_FUNCTIONAL_MODE_INDIRECT_READ);
	// LL_QPSI_SetAltBytes(0xFFFFFFFF);

	LL_QSPI_SetCommConfig(QSPI_DDR_MODE_DISABLE | QSPI_DDR_HHC_ANALOG_DELAY | QSPI_SIOO_INST_EVERY_CMD |
						  QSPI_INSTRUCTION_1_LINE | QSPI_DATA_2_LINES | (QSPI_DUMMY_CYCLES_DUAL_READ << 18) |
						  QSPI_ALTERNATE_BYTES_8_BITS | QSPI_ALTERNATE_BYTES_NONE | QSPI_ADDRESS_24_BITS |
						  QSPI_ADDRESS_1_LINE | DUAL_OUT_FAST_READ_CMD | QSPI_FUNCTIONAL_MODE_INDIRECT_READ);
	LL_QPSI_SetAltBytes(0);

	LL_QSPI_SetAddress(read_addr);

	return LL_QSPI_Receive(pData);
}
