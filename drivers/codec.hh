#pragma once
#include <cstdint>
#include <functional>

class ICodec {
public:
	virtual void start() = 0;
	virtual void init() = 0;
	virtual uint32_t get_samplerate() = 0;
	virtual void set_txrx_buffers(uint8_t *tx_buf_ptr, uint8_t *rx_buf_ptr, uint32_t block_size_) = 0;
	virtual void set_callbacks(std::function<void()> &&tx_complete_cb, std::function<void()> &&tx_half_complete_cb) = 0;
};
