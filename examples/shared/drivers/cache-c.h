#pragma once
#include "core_ca.h"
#include <stdint.h>

#define DCACHEROWSIZE 64

__STATIC_FORCEINLINE void L1_CleanDCache_by_Addr(volatile void *addr, int32_t dsize)
{
	if (dsize > 0) {
		int32_t op_size = dsize + (((uintptr_t)addr) & (DCACHEROWSIZE - 1U));
		uintptr_t op_mva = (uintptr_t)addr;
		__DSB();
		do {
			__set_DCCMVAC(op_mva); // Clean data cache line by address.
			op_mva += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);
		__DMB(); // ensure the ordering of data cache maintenance operations and their effects
	}
}

__STATIC_FORCEINLINE void L1_CleanInvalidateDCache_by_Addr(volatile void *addr, int32_t dsize)
{
	if (dsize > 0) {
		int32_t op_size = dsize + (((uintptr_t)addr) & (DCACHEROWSIZE - 1U));
		uintptr_t op_mva = (uintptr_t)addr;
		__DSB();
		do {
			__set_DCCIMVAC(op_mva); // Clean and Invalidate data cache by address.
			op_mva += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);
		__DMB(); // ensure the ordering of data cache maintenance operations and their effects
	}
}

__STATIC_FORCEINLINE void L1_InvalidateDCache_by_Addr(volatile void *addr, int32_t dsize)
{
	if (dsize > 0) {
		int32_t op_size = dsize + (((uintptr_t)addr) & (DCACHEROWSIZE - 1U));
		uintptr_t op_mva = (uintptr_t)addr;
		do {
			__set_DCIMVAC(op_mva); // Invalidate data cache line by address.
			op_mva += DCACHEROWSIZE;
			op_size -= DCACHEROWSIZE;
		} while (op_size > 0);
		// Cache Invalidate operation is not follow by memory-writes
		__DMB(); // ensure the ordering of data cache maintenance operations and their effects
	}
}
