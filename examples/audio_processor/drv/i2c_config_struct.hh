#pragma once
#include "pin.hh"

namespace mdrivlib
{
struct I2CTimingConfig {
	uint8_t PRESC;		   // top 4 bits: (PRESC + 1) * tI2CCLK = tPRESC
						   // bottom 4 bits is ignored
	uint8_t SCLDEL_SDADEL; // top 4 bits: SCLDEL * tPRESC = SCL Delay between SDA edge and SCL
						   // rising edge bottom 4 bits: = SDADEL * tPRESC = SDA Delay between SCL
						   // falling edge and SDA edge
	uint8_t SCLH;		   // SCL high period = (SCLH+1) * tPRESC
	uint8_t SCLL;		   // SCL low period = (SCLL+1) * tPRESC

	[[nodiscard]] constexpr uint32_t calc() const
	{
		return ((PRESC & 0xF0) << 24) | ((SCLDEL_SDADEL) << 16) | ((SCLH) << 8) | ((SCLL) << 0);
	}
};

struct I2CConfig {
	I2C_TypeDef *I2Cx;
	PinNoInit SCL;
	PinNoInit SDA;
	I2CTimingConfig timing;
	unsigned priority1;
	unsigned priority2;
};
} // namespace mdrivlib
