#pragma once
#include "drivers/stm32xx.h"

namespace mdrivlib::SystemCache
{
// Data clean by VMA to point of unification
static inline void __set_DCCMVAU(uint32_t value) {
	__set_CP(15, 0, value, 7, 11, 1);
}

inline void invalidate_dcache() {
	L1C_InvalidateDCacheAll();
}

template<typename ptr>
inline void invalidate_dcache_by_addr(ptr addr) {
	L1C_InvalidateDCacheMVA(reinterpret_cast<void *>(addr));
}

inline void invalidate_dcache_by_range(void *addr, int32_t size) {
	auto addr_line = reinterpret_cast<uint32_t>(addr) & ~0b11111; // just keep Tag and Set/Index
	while (size > 0) {
		__set_DCIMVAC(addr_line);
		addr_line += 0b100000;
		size -= 4;
	}
	__DMB();
}

inline void clean_dcache() {
	L1C_CleanDCacheAll();
}

template<typename ptr>
inline void clean_dcache_by_addr(ptr addr) {
	L1C_CleanDCacheMVA(reinterpret_cast<void *>(addr));
}

inline void clean_dcache_by_range(void *addr, int32_t size) {
	auto addr_line = reinterpret_cast<uint32_t>(addr) & ~0b11111; // just keep Tag and Set/Index
	// auto *u32_ptr = reinterpret_cast<uint32_t *>(addr_line);
	// auto *u32_ptr = reinterpret_cast<uint32_t *>(addr);
	while (size > 0) {
		__set_DCCMVAC(addr_line);
		addr_line += 0b100000;
		// __set_DCCMVAC((uint32_t)u32_ptr);
		// u32_ptr += 1;
		size -= 4;
	}
	__DMB();
}

inline void clean_and_invalidate_dcache_by_range(void *addr, int32_t size) {
	auto *u32_ptr = reinterpret_cast<uint32_t *>(addr);
	while (size > 0) {
		__set_DCCIMVAC((uint32_t)u32_ptr);
		u32_ptr += 1;
		size -= 4;
	}
	__DMB();
}
} // namespace mdrivlib::SystemCache
