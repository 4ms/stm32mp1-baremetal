/**
  ******************************************************************************
  * @file    stm32mp15xx_eval_bus.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for STM32MP15XX-EVAL I2C,
  *          push-buttons hardware resources.
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
#ifndef STM32MP15XX_EVAL_BUS_H
#define STM32MP15XX_EVAL_BUS_H

#ifdef __cplusplus
 extern "C" {
#endif


 /* Includes ------------------------------------------------------------------*/
#include "stm32mp15xx_eval_conf.h"
#include "stm32mp15xx_eval_errno.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32MP15XX_EVAL_BUS 
  * @{
  */
 
/** @defgroup STM32MP15XX_EVAL_BUS_Exported_Constants BUS Exported Constants
  * @{
  */


/* Exported constant IO ------------------------------------------------------*/

 /**
   * @brief AUDIO Slave I2C address
   */
 #define AUDIO_I2C_ADDRESS                ((uint16_t)0x36)

/**
  * @brief STPMU1 I2C address
  * (NVM_SECTOR3_REGISTER_7 & 0x7F) << 1
  */
#define STPMU1_I2C_ADDRESS               ((0x33 & 0x7F) << 1 )

/* I2C clock speed configuration (in Hz) 
   WARNING: 
   Make sure that this define is not already declared in other files (ie. 
   stm32mp15xx_eval.h file). It can be used in parallel by other modules. */
#ifndef I2C_SPEED
 #define I2C_SPEED                        ((uint32_t)100000)
#endif /* I2C_SPEED */


/* User can use this section to tailor I2Cx/I2Cx instance used and associated 
   resources */

 /* Definition for I2C2 clock resources */
 #define EVAL_I2C2                             I2C2
 #define EVAL_I2C2_CLK_ENABLE()                __HAL_RCC_I2C2_CLK_ENABLE()
 #define EVAL_I2C2_CLK_DISABLE()               __HAL_RCC_I2C2_CLK_DISABLE()
 #define EVAL_I2C2_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOH_CLK_ENABLE()
 #define EVAL_I2C2_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOH_CLK_DISABLE()

 #define EVAL_I2C2_FORCE_RESET()               __HAL_RCC_I2C2_FORCE_RESET()
 #define EVAL_I2C2_RELEASE_RESET()             __HAL_RCC_I2C2_RELEASE_RESET()

 /* Definition for I2Cx Pins */
 #define EVAL_I2C2_SCL_PIN                     GPIO_PIN_4
 #define EVAL_I2C2_SCL_SDA_GPIO_PORT           GPIOH
 #define EVAL_I2C2_SCL_SDA_AF                  GPIO_AF4_I2C2
 #define EVAL_I2C2_SDA_PIN                     GPIO_PIN_5

 /* I2C interrupt requests */
 #define EVAL_I2C2_EV_IRQn                     I2C2_EV_IRQn
 #define EVAL_I2C2_ER_IRQn                     I2C2_ER_IRQn

/* Definition for I2C4 clock resources */
#define EVAL_I2C4                             I2C4
#define EVAL_I2C4_CLK_ENABLE()                __HAL_RCC_I2C4_CLK_ENABLE()
#define EVAL_I2C4_CLK_DISABLE()               __HAL_RCC_I2C4_CLK_DISABLE()
#define EVAL_I2C4_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOZ_CLK_ENABLE()
#define EVAL_I2C4_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOZ_CLK_DISABLE()

#define EVAL_I2C4_FORCE_RESET()               __HAL_RCC_I2C4_FORCE_RESET()
#define EVAL_I2C4_RELEASE_RESET()             __HAL_RCC_I2C4_RELEASE_RESET()
   
/* Definition for I2Cx Pins */
#define EVAL_I2C4_SCL_PIN                     GPIO_PIN_4
#define EVAL_I2C4_SCL_SDA_GPIO_PORT           GPIOZ
#define EVAL_I2C4_SCL_SDA_AF                  GPIO_AF6_I2C4
#define EVAL_I2C4_SDA_PIN                     GPIO_PIN_5

/* I2C interrupt requests */
#define EVAL_I2C4_EV_IRQn                     I2C4_EV_IRQn
#define EVAL_I2C4_ER_IRQn                     I2C4_ER_IRQn

/* I2C TIMING Register define when I2C clock source is SYSCLK */
/* I2C TIMING is calculated from Bus clock (HSI) = 64 MHz */

#ifndef EVAL_I2Cx_TIMING
#define EVAL_I2Cx_TIMING                      ((uint32_t)0x10805E89)
#endif /* EVAL_I2Cx_TIMING */

/**
  * @}
  */
/** @defgroup STM32MP15XX_EVAL_BUS_Exported_Types BUS Exported Types
  * @{
  */
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
typedef struct
{
  pI2C_CallbackTypeDef  pMspI2cInitCb;
  pI2C_CallbackTypeDef  pMspI2cDeInitCb;
}BSP_I2C_Cb_t;
#endif /* (USE_HAL_I2C_REGISTER_CALLBACKS == 1) */

/**
  * @}
  */

/** @defgroup STM32MP15XX_EVAL_BUS_Exported_Functions BUS Exported Functions
  * @{
  */

int32_t BSP_I2C2_Init(void);
int32_t BSP_I2C2_DeInit(void);
int32_t BSP_I2C2_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C2_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C2_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C2_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C2_Recv(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C2_Send(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C2_IsReady(uint16_t DevAddr, uint32_t Trials);

int32_t BSP_I2C4_Init(void);
int32_t BSP_I2C4_DeInit(void);
int32_t BSP_I2C4_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C4_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C4_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C4_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C4_Recv(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C4_Send(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C4_IsReady(uint16_t DevAddr, uint32_t Trials);
int32_t BSP_GetTick(void);

int32_t BSP_I2C4_WriteBuffer(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C4_ReadBuffer(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length);




#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1) 
int32_t BSP_I2C2_RegisterDefaultMspCallbacks (void);
int32_t BSP_I2C2_RegisterMspCallbacks (BSP_I2C_Cb_t *Callback);
int32_t BSP_I2C4_RegisterDefaultMspCallbacks (void);
int32_t BSP_I2C4_RegisterMspCallbacks (BSP_I2C_Cb_t *Callback);
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
__weak HAL_StatusTypeDef MX_I2C2_Init(I2C_HandleTypeDef *hI2c);
__weak HAL_StatusTypeDef MX_I2C4_Init(I2C_HandleTypeDef *hI2c);


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32MP15XX_EVAL_BUS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
