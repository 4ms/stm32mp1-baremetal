
// Pointer offset method to set/clear bits in registers,
// given a pointer to the base address (RCC) and offset from the CMSIS header
//?? incomplete example? We need a struct that defines ::reg

static inline void _set_bit_in_struct_by_offset(uint8_t *base, RegisterDataT offset, RegisterDataT bit) {
	*(RegisterDataT *)(base + offset) |= bit;
}

static inline void _clear_bit_in_struct_by_offset(uint8_t *base, RegisterDataT offset, RegisterDataT bit) {
	*(RegisterDataT *)(base + offset) &= ~bit;
}

static inline RegisterDataT _read_bit_in_struct_by_offset(uint8_t *base, RegisterDataT offset, RegisterDataT bit) {
	return (*(RegisterDataT *)(base + offset)) & bit;
}

template<typename T>
void enable_rcc(T en_bit) {
	_set_bit_in_struct_by_offset(
		(uint8_t *)RCC, static_cast<volatile RegisterDataT>(T::reg), static_cast<RegisterDataT>(en_bit));
}

template<typename T>
void disable_rcc(T en_bit) {
	_clear_bit_in_struct_by_offset(
		(uint8_t *)RCC, static_cast<volatile RegisterDataT>(T::reg), static_cast<RegisterDataT>(en_bit));
}

template<typename T>
bool is_enabled_rcc(T en_bit) {
	return _read_bit_in_struct_by_offset(
		(uint8_t *)RCC, static_cast<volatile RegisterDataT>(T::reg), static_cast<RegisterDataT>(en_bit));
}

///////////
// Method #1:
template<typename PeriphT>
struct RCCPeriph {
	static void enable() {
		*PeriphT::_reg |= PeriphT::_bit;
	}
	static void disable() {
		*PeriphT::_reg &= ~PeriphT::_bit;
	}
	static bool is_enabled() {
		return *PeriphT::_reg & PeriphT::_bit;
	}
};

// Method #2:
template<typename PeriphT>
struct RCCPeriphs {
	static void enable(unsigned pnum) {
		*PeriphT::_regs[pnum - 1] |= PeriphT::_bits[pnum - 1];
	}
	static void disable(unsigned pnum) {
		*PeriphT::_regs[pnum - 1] &= ~PeriphT::_bits[pnum - 1];
	}
	static bool is_enabled(unsigned pnum) {
		return *PeriphT::_regs[pnum - 1] & PeriphT::_bits[pnum - 1];
	}
};

// Method #3:
struct EnableFlag {
	volatile RegisterDataT *const reg;
	const RegisterDataT bit;
};

template<typename PeriphT>
struct RCCPeriphControl {
	static void enable(unsigned pnum = 1) {
		*PeriphT::_enableflags[pnum - 1].reg |= PeriphT::_enableflags[pnum - 1].bit;
	}
	static void disable(unsigned pnum = 1) {
		*PeriphT::_enableflags[pnum - 1].reg &= ~PeriphT::_enableflags[pnum - 1].bit;
	}
	static bool is_enabled(unsigned pnum = 1) {
		return *PeriphT::_enableflags[pnum - 1].reg & PeriphT::_enableflags[pnum - 1].bit;
	}
};

// Methods 1-3:
struct RCCControl {
	// special-case: GPIO port base address can be used to calc bit-offset of RCC enable bit
	struct GPIO {
		static inline volatile RegisterDataT *const _reg = &(RCC->AHB4ENR);
		static uint32_t get_gpio_bit(uint32_t periph_base_addr) {
			return 1 << ((periph_base_addr & 0x00003C00) >> 10);
		}
		static void enable(uint32_t periph_base_addr) {
			// std::atomic
			auto tmp = *_reg;
			tmp |= get_gpio_bit(periph_base_addr);
			*_reg = tmp;
			// end
		}
		static void disable(uint32_t periph_base_addr) {
			// std::atomic
			auto tmp = *_reg;
			tmp &= ~get_gpio_bit(periph_base_addr);
			*_reg = tmp;
			// end
		}
		static bool is_enabled(uint32_t periph_base_addr) {
			return (*_reg) & get_gpio_bit(periph_base_addr);
		}
	};

	struct SYS_CFG : RCCPeriphControl<SYS_CFG> {
		const static inline unsigned NumP = 1;
		static inline EnableFlag _enableflags[NumP] = {
			{&RCC->APB4ENR, RCC_APB4ENR_SYSCFGEN},
		};
	};

	struct BDMA_P : RCCPeriphControl<BDMA_P> {
		const static inline unsigned NumP = 1;
		static inline EnableFlag _enableflags[NumP] = {
			{&RCC->AHB4ENR, RCC_AHB4ENR_BDMAEN},
		};
	};

	struct ADC : RCCPeriphs<ADC> {
		const static inline unsigned NumP = 3;
		volatile static inline RegisterDataT *const _regs[NumP] = {
			&RCC->AHB1ENR,
			&RCC->AHB1ENR,
			&RCC->AHB4ENR,
		};
		const static inline RegisterDataT _bits[NumP] = {
			RCC_AHB1ENR_ADC12EN,
			RCC_AHB1ENR_ADC12EN,
			RCC_AHB4ENR_ADC3EN,
		};
	};

	struct ADC_1 : public RCCPeriph<ADC_1> {
		volatile static inline RegisterDataT *const _reg = &(RCC->AHB1ENR);
		const static inline RegisterDataT _bit = RCC_AHB1ENR_ADC12EN;
	};
	struct ADC_2 : public RCCPeriph<ADC_2> {
		volatile static inline RegisterDataT *const _reg = &(RCC->AHB1ENR);
		const static inline RegisterDataT _bit = RCC_AHB1ENR_ADC12EN;
	};
	struct ADC_3 : public RCCPeriph<ADC_3> {
		volatile static inline RegisterDataT *const _reg = &(RCC->AHB4ENR);
		const static inline RegisterDataT _bit = RCC_AHB4ENR_ADC3EN;
	};

	// Todo: decide if the ADC : RCCPeriphs<ADC> method or the ADC_1 : RCCPeriph<ADC_1> method is better, and apply to
	// all (including GPIO):
	struct DMA_1 : public RCCPeriph<DMA_1> {
		volatile static inline RegisterDataT *const _reg = &(RCC->AHB1ENR);
		const static inline RegisterDataT _bit = RCC_AHB1ENR_DMA1EN;
	};
	struct DMA_2 : public RCCPeriph<DMA_2> {
		volatile static inline RegisterDataT *const _reg = &(RCC->AHB1ENR);
		const static inline RegisterDataT _bit = RCC_AHB1ENR_DMA2EN;
	};

	struct I2C_1 : public RCCPeriph<I2C_1> {
		volatile static inline RegisterDataT *const _reg = &(RCC->APB1LENR);
		const static inline RegisterDataT _bit = RCC_APB1LENR_I2C1EN;
	};
	struct I2C_2 : public RCCPeriph<I2C_2> {
		volatile static inline RegisterDataT *const _reg = &(RCC->APB1LENR);
		const static inline RegisterDataT _bit = RCC_APB1LENR_I2C2EN;
	};
	struct I2C_3 : public RCCPeriph<I2C_3> {
		volatile static inline RegisterDataT *const _reg = &(RCC->APB1LENR);
		const static inline RegisterDataT _bit = RCC_APB1LENR_I2C3EN;
	};

	struct SAI_1 : public RCCPeriph<SAI_1> {
		volatile static inline RegisterDataT *const _reg = &(RCC->APB2ENR);
		const static inline RegisterDataT _bit = RCC_APB2ENR_SAI1EN;
	};
	struct SAI_2 : public RCCPeriph<SAI_2> {
		volatile static inline RegisterDataT *const _reg = &(RCC->APB2ENR);
		const static inline RegisterDataT _bit = RCC_APB2ENR_SAI2EN;
	};
	struct SAI_3 : public RCCPeriph<SAI_3> {
		volatile static inline RegisterDataT *const _reg = &(RCC->APB2ENR);
		const static inline RegisterDataT _bit = RCC_APB2ENR_SAI3EN;
	};
	struct SAI_4 : public RCCPeriph<SAI_4> {
		volatile static inline RegisterDataT *const _reg = &(RCC->APB4ENR);
		const static inline RegisterDataT _bit = RCC_APB4ENR_SAI4EN;
	};

	struct TIM : RCCPeriphs<TIM> {
		const static inline unsigned NumP = 17;
		volatile static inline RegisterDataT *const _regs[NumP] = {
			&RCC->APB2ENR,
			&RCC->APB1LENR,
			&RCC->APB1LENR,
			&RCC->APB1LENR,
			&RCC->APB1LENR,
			&RCC->APB1LENR,
			&RCC->APB1LENR,
			&RCC->APB2ENR,
			&RCC->APB2ENR, // TIM9 is not present: map to TIM1
			&RCC->APB2ENR, // TIM10 is not present: map to TIM1
			&RCC->APB2ENR, // TIM11 is not present: map to TIM1
			&RCC->APB1LENR,
			&RCC->APB1LENR,
			&RCC->APB1LENR,
			&RCC->APB2ENR,
			&RCC->APB2ENR,
			&RCC->APB2ENR,
		};
		const static inline RegisterDataT _bits[NumP] = {
			RCC_APB2ENR_TIM1EN,
			RCC_APB1LENR_TIM2EN,
			RCC_APB1LENR_TIM3EN,
			RCC_APB1LENR_TIM4EN,
			RCC_APB1LENR_TIM5EN,
			RCC_APB1LENR_TIM6EN,
			RCC_APB1LENR_TIM7EN,
			RCC_APB2ENR_TIM8EN,
			RCC_APB2ENR_TIM1EN, //?? TIM9 is not present: map to TIM1
			RCC_APB2ENR_TIM1EN, //?? TIM10 is not present: map to TIM1
			RCC_APB2ENR_TIM1EN, //?? TIM11 is not present: map to TIM1
			RCC_APB1LENR_TIM12EN,
			RCC_APB1LENR_TIM13EN,
			RCC_APB1LENR_TIM14EN,
			RCC_APB2ENR_TIM15EN,
			RCC_APB2ENR_TIM16EN,
			RCC_APB2ENR_TIM17EN,
		};
	};
};
