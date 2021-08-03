#ifndef _DMA2D_TRANSFER_HH
#define _DMA2D_TRANSFER_HH

#include "interrupt.hh"
#include "rcc.hh"

namespace mdrivlib
{

struct DMA2DTransfer {
	static volatile inline bool is_dma2d_done = true;

	void init() {
	}

	// FixMe: Why does this occasionally mis some bits? Copying 0xFFFF to a region sometimes copies 0x003F
	// for a lower portion of the region. Even if the memory is non-cacheable.
	void
	fillrect_rgb565(uintptr_t starting_addr, uint16_t width, uint16_t height, uint16_t screen_width, uint16_t color) {
		uint16_t *frameptr = reinterpret_cast<uint16_t *>(starting_addr);

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				*frameptr = color;
				frameptr++;
			}
			frameptr += screen_width - width;
		}
	}
};
} // namespace mdrivlib

#endif
