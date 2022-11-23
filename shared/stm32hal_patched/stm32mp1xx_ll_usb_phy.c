#include "stm32mp1xx_hal.h"
#include "stm32mp1xx_ll_rcc.h"
#include <stdlib.h> //For ldiv

#if defined(HAL_PCD_MODULE_ENABLED) || defined(HAL_HCD_MODULE_ENABLED)
#if defined(USB_OTG_FS) || defined(USB_OTG_HS)

HAL_StatusTypeDef USB_HS_PHYCDeInit(void) {
	__HAL_RCC_USBPHY_FORCE_RESET();
	__HAL_RCC_USBPHY_RELEASE_RESET();

	__HAL_RCC_USBPHY_CLK_DISABLE();
	__HAL_RCC_USBPHY_CLK_SLEEP_DISABLE();
	return HAL_OK;
}

HAL_StatusTypeDef USB_HS_PHYCInit(void) {
	__HAL_RCC_USBPHY_CLK_ENABLE();

	// From hftrx: https://github.com/ua1arn/hftrx
	const uint32_t USBPHYCPLLFREQUENCY = 1440000000uL; // 1.44 GHz
	const uint32_t usbphyref = LL_RCC_GetUSBPHYClockFreq(LL_RCC_USBPHY_CLKSOURCE);
	const uint32_t ODF = 0;
	const ldiv_t d = ldiv(USBPHYCPLLFREQUENCY / 4, usbphyref / 4);
	const uint32_t N = d.quot; // 0b0111100;

	const uint32_t FRACTMAX = (USBPHYC_PLL_PLLFRACIN_Msk >> USBPHYC_PLL_PLLFRACIN_Pos) + 1;
	const uint32_t FRACT = d.rem * (uint_fast64_t)FRACTMAX / usbphyref;

	const uint32_t validmask = USBPHYC_PLL_PLLDITHEN1_Msk | USBPHYC_PLL_PLLDITHEN0_Msk | USBPHYC_PLL_PLLFRACCTL_Msk |
							   USBPHYC_PLL_PLLFRACIN_Msk | USBPHYC_PLL_PLLODF_Msk | USBPHYC_PLL_PLLNDIV_Msk |
							   USBPHYC_PLL_PLLSTRBYP_Msk | 0;

	const uint32_t PLLFRACCTL_VAL = (d.rem == 0) ? 0 : USBPHYC_PLL_PLLFRACCTL;
	const uint32_t newPLLvalue = ((N << USBPHYC_PLL_PLLNDIV_Pos) & USBPHYC_PLL_PLLNDIV_Msk) |
								 (ODF << USBPHYC_PLL_PLLODF_Pos) |
								 ((FRACT << USBPHYC_PLL_PLLFRACIN_Pos) & USBPHYC_PLL_PLLFRACIN_Msk) | PLLFRACCTL_VAL |
								 USBPHYC_PLL_PLLSTRBYP_Msk | USBPHYC_PLL_PLLDITHEN0_Msk | USBPHYC_PLL_PLLDITHEN1_Msk;

	if ((newPLLvalue & validmask) != (USBPHYC->PLL & validmask) || (USBPHYC->PLL & USBPHYC_PLL_PLLEN) == 0) {
		USBPHYC->PLL &= ~USBPHYC_PLL_PLLEN;
		while ((USBPHYC->PLL & USBPHYC_PLL_PLLEN) != 0)
			;

		USBPHYC->PLL = (USBPHYC->PLL & ~validmask) | newPLLvalue;
		USBPHYC->PLL = USBPHYC->PLL | USBPHYC_PLL_PLLEN;

		HAL_Delay(10);

		while ((USBPHYC->PLL & USBPHYC_PLL_PLLEN) == 0)
			;
	}
	return HAL_OK;
}

#endif
#endif
