/** 
  ******************************************************************************
  * @file    stm32mp15xx_disco_conf.h
  * @author  MCD Application Team
  * @brief   configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics. 
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the 
  * License. You may obtain a copy of the License at:
  *                       opensource.org/licenses/BSD-3-Clause
  *
  *
  ******************************************************************************  
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32MP15XX_DISCO_CONFIG_H
#define __STM32MP15XX_DISCO_CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32mp1xx_hal.h"


/* Activation of Critical Section */
#define USE_BSP_CRITICAL_SECTION            1U

/* Activation of PMIC */
#define USE_STPMIC1x                        0U

/* Usage of COM feature */
#define USE_BSP_COM_FEATURE                 0U
/* Activation of COM port for log output */
#define USE_COM_LOG                         0U

#if (USE_BSP_CRITICAL_SECTION == 1)
#include "lock_resource.h"
#define BSP_ENTER_CRITICAL_SECTION       PERIPH_LOCK
#define BSP_EXIT_CRITICAL_SECTION        PERIPH_UNLOCK
#else
#define BSP_ENTER_CRITICAL_SECTION(periph)       ((void)0)
#define BSP_EXIT_CRITICAL_SECTION(periph)        ((void)0)
#endif

/* IRQ priorities */
#define BSP_PMU1_IT_PRIORITY                   0x03UL
#define BSP_BUTTON_WAKEUP_IT_PRIORITY          0x0FUL
#define BSP_BUTTON_USER_IT_PRIORITY            0x0FUL
#define BSP_BUTTON_USER2_IT_PRIORITY           0x0FUL
   
#endif /* __STM32MP15XX_DISCO_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
