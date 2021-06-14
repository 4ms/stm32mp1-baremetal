#pragma once
#include "pin.hh"

struct ADC_I2C_Config {
	//
};

struct MuxedADC_Config {
	PinNoInit SEL0;
	PinNoInit SEL1;
	PinNoInit SEL2;
	ADC_I2C_Config adc_conf;
};
