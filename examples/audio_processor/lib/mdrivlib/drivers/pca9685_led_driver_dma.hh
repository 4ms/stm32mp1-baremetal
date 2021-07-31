#pragma once
#include "dma_config_struct.hh"
#include "pca9685_led_driver.hh"

namespace mdrivlib
{
class PCA9685DmaDriver : public PCA9685Driver {
public:
	PCA9685DmaDriver(I2CPeriph &i2c, uint32_t num_chips, const DMA_Config &dma_defs, uint32_t *frame_buf);

	LEDDriverError start_dma_mode();

private:
	LEDDriverError init_dma();
	LEDDriverError start_dma();
	void advance_frame_buffer();
	void write_current_frame_to_chip();

	DMA_HandleTypeDef hal_dma_h_;
	Interrupt dma_task;
	uint8_t cur_chip_num_ = 0;
	uint32_t *frame_buffer_cur_pos;
	const DMA_Config &dma_defs_;
	bool transfer_complete;
	// Todo: Fix HALCallbacks so we can move a temporary (stack-created) lambda into HALCBs
	HALCallback transfer_complete_cb{HALCallbackID::I2C_MemTxCplt, [this]() {
										 advance_frame_buffer();
										 if (!dma_defs_.continuous && cur_chip_num_ == 0)
											 transfer_complete = true;
										 else
											 write_current_frame_to_chip();
									 }};
};
} // namespace mdrivlib
