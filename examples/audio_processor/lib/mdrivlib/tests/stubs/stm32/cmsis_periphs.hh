#pragma once
#include "stubs/core_m7_stubs.hh"
#include <cstdint>

using IRQn_Type = uint32_t;

enum {
	ADC_IRQn,
	CEC_IRQn,
	COMP_IRQn,
	CRS_IRQn,
	ECC_IRQn,
	DCMI_IRQn,
	FPU_IRQn,
};
