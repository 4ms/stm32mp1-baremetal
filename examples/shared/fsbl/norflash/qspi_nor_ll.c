#include "qspi_nor_ll.h"
#include "stm32mp1xx.h"

void LL_QPSI_SetAltBytes(uint32_t AlternateBytes)
{
	WRITE_REG(QUADSPI->ABR, AlternateBytes);
}

void LL_QPSI_SetDataLength(uint32_t NbData)
{
	WRITE_REG(QUADSPI->DLR, (NbData - 1));
}

void LL_QSPI_SetAddress(uint32_t address)
{
	WRITE_REG(QUADSPI->AR, address);
}

void LL_QSPI_WaitNotBusy(void)
{
	while (READ_BIT(QUADSPI->SR, QSPI_FLAG_BUSY) != 0) {
		;
	}
}

uint32_t LL_QSPI_WaitFlagTimeout(uint32_t flag)
{
	// wait for flag to go high
	uint32_t timeout = 0x00FFFFFF;
	while (--timeout && (READ_BIT(QUADSPI->SR, flag) == 0)) {
		;
	}
	return timeout;
}

void LL_QSPI_WaitFlag(uint32_t flag)
{
	// wait for flag to go high
	while (!(READ_BIT(QUADSPI->SR, flag))) {
		;
	}
}

void LL_QSPI_ClearFlag(uint32_t flag)
{
	WRITE_REG(QUADSPI->FCR, flag);
}

void LL_QSPI_SetCommConfig(uint32_t commconfig)
{
	WRITE_REG(QUADSPI->CCR, commconfig);
}

uint32_t LL_QSPI_SendInstructionNoDataNoAddress(uint32_t instruction)
{
	LL_QSPI_WaitNotBusy();
	LL_QSPI_SetCommConfig(QSPI_DDR_MODE_DISABLE | QSPI_DDR_HHC_ANALOG_DELAY | QSPI_SIOO_INST_EVERY_CMD |
						  QSPI_INSTRUCTION_1_LINE | QSPI_DATA_NONE | (0 << 18) | QSPI_ALTERNATE_BYTES_SIZE_NONE |
						  QSPI_ALTERNATE_BYTES_NONE | QSPI_ADDRESSSIZE_NONE | QSPI_ADDRESS_NONE | instruction |
						  QSPI_FUNCTIONAL_MODE_INDIRECT_WRITE);
	if (!LL_QSPI_WaitFlagTimeout(QSPI_FLAG_TC))
		return 0;
	LL_QSPI_ClearFlag(QSPI_FLAG_TC);
	return 1;
}

uint32_t LL_QSPI_Transmit(uint8_t *pData)
{
	__IO uint32_t *data_reg = &(QUADSPI->DR);
	uint32_t cnt = READ_REG(QUADSPI->DLR) + 1;
	uint32_t ok = 1;

	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, QSPI_FUNCTIONAL_MODE_INDIRECT_WRITE);

	while (cnt > 0) {
		// Wait until FT flag is set to send data
		ok = LL_QSPI_WaitFlagTimeout(QSPI_FLAG_FT);
		// LL_QSPI_ClearFlag(QSPI_FLAG_FT);
		if (!ok)
			break;

		// byte-mode write to DR to add 1 byte to FIFO
		*(__IO uint8_t *)data_reg = *pData++;
		cnt--;
	}
	if (ok) {
		ok = LL_QSPI_WaitFlagTimeout(QSPI_FLAG_TC);
		LL_QSPI_ClearFlag(QSPI_FLAG_TC);
	}
	return ok;
}

uint32_t LL_QSPI_Receive(uint8_t *pData)
{
	uint32_t addr_reg = READ_REG(QUADSPI->AR);
	__IO uint32_t *data_reg = &QUADSPI->DR;
	uint32_t cnt = READ_REG(QUADSPI->DLR) + 1;
	uint32_t ok = 1;

	MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, QSPI_FUNCTIONAL_MODE_INDIRECT_READ);

	// Start the transfer by re-writing the address in AR register
	WRITE_REG(QUADSPI->AR, addr_reg);

	while (cnt > 0) {
		// Wait until FT or TC flag is set to read data
		ok = LL_QSPI_WaitFlagTimeout(QSPI_FLAG_FT | QSPI_FLAG_TC);
		// LL_QSPI_ClearFlag(QSPI_FLAG_FT);
		if (!ok)
			break;

		// byte-mode read from DR
		*pData++ = *(__IO uint8_t *)data_reg;
		cnt--;
	}
	if (ok) {
		ok = LL_QSPI_WaitFlagTimeout(QSPI_FLAG_TC);
		LL_QSPI_ClearFlag(QSPI_FLAG_TC);
	}
	return ok;
}

uint32_t LL_QSPI_WriteEnable(void)
{
	uint32_t ok;

	LL_QSPI_WaitNotBusy();
	WRITE_REG(QUADSPI->CCR,
			  (QSPI_DDR_MODE_DISABLE | QSPI_DDR_HHC_ANALOG_DELAY | QSPI_SIOO_INST_EVERY_CMD | QSPI_INSTRUCTION_1_LINE |
			   QSPI_DATA_NONE | (0 << 18) | QSPI_ALTERNATE_BYTES_SIZE_NONE | QSPI_ALTERNATE_BYTES_NONE |
			   QSPI_ADDRESSSIZE_NONE | QSPI_ADDRESS_NONE | WRITE_ENABLE_CMD | QSPI_FUNCTIONAL_MODE_INDIRECT_WRITE));
	ok = LL_QSPI_WaitFlagTimeout(QSPI_FLAG_TC);
	LL_QSPI_ClearFlag(QSPI_FLAG_TC);

	if (ok) {
		LL_QSPI_StartAutoPoll(QSPI_SR_WREN, QSPI_SR_WREN, 0x10, QSPI_MATCH_MODE_AND);
		ok = LL_QSPI_WaitFlagTimeout(QSPI_FLAG_SM);
		LL_QSPI_ClearFlag(QSPI_FLAG_SM);
	}
	return ok;
}

void LL_QSPI_StartAutoPoll(uint32_t Match, uint32_t Mask, uint32_t Interval, uint32_t MatchMode)
{
	// QSPI_CommandTypeDef s_command;

	LL_QSPI_WaitNotBusy();

	WRITE_REG(QUADSPI->PSMAR, Match);
	WRITE_REG(QUADSPI->PSMKR, Mask);
	WRITE_REG(QUADSPI->PIR, Interval);
	// Configure QSPI: CR register with Match mode and Automatic stop enabled
	// (otherwise there will be an infinite loop in blocking mode)
	MODIFY_REG(QUADSPI->CR, (QUADSPI_CR_PMM | QUADSPI_CR_APMS), (MatchMode | QSPI_AUTOMATIC_STOP_ENABLE));

	LL_QPSI_SetDataLength(1);
	WRITE_REG(QUADSPI->CCR,
			  (QSPI_DDR_MODE_DISABLE | QSPI_DDR_HHC_ANALOG_DELAY | QSPI_SIOO_INST_EVERY_CMD | QSPI_INSTRUCTION_1_LINE |
			   QSPI_DATA_1_LINE | (0 << 18) | QSPI_ALTERNATE_BYTES_SIZE_NONE | QSPI_ALTERNATE_BYTES_NONE |
			   QSPI_ADDRESSSIZE_NONE | QSPI_ADDRESS_NONE | READ_STATUS_REG_CMD | QSPI_FUNCTIONAL_MODE_AUTO_POLLING));
}
