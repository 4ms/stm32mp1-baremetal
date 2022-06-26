#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int stm32mp1_ddr_setup(void);
uint32_t stm32mp1_ddr_get_size();

#ifdef __cplusplus
}
#endif
