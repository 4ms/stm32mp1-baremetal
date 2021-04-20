#include "irq_ctrl.h"
#include "stm32mp1xx.h"

#ifndef HSE_VALUE
#define HSE_VALUE 24000000
#endif

uint32_t SystemCoreClock = HSE_VALUE;

void SystemInit(void) {
	// Do not use global variables because this function is called in startup.s
	// and globals might not yet be initialized.

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

	IRQ_Initialize();
}
