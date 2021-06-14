#include "stm32xx.h"

extern "C" {
uint32_t HAL_GetTick(void) {
	// floats divide faster than ints
	// return (uint32_t)((float)PL1_GetCurrentPhysicalValue() / ((float)HSE_VALUE / 1000.f));
	return (PL1_GetCurrentPhysicalValue() / (HSE_VALUE / 1000));
}
}
