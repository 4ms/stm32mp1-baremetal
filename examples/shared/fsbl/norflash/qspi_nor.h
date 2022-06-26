#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t QSPI_read_SIO(uint8_t *pData, uint32_t read_addr, uint32_t num_bytes);

#ifdef __cplusplus
}
#endif
