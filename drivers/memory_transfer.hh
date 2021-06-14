#pragma once
#include "arch.hh"
#include "interrupt.hh"
#include "mdma_registers.hh"
#include "rcc.hh"
#include "stm32xx.h"

#include <cstddef>
struct MemoryTransferDefaultConfT {
	static constexpr unsigned channel = 0;

	// Endianess
	static constexpr bool swap_bytes = false;
	static constexpr bool swap_halfwords = false;
	static constexpr bool swap_words = false;

	// Data sizes
	enum Sizes { Byte = 0b00, HalfWord = 0b01, Word = 0b10, DoubleWord = 0b11 };
	static constexpr uint32_t src_data_size = Word;
	static constexpr uint32_t dst_data_size = Word;
	static constexpr uint32_t src_data_inc = Word;
	static constexpr uint32_t dst_data_inc = Word;
	static constexpr uint32_t src_burst = 0b000;
	static constexpr uint32_t dst_burst = 0b000;

	enum Directions { DoNotInc = 0b00, Up = 0b10, Down = 0b11 };
	static constexpr uint32_t src_data_dir = Up;
	static constexpr uint32_t dst_data_dir = Up;

	enum Priority { Low = 0b00, Medium = 0b01, High = 0b10, VeryHigh = 0b11 };
	static constexpr uint32_t PriorityLevel = Medium;

	static constexpr bool bufferable_write_mode = false;
};

namespace mdrivlib
{
// Todo: create `register_channel_isr` and `enable_global_isr` like HSEM does, to handle multiple MemoryTransfer active
// at once, on different channels.
// - Add ability to choose which ISR flags are set (CTC, BRTC, BTC, etc), and which ISR gets the callback (or is it
// always CTC?)
template<typename ConfT = MemoryTransferDefaultConfT>
struct MemoryTransfer {
	using MDMAX = target::MDMAx<ConfT::channel>;

	uint32_t _src_addr;
	uint32_t _dst_addr;
	uint32_t _transfer_block_size;
	uint32_t _num_blocks;

	MemoryTransfer() {
	}

	template<typename CallbackT>
	void register_callback(CallbackT &&callback) {
		InterruptManager::registerISR(MDMA_IRQn, 0, 2, [=]() {
			// if (MDMAX::BlockRepeatTransferComplISRFlag::read() && MDMAX::BlockRepeatTransferComplISREnable::read()) {
			// 	MDMAX::BlockRepeatTransferComplISRClear::set();
			// }

			// if (MDMAX::BlockTransferComplISRFlag::read() && MDMAX::BlockTransferComplISREnable::read()) {
			// 	MDMAX::BlockTransferComplISRClear::set();
			// }

			if (MDMAX::ChannelTransferComplISRFlag::read() && MDMAX::ChannelTransferComplISREnable::read()) {
				MDMAX::ChannelTransferComplISRClear::set();
				callback();
			}

			// if (MDMAX::BufferTransferComplISRFlag::read() && MDMAX::BufferTransferComplISREnable::read()) {
			// 	MDMAX::BufferTransferComplISRClear::set();
			// }

			if (MDMAX::TransferErrISRFlag::read() && MDMAX::TransferErrISREnable::read()) {
				MDMAX::TransferErrISRClear::set();
			}
		});
	}

	void config_transfer(void *dst, const void *src, size_t sz) {
		_dst_addr = reinterpret_cast<uint32_t>(dst);
		_src_addr = reinterpret_cast<uint32_t>(src);

		target::RCC_Enable::MDMA_::set();
		MDMAX::Enable::clear();

		_transfer_block_size = sz;
		_num_blocks = 1;
		while (_transfer_block_size >= 0x8000U) {
			_num_blocks++;
			_transfer_block_size = sz / _num_blocks;
		}
		// Deal with leftover bytes due to _transfer_block_size * _num_blocks < sz
		// Todo: test this!
		while (_transfer_block_size * _num_blocks < sz)
			_transfer_block_size += (ConfT::src_data_size == 0b00) ? 1
								  : (ConfT::src_data_size == 0b01) ? 2
								  : (ConfT::src_data_size == 0b10) ? 4
																   : 8;

		_num_blocks--;

		MDMAX::BlockNumDataBytesToXfer::write(_transfer_block_size);
		MDMAX::BlockRepeatCount::write(_num_blocks);

		MDMAX::SrcSize::write(ConfT::src_data_size);
		MDMAX::SrcIncDir::write(MDMAX::Up);
		MDMAX::SrcIncAmount::write(ConfT::src_data_inc);
		MDMAX::DstSize::write(ConfT::dst_data_size);
		MDMAX::DstIncDir::write(MDMAX::Up);
		MDMAX::DstIncAmount::write(ConfT::dst_data_inc);
		MDMAX::SrcBurst::write(ConfT::src_burst);
		MDMAX::DstBurst::write(ConfT::dst_burst);

		MDMAX::TransferLength::write(sz >= 128 ? 127 : sz - 1);

		// Todo: try if we can just set TriggerMode to 0b11 (transfer all)
		if (_num_blocks > 0)
			MDMAX::TriggerMode::write(0b10); // Repeated Block transfer
		else if (sz > 127)
			MDMAX::TriggerMode::write(0b01); // Block transfer
		else
			MDMAX::TriggerMode::write(0b00); // Buffer transfer

		MDMAX::PaddingAlignmentMode::write(0b00);
		MDMAX::PackEnable::set();

		// Use software request mode for memory-to-memory transfers
		MDMAX::SWRequestMode::set();

		if constexpr (ConfT::bufferable_write_mode)
			MDMAX::BufferableWriteMode::set();
		else
			MDMAX::BufferableWriteMode::clear();

		if constexpr (ConfT::swap_bytes)
			MDMAX::ByteEndianessExchange::set();
		if constexpr (ConfT::swap_halfwords)
			MDMAX::HalfwordEndianessExchange::set();
		if constexpr (ConfT::swap_words)
			MDMAX::WordEndianessExchange::set();

		MDMAX::PriorityLevel::write(ConfT::PriorityLevel);

		// Clear all ISRs
		MDMAX::BufferTransferComplISRClear::set();
		MDMAX::BlockTransferComplISRClear::set();
		MDMAX::BlockRepeatTransferComplISRClear::set();
		MDMAX::ChannelTransferComplISRClear::set();
		MDMAX::TransferErrISRClear::set();

		_set_addrs();
		_set_addr_bus_bits();

		// Set certain ISRs (todo: allow config)
		MDMAX::TransferErrISREnable::set();
		// MDMAX::BufferTransferComplISREnable::set();
		// MDMAX::BlockTransferComplISREnable::set();
		// MDMAX::BlockRepeatTransferComplISREnable::set();
		MDMAX::ChannelTransferComplISREnable::set();
	}

	void repeat_transfer_new_dst(void *dst) {
		MDMAX::Enable::clear();
		MDMAX::BlockNumDataBytesToXfer::write(_transfer_block_size);
		MDMAX::BlockRepeatCount::write(_num_blocks);
		_dst_addr = reinterpret_cast<uint32_t>(dst);
		_set_addrs();
		MDMAX::Enable::set();
		MDMAX::SWRequest::set();
	}

	void start_transfer() {
		MDMAX::Enable::clear();
		MDMAX::BlockNumDataBytesToXfer::write(_transfer_block_size);
		MDMAX::BlockRepeatCount::write(_num_blocks);
		_set_addrs();
		MDMAX::Enable::set();
		// Debug point:
		// if (MDMAX::RequestIsActive::read())
		// 	while (1)
		// 		;
		MDMAX::SWRequest::set();
	}

private:
	void _set_addrs() {
		MDMAX::SrcAddr::write(_src_addr);
		MDMAX::DstAddr::write(_dst_addr);
	}

	void _set_addr_bus_bits() {
		const uint32_t src_bus = _src_addr & 0xFF000000;
		MDMAX::SrcBusIsAHBTCM::write(src_bus == 0x20000000 || src_bus == 0);

		const uint32_t dst_bus = _dst_addr & 0xFF000000;
		MDMAX::DstBusIsAHBTCM::write(dst_bus == 0x20000000 || dst_bus == 0);
	}
};
} // namespace mdrivlib
