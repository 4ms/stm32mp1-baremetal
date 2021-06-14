#pragma once
#include "pin.hh"

struct I2CTimingConfig {
	uint8_t PRESC;		   // top 4 bits: (PRESC + 1) * tI2CCLK = tPRESC
						   // bottom 4 bits is ignored
	uint8_t SCLDEL_SDADEL; // top 4 bits: SCLDEL * tPRESC = SCL Delay between SDA edge and SCL
						   // rising edge bottom 4 bits: = SDADEL * tPRESC = SDA Delay between SCL
						   // falling edge and SDA edge
	uint8_t SCLH;		   // SCL high period = (SCLH+1) * tPRESC
	uint8_t SCLL;		   // SCL low period = (SCLL+1) * tPRESC
	constexpr uint32_t calc() const {
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
//
// I2C clock = I2CPeriphClock / (SCLDEL + SDADEL + SCLH+1 + SCLL+1)
// I2CPeriphClock = ClockSource / (PRESC>>4 + 2)

// SCL Period = tSCL = tSYNC1 + tSYNC2 + ( (SCLH+1 + SCLL+1) * (PRESC>>4 + 1) * tClockSource) )
// tSYNC1 = (SDADEL * (PRESC>>4 + 1) + 1) * tClockSource
// tSYNC2 = (SCLDEL * (PRESC>>4 + 1) + 1) * tClockSource

// stable with M7:
// SCL Freq = 1/tSCL = fClockSource / (PRESC>>4 + 1) / (SCLL + SCLH + 2) .... - some more for tSNC1
// 0x60 -> 400kHz: 120MHz / (6+2) = 15MHz I2CperiphClock
// 15MHz / (1 + 1 + 17+1 + 17+1) = 394kHz
// .PRESC = 0x10,
// .SCLDEL_SDADEL = 0b00110011,
// .SCLH = 28,
// .SCLL = 28,
