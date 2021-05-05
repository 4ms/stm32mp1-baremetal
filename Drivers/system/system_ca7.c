#include "irq_ctrl.h"
#include "mmu_ca7.h"
#include "stm32mp1xx.h"

uint32_t SystemCoreClock = HSE_VALUE;

void SystemInit(void)
{
	// do not use global variables because this function is called before
	// reaching pre-main. RW section may be overwritten afterwards.

	RCC->APB1RSTSETR = 0xFFFFFFFF;
	RCC->APB1RSTCLRR = 0xFFFFFFFF;

	RCC->APB2RSTSETR = 0xFFFFFFFF;
	RCC->APB2RSTCLRR = 0xFFFFFFFF;

	RCC->APB3RSTSETR = 0xFFFFFFFF;
	RCC->APB3RSTCLRR = 0xFFFFFFFF;

	RCC->APB4RSTSETR = 0xFFFFFFFF;
	RCC->APB4RSTCLRR = 0xFFFFFFFF;

	RCC->AHB2RSTSETR = 0xFFFFFFFF;
	RCC->AHB2RSTCLRR = 0xFFFFFFFF;

	RCC->AHB4RSTSETR = 0xFFFFFFFF;
	RCC->AHB4RSTCLRR = 0xFFFFFFFF;

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

	// #if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
	__FPU_Enable();

	MMU_CreateTranslationTable();
	MMU_Enable();

	L1C_EnableCaches();
	L1C_EnableBTAC();

#if (__L2C_PRESENT == 1)
	L2C_Enable();
#endif

	IRQ_Initialize();
}
