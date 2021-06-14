#pragma once
#include <stdint.h>

enum LL_GPIO_STUB_VALS {
	LL_GPIO_MODE_INPUT,
	LL_GPIO_MODE_OUTPUT,
	LL_GPIO_MODE_ANALOG,
	LL_GPIO_MODE_ALTERNATE,
	LL_GPIO_PULL_UP,
	LL_GPIO_PULL_DOWN,
	LL_GPIO_PULL_NO,
	LL_GPIO_SPEED_FREQ_LOW,
	LL_GPIO_SPEED_FREQ_MEDIUM,
	LL_GPIO_SPEED_FREQ_HIGH,
	LL_GPIO_SPEED_FREQ_VERY_HIGH,
	LL_GPIO_OUTPUT_PUSHPULL,
	LL_GPIO_OUTPUT_OPENDRAIN,
};

struct GPIO_TypeDef {
	uint32_t BSRR;
	uint32_t IDR;
};

struct FakeGPIOPeriph {
	static inline GPIO_TypeDef GPIOA;
	static inline GPIO_TypeDef GPIOB;
	static inline GPIO_TypeDef GPIOC;
	static inline GPIO_TypeDef GPIOD;
	static inline GPIO_TypeDef GPIOE;
	static inline GPIO_TypeDef GPIOF;
	static inline GPIO_TypeDef GPIOG;
	static inline GPIO_TypeDef GPIOH;
	static inline GPIO_TypeDef GPIOI;
	static inline GPIO_TypeDef GPIOJ;
	static inline GPIO_TypeDef GPIOK;
};

// #define GPIOA_BASE reinterpret_cast<uintptr_t>(&FakeGPIOPeriph::GPIOA) //error in clang
constexpr uint32_t GPIOA_BASE = 0;
constexpr uint32_t GPIOB_BASE = 0;
constexpr uint32_t GPIOC_BASE = 0;
constexpr uint32_t GPIOD_BASE = 0;
constexpr uint32_t GPIOE_BASE = 0;
constexpr uint32_t GPIOF_BASE = 0;
constexpr uint32_t GPIOG_BASE = 0;
constexpr uint32_t GPIOH_BASE = 0;
constexpr uint32_t GPIOI_BASE = 0;
constexpr uint32_t GPIOJ_BASE = 0;
constexpr uint32_t GPIOK_BASE = 0;

enum class FakeGPIO : uint8_t {
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
};
bool read_fake_pin(FakeGPIO port, uint16_t pin_num);
void set_fake_pin(FakeGPIO port, uint16_t pin_num, bool newstate);
