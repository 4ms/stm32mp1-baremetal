#pragma once
#ifndef STM32XX_H_INCLUDE_ONCE
#define STM32XX_H_INCLUDE_ONCE // override actual stm32xx.h
#endif

#include "stubs/core_m7_stubs.hh"
#include <cstdint>

// Copied from core_cm7.h:
typedef struct {
	__IM uint32_t TYPE;		/*!< Offset: 0x000 (R/ )  MPU Type Register */
	__IOM uint32_t CTRL;	/*!< Offset: 0x004 (R/W)  MPU Control Register */
	__IOM uint32_t RNR;		/*!< Offset: 0x008 (R/W)  MPU Region RNRber Register */
	__IOM uint32_t RBAR;	/*!< Offset: 0x00C (R/W)  MPU Region Base Address Register */
	__IOM uint32_t RASR;	/*!< Offset: 0x010 (R/W)  MPU Region Attribute and Size Register */
	__IOM uint32_t RBAR_A1; /*!< Offset: 0x014 (R/W)  MPU Alias 1 Region Base Address Register */
	__IOM uint32_t RASR_A1; /*!< Offset: 0x018 (R/W)  MPU Alias 1 Region Attribute and Size Register */
	__IOM uint32_t RBAR_A2; /*!< Offset: 0x01C (R/W)  MPU Alias 2 Region Base Address Register */
	__IOM uint32_t RASR_A2; /*!< Offset: 0x020 (R/W)  MPU Alias 2 Region Attribute and Size Register */
	__IOM uint32_t RBAR_A3; /*!< Offset: 0x024 (R/W)  MPU Alias 3 Region Base Address Register */
	__IOM uint32_t RASR_A3; /*!< Offset: 0x028 (R/W)  MPU Alias 3 Region Attribute and Size Register */
} MPU_Type;

//
// Fake MPU:
//
static MPU_Type MPU_fake;
static MPU_Type *MPU = &MPU_fake;

// Everything else is copied from core_cm7.h:

#define MPU_TYPE_RALIASES 4U

/* MPU Type Register Definitions */
#define MPU_TYPE_IREGION_Pos 16U							  /*!< MPU TYPE: IREGION Position */
#define MPU_TYPE_IREGION_Msk (0xFFUL << MPU_TYPE_IREGION_Pos) /*!< MPU TYPE: IREGION Mask */

#define MPU_TYPE_DREGION_Pos 8U								  /*!< MPU TYPE: DREGION Position */
#define MPU_TYPE_DREGION_Msk (0xFFUL << MPU_TYPE_DREGION_Pos) /*!< MPU TYPE: DREGION Mask */

#define MPU_TYPE_SEPARATE_Pos 0U								 /*!< MPU TYPE: SEPARATE Position */
#define MPU_TYPE_SEPARATE_Msk (1UL /*<< MPU_TYPE_SEPARATE_Pos*/) /*!< MPU TYPE: SEPARATE Mask */

/* MPU Control Register Definitions */
#define MPU_CTRL_PRIVDEFENA_Pos 2U								 /*!< MPU CTRL: PRIVDEFENA Position */
#define MPU_CTRL_PRIVDEFENA_Msk (1UL << MPU_CTRL_PRIVDEFENA_Pos) /*!< MPU CTRL: PRIVDEFENA Mask */

#define MPU_CTRL_HFNMIENA_Pos 1U							 /*!< MPU CTRL: HFNMIENA Position */
#define MPU_CTRL_HFNMIENA_Msk (1UL << MPU_CTRL_HFNMIENA_Pos) /*!< MPU CTRL: HFNMIENA Mask */

#define MPU_CTRL_ENABLE_Pos 0U								 /*!< MPU CTRL: ENABLE Position */
#define MPU_CTRL_ENABLE_Msk (1UL /*<< MPU_CTRL_ENABLE_Pos*/) /*!< MPU CTRL: ENABLE Mask */

/* MPU Region Number Register Definitions */
#define MPU_RNR_REGION_Pos 0U								  /*!< MPU RNR: REGION Position */
#define MPU_RNR_REGION_Msk (0xFFUL /*<< MPU_RNR_REGION_Pos*/) /*!< MPU RNR: REGION Mask */

/* MPU Region Base Address Register Definitions */
#define MPU_RBAR_ADDR_Pos 5U								 /*!< MPU RBAR: ADDR Position */
#define MPU_RBAR_ADDR_Msk (0x7FFFFFFUL << MPU_RBAR_ADDR_Pos) /*!< MPU RBAR: ADDR Mask */

#define MPU_RBAR_VALID_Pos 4U						   /*!< MPU RBAR: VALID Position */
#define MPU_RBAR_VALID_Msk (1UL << MPU_RBAR_VALID_Pos) /*!< MPU RBAR: VALID Mask */

#define MPU_RBAR_REGION_Pos 0U								   /*!< MPU RBAR: REGION Position */
#define MPU_RBAR_REGION_Msk (0xFUL /*<< MPU_RBAR_REGION_Pos*/) /*!< MPU RBAR: REGION Mask */

/* MPU Region Attribute and Size Register Definitions */
#define MPU_RASR_ATTRS_Pos 16U								/*!< MPU RASR: MPU Region Attribute field Position */
#define MPU_RASR_ATTRS_Msk (0xFFFFUL << MPU_RASR_ATTRS_Pos) /*!< MPU RASR: MPU Region Attribute field Mask */

#define MPU_RASR_XN_Pos 28U						 /*!< MPU RASR: ATTRS.XN Position */
#define MPU_RASR_XN_Msk (1UL << MPU_RASR_XN_Pos) /*!< MPU RASR: ATTRS.XN Mask */

#define MPU_RASR_AP_Pos 24U						   /*!< MPU RASR: ATTRS.AP Position */
#define MPU_RASR_AP_Msk (0x7UL << MPU_RASR_AP_Pos) /*!< MPU RASR: ATTRS.AP Mask */

#define MPU_RASR_TEX_Pos 19U						 /*!< MPU RASR: ATTRS.TEX Position */
#define MPU_RASR_TEX_Msk (0x7UL << MPU_RASR_TEX_Pos) /*!< MPU RASR: ATTRS.TEX Mask */

#define MPU_RASR_S_Pos 18U					   /*!< MPU RASR: ATTRS.S Position */
#define MPU_RASR_S_Msk (1UL << MPU_RASR_S_Pos) /*!< MPU RASR: ATTRS.S Mask */

#define MPU_RASR_C_Pos 17U					   /*!< MPU RASR: ATTRS.C Position */
#define MPU_RASR_C_Msk (1UL << MPU_RASR_C_Pos) /*!< MPU RASR: ATTRS.C Mask */

#define MPU_RASR_B_Pos 16U					   /*!< MPU RASR: ATTRS.B Position */
#define MPU_RASR_B_Msk (1UL << MPU_RASR_B_Pos) /*!< MPU RASR: ATTRS.B Mask */

#define MPU_RASR_SRD_Pos 8U							  /*!< MPU RASR: Sub-Region Disable Position */
#define MPU_RASR_SRD_Msk (0xFFUL << MPU_RASR_SRD_Pos) /*!< MPU RASR: Sub-Region Disable Mask */

#define MPU_RASR_SIZE_Pos 1U							/*!< MPU RASR: Region Size Field Position */
#define MPU_RASR_SIZE_Msk (0x1FUL << MPU_RASR_SIZE_Pos) /*!< MPU RASR: Region Size Field Mask */

#define MPU_RASR_ENABLE_Pos 0U								 /*!< MPU RASR: Region enable bit Position */
#define MPU_RASR_ENABLE_Msk (1UL /*<< MPU_RASR_ENABLE_Pos*/) /*!< MPU RASR: Region enable bit Disable Mask */
