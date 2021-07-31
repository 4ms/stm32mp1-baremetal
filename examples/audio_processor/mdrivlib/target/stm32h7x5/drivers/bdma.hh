#pragma once
#include "arch.hh"
#include "dma_config_struct.hh"
#include "interrupt.hh"
#include "rcc.hh"
#include "stm32xx.h"
#include <cstddef>

namespace mdrivlib
{
template<typename ConfT>
struct BDMATransfer {
	uint32_t _src_addr;
	uint32_t _dst_addr;
	uint32_t _transfer_size;

	BDMA_Channel_TypeDef *stream;
	volatile uint32_t *dma_ifcr_reg;
	volatile uint32_t *dma_isr_reg;
	uint32_t dma_tc_flag_index;
	uint32_t dma_ht_flag_index;
	uint32_t dma_te_flag_index;

	DMAMUX_Channel_TypeDef *dmamux_chan;

	BDMATransfer() {
		RCC_Enable::BDMA_::set();

		if constexpr (ConfT::StreamNum == 0) {
			stream = BDMA_Channel0;
			dmamux_chan = DMAMUX2_Channel0;
		}
		if constexpr (ConfT::StreamNum == 1) {
			stream = BDMA_Channel1;
			dmamux_chan = DMAMUX2_Channel1;
		}
		if constexpr (ConfT::StreamNum == 2) {
			stream = BDMA_Channel2;
			dmamux_chan = DMAMUX2_Channel2;
		}
		if constexpr (ConfT::StreamNum == 3) {
			stream = BDMA_Channel3;
			dmamux_chan = DMAMUX2_Channel3;
		}
		if constexpr (ConfT::StreamNum == 4) {
			stream = BDMA_Channel4;
			dmamux_chan = DMAMUX2_Channel4;
		}
		if constexpr (ConfT::StreamNum == 5) {
			stream = BDMA_Channel5;
			dmamux_chan = DMAMUX2_Channel5;
		}
		if constexpr (ConfT::StreamNum == 6) {
			stream = BDMA_Channel6;
			dmamux_chan = DMAMUX2_Channel6;
		}
		if constexpr (ConfT::StreamNum == 7) {
			stream = BDMA_Channel7;
			dmamux_chan = DMAMUX2_Channel7;
		}

		dma_tc_flag_index = dma_get_TC_flag_index(stream);
		dma_te_flag_index = dma_get_TE_flag_index(stream);
		dma_te_flag_index = dma_get_HT_flag_index(stream);
		dma_isr_reg = dma_get_ISR_reg(stream);
		dma_ifcr_reg = dma_get_IFCR_reg(stream);
	}

	template<typename CallbackT>
	void register_callback(CallbackT &&callback) {
		HAL_NVIC_DisableIRQ(ConfT::IRQn);
		InterruptManager::registerISR(ConfT::IRQn, ConfT::pri, ConfT::subpri, [callback, this]() {
			if (*dma_isr_reg & dma_tc_flag_index) {
				*dma_ifcr_reg = dma_tc_flag_index;
				if constexpr (ConfT::half_transfer_interrupt_enable)
					callback(1);
				else
					callback();
			}
			if constexpr (ConfT::half_transfer_interrupt_enable) {
				if (*dma_isr_reg & dma_ht_flag_index) {
					*dma_ifcr_reg = dma_ht_flag_index;
					callback(0);
				}
			}
			if (*dma_isr_reg & dma_te_flag_index) {
				*dma_ifcr_reg = dma_te_flag_index;
				// Error: debug breakpoint or logging here
			}
		});
	}

	void config_transfer(uint32_t dst, uint32_t src, size_t sz) {
		if constexpr (ConfT::transfer_size_periph == ConfT::Byte)
			_transfer_size = sz;
		if constexpr (ConfT::transfer_size_periph == ConfT::HalfWord)
			_transfer_size = sz / 2;
		if constexpr (ConfT::transfer_size_periph == ConfT::Word)
			_transfer_size = sz / 4;

		_transfer_size = (_transfer_size > 0xFFFF) ? 0xFFFF : _transfer_size;
		_src_addr = src;
		_dst_addr = dst;

		BDMA_::ClearGlobalISRFlag::set();
		BDMA_::ClearHalfTransferComplISRFlag::set();
		BDMA_::ClearTransferComplISRFlag::set();
		BDMA_::ClearTransferErrorISRFlag::set();

		stream->CCR &= ~BDMA_CCR_EN;
		uint32_t _ccr = 0;
		_ccr |= BDMA_CCR_TCIE | BDMA_CCR_TEIE;

		if constexpr (ConfT::half_transfer_interrupt_enable)
			_ccr |= BDMA_CCR_HTIE;

		if constexpr (ConfT::dir == ConfT::Mem2Mem) {
			_ccr |= BDMA_CCR_MEM2MEM;
			stream->CM0AR = dst;
			stream->CPAR = src;
		}
		if constexpr (ConfT::dir == ConfT::Mem2Periph) {
			_ccr |= BDMA_CCR_DIR;
			stream->CM0AR = src;
			stream->CPAR = dst;
		}
		if constexpr (ConfT::dir == ConfT::Periph2Mem) {
			stream->CM0AR = dst;
			stream->CPAR = src;
		}
		if constexpr (ConfT::dir == ConfT::Periph2Periph) {
			stream->CM0AR = dst;
			stream->CPAR = src;
		}

		if constexpr (ConfT::circular)
			_ccr |= BDMA_CCR_CIRC;

		_ccr |= ConfT::dma_priority << BDMA_CCR_PL_Pos;

		if constexpr (ConfT::transfer_size_mem == ConfT::HalfWord)
			_ccr |= 0b01 << BDMA_CCR_MSIZE_Pos;
		if constexpr (ConfT::transfer_size_mem == ConfT::Word)
			_ccr |= 0b10 << BDMA_CCR_MSIZE_Pos;
		if constexpr (ConfT::transfer_size_periph == ConfT::HalfWord)
			_ccr |= 0b01 << BDMA_CCR_PSIZE_Pos;
		if constexpr (ConfT::transfer_size_periph == ConfT::Word)
			_ccr |= 0b10 << BDMA_CCR_PSIZE_Pos;
		// 1 byte: no action

		if constexpr (ConfT::mem_inc)
			_ccr |= BDMA_CCR_MINC;
		if constexpr (ConfT::periph_inc)
			_ccr |= BDMA_CCR_PINC;

		// Todo:
		// DBM = 0 //double buffer mode
		// CT = 0 //current target

		stream->CCR = _ccr;

		init_DMAMUX();
	}

	void config_transfer(void *dst, const void *src, size_t sz) {
		config_transfer(reinterpret_cast<uint32_t>(dst), reinterpret_cast<uint32_t>(src), sz);
	}

	void start_transfer() {
		BDMA_::ClearGlobalISRFlag::set();
		BDMA_::ClearHalfTransferComplISRFlag::set();
		BDMA_::TransferComplISRFlag::set();
		BDMA_::TransferErrorISRFlag::set();
		HAL_NVIC_EnableIRQ(ConfT::IRQn);

		stream->CCR &= ~BDMA_CCR_EN;

		DMAMUX_::ClearFlag::write(1 << ConfT::StreamNum);
		init_DMAMUX_request();

		stream->CNDTR = _transfer_size;
		stream->CCR |= BDMA_CCR_EN;
	}

	uint32_t get_transfer_size() {
		return _transfer_size;
	}

	void init_DMAMUX() {
		if constexpr (ConfT::dir == ConfT::Mem2Mem)
			dmamux_chan->CCR = 0;
		else
			dmamux_chan->CCR = ConfT::RequestNum & DMAMUX_CxCR_DMAREQ_ID_Msk;

		DMAMUX_::ClearFlag::write(1 << ConfT::StreamNum);
		init_DMAMUX_request();
	}

	void init_DMAMUX_request() {
		if constexpr (ConfT::RequestNum >= 0 && ConfT::RequestNum <= 7) {
			// These ones are special somehow...Todo: why?
			auto DMAmuxRequestGen = (DMAMUX_RequestGen_TypeDef *)((uint32_t)(((uint32_t)DMAMUX2_RequestGenerator0) +
																			 ((ConfT::RequestNum - 1U) * 4U)));
			DMAmuxRequestGen->RGCR = 0;
			DMAMUX2_RequestGenStatus->RGCFR = 1 << (ConfT::Request - 1);
		}
	}

private:
	struct BDMA_ {
		static constexpr uint32_t ISR_Base = BDMA_BASE + offsetof(BDMA_TypeDef, ISR);
		using GlobalISRFlag = RegisterSection<ReadWrite, ISR_Base, BDMA_ISR_GIF0_Pos + ConfT::StreamNum, 1>;
		using TransferComplISRFlag = RegisterSection<ReadWrite, ISR_Base, BDMA_ISR_TCIF0_Pos + ConfT::StreamNum, 1>;
		using HalfTransferComplISRFlag = RegisterSection<ReadWrite, ISR_Base, BDMA_ISR_HTIF0_Pos + ConfT::StreamNum, 1>;
		using TransferErrorISRFlag = RegisterSection<ReadWrite, ISR_Base, BDMA_ISR_TEIF0_Pos + ConfT::StreamNum, 1>;

		static constexpr uint32_t IFCR_Base = BDMA_BASE + offsetof(BDMA_TypeDef, IFCR);
		using ClearGlobalISRFlag = RegisterSection<WriteOnly, IFCR_Base, BDMA_IFCR_CGIF0 + ConfT::StreamNum, 1>;
		using ClearTransferComplISRFlag =
			RegisterSection<WriteOnly, IFCR_Base, BDMA_IFCR_CTCIF0_Pos + ConfT::StreamNum, 1>;
		using ClearHalfTransferComplISRFlag =
			RegisterSection<WriteOnly, IFCR_Base, BDMA_IFCR_CHTIF0_Pos + ConfT::StreamNum, 1>;
		using ClearTransferErrorISRFlag =
			RegisterSection<WriteOnly, IFCR_Base, BDMA_IFCR_CTEIF0_Pos + ConfT::StreamNum, 1>;
	};
	struct DMAMUX_ {
		using ClearFlag = RegisterBits<WriteOnly, DMAMUX2_BASE, 0x000000FF>;
	};
};
} // namespace mdrivlib
