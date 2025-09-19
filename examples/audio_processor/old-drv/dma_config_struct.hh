#pragma once
#include "stm32xx.h"

namespace mdrivlib
{
struct DefaultDMAConf {
	static constexpr unsigned DMAx = 0;
	static constexpr unsigned StreamNum = 0;
	static constexpr unsigned RequestNum = 0;
	static constexpr IRQn_Type IRQn = (IRQn_Type)0;
	static constexpr uint32_t pri = 0;
	static constexpr uint32_t subpri = 0;

	enum Direction { Mem2Mem, Mem2Periph, Periph2Mem, Periph2Periph };
	static constexpr Direction dir = Mem2Periph;

	static constexpr bool circular = false;

	enum TransferSize { Byte, HalfWord, Word };
	static constexpr TransferSize transfer_size_mem = Byte;	   // Dest, in P2P
	static constexpr TransferSize transfer_size_periph = Byte; // Source, in M2M

	// Todo: Double-buffer mode
	enum Priority { Low = 0, Medium = 1, High = 2, VeryHigh = 3 };
	static constexpr uint8_t dma_priority = Low;

	static constexpr bool mem_inc = true;
	static constexpr bool periph_inc = false;

	static constexpr bool half_transfer_interrupt_enable = false;
};

struct DMA_Config {
	DMA_TypeDef *DMAx;
	DMA_Stream_TypeDef *stream;
	uint32_t channel;
	IRQn_Type IRQn;
	uint32_t pri;
	uint32_t subpri;
	bool continuous;
};

struct DMA_LL_Config {
	DMA_TypeDef *DMAx;
	uint32_t stream;
	uint32_t channel;
	IRQn_Type IRQn;
	uint32_t pri;
	uint32_t subpri;
	// bool continuous;
};

// Todo: this should be in some dma manager class

#if defined(HAS_BDMA)

template<typename T>
constexpr volatile uint32_t *dma_get_ISR_reg(T stream) {
	auto s = (uint32_t)stream;
	return (s > (uint32_t)DMA2_Stream7) ? &(BDMA->ISR)
		 : (s > (uint32_t)DMA2_Stream3) ? &(DMA2->HISR)
		 : (s > (uint32_t)DMA1_Stream7) ? &(DMA2->LISR)
		 : (s > (uint32_t)DMA1_Stream3) ? &(DMA1->HISR)
										: &(DMA1->LISR);
}

template<typename T>
constexpr volatile uint32_t *dma_get_IFCR_reg(T stream) {
	auto s = (uint32_t)stream;
	return s > (uint32_t)DMA2_Stream7 ? &(BDMA->IFCR)
		 : s > (uint32_t)DMA2_Stream3 ? &(DMA2->HIFCR)
		 : s > (uint32_t)DMA1_Stream7 ? &(DMA2->LIFCR)
		 : s > (uint32_t)DMA1_Stream3 ? &(DMA1->HIFCR)
									  : &(DMA1->LIFCR);
}

#else

template<typename T>
constexpr volatile uint32_t *dma_get_ISR_reg(T stream) {
	auto s = (uint32_t)stream;
	return (s > (uint32_t)DMA2_Stream3) ? &(DMA2->HISR)
		 : (s > (uint32_t)DMA1_Stream7) ? &(DMA2->LISR)
		 : (s > (uint32_t)DMA1_Stream3) ? &(DMA1->HISR)
										: &(DMA1->LISR);
}

template<typename T>
constexpr volatile uint32_t *dma_get_IFCR_reg(T stream) {
	auto s = (uint32_t)stream;
	return s > (uint32_t)DMA2_Stream3 ? &(DMA2->HIFCR)
		 : s > (uint32_t)DMA1_Stream7 ? &(DMA2->LIFCR)
		 : s > (uint32_t)DMA1_Stream3 ? &(DMA1->HIFCR)
									  : &(DMA1->LIFCR);
}

#endif

template<typename T>
constexpr uint32_t dma_get_TC_flag_index(T stream) {
	auto s = (uint32_t)stream;
	return s == ((uint32_t)DMA1_Stream0) ? DMA_FLAG_TCIF0_4
		 : s == ((uint32_t)DMA2_Stream0) ? DMA_FLAG_TCIF0_4
		 : s == ((uint32_t)DMA1_Stream4) ? DMA_FLAG_TCIF0_4
		 : s == ((uint32_t)DMA2_Stream4) ? DMA_FLAG_TCIF0_4
		 : s == ((uint32_t)DMA1_Stream1) ? DMA_FLAG_TCIF1_5
		 : s == ((uint32_t)DMA2_Stream1) ? DMA_FLAG_TCIF1_5
		 : s == ((uint32_t)DMA1_Stream5) ? DMA_FLAG_TCIF1_5
		 : s == ((uint32_t)DMA2_Stream5) ? DMA_FLAG_TCIF1_5
		 : s == ((uint32_t)DMA1_Stream2) ? DMA_FLAG_TCIF2_6
		 : s == ((uint32_t)DMA2_Stream2) ? DMA_FLAG_TCIF2_6
		 : s == ((uint32_t)DMA1_Stream6) ? DMA_FLAG_TCIF2_6
		 : s == ((uint32_t)DMA2_Stream6) ? DMA_FLAG_TCIF2_6
		 : s == ((uint32_t)DMA1_Stream3) ? DMA_FLAG_TCIF3_7
		 : s == ((uint32_t)DMA2_Stream3) ? DMA_FLAG_TCIF3_7
		 : s == ((uint32_t)DMA1_Stream7) ? DMA_FLAG_TCIF3_7
		 : s == ((uint32_t)DMA2_Stream7) ? DMA_FLAG_TCIF3_7
#if defined(HAS_BDMA)
		 : s == ((uint32_t)BDMA_Channel0) ? BDMA_FLAG_TC0
		 : s == ((uint32_t)BDMA_Channel1) ? BDMA_FLAG_TC1
		 : s == ((uint32_t)BDMA_Channel2) ? BDMA_FLAG_TC2
		 : s == ((uint32_t)BDMA_Channel3) ? BDMA_FLAG_TC3
		 : s == ((uint32_t)BDMA_Channel4) ? BDMA_FLAG_TC4
		 : s == ((uint32_t)BDMA_Channel5) ? BDMA_FLAG_TC5
		 : s == ((uint32_t)BDMA_Channel6) ? BDMA_FLAG_TC6
		 : s == ((uint32_t)BDMA_Channel7) ? BDMA_FLAG_TC7
#endif
										  : 0x00000000;
}

template<typename T>
constexpr uint32_t dma_get_TE_flag_index(T stream) {
	auto s = (uint32_t)stream;
	return s == ((uint32_t)DMA1_Stream0) ? DMA_FLAG_TEIF0_4
		 : s == ((uint32_t)DMA2_Stream0) ? DMA_FLAG_TEIF0_4
		 : s == ((uint32_t)DMA1_Stream4) ? DMA_FLAG_TEIF0_4
		 : s == ((uint32_t)DMA2_Stream4) ? DMA_FLAG_TEIF0_4
		 : s == ((uint32_t)DMA1_Stream1) ? DMA_FLAG_TEIF1_5
		 : s == ((uint32_t)DMA2_Stream1) ? DMA_FLAG_TEIF1_5
		 : s == ((uint32_t)DMA1_Stream5) ? DMA_FLAG_TEIF1_5
		 : s == ((uint32_t)DMA2_Stream5) ? DMA_FLAG_TEIF1_5
		 : s == ((uint32_t)DMA1_Stream2) ? DMA_FLAG_TEIF2_6
		 : s == ((uint32_t)DMA2_Stream2) ? DMA_FLAG_TEIF2_6
		 : s == ((uint32_t)DMA1_Stream6) ? DMA_FLAG_TEIF2_6
		 : s == ((uint32_t)DMA2_Stream6) ? DMA_FLAG_TEIF2_6
		 : s == ((uint32_t)DMA1_Stream3) ? DMA_FLAG_TEIF3_7
		 : s == ((uint32_t)DMA2_Stream3) ? DMA_FLAG_TEIF3_7
		 : s == ((uint32_t)DMA1_Stream7) ? DMA_FLAG_TEIF3_7
		 : s == ((uint32_t)DMA2_Stream7) ? DMA_FLAG_TEIF3_7
#if defined(HAS_BDMA)
		 : s == ((uint32_t)BDMA_Channel0) ? BDMA_FLAG_TE0
		 : s == ((uint32_t)BDMA_Channel1) ? BDMA_FLAG_TE1
		 : s == ((uint32_t)BDMA_Channel2) ? BDMA_FLAG_TE2
		 : s == ((uint32_t)BDMA_Channel3) ? BDMA_FLAG_TE3
		 : s == ((uint32_t)BDMA_Channel4) ? BDMA_FLAG_TE4
		 : s == ((uint32_t)BDMA_Channel5) ? BDMA_FLAG_TE5
		 : s == ((uint32_t)BDMA_Channel6) ? BDMA_FLAG_TE6
		 : s == ((uint32_t)BDMA_Channel7) ? BDMA_FLAG_TE7
#endif
										  : 0x00000000;
}

template<typename T>
constexpr uint32_t dma_get_HT_flag_index(T stream) {
	auto s = (uint32_t)stream;
	return s == ((uint32_t)DMA1_Stream0) ? DMA_FLAG_HTIF0_4
		 : s == ((uint32_t)DMA2_Stream0) ? DMA_FLAG_HTIF0_4
		 : s == ((uint32_t)DMA1_Stream4) ? DMA_FLAG_HTIF0_4
		 : s == ((uint32_t)DMA2_Stream4) ? DMA_FLAG_HTIF0_4
		 : s == ((uint32_t)DMA1_Stream1) ? DMA_FLAG_HTIF1_5
		 : s == ((uint32_t)DMA2_Stream1) ? DMA_FLAG_HTIF1_5
		 : s == ((uint32_t)DMA1_Stream5) ? DMA_FLAG_HTIF1_5
		 : s == ((uint32_t)DMA2_Stream5) ? DMA_FLAG_HTIF1_5
		 : s == ((uint32_t)DMA1_Stream2) ? DMA_FLAG_HTIF2_6
		 : s == ((uint32_t)DMA2_Stream2) ? DMA_FLAG_HTIF2_6
		 : s == ((uint32_t)DMA1_Stream6) ? DMA_FLAG_HTIF2_6
		 : s == ((uint32_t)DMA2_Stream6) ? DMA_FLAG_HTIF2_6
		 : s == ((uint32_t)DMA1_Stream3) ? DMA_FLAG_HTIF3_7
		 : s == ((uint32_t)DMA2_Stream3) ? DMA_FLAG_HTIF3_7
		 : s == ((uint32_t)DMA1_Stream7) ? DMA_FLAG_HTIF3_7
		 : s == ((uint32_t)DMA2_Stream7) ? DMA_FLAG_HTIF3_7
#if defined(HAS_BDMA)
		 : s == ((uint32_t)BDMA_Channel0) ? BDMA_FLAG_HT0
		 : s == ((uint32_t)BDMA_Channel1) ? BDMA_FLAG_HT1
		 : s == ((uint32_t)BDMA_Channel2) ? BDMA_FLAG_HT2
		 : s == ((uint32_t)BDMA_Channel3) ? BDMA_FLAG_HT3
		 : s == ((uint32_t)BDMA_Channel4) ? BDMA_FLAG_HT4
		 : s == ((uint32_t)BDMA_Channel5) ? BDMA_FLAG_HT5
		 : s == ((uint32_t)BDMA_Channel6) ? BDMA_FLAG_HT6
		 : s == ((uint32_t)BDMA_Channel7) ? BDMA_FLAG_HT7
#endif
										  : 0x00000000;
}
} // namespace mdrivlib
