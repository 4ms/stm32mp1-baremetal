#pragma once
#include "register_access.hh"
#include "stm32xx.h"

namespace mdrivlib::RCC_Enable_LowPower
{
// Low Power
using LP_ADC12_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2LPENSETR), RCC_MP_AHB2LPENSETR_ADC12LPEN>;
using LP_ADFSDM_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2LPENSETR), RCC_MP_APB2LPENSETR_ADFSDMLPEN>;
using LP_BKPSRAM_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB5LPENSETR), RCC_MP_AHB5LPENSETR_BKPSRAMLPEN>;
using LP_BSEC_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5LPENSETR), RCC_MP_APB5LPENSETR_BSECLPEN>;
using LP_CEC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_CECLPEN>;
using LP_CRC1_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6LPENSETR), RCC_MP_AHB6LPENSETR_CRC1LPEN>;
using LP_CRC2_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3LPENSETR), RCC_MP_AHB3LPENSETR_CRC2LPEN>;
using LP_CRYP1_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB5LPENSETR), RCC_MP_AHB5LPENSETR_CRYP1LPEN>;
using LP_CRYP2_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3LPENSETR), RCC_MP_AHB3LPENSETR_CRYP2LPEN>;
using LP_DAC12_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_DAC12LPEN>;
using LP_DCMI_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3LPENSETR), RCC_MP_AHB3LPENSETR_DCMILPEN>;
using LP_DDRPERFM_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4LPENSETR), RCC_MP_APB4LPENSETR_DDRPERFMLPEN>;
using LP_DFSDM_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2LPENSETR), RCC_MP_APB2LPENSETR_DFSDMLPEN>;
using LP_DMA1_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2LPENSETR), RCC_MP_AHB2LPENSETR_DMA1LPEN>;
using LP_DMA2_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2LPENSETR), RCC_MP_AHB2LPENSETR_DMA2LPEN>;
using LP_DMAMUX_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2LPENSETR), RCC_MP_AHB2LPENSETR_DMAMUXLPEN>;
using LP_DSI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4LPENSETR), RCC_MP_APB4LPENSETR_DSILPEN>;
using LP_DTS_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3LPENSETR), RCC_MP_APB3LPENSETR_DTSLPEN>;
using LP_ETHCK_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6LPENSETR), RCC_MP_AHB6LPENSETR_ETHCKLPEN>;
using LP_ETHMAC_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6LPENSETR), RCC_MP_AHB6LPENSETR_ETHMACLPEN>;
using LP_ETHRX_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6LPENSETR), RCC_MP_AHB6LPENSETR_ETHRXLPEN>;
using LP_ETHTX_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6LPENSETR), RCC_MP_AHB6LPENSETR_ETHTXLPEN>;
using LP_FDCAN_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2LPENSETR), RCC_MP_APB2LPENSETR_FDCANLPEN>;
using LP_FMC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6LPENSETR), RCC_MP_AHB6LPENSETR_FMCLPEN>;
using LP_GPIOA_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4LPENSETR), RCC_MP_AHB4LPENSETR_GPIOALPEN>;
using LP_GPIOB_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4LPENSETR), RCC_MP_AHB4LPENSETR_GPIOBLPEN>;
using LP_GPIOC_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4LPENSETR), RCC_MP_AHB4LPENSETR_GPIOCLPEN>;
using LP_GPIOD_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4LPENSETR), RCC_MP_AHB4LPENSETR_GPIODLPEN>;
using LP_GPIOE_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4LPENSETR), RCC_MP_AHB4LPENSETR_GPIOELPEN>;
using LP_GPIOF_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4LPENSETR), RCC_MP_AHB4LPENSETR_GPIOFLPEN>;
using LP_GPIOG_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4LPENSETR), RCC_MP_AHB4LPENSETR_GPIOGLPEN>;
using LP_GPIOH_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4LPENSETR), RCC_MP_AHB4LPENSETR_GPIOHLPEN>;
using LP_GPIOI_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4LPENSETR), RCC_MP_AHB4LPENSETR_GPIOILPEN>;
using LP_GPIOJ_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4LPENSETR), RCC_MP_AHB4LPENSETR_GPIOJLPEN>;
using LP_GPIOK_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4LPENSETR), RCC_MP_AHB4LPENSETR_GPIOKLPEN>;
using LP_GPIOZ_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB5LPENSETR), RCC_MP_AHB5LPENSETR_GPIOZLPEN>;
using LP_GPU_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6LPENSETR), RCC_MP_AHB6LPENSETR_GPULPEN>;
using LP_HASH1_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB5LPENSETR), RCC_MP_AHB5LPENSETR_HASH1LPEN>;
using LP_HASH2_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3LPENSETR), RCC_MP_AHB3LPENSETR_HASH2LPEN>;
using LP_HSEM_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3LPENSETR), RCC_MP_AHB3LPENSETR_HSEMLPEN>;
using LP_I2C1_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_I2C1LPEN>;
using LP_I2C2_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_I2C2LPEN>;
using LP_I2C3_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_I2C3LPEN>;
using LP_I2C4_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5LPENSETR), RCC_MP_APB5LPENSETR_I2C4LPEN>;
using LP_I2C5_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_I2C5LPEN>;
using LP_I2C6_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5LPENSETR), RCC_MP_APB5LPENSETR_I2C6LPEN>;
using LP_IPCC_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3LPENSETR), RCC_MP_AHB3LPENSETR_IPCCLPEN>;
using LP_IWDG1APB_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5LPENSETR), RCC_MP_APB5LPENSETR_IWDG1APBLPEN>;
using LP_IWDG2APB_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4LPENSETR), RCC_MP_APB4LPENSETR_IWDG2APBLPEN>;
using LP_LPTIM1_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_LPTIM1LPEN>;
using LP_LPTIM2_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3LPENSETR), RCC_MP_APB3LPENSETR_LPTIM2LPEN>;
using LP_LPTIM3_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3LPENSETR), RCC_MP_APB3LPENSETR_LPTIM3LPEN>;
using LP_LPTIM4_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3LPENSETR), RCC_MP_APB3LPENSETR_LPTIM4LPEN>;
using LP_LPTIM5_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3LPENSETR), RCC_MP_APB3LPENSETR_LPTIM5LPEN>;
using LP_LTDC_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4LPENSETR), RCC_MP_APB4LPENSETR_LTDCLPEN>;
using LP_MDIOS_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_MDIOSLPEN>;
using LP_MDMA_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6LPENSETR), RCC_MP_AHB6LPENSETR_MDMALPEN>;
// using LP_MDMA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_TZAHB6LPENSETR),
// RCC_MP_TZAHB6LPENSETR_MDMALPEN>;
using LP_QSPI_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6LPENSETR), RCC_MP_AHB6LPENSETR_QSPILPEN>;
using LP_RETRAM_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_MLAHBLPENSETR), RCC_MP_MLAHBLPENSETR_RETRAMLPEN>;
using LP_RNG1_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB5LPENSETR), RCC_MP_AHB5LPENSETR_RNG1LPEN>;
using LP_RNG2_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3LPENSETR), RCC_MP_AHB3LPENSETR_RNG2LPEN>;
using LP_RTCAPB_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5LPENSETR), RCC_MP_APB5LPENSETR_RTCAPBLPEN>;
using LP_SAI1_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2LPENSETR), RCC_MP_APB2LPENSETR_SAI1LPEN>;
using LP_SAI2_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2LPENSETR), RCC_MP_APB2LPENSETR_SAI2LPEN>;
using LP_SAI3_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2LPENSETR), RCC_MP_APB2LPENSETR_SAI3LPEN>;
using LP_SAI4_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3LPENSETR), RCC_MP_APB3LPENSETR_SAI4LPEN>;
using LP_SDMMC1_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6LPENSETR), RCC_MP_AHB6LPENSETR_SDMMC1LPEN>;
using LP_SDMMC2_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6LPENSETR), RCC_MP_AHB6LPENSETR_SDMMC2LPEN>;
using LP_SDMMC3_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2LPENSETR), RCC_MP_AHB2LPENSETR_SDMMC3LPEN>;
using LP_SPDIF_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_SPDIFLPEN>;
using LP_SPI1_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2LPENSETR), RCC_MP_APB2LPENSETR_SPI1LPEN>;
using LP_SPI2_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_SPI2LPEN>;
using LP_SPI3_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_SPI3LPEN>;
using LP_SPI4_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2LPENSETR), RCC_MP_APB2LPENSETR_SPI4LPEN>;
using LP_SPI5_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2LPENSETR), RCC_MP_APB2LPENSETR_SPI5LPEN>;
using LP_SPI6_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5LPENSETR), RCC_MP_APB5LPENSETR_SPI6LPEN>;
using LP_SRAM1_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_MLAHBLPENSETR), RCC_MP_MLAHBLPENSETR_SRAM1LPEN>;
using LP_SRAM2_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_MLAHBLPENSETR), RCC_MP_MLAHBLPENSETR_SRAM2LPEN>;
using LP_SRAM34_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_MLAHBLPENSETR), RCC_MP_MLAHBLPENSETR_SRAM34LPEN>;
using LP_STGENRO_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4LPENSETR), RCC_MP_APB4LPENSETR_STGENROLPEN>;
using LP_STGEN_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5LPENSETR), RCC_MP_APB5LPENSETR_STGENLPEN>;
using LP_SYSCFG_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3LPENSETR), RCC_MP_APB3LPENSETR_SYSCFGLPEN>;
using LP_SYSRAM_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AXIMLPENSETR), RCC_MP_AXIMLPENSETR_SYSRAMLPEN>;
using LP_TIM12_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_TIM12LPEN>;
using LP_TIM13_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_TIM13LPEN>;
using LP_TIM14_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_TIM14LPEN>;
using LP_TIM15_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2LPENSETR), RCC_MP_APB2LPENSETR_TIM15LPEN>;
using LP_TIM16_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2LPENSETR), RCC_MP_APB2LPENSETR_TIM16LPEN>;
using LP_TIM17_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2LPENSETR), RCC_MP_APB2LPENSETR_TIM17LPEN>;
using LP_TIM1_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2LPENSETR), RCC_MP_APB2LPENSETR_TIM1LPEN>;
using LP_TIM2_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_TIM2LPEN>;
using LP_TIM3_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_TIM3LPEN>;
using LP_TIM4_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_TIM4LPEN>;
using LP_TIM5_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_TIM5LPEN>;
using LP_TIM6_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_TIM6LPEN>;
using LP_TIM7_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_TIM7LPEN>;
using LP_TIM8_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2LPENSETR), RCC_MP_APB2LPENSETR_TIM8LPEN>;
using LP_TZC1_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5LPENSETR), RCC_MP_APB5LPENSETR_TZC1LPEN>;
using LP_TZC2_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5LPENSETR), RCC_MP_APB5LPENSETR_TZC2LPEN>;
using LP_TZPC_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5LPENSETR), RCC_MP_APB5LPENSETR_TZPCLPEN>;
using LP_UART4_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_UART4LPEN>;
using LP_UART5_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_UART5LPEN>;
using LP_UART7_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_UART7LPEN>;
using LP_UART8_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_UART8LPEN>;
using LP_USART1_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5LPENSETR), RCC_MP_APB5LPENSETR_USART1LPEN>;
using LP_USART2_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_USART2LPEN>;
using LP_USART3_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1LPENSETR), RCC_MP_APB1LPENSETR_USART3LPEN>;
using LP_USART6_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2LPENSETR), RCC_MP_APB2LPENSETR_USART6LPEN>;
using LP_USBH_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6LPENSETR), RCC_MP_AHB6LPENSETR_USBHLPEN>;
using LP_USBO_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2LPENSETR), RCC_MP_AHB2LPENSETR_USBOLPEN>;
using LP_USBPHY_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4LPENSETR), RCC_MP_APB4LPENSETR_USBPHYLPEN>;
using LP_VREF_ =
	RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3LPENSETR), RCC_MP_APB3LPENSETR_VREFLPEN>;
} // namespace mdrivlib::RCC_Enable_LowPower
