#include "delay.h"
#include "qspi_ll.h"
#include "stm32mp1xx.h"

#define QSPI_DUMMY_CYCLES_READ 0

void QSPI_init()
{
	// Select ACLK 266MHz
	RCC->QSPICKSELR = 0;
	// 0b10 = prescale /3 = 88MHz
	QUADSPI->CR = (QUADSPI->CR & ~QUADSPI_CR_PRESCALER_Msk) | QUADSPI_CR_PRESCALER_1;
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
