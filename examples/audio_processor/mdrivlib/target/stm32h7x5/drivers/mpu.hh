#pragma once
#include "stm32xx.h"
#include "util/math.hh"

namespace mdrivlib
{
struct MPU_ {
	static void reset() {
		_region_ctr = 0;
	}
	static void disable() {
		// Make sure outstanding transfers are done
		__DMB();
		// Disable fault exceptions
		SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;
		// Disable the MPU and clear the control register
		MPU->CTRL = 0;
	}

	static void enable() {
		MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_ENABLE_Msk;
		SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
		__DSB();
		__ISB();
	}

private:
	static inline uint32_t _region_ctr = 0;

	// Settings:
	static const uint32_t INSTR_ACCESS_DISABLE = 0;
	static const uint32_t REGION_FULL_ACCESS = 0b011;
	static const uint32_t ACCESS_SHAREABLE = 1;
	static const uint32_t ACCESS_NOT_CACHEABLE = 0;
	static const uint32_t ACCESS_NOT_BUFFERABLE = 0;
	static const uint32_t ENABLE_ALL_SUBREGIONS = 0x00;

public:
	// Returns region_id
	static int32_t disable_cache_for_dma_buffer(uint32_t baseaddr, uint32_t region_size) {
		auto region_id = _disable_cache_for_dma_buffer(baseaddr, region_size, _region_ctr);
		_region_ctr++;
		return region_id;
	}

	static int32_t _disable_cache_for_dma_buffer(uint32_t baseaddr, uint32_t region_size, uint32_t region_id) {
		if (_region_ctr >= (MPU->TYPE & MPU_TYPE_DREGION_Msk) >> MPU_TYPE_DREGION_Pos)
			return -1; // ERROR! too many MPU regions defined

		disable();

		uint32_t log2_region_size = MathTools::log2_floor(region_size);
		if (MathTools::is_power_of_2(region_size))
			log2_region_size--;

		// Mimimum size is 32B, SIZE=4
		if (log2_region_size < 0b100)
			log2_region_size = 0b100;

		MPU->RNR = region_id;

		// Method 1: VALID = 0, REGION is ignored OR update base addr for region in RNR
		MPU->RBAR = baseaddr & MPU_RBAR_ADDR_Msk;

		// Method 2: VALID = 1, Update RNR to value in REGION --OR update base addr for region in REGION
		// MPU->RBAR = (baseaddr & MPU_RBAR_ADDR_Msk) & (MPU_RBAR_VALID_Msk) & (region_id << MPU_RBAR_REGION_Pos);

		// clang-format off
		MPU->RASR = (INSTR_ACCESS_DISABLE << MPU_RASR_XN_Pos) |
					(REGION_FULL_ACCESS << MPU_RASR_AP_Pos) |
					(0b001 << MPU_RASR_TEX_Pos) |
					(ACCESS_NOT_CACHEABLE << MPU_RASR_C_Pos) |
					(ACCESS_NOT_BUFFERABLE << MPU_RASR_B_Pos) |
					(ACCESS_SHAREABLE << MPU_RASR_S_Pos) |
					(ENABLE_ALL_SUBREGIONS << MPU_RASR_SRD_Pos) |
					(log2_region_size << MPU_RASR_SIZE_Pos) |
					(0x01 << MPU_RASR_ENABLE_Pos);
		// clang-format on
		enable();

		/* Try this:
			; R1 = region number
			; R2 = size/enable
			; R3 = attributes
			; R4 = address
			LDR R0,=MPU_RNR 	; 0xE000ED98, MPU region number register
			STR R1, [R0, #0x0] 	; Region Number
			STR R4, [R0, #0x4] 	; Region Base Address
			LSL R3, R3, #16 	; Move attributes to upper half of word
			ORR R2, R2, R3 		; Combine attributes and size/enable
			STR R2, [R0, #0x8] 	; Update MPU_RASR

			//or:
			; R1 = region number
			; R2 = address
			; R3 = size, attributes in one
			LDR R0,=MPU_RNR 	; 0xE000ED98, MPU region number register
			STR R1, [R0, #0x0] 	; Region Number
			STR R2, [R0, #0x4] 	; Region Base Address
			STR R3, [R0, #0x8] 	; Update MPU_RASR

			//or:
			LDR R0,=MPU_RNR 	; 0xE000ED98, MPU region number register
			STM R0, {R1-R3} 	; Region Number, address, attribute, size and enable
		*/

		return _region_ctr;
	}
};
} // namespace mdrivlib
