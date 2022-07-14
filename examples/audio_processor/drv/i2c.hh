#pragma once
#include "i2c_config_struct.hh"
#include "interrupt.hh"
#include "pin.hh"
#include <stdint.h>

namespace mdrivlib
{
class I2CPeriph {
public:
	enum Error { I2C_NO_ERR, I2C_INIT_ERR, I2C_ALREADY_INIT, I2C_XMIT_ERR };

	I2CPeriph() = default;
	~I2CPeriph() = default;

	I2CPeriph(const I2CConfig &defs) {
		init(defs);
	}
	Error init(const I2CConfig &defs);
	void deinit();

	bool is_ready();

	Error mem_read(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size);
	Error mem_write(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size);
	Error mem_write_IT(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size);
	Error mem_write_dma(uint16_t dev_address, uint16_t mem_address, uint32_t memadd_size, uint8_t *data, uint16_t size);
	Error read(uint16_t dev_address, uint8_t *data, uint16_t size);
	Error read_IT(uint16_t dev_address, uint8_t *data, uint16_t size);
	Error write(uint16_t address, uint8_t *data, uint16_t size);
	Error write_IT(uint16_t address, uint8_t *data, uint16_t size);
	void enable_IT(uint8_t pri1 = 2, uint8_t pri2 = 2);
	void disable_IT();
	void link_DMA_TX(DMA_HandleTypeDef *dmatx);
	void link_DMA_RX(DMA_HandleTypeDef *dmarx);

	void enable_FMP();

private:
	bool already_init = false;
	I2C_HandleTypeDef hal_i2c_;
	IRQn_Type i2c_irq_num_;
	IRQn_Type i2c_err_irq_num_;
	Interrupt event_isr;
	Interrupt error_isr;
	void i2c_error_handler();
	void i2c_event_handler();

	Error _init_periph(I2C_TypeDef *periph, const I2CTimingConfig &timing);
	Error _init_periph(I2C_TypeDef *periph) {
		return _init_periph(periph, I2CTimingConfig{.PRESC = 0x50, .SCLDEL_SDADEL = 0x70, .SCLH = 0x75, .SCLL = 0xB1});
	}
};
} // namespace mdrivlib
