#pragma once
#include "drivers/cache.hh"
#include "drivers/stm32xx.h"

namespace mdrivlib
{
struct SystemCache {
	static void invalidate_dcache() {
		SCB_InvalidateDCache();
	}

	template<typename ptr>
	static void invalidate_dcache_by_addr(ptr addr) {
		SCB_InvalidateDCache_by_Addr(addr, 4);
	}

	static void invalidate_dcache_by_range(void *addr, int32_t size) {
		SCB_InvalidateDCache_by_Addr(addr, size);
	}

	static void clean_dcache() {
		SCB_CleanDCache();
	}

	static void clean_dcache_by_addr(void *addr) {
		SCB_CleanDCache_by_Addr(reinterpret_cast<uint32_t *>(addr), 4);
	}

	static void clean_dcache_by_range(void *addr, int32_t size) {
		SCB_CleanDCache_by_Addr(reinterpret_cast<uint32_t *>(addr), size);
	}

	static void clean_and_invalidate_dcache_by_range(void *addr, int32_t size) {
		SCB_CleanInvalidateDCache_by_Addr(reinterpret_cast<uint32_t *>(addr), size);
	}
};
} // namespace mdrivlib
