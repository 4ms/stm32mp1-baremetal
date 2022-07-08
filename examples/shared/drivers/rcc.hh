#pragma once
#include "drivers/rcc_common.hh"
#include "drivers/register_access.hh"
#include "stm32mp1xx.h"
#include <variant>

namespace mdrivlib
{
namespace RCC_Enable
{
using RegisterDataT = uint32_t;

// Auto-generated by:
// :s/\v#define RCC_(.{-}ENR)_([^ ]{-})EN[^_] /using \2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, \1),
// RCC_\1_\2EN>;\r\0

// clang-format off
using ADC12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2ENSETR), RCC_MP_AHB2ENSETR_ADC12EN>;
using ADFSDM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_ADFSDMEN>;
using BKPSRAM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB5ENSETR), RCC_MP_AHB5ENSETR_BKPSRAMEN>;
using BSEC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_BSECEN>;
using CEC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_CECEN>;
using CRC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_CRC1EN>;
using CRC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3ENSETR), RCC_MP_AHB3ENSETR_CRC2EN>;
using CRYP1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB5ENSETR), RCC_MP_AHB5ENSETR_CRYP1EN>;
using CRYP2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3ENSETR), RCC_MP_AHB3ENSETR_CRYP2EN>;
using DAC12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_DAC12EN>;
using DCMI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3ENSETR), RCC_MP_AHB3ENSETR_DCMIEN>;
using DDRPERFM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4ENSETR), RCC_MP_APB4ENSETR_DDRPERFMEN>;
using DFSDM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_DFSDMEN>;
using DMA1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2ENSETR), RCC_MP_AHB2ENSETR_DMA1EN>;
using DMA2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2ENSETR), RCC_MP_AHB2ENSETR_DMA2EN>;
using DMAMUX_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2ENSETR), RCC_MP_AHB2ENSETR_DMAMUXEN>;
using DSI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4ENSETR), RCC_MP_APB4ENSETR_DSIEN>;
using DTS_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_DTSEN>;
using ETHCK_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_ETHCKEN>;
using ETHMAC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_ETHMACEN>;
using ETHRX_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_ETHRXEN>;
using ETHSTP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6LPENSETR), RCC_MP_AHB6LPENSETR_ETHSTPEN>;
using ETHTX_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_ETHTXEN>;
using FDCAN_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_FDCANEN>;
using FMC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_FMCEN>;
using GPIOA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOAEN>;
using GPIOB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOBEN>;
using GPIOC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOCEN>;
using GPIOD_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIODEN>;
using GPIOE_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOEEN>;
using GPIOF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOFEN>;
using GPIOG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOGEN>;
using GPIOH_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOHEN>;
using GPIOI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOIEN>;
using GPIOJ_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOJEN>;
using GPIOK_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOKEN>;
using GPIOZ_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB5ENSETR), RCC_MP_AHB5ENSETR_GPIOZEN>;
using GPU_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_GPUEN>;
using HASH1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB5ENSETR), RCC_MP_AHB5ENSETR_HASH1EN>;
using HASH2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3ENSETR), RCC_MP_AHB3ENSETR_HASH2EN>;
using HDP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_HDPEN>;
using HSEM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3ENSETR), RCC_MP_AHB3ENSETR_HSEMEN>;
using I2C1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_I2C1EN>;
using I2C2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_I2C2EN>;
using I2C3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_I2C3EN>;
using I2C4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_I2C4EN>;
using I2C5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_I2C5EN>;
using I2C6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_I2C6EN>;
using IPCC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3ENSETR), RCC_MP_AHB3ENSETR_IPCCEN>;
using IWDG1APB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_IWDG1APBEN>;
using IWDG2APB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4ENSETR), RCC_MP_APB4ENSETR_IWDG2APBEN>;
using LPTIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_LPTIM1EN>;
using LPTIM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_LPTIM2EN>;
using LPTIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_LPTIM3EN>;
using LPTIM4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_LPTIM4EN>;
using LPTIM5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_LPTIM5EN>;
using LTDC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4ENSETR), RCC_MP_APB4ENSETR_LTDCEN>;
using MDIOS_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_MDIOSEN>;
using MDMA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_MDMAEN>;
using QSPI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_QSPIEN>;
using RETRAM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_MLAHBENSETR), RCC_MP_MLAHBENSETR_RETRAMEN>;
using RNG1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB5ENSETR), RCC_MP_AHB5ENSETR_RNG1EN>;
using RNG2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3ENSETR), RCC_MP_AHB3ENSETR_RNG2EN>;
using RTCAPB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_RTCAPBEN>;
using SAI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_SAI1EN>;
using SAI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_SAI2EN>;
using SAI3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_SAI3EN>;
using SAI4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_SAI4EN>;
using SDMMC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_SDMMC1EN>;
using SDMMC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_SDMMC2EN>;
using SDMMC3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2ENSETR), RCC_MP_AHB2ENSETR_SDMMC3EN>;
using SPDIF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_SPDIFEN>;
using SPI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_SPI1EN>;
using SPI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_SPI2EN>;
using SPI3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_SPI3EN>;
using SPI4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_SPI4EN>;
using SPI5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_SPI5EN>;
using SPI6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_SPI6EN>;
using STGENROSTP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4LPENSETR), RCC_MP_APB4LPENSETR_STGENROSTPEN>;
using STGENRO_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4ENSETR), RCC_MP_APB4ENSETR_STGENROEN>;
using STGENSTP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5LPENSETR), RCC_MP_APB5LPENSETR_STGENSTPEN>;
using STGEN_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_STGENEN>;
using SYSCFG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_SYSCFGEN>;
using TIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_TIM1EN>;
using TIM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM2EN>;
using TIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM3EN>;
using TIM4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM4EN>;
using TIM5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM5EN>;
using TIM6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM6EN>;
using TIM7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM7EN>;
using TIM8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_TIM8EN>;
using TIM12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM12EN>;
using TIM13_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM13EN>;
using TIM14_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM14EN>;
using TIM15_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_TIM15EN>;
using TIM16_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_TIM16EN>;
using TIM17_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_TIM17EN>;
using TZC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_TZC1EN>;
using TZC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_TZC2EN>;
using TZPC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_TZPCEN>;
using UART4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_UART4EN>;
using UART5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_UART5EN>;
using UART7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_UART7EN>;
using UART8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_UART8EN>;
using USART1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_USART1EN>;
using USART2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_USART2EN>;
using USART3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_USART3EN>;
using USART6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_USART6EN>;
using USBH_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_USBHEN>;
using USBO_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2ENSETR), RCC_MP_AHB2ENSETR_USBOEN>;
using USBPHY_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4ENSETR), RCC_MP_APB4ENSETR_USBPHYEN>;
using VREF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_VREFEN>;
// clang-format on
// Aliases:

using ADC1_ = ADC12_;
using ADC2_ = ADC12_;

// Blanks:

using ADC3_ = NonexistantRegister;
using BDMA_ = NonexistantRegister;
using TIM9_ = NonexistantRegister;
using TIM10_ = NonexistantRegister;
using TIM11_ = NonexistantRegister;

// special-case: GPIO port base address can be used to calc bit-offset of RCC enable bit
struct GPIO {
	static inline volatile RegisterDataT *const _reg = &(RCC->MP_AHB4ENSETR);

	static uint32_t get_gpio_bit(RegisterDataT periph)
	{
		return 1 << (((periph >> 12) - 2) & 0b1111);
	}
	static void enable(GPIO_TypeDef *periph)
	{
		if (periph == GPIOZ)
			GPIOZ_::set();
		else
			*_reg = *_reg | get_gpio_bit(reinterpret_cast<RegisterDataT>(periph));
		[[maybe_unused]] bool delay_after_enabling = is_enabled(periph);
	}
	static void disable(GPIO_TypeDef *periph)
	{
		if (periph == GPIOZ)
			GPIOZ_::clear();
		else
			*_reg = *_reg & ~get_gpio_bit(reinterpret_cast<RegisterDataT>(periph));
		[[maybe_unused]] bool delay_after_disabling = is_enabled(periph);
	}
	static bool is_enabled(GPIO_TypeDef *periph)
	{
		if (periph == GPIOZ)
			return GPIOZ_::read();
		else
			return (*_reg) & get_gpio_bit(reinterpret_cast<RegisterDataT>(periph));
	}
};

using SAI_variant = std::variant<SAI1_, SAI2_, SAI3_, SAI4_>;
template<unsigned N>
using SAI = typename std::variant_alternative_t<N - 1, SAI_variant>;

using SPI_variant = std::variant<SPI1_, SPI2_, SPI3_, SPI4_, SPI5_, SPI6_>;
template<unsigned N>
using SPI = typename std::variant_alternative_t<N - 1, SPI_variant>;

} // namespace RCC_Enable
namespace RCC_Clocks
{
// clang-format off
using MPUClockSrcReady = RegisterBits<ReadOnly, RCC_BASE + offsetof(RCC_TypeDef, MPCKSELR), RCC_MPCKSELR_MPUSRCRDY_Msk>;
using MPUClockSrc = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MPCKSELR), RCC_MPCKSELR_MPUSRC_Msk>;
enum : uint32_t { MPUClockSrcHSI = 0b00, MPUClockSrcHSE = 0b01, MPUClockSrcPLL1 = 0b10, MPUClockSrcPLL1DIV = 0b11 };

using AXISClockSrcReady = RegisterBits<ReadOnly, RCC_BASE + offsetof(RCC_TypeDef, MPCKSELR), RCC_ASSCKSELR_AXISSRCRDY_Msk>;
using AXISClockSrc = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, ASSCKSELR), RCC_ASSCKSELR_AXISSRC_Msk>;
enum : uint32_t { AXISClockSrcHSI = 0b00, AXISClockSrcHSE = 0b01, AXISClockSrcPLL2 = 0b10 };

using AXIDiv = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AXIDIVR), RCC_AXIDIVR_AXIDIV>;
enum : uint32_t { AXIDivOff = 0b000, AXIDiv2 = 0b001, AXIDiv3 = 0b010, AXIDiv4 = 0b011 };
using AXIDivReady = RegisterBits<ReadOnly, RCC_BASE + offsetof(RCC_TypeDef, AXIDIVR), RCC_AXIDIVR_AXIDIVRDY>;

// Auto-generated by:
// :s/\v#define RCC_([^_]{-})_([^ _]{-})\s /using \2 = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, \1), RCC_\1_\2>;\r\0/
using OscEnableSetHSION = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, OCENSETR), RCC_OCENSETR_HSION>;
using OscEnableSetHSIKERON = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, OCENSETR), RCC_OCENSETR_HSIKERON>;
using OscEnableSetCSION = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, OCENSETR), RCC_OCENSETR_CSION>;
using OscEnableSetCSIKERON = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, OCENSETR), RCC_OCENSETR_CSIKERON>;
using OscEnableSetDIGBYP = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, OCENSETR), RCC_OCENSETR_DIGBYP>;
using OscEnableSetHSEON = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, OCENSETR), RCC_OCENSETR_HSEON>;
using OscEnableSetHSEKERON = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, OCENSETR), RCC_OCENSETR_HSEKERON>;
using OscEnableSetHSEBYP = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, OCENSETR), RCC_OCENSETR_HSEBYP>;
using OscEnableSetHSECSSON = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, OCENSETR), RCC_OCENSETR_HSECSSON>;

using OscEnableClearHSION = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, OCENCLRR), RCC_OCENCLRR_HSION>;
using OscEnableClearHSIKERON = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, OCENCLRR), RCC_OCENCLRR_HSIKERON>;
using OscEnableClearCSION = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, OCENCLRR), RCC_OCENCLRR_CSION>;
using OscEnableClearCSIKERON = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, OCENCLRR), RCC_OCENCLRR_CSIKERON>;
using OscEnableClearDIGBYP = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, OCENCLRR), RCC_OCENCLRR_DIGBYP>;
using OscEnableClearHSEON = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, OCENCLRR), RCC_OCENCLRR_HSEON>;
using OscEnableClearHSEKERON = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, OCENCLRR), RCC_OCENCLRR_HSEKERON>;
using OscEnableClearHSEBYP = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, OCENCLRR), RCC_OCENCLRR_HSEBYP>;

using OscEnableHSION = RegisterSetClear<OscEnableSetHSION, OscEnableClearHSION>;
using OscEnableHSIKERON = RegisterSetClear<OscEnableSetHSIKERON, OscEnableClearHSIKERON>;
using OscEnableCSION = RegisterSetClear<OscEnableSetCSION, OscEnableClearCSION>;
using OscEnableCSIKERON = RegisterSetClear<OscEnableSetCSIKERON, OscEnableClearCSIKERON>;
using OscEnableDIGBYP = RegisterSetClear<OscEnableSetDIGBYP, OscEnableClearDIGBYP>;
using OscEnableHSEON = RegisterSetClear<OscEnableSetHSEON, OscEnableClearHSEON>;
using OscEnableHSEKERON = RegisterSetClear<OscEnableSetHSEKERON, OscEnableClearHSEKERON>;
using OscEnableHSEBYP = RegisterSetClear<OscEnableSetHSEBYP, OscEnableClearHSEBYP>;

using AXISClockReady = RegisterBits<ReadOnly, RCC_BASE + offsetof(RCC_TypeDef, OCRDYR), RCC_OCRDYR_AXICKRDY>;
using ClockRestoreReady = RegisterBits<ReadOnly, RCC_BASE + offsetof(RCC_TypeDef, OCRDYR), RCC_OCRDYR_CKREST>;
using CSIClockReady = RegisterBits<ReadOnly, RCC_BASE + offsetof(RCC_TypeDef, OCRDYR), RCC_OCRDYR_CSIRDY>;
using HSEClockReady = RegisterBits<ReadOnly, RCC_BASE + offsetof(RCC_TypeDef, OCRDYR), RCC_OCRDYR_HSERDY>;
using HSIDividerReady = RegisterBits<ReadOnly, RCC_BASE + offsetof(RCC_TypeDef, OCRDYR), RCC_OCRDYR_HSIDIVRDY>;
using HSIClockReady = RegisterBits<ReadOnly, RCC_BASE + offsetof(RCC_TypeDef, OCRDYR), RCC_OCRDYR_HSIRDY>;
using MPUClockReady = RegisterBits<ReadOnly, RCC_BASE + offsetof(RCC_TypeDef, OCRDYR), RCC_OCRDYR_MPUCKRDY>;

using PLL12Source = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, RCK12SELR), RCC_RCK12SELR_PLL12SRC>;
enum {PLL12SourceHSI = 0, PLL12SourceHSE = 1};
using PLL12SourceReady = RegisterBits<ReadOnly, RCC_BASE + offsetof(RCC_TypeDef, RCK12SELR), RCC_RCK12SELR_PLL12SRCRDY>;

namespace PLL1 {
using Enable = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL1CR), RCC_PLL1CR_PLLON>;
using Ready = RegisterBits<ReadOnly, RCC_BASE + offsetof(RCC_TypeDef, PLL1CR), RCC_PLL1CR_PLL1RDY>;
using DIVPEnable = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL1CR), RCC_PLL1CR_DIVPEN>;
using DIVQEnable = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL1CR), RCC_PLL1CR_DIVQEN>;
using DIVREnable = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL1CR), RCC_PLL1CR_DIVREN>;

using DIVM1 = RegisterSection<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL1CFGR1), RCC_PLL1CFGR1_DIVM1_Pos ,6 >;
using DIVN = RegisterSection<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL1CFGR1), RCC_PLL1CFGR1_DIVN_Pos, 9>;
using DIVP = RegisterSection<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL1CFGR2), RCC_PLL1CFGR2_DIVP_Pos, 7>;
using DIVQ = RegisterSection<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL1CFGR2), RCC_PLL1CFGR2_DIVQ_Pos, 7>;
using DIVR = RegisterSection<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL1CFGR2), RCC_PLL1CFGR2_DIVR_Pos, 7>;
using FRACValue = RegisterSection<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL1FRACR), RCC_PLL1FRACR_FRACV_Pos, 13>;
using FRACLatch = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL1FRACR), RCC_PLL1FRACR_FRACLE>;

namespace SpreadSpectrumClockGen {
using Enable = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL1CR), RCC_PLL1CR_SSCG_CTRL>;
using ModPeriod = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL1CSGR), RCC_PLL1CSGR_MOD_PER>;
using IncStep = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL1CSGR), RCC_PLL1CSGR_INC_STEP>;
using DitheringRPDFDisable = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL1CSGR), RCC_PLL1CSGR_RPDFN_DIS>;
using ModeCenterDown = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL1CSGR), RCC_PLL1CSGR_SSCG_MODE>;
using DitheringTPDFDisable = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL1CSGR), RCC_PLL1CSGR_TPDFN_DIS>;
} // namespace SpreadSpectrumClock
} // namespace PLL1

namespace PLL2 {
using Enable = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL2CR), RCC_PLL2CR_PLLON>;
using Ready = RegisterBits<ReadOnly, RCC_BASE + offsetof(RCC_TypeDef, PLL2CR), RCC_PLL2CR_PLL2RDY>;
using DIVPEnable = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL2CR), RCC_PLL2CR_DIVPEN>;
using DIVQEnable = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL2CR), RCC_PLL2CR_DIVQEN>;
using DIVREnable = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL2CR), RCC_PLL2CR_DIVREN>;

using DIVM2 = RegisterSection<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL2CFGR1), RCC_PLL2CFGR1_DIVM2_Pos ,6 >;
using DIVN = RegisterSection<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL2CFGR1), RCC_PLL2CFGR1_DIVN_Pos, 9>;
using DIVP = RegisterSection<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL2CFGR2), RCC_PLL2CFGR2_DIVP_Pos, 7>;
using DIVQ = RegisterSection<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL2CFGR2), RCC_PLL2CFGR2_DIVQ_Pos, 7>;
using DIVR = RegisterSection<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL2CFGR2), RCC_PLL2CFGR2_DIVR_Pos, 7>;
using FRACValue = RegisterSection<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL2FRACR), RCC_PLL2FRACR_FRACV_Pos, 13>;
using FRACLatch = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL2FRACR), RCC_PLL2FRACR_FRACLE>;

namespace SpreadSpectrumClockGen {
using Enable = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL2CR), RCC_PLL2CR_SSCG_CTRL>;
using ModPeriod = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL2CSGR), RCC_PLL2CSGR_MOD_PER>;
using IncStep = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL2CSGR), RCC_PLL2CSGR_INC_STEP>;
using DitheringRPDFDisable = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL2CSGR), RCC_PLL2CSGR_RPDFN_DIS>;
using ModeCenterDown = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL2CSGR), RCC_PLL2CSGR_SSCG_MODE>;
using DitheringTPDFDisable = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, PLL2CSGR), RCC_PLL2CSGR_TPDFN_DIS>;
} // namespace SpreadSpectrumClock
} // namespace PLL2


//RCK12SELR
//  clang-format on

} // namespace RCC_Clocks

} // namespace mdrivlib
