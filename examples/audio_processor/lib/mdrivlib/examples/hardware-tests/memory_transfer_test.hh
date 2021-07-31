#pragma once
#include "drivers/memory_transfer.hh"

namespace mdrivlib
{
namespace HardwareExample
{

struct MemoryTransferByteSwapTest {
	static void run(void *src, void *dst, size_t num_bytes) {
		volatile bool success = false;

		auto srcbuf = reinterpret_cast<uint16_t *>(src);
		size_t i = num_bytes;
		uint32_t x = 0;
		while (i--)
			*srcbuf++ = x++;

		// Debug::Pin2::high();
		mem_xfer.config_transfer(dst, src, num_bytes);
		mem_xfer.register_callback([&success]() {
			// Debug::Pin2::low();
			success = true;
		});
		mem_xfer.start_transfer();

		while (success == false)
			;
	}

	struct ScreenMemXferConfT : MemoryTransferDefaultConfT {
		static constexpr unsigned channel = 0;
		static constexpr bool swap_bytes = true;
		static constexpr bool bufferable_write_mode = true;
		static constexpr uint32_t PriorityLevel = Medium;
		static constexpr uint32_t src_burst = 0b100;
		static constexpr uint32_t dst_burst = 0b100;
	};
	static inline MemoryTransfer<ScreenMemXferConfT> mem_xfer;
};

} // namespace HardwareExample
} // namespace mdrivlib
