#pragma once
#include "sai_config_struct.hh"
#include "sai_tdm.hh"
#include <cstdint>
#include <functional>

namespace mdrivlib
{

class CodecBase {
public:
	CodecBase(const SaiConfig &sai_def)
		: sai_{sai_def} {
	}

	template<typename FrameT, size_t BUFSIZE>
	void set_tx_buffers(std::array<FrameT, BUFSIZE> &tx_buf) {
		constexpr uint32_t NumHalfTransfers = 2;
		constexpr uint32_t BytesPerSample = 4;
		constexpr uint32_t block_size = sizeof(tx_buf) * NumHalfTransfers / BytesPerSample;
		sai_.set_tx_buffers(reinterpret_cast<uint8_t *>(tx_buf.data()), block_size);
	}

	template<typename FrameT, size_t BUFSIZE>
	void set_rx_buffers(std::array<FrameT, BUFSIZE> &rx_buf) {
		constexpr uint32_t NumHalfTransfers = 2;
		constexpr uint32_t BytesPerSample = 4;
		constexpr uint32_t block_size = sizeof(rx_buf) * NumHalfTransfers / BytesPerSample;
		sai_.set_rx_buffers(reinterpret_cast<uint8_t *>(rx_buf.data()), block_size);
	}

	void set_callbacks(std::function<void()> &&tx_complete_cb, std::function<void()> &&tx_half_complete_cb) {
		sai_.set_callbacks(std::move(tx_complete_cb), std::move(tx_half_complete_cb));
	}

protected:
	SaiTdmPeriph sai_;
};

} // namespace mdrivlib
