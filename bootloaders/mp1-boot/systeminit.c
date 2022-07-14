#include "stm32mp1xx.h"

uint32_t SystemCoreClock = 24000000;

void SystemInit(void)
{
	// do not use global variables because this function is called before
	// reaching pre-main. RW section may be overwritten afterwards.

	// Invalidate entire Unified TLB
	__set_TLBIALL(0);

	// Invalidate entire branch predictor array
	__set_BPIALL(0);
	__DSB();
	__ISB();

	//  Invalidate instruction cache and flush branch target cache
	__set_ICIALLU(0);
	__DSB();
	__ISB();

	L1C_InvalidateDCacheAll();

	__FPU_Enable();

	L1C_EnableCaches();
	L1C_EnableBTAC();

#if (__L2C_PRESENT == 1)
	L2C_Enable();
#endif
}

void security_init()
{
	// Disable protection to RCC->BDCR, PWR->CR2, RTC, backup registers
	PWR->CR1 = PWR->CR1 | PWR_CR1_DBP;
	while (!(PWR->CR1 & PWR_CR1_DBP))
		;

	// Disable Trust Zone
	RCC->TZCR = 0;

	// Allow read/write for all securable peripherals (top 7 bits are reserved)
	TZPC->DECPROT0 = 0x01FFFFFF;

	// Allow non-secure access to SYSRAM
	TZPC->TZMA1_SIZE = 0;

	// Disable all tamper detection
	TAMP->CR1 = 0;

	// Allow non-secure access to GPIOZ
	RCC->MP_AHB5ENSETR = RCC_MP_AHB5ENSETR_GPIOZEN;
	GPIOZ->SECCFGR = 0;

	// TZC AXI port 1 and 2 clocks enable
	RCC->MP_APB5ENSETR = RCC_MP_APB5ENSETR_TZC1EN;
	RCC->MP_APB5ENSETR = RCC_MP_APB5ENSETR_TZC2EN;

	// Read/write enable on all nsaid
	TZC->REG_ID_ACCESSO = 0xFFFFFFFF;

	// bit 30 / 31 => Secure Global Enable : write/read
	// bit 0 / 1 => Region Enable for filter 0/1
	TZC->REG_ATTRIBUTESO = TZC->REG_ATTRIBUTESO | (1 << 0) | (1 << 1) | (1 << 30) | (1 << 31);

	// Enable Filter 0 and 1
	TZC->GATE_KEEPER = TZC->GATE_KEEPER | (1 << 0) | (1 << 1);
}
