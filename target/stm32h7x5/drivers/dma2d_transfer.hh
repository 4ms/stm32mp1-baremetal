#ifndef _DMA2D_TRANSFER_HH
#define _DMA2D_TRANSFER_HH

#include "interrupt.hh"
#include "rcc.hh"

namespace mdrivlib
{
namespace stm32h7x5
{

struct DMA2DTransfer {
	static volatile inline bool is_dma2d_done;

	void init() {
		target::RCC_Enable::DMA2D_::set();
		target::System::disable_irq(DMA2D_IRQn);
		InterruptManager::registerISR(DMA2D_IRQn, [&]() {
			DMA2D->IFCR = DMA2D->IFCR | DMA2D_IFCR_CTCIF;
			is_dma2d_done = true;
			target::System::disable_irq(DMA2D_IRQn);
		});
		target::System::set_irq_priority(DMA2D_IRQn, 0, 0);
		is_dma2d_done = true;
	}

	void
	fillrect_rgb565(uintptr_t starting_addr, uint16_t width, uint16_t height, uint16_t screen_width, uint16_t color) {
		DMA2D->NLR = height | (width << DMA2D_NLR_PL_Pos);
		DMA2D->OOR = screen_width - width;
		DMA2D->OMAR = starting_addr;
		DMA2D->OCOLR = color;
		DMA2D->OPFCCR = (0 << DMA2D_OPFCCR_RBS_Pos) | (0 << DMA2D_OPFCCR_AI_Pos) | (0 << DMA2D_OPFCCR_SB_Pos) |
						(0b010 << DMA2D_OPFCCR_CM_Pos);

		DMA2D->AMTCR = 0;
		DMA2D->IFCR = DMA2D_IFCR_CTCIF;
		DMA2D->CR = (0b011 << DMA2D_CR_MODE_Pos) | DMA2D_CR_TCIE; // clear everything else

		is_dma2d_done = false;
		target::System::enable_irq(DMA2D_IRQn);
		DMA2D->CR |= DMA2D_CR_START;

		while (!is_dma2d_done) {
		}
		target::System::disable_irq(DMA2D_IRQn);
	}
};

} // namespace stm32h7x5
} // namespace mdrivlib

#endif
