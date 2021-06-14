#pragma once
#include "drivers/register_access.hh"
#include "drivers/stm32xx.h"
#include <variant>

namespace mdrivlib
{
namespace stm32h7x5
{

namespace RCC_Enable
{
using RegisterDataT = uint32_t;

using GPIO_A = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOAEN>;
using GPIO_B = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOBEN>;
using GPIO_C = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOCEN>;
using GPIO_D = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIODEN>;
using GPIO_E = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOEEN>;
using GPIO_F = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOFEN>;
using GPIO_G = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOGEN>;
using GPIO_H = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOHEN>;
using GPIO_I = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOIEN>;
using GPIO_J = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_GPIOJEN>;
// special-case: GPIO port base address can be used to calc bit-offset of RCC enable bit
struct GPIO {
	static inline volatile RegisterDataT *const _reg = &(RCC->AHB4ENR);

	static uint32_t get_gpio_bit(RegisterDataT periph) {
		return 1 << ((periph & 0x00003C00) >> 10);
	}
	static void enable(GPIO_TypeDef *periph) {
		*_reg = *_reg | get_gpio_bit(reinterpret_cast<RegisterDataT>(periph));
		[[maybe_unused]] bool delay_after_enabling = is_enabled(periph);
	}
	static void disable(GPIO_TypeDef *periph) {
		*_reg = *_reg & ~get_gpio_bit(reinterpret_cast<RegisterDataT>(periph));
		[[maybe_unused]] bool delay_after_disabling = is_enabled(periph);
	}
	static bool is_enabled(GPIO_TypeDef *periph) {
		return (*_reg) & get_gpio_bit(reinterpret_cast<RegisterDataT>(periph));
	}
};

using ADC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_ADC12EN>;
using ADC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_ADC12EN>;
using ADC3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_ADC3EN>;

using ADC = std::variant<ADC1_, ADC2_, ADC3_>;

using SYSCFG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4ENR), RCC_APB4ENR_SYSCFGEN>;

using HSEM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_HSEMEN>;

using SAI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SAI1EN>;
using SAI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SAI2EN>;
using SAI3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SAI3EN>;
using SAI4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB4ENR_SAI4EN>;

using TIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM1EN>;
using TIM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM2EN>;
using TIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM3EN>;
using TIM4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM4EN>;
using TIM5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM5EN>;
using TIM6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM6EN>;
using TIM7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM7EN>;
using TIM8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM8EN>;
using TIM9_ = NonexistantRegister;
using TIM10_ = NonexistantRegister;
using TIM11_ = NonexistantRegister;
using TIM12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM12EN>;
using TIM13_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM13EN>;
using TIM14_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_TIM14EN>;
using TIM15_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM15EN>;
using TIM16_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM16EN>;
using TIM17_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_TIM17EN>;

using DMA1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_DMA1EN>;
using DMA2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1ENR), RCC_AHB1ENR_DMA2EN>;
using BDMA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4ENR), RCC_AHB4ENR_BDMAEN>;
using MDMA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3ENR), RCC_AHB3ENR_MDMAEN>;
using DMAMUX_ = NonexistantRegister;

using DMA2D_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3ENR), RCC_AHB3ENR_DMA2DEN>;

using I2C1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_I2C1EN>;
using I2C2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_I2C2EN>;
using I2C3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_I2C3EN>;

using SPI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SPI1EN>;
using SPI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_SPI2EN>;
using SPI3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LENR), RCC_APB1LENR_SPI3EN>;
using SPI4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SPI4EN>;
using SPI5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2ENR), RCC_APB2ENR_SPI5EN>;
using SPI6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4ENR), RCC_APB4ENR_SPI6EN>;

using SAI_variant = std::variant<SAI1_, SAI2_, SAI3_, SAI4_>;
template<unsigned N>
using SAI = typename std::variant_alternative_t<N - 1, SAI_variant>;

using SPI_variant = std::variant<SPI1_, SPI2_, SPI3_, SPI4_, SPI5_, SPI6_>;
template<unsigned N>
using SPI = typename std::variant_alternative_t<N - 1, SPI_variant>;

} // namespace RCC_Enable

namespace RCC_Reset
{
//
using GPIO_A = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOARST>;
using GPIO_B = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOBRST>;
using GPIO_C = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOCRST>;
using GPIO_D = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIODRST>;
using GPIO_E = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOERST>;
using GPIO_F = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOFRST>;
using GPIO_G = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOGRST>;
using GPIO_H = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOHRST>;
using GPIO_I = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOIRST>;
using GPIO_J = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_GPIOJRST>;

using ADC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_ADC12RST>;
using ADC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_ADC12RST>;
using ADC3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_ADC3RST>;

using SYSCFG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTR), RCC_APB4RSTR_SYSCFGRST>;

using HSEM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_HSEMRST>;

using SAI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SAI1RST>;
using SAI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SAI2RST>;
using SAI3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SAI3RST>;
using SAI4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB4RSTR_SAI4RST>;

using TIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM1RST>;
using TIM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM2RST>;
using TIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM3RST>;
using TIM4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM4RST>;
using TIM5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM5RST>;
using TIM6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM6RST>;
using TIM7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM7RST>;
using TIM8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM8RST>;
using TIM9_ = NonexistantRegister;
using TIM10_ = NonexistantRegister;
using TIM11_ = NonexistantRegister;
using TIM12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM12RST>;
using TIM13_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM13RST>;
using TIM14_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_TIM14RST>;
using TIM15_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM15RST>;
using TIM16_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM16RST>;
using TIM17_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_TIM17RST>;

using DMA1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_DMA1RST>;
using DMA2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB1RSTR), RCC_AHB1RSTR_DMA2RST>;
using BDMA_P = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTR), RCC_AHB4RSTR_BDMARST>;
using MDMA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3RSTR), RCC_AHB3RSTR_MDMARST>;

using I2C1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_I2C1RST>;
using I2C2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_I2C2RST>;
using I2C3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_I2C3RST>;

using SPI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SPI1RST>;
using SPI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_SPI2RST>;
using SPI3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1LRSTR), RCC_APB1LRSTR_SPI3RST>;
using SPI4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SPI4RST>;
using SPI5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTR), RCC_APB2RSTR_SPI5RST>;
using SPI6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTR), RCC_APB4RSTR_SPI6RST>;

} // namespace RCC_Reset

namespace RCC_Clocks
{
using CR = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, CR), 0xFFFFFFFF>;
using D1CKREADY = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, CR), RCC_CR_D1CKRDY>;
using D2CKREADY = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, CR), RCC_CR_D2CKRDY>;

} // namespace RCC_Clocks
} // namespace stm32h7x5
} // namespace mdrivlib
