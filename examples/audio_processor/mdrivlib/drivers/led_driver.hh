#pragma once
#include <cstdint>

enum class LEDDriverError {
	None = 0,
	HAL_INIT_ERR = 1,
	I2C_XMIT_TIMEOUT = 2,
	I2C_XMIT_ERR = 3,
	SET_LED_ERR = 4,
	BAD_LED_PARAM = 5,
	DMA_XMIT_ERR = 6,
	IT_XMIT_ERR = 7
};

// class ILedDmaDriver {
// public:
// 	virtual LEDDriverError start() = 0;
// 	virtual LEDDriverError start_dma_mode() = 0;
// 	virtual void start_it_mode() = 0;
// 	virtual LEDDriverError write_once(uint16_t chipid) = 0;

// 	virtual uint32_t *const get_buf_addr(const uint32_t chipid, const uint32_t ledid) = 0;
// };
