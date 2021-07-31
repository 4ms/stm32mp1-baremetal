#pragma once
#include "rcc.hh"
#include <cstring>

namespace mdrivlib
{
struct CoproController {

	// Allow MCU to boot if it's reset.
	// If it's running, do nothing.
	static void start() {
		RCC->MP_GCR = RCC->MP_GCR | RCC_MP_GCR_BOOT_MCU;
	}

	// Stop and reset MCU.
	// TODO: It's recommended to put the MCU to sleep first.
	static void reset() {
		// HOLD BOOT for MCU (prevents it from starting up after a reset)
		RCC->MP_GCR = RCC->MP_GCR & ~RCC_MP_GCR_BOOT_MCU;

		// Reset MCU
		RCC->MP_GRSTCSETR = RCC_MP_GRSTCSETR_MCURST;
	}

	// Loads the given data to M4's 0x00000000 (which is 0x38000000 as seen by the A7 core).
	// len is the number of bytes
	static void load_vector_data(const unsigned char *data, uint32_t len) {
		std::memcpy((void *)0x38000000, data, len);
	}

	// Loads the given data to SRAM1 (0x30000000)
	// len is the number of bytes
	static void load_firmware_data(const unsigned char *data, uint32_t len) {
		std::memcpy((void *)0x30000000, data, len);
	}
};

using Copro = CoproController;

} // namespace mdrivlib
