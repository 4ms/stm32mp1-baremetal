#pragma once
#include "arch.hh"
#include "dma_config_struct.hh"
#include "interrupt.hh"
#include "rcc.hh"
#include "stm32xx.h"

// Todo: Finish converting to using CMSIS instead of STM32-HAL
namespace mdrivlib
{
template<typename ConfT>
struct DMATransfer {
	uint32_t _src_addr;
	uint32_t _dst_addr;
	uint32_t _transfer_size;

	DMA_TypeDef *dmax;
	DMA_Stream_TypeDef *stream;
	volatile uint32_t *dma_ifcr_reg;
	volatile uint32_t *dma_isr_reg;
	uint32_t dma_tc_flag_index;
	uint32_t dma_ht_flag_index;
	uint32_t dma_te_flag_index;

	DMAMUX_Channel_TypeDef *dmamux_chan;

	// ST-HAL:
	DMA_HandleTypeDef hdma_tx;

	DMATransfer() {
		RCC_Enable::DMAMUX_::set();

		if constexpr (ConfT::DMAx == 1) {
			RCC_Enable::DMA1_::set();

			if constexpr (ConfT::StreamNum == 0) {
				stream = DMA1_Stream0;
				dmamux_chan = DMAMUX1_Channel0;
			}
			if constexpr (ConfT::StreamNum == 1) {
				stream = DMA1_Stream1;
				dmamux_chan = DMAMUX1_Channel1;
			}
			if constexpr (ConfT::StreamNum == 2) {
				stream = DMA1_Stream2;
				dmamux_chan = DMAMUX1_Channel2;
			}
			if constexpr (ConfT::StreamNum == 3) {
				stream = DMA1_Stream3;
				dmamux_chan = DMAMUX1_Channel3;
			}
			if constexpr (ConfT::StreamNum == 4) {
				stream = DMA1_Stream4;
				dmamux_chan = DMAMUX1_Channel4;
			}
			if constexpr (ConfT::StreamNum == 5) {
				stream = DMA1_Stream5;
				dmamux_chan = DMAMUX1_Channel5;
			}
			if constexpr (ConfT::StreamNum == 6) {
				stream = DMA1_Stream6;
				dmamux_chan = DMAMUX1_Channel6;
			}
			if constexpr (ConfT::StreamNum == 7) {
				stream = DMA1_Stream7;
				dmamux_chan = DMAMUX1_Channel7;
			}
		}

		if constexpr (ConfT::DMAx == 2) {
			RCC_Enable::DMA2_::set();

			if constexpr (ConfT::StreamNum == 0) {
				stream = DMA2_Stream0;
				dmamux_chan = DMAMUX1_Channel0;
			}
			if constexpr (ConfT::StreamNum == 1) {
				stream = DMA2_Stream1;
				dmamux_chan = DMAMUX1_Channel1;
			}
			if constexpr (ConfT::StreamNum == 2) {
				stream = DMA2_Stream2;
				dmamux_chan = DMAMUX1_Channel2;
			}
			if constexpr (ConfT::StreamNum == 3) {
				stream = DMA2_Stream3;
				dmamux_chan = DMAMUX1_Channel3;
			}
			if constexpr (ConfT::StreamNum == 4) {
				stream = DMA2_Stream4;
				dmamux_chan = DMAMUX1_Channel4;
			}
			if constexpr (ConfT::StreamNum == 5) {
				stream = DMA2_Stream5;
				dmamux_chan = DMAMUX1_Channel5;
			}
			if constexpr (ConfT::StreamNum == 6) {
				stream = DMA2_Stream6;
				dmamux_chan = DMAMUX1_Channel6;
			}
			if constexpr (ConfT::StreamNum == 7) {
				stream = DMA2_Stream7;
				dmamux_chan = DMAMUX1_Channel7;
			}
		}
		dma_tc_flag_index = dma_get_TC_flag_index(stream);
		dma_te_flag_index = dma_get_TE_flag_index(stream);
		dma_te_flag_index = dma_get_HT_flag_index(stream);
		dma_isr_reg = dma_get_ISR_reg(stream);
		dma_ifcr_reg = dma_get_IFCR_reg(stream);
	}

	template<typename CallbackT>
	void register_callback(CallbackT &&callback) {
		InterruptControl::disable_irq(ConfT::IRQn);
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
				// __BKPT();
				*dma_ifcr_reg = dma_te_flag_index;
				// Error: debug breakpoint or logging here
			}
		});
	}

	void config_transfer(uint32_t dst, uint32_t src, size_t sz) {
		hdma_tx.Init.Request = ConfT::RequestNum;
		hdma_tx.Instance = stream;
		hdma_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_tx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		hdma_tx.Init.Mode = DMA_NORMAL;
		hdma_tx.Init.Priority = DMA_PRIORITY_LOW;
		hdma_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_tx.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;

		// __HAL_LINKDMA(&hsai_tx, hdmatx, hdma_tx);
		// hdma_tx.Parent = &hsai_tx;

		HAL_DMA_DeInit(&hdma_tx);
		HAL_DMA_Init(&hdma_tx);
		// stream->CR = stream->CR | DMA_SxCR_PFCTRL;

		_transfer_size = sz;
		_src_addr = src;
		_dst_addr = dst;

		// BDMA_::ClearGlobalISRFlag::set();
		// BDMA_::ClearHalfTransferComplISRFlag::set();
		// BDMA_::ClearTransferComplISRFlag::set();
		// BDMA_::ClearTransferErrorISRFlag::set();

		// stream->CCR &= ~BDMA_CCR_EN;
		// uint32_t _ccr = 0;
		// _ccr |= BDMA_CCR_TCIE | BDMA_CCR_TEIE;

		// if constexpr (ConfT::half_transfer_interrupt_enable)
		// 	_ccr |= BDMA_CCR_HTIE;

		// if constexpr (ConfT::dir == ConfT::Mem2Mem) {
		// 	_ccr |= BDMA_CCR_MEM2MEM;
		// 	stream->CM0AR = dst;
		// 	stream->CPAR = src;
		// }
		// if constexpr (ConfT::dir == ConfT::Mem2Periph) {
		// 	_ccr |= BDMA_CCR_DIR;
		// 	stream->CM0AR = src;
		// 	stream->CPAR = dst;
		// }
		// if constexpr (ConfT::dir == ConfT::Periph2Mem) {
		// 	stream->CM0AR = dst;
		// 	stream->CPAR = src;
		// }
		// if constexpr (ConfT::dir == ConfT::Periph2Periph) {
		// 	stream->CM0AR = dst;
		// 	stream->CPAR = src;
		// }

		// if constexpr (ConfT::circular)
		// 	_ccr |= BDMA_CCR_CIRC;

		// _ccr |= ConfT::dma_priority << BDMA_CCR_PL_Pos;

		// if constexpr (ConfT::transfer_size_mem == ConfT::HalfWord)
		// 	_ccr |= 0b01 << BDMA_CCR_MSIZE_Pos;
		// if constexpr (ConfT::transfer_size_mem == ConfT::Word)
		// 	_ccr |= 0b10 << BDMA_CCR_MSIZE_Pos;
		// if constexpr (ConfT::transfer_size_periph == ConfT::HalfWord)
		// 	_ccr |= 0b01 << BDMA_CCR_PSIZE_Pos;
		// if constexpr (ConfT::transfer_size_periph == ConfT::Word)
		// 	_ccr |= 0b10 << BDMA_CCR_PSIZE_Pos;
		// // 1 byte: no action

		// if constexpr (ConfT::mem_inc)
		// 	_ccr |= BDMA_CCR_MINC;
		// if constexpr (ConfT::periph_inc)
		// 	_ccr |= BDMA_CCR_PINC;

		// // Todo:
		// // DBM = 0 //double buffer mode
		// // CT = 0 //current target

		// stream->CCR = _ccr;

		// init_DMAMUX();
	}

	void config_transfer(void *dst, const void *src, size_t sz) {
		config_transfer(reinterpret_cast<uint32_t>(dst), reinterpret_cast<uint32_t>(src), sz);
	}

	void start_transfer() {
		InterruptControl::enable_irq(ConfT::IRQn);
		HAL_DMA_Start_IT(&hdma_tx, _src_addr, _dst_addr, _transfer_size / 2);

		// BDMA_::ClearGlobalISRFlag::set();
		// BDMA_::ClearHalfTransferComplISRFlag::set();
		// BDMA_::TransferComplISRFlag::set();
		// BDMA_::TransferErrorISRFlag::set();
		// HAL_NVIC_EnableIRQ(ConfT::BDMAConf::IRQn);

		// stream->CCR &= ~BDMA_CCR_EN;

		// DMAMUX_::ClearFlag::write(1 << ConfT::StreamNum);
		// init_DMAMUX_request();

		// stream->CNDTR = _transfer_size;
		// stream->CCR |= BDMA_CCR_EN;
	}

	uint32_t get_transfer_size() {
		return _transfer_size;
	}

	void init_DMAMUX() {
		// if constexpr (ConfT::dir == ConfT::Mem2Mem)
		// 	dmamux_chan->CCR = 0;
		// else
		// 	dmamux_chan->CCR = ConfT::RequestNum & DMAMUX_CxCR_DMAREQ_ID_Msk;

		// DMAMUX_::ClearFlag::write(1 << ConfT::StreamNum);
		// init_DMAMUX_request();
	}

	void init_DMAMUX_request() {
		// if constexpr (ConfT::RequestNum >= 0 && ConfT::RequestNum <= 7) {
		// 	// These ones are special somehow...Todo: why?
		// 	auto DMAmuxRequestGen = (DMAMUX_RequestGen_TypeDef *)((uint32_t)(((uint32_t)DMAMUX1_RequestGenerator0) +
		// 																	 ((ConfT::RequestNum - 1U) * 4U)));
		// 	DMAmuxRequestGen->RGCR = 0;
		// 	DMAMUX1_RequestGenStatus->RGCFR = 1 << (ConfT::Request - 1);
		// }
	}

private:
	// struct BDMA_ {
	// 	static constexpr uint32_t ISR_Base = BDMA_BASE + offsetof(BDMA_TypeDef, ISR);
	// 	using GlobalISRFlag = RegisterSection<ReadWrite, ISR_Base, BDMA_ISR_GIF0_Pos + ConfT::StreamNum, 1>;
	// 	using TransferComplISRFlag = RegisterSection<ReadWrite, ISR_Base, BDMA_ISR_TCIF0_Pos + ConfT::StreamNum, 1>;
	// 	using HalfTransferComplISRFlag = RegisterSection<ReadWrite, ISR_Base, BDMA_ISR_HTIF0_Pos + ConfT::StreamNum, 1>;
	// 	using TransferErrorISRFlag = RegisterSection<ReadWrite, ISR_Base, BDMA_ISR_TEIF0_Pos + ConfT::StreamNum, 1>;

	// 	static constexpr uint32_t IFCR_Base = BDMA_BASE + offsetof(BDMA_TypeDef, IFCR);
	// 	using ClearGlobalISRFlag = RegisterSection<WriteOnly, IFCR_Base, BDMA_IFCR_CGIF0 + ConfT::StreamNum, 1>;
	// 	using ClearTransferComplISRFlag =
	// 		RegisterSection<WriteOnly, IFCR_Base, BDMA_IFCR_CTCIF0_Pos + ConfT::StreamNum, 1>;
	// 	using ClearHalfTransferComplISRFlag =
	// 		RegisterSection<WriteOnly, IFCR_Base, BDMA_IFCR_CHTIF0_Pos + ConfT::StreamNum, 1>;
	// 	using ClearTransferErrorISRFlag =
	// 		RegisterSection<WriteOnly, IFCR_Base, BDMA_IFCR_CTEIF0_Pos + ConfT::StreamNum, 1>;
	// };
	// struct DMAMUX_ {
	// 	using ClearFlag = RegisterBits<WriteOnly, DMAMUX1_BASE, 0x000000FF>;
	// };
};
} // namespace mdrivlib
