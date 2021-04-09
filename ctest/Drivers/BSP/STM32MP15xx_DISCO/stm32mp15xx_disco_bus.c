/**
  ******************************************************************************
  * @file    stm32mp15xx_disco_bus.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions to manage LEDs,
  *          push-buttons, external SDRAM, external QSPI Flash, RF EEPROM,
  *          available on STM32MP15XX-DISCO board from 
  *          STMicroelectronics.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32mp15xx_disco_bus.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32MP15XX_DISCO_BUS
  * @{
  */

/** @defgroup STM32MP15XX_DISCO_BUS_Private_Variables BUS Private Variables
  * @{
  */
I2C_HandleTypeDef hI2c4;

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
static uint32_t IsI2c4MspCbValid = 0;
#endif
/**
  * @}
  */

/** @defgroup STM32MP15XX_DISCO_BUS_Private_FunctionPrototypes BUS Private FunctionPrototypes
  * @{
  */

static void I2C4_MspInit(I2C_HandleTypeDef *hI2c);
static void I2C4_MspDeInit(I2C_HandleTypeDef *hI2c);
static int32_t I2C4_WriteReg(uint16_t DevAddr, uint16_t MemAddSize, uint16_t Reg, uint8_t *pData, uint16_t Length);
static int32_t I2C4_ReadReg(uint16_t DevAddr, uint16_t MemAddSize, uint16_t Reg, uint8_t *pData, uint16_t Length);

/**
  * @}
  */

/** @defgroup STM32MP15XX_DISCO_BUS_Private_Functions BUS Private Functions
  * @{
  */

/**
  * @brief  Initialize BSP I2C4.
  * @retval BSP status.
  */
int32_t BSP_I2C4_Init(void)
{
  int32_t status = BSP_ERROR_NONE;
  
  if(HAL_I2C_GetState(&hI2c4) == HAL_I2C_STATE_RESET)
  {
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
    /* Init the I2C Msp */
    I2C4_MspInit(&hI2c4);
#else
    if(IsI2c4MspCbValid == 0U)
    {
      if(BSP_I2C4_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
      {
    	status = BSP_ERROR_MSP_FAILURE;
      }
    }
 #endif   
    if( MX_I2C4_Init(&hI2c4) != HAL_OK)
    {
        status = BSP_ERROR_BUS_FAILURE;
    }

  }
  return status;
}

/**
  * @brief  DeInitializes BSP I2C4.
  * @retval BSP status.
  */
int32_t BSP_I2C4_DeInit(void)
{
  int32_t status = BSP_ERROR_NONE;

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)  
  I2C4_MspDeInit(&hI2c4);  
#endif /* (USE_HAL_I2C_REGISTER_CALLBACKS == 0) */
  
  /* Init the I2C */  
  if (HAL_I2C_DeInit(&hI2c4) != HAL_OK)
  {
    status = BSP_ERROR_PERIPH_FAILURE;
  }
  
  return status;
}

/**
  * @brief  MX I2C4 initialization.
  * @param hI2c I2C handle
  * @retval HAL status
  */
__weak HAL_StatusTypeDef MX_I2C4_Init(I2C_HandleTypeDef *hI2c)
{
  hI2c4.Instance              = DISCO_I2C4;
  hI2c->Init.Timing           = DISCO_I2C4_TIMING;
  hI2c->Init.OwnAddress1      = STPMU1_I2C_ADDRESS;
  hI2c->Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
  hI2c->Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
  hI2c->Init.OwnAddress2      = 0;
  hI2c->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hI2c->Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
  hI2c->Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
  
  return HAL_I2C_Init(hI2c);
}


/**
  * @brief  Write a 8bit value in a register of the device through BUS.
  * @param  DevAddr   Device address on Bus.
  * @param  Reg    The target register address to write
  * @param  pData  The target register value to be written
  * @param  Length buffer size to be written
  * @retval BSP status
  */
int32_t BSP_I2C4_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  return I2C4_WriteReg(DevAddr, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length);
}

/**
  * @brief  Read a 8bit register of the device through BUS
  * @param  DevAddr Device address on BUS
  * @param  Reg     The target register address to read
  * @param  pData   Pointer to data buffer
  * @param  Length  Length of the data
  * @retval BSP status
  */
int32_t BSP_I2C4_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  return I2C4_ReadReg(DevAddr, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length);
}

/**
  * @brief  Write a 16bit value in a register of the device through BUS.
  * @param  DevAddr   Device address on Bus.
  * @param  Reg    The target register address to write
  * @param  pData  The target register value to be written
  * @param  Length buffer size to be written
  * @retval BSP status
  */
int32_t BSP_I2C4_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  return I2C4_WriteReg(DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length);
}

/**
  * @brief  Read a 16bit register of the device through BUS
  * @param  DevAddr Device address on BUS
  * @param  Reg     The target register address to read
  * @param  pData   Pointer to data buffer
  * @param  Length  Length of the data
  * @retval BSP status
  */
int32_t BSP_I2C4_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length)
{
  return I2C4_ReadReg(DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length);
}

/**
  * @brief  Checks if target device is ready for communication. 
  * @note   This function is used with Memory devices
  * @param  DevAddr  Target device address
  * @param  Trials      Number of trials
  * @retval BSP status
  */
int32_t BSP_I2C4_IsReady(uint16_t DevAddr, uint32_t Trials)
{
  int32_t status = BSP_ERROR_NONE;

  if(HAL_I2C_IsDeviceReady(&hI2c4, DevAddr, Trials, 1000) != HAL_OK)
  {
    status = BSP_ERROR_BUSY;
  }
  
  return status;
}

/**
  * @brief  Delay function
  * @retval Tick value
  */
int32_t BSP_GetTick(void)
{
  return (int32_t)HAL_GetTick();
}

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1) 
/**
  * @brief Register Default I2C4 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_I2C4_RegisterDefaultMspCallbacks (void)
{
  int32_t ret = BSP_ERROR_NONE;
  
  __HAL_I2C_RESET_HANDLE_STATE(&hI2c4);
  
  /* Register default MspInit/MspDeInit Callback */
  if(HAL_I2C_RegisterCallback(&hI2c4, HAL_I2C_MSPINIT_CB_ID, I2C4_MspInit) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else if(HAL_I2C_RegisterCallback(&hI2c4, HAL_I2C_MSPDEINIT_CB_ID, I2C4_MspDeInit) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    IsI2c4MspCbValid = 1U;
  }
  
  /* BSP status */  
  return ret;
}

/**
  * @brief Register I2C4 Bus Msp Callback registering
  * @param Callback     pointer to I2C4 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_I2C4_RegisterMspCallbacks (BSP_I2C_Cb_t *Callback)
{
  int32_t ret = BSP_ERROR_NONE;
  
  __HAL_I2C_RESET_HANDLE_STATE(&hI2c4);
  
  /* Register MspInit/MspDeInit Callbacks */
  if(HAL_I2C_RegisterCallback(&hI2c4, HAL_I2C_MSPINIT_CB_ID, Callback->pMspI2cInitCb) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else if(HAL_I2C_RegisterCallback(&hI2c4, HAL_I2C_MSPDEINIT_CB_ID, Callback->pMspI2cDeInitCb) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    IsI2c4MspCbValid = 1U;
  }
  
  /* BSP status */  
  return ret; 
}
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */

/**
  * @brief  Initializes I2C MSP.
  * @param  hI2c : I2C handler
  * @retval None
  */
static void I2C4_MspInit(I2C_HandleTypeDef *hI2c)
{
  GPIO_InitTypeDef  gpio_init_structure;
  RCC_PeriphCLKInitTypeDef I2C4_clock_source_config;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hI2c);
  
  /*** Configure the GPIOs ***/  
  /* Enable GPIO clock */
  DISCO_I2C4_GPIO_CLK_ENABLE();
  
  /* Configure I2C Tx as alternate function */
  gpio_init_structure.Pin = DISCO_I2C4_SCL_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_OD;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Alternate = DISCO_I2C4_SCL_SDA_AF;
  BSP_ENTER_CRITICAL_SECTION(DISCO_I2C4_SCL_SDA_GPIO_PORT);
  HAL_GPIO_Init(DISCO_I2C4_SCL_SDA_GPIO_PORT, &gpio_init_structure);
  BSP_EXIT_CRITICAL_SECTION(DISCO_I2C4_SCL_SDA_GPIO_PORT);
  
  /* Configure I2C Rx as alternate function */
  gpio_init_structure.Pin = DISCO_I2C4_SDA_PIN;
  BSP_ENTER_CRITICAL_SECTION(DISCO_I2C4_SCL_SDA_GPIO_PORT);
  HAL_GPIO_Init(DISCO_I2C4_SCL_SDA_GPIO_PORT, &gpio_init_structure);
  BSP_EXIT_CRITICAL_SECTION(DISCO_I2C4_SCL_SDA_GPIO_PORT);
  
  /* Enable I2C clock */
  DISCO_I2C4_CLK_ENABLE();
  
  /* Force the I2C peripheral clock reset */  
  DISCO_I2C4_FORCE_RESET();
  
  /* Release the I2C peripheral clock reset */  
  DISCO_I2C4_RELEASE_RESET();
  
  /*** Configure the I2C peripheral clock ***/ 
  I2C4_clock_source_config.I2c46ClockSelection = RCC_I2C46CLKSOURCE_HSI;
  I2C4_clock_source_config.PeriphClockSelection = RCC_PERIPHCLK_I2C46;
  HAL_RCCEx_PeriphCLKConfig(&I2C4_clock_source_config); 
  
#if defined(CORE_CA7)
  IRQ_SetPriority(I2C4_ER_IRQn, 0);
  IRQ_Enable(I2C4_ER_IRQn);
  IRQ_SetPriority(I2C4_EV_IRQn, 0);
  IRQ_Enable(I2C4_EV_IRQn);
#elif defined(CORE_CM4)
  HAL_NVIC_SetPriority(DISCO_I2C4_ER_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(DISCO_I2C4_ER_IRQn);
  HAL_NVIC_SetPriority(DISCO_I2C4_EV_IRQn, 0, 2);
  HAL_NVIC_EnableIRQ(DISCO_I2C4_EV_IRQn);
#endif
}

/**
  * @brief  DeInitializes I2C MSP.
  * @param  hI2c : I2C handler
  * @retval None
  */
static void I2C4_MspDeInit(I2C_HandleTypeDef *hI2c)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hI2c);
  
  /* Configure I2C Tx, Rx as alternate function */
  gpio_init_structure.Pin = DISCO_I2C4_SCL_PIN | DISCO_I2C4_SDA_PIN;
  BSP_ENTER_CRITICAL_SECTION(DISCO_I2C4_SCL_SDA_GPIO_PORT);
  HAL_GPIO_DeInit(DISCO_I2C4_SCL_SDA_GPIO_PORT, gpio_init_structure.Pin);
  BSP_EXIT_CRITICAL_SECTION(DISCO_I2C4_SCL_SDA_GPIO_PORT);

  /* Disable GPIO clock */
  DISCO_I2C4_GPIO_CLK_DISABLE();
  
  /* Disable I2C clock */
  DISCO_I2C4_CLK_DISABLE();

  /* Disable NVIC for I2C */
#if defined(CORE_CA7)
  IRQ_Disable(DISCO_I2C4_ER_IRQn);
  IRQ_Disable(DISCO_I2C4_EV_IRQn);
#elif defined(CORE_CM4)
  HAL_NVIC_DisableIRQ(DISCO_I2C4_ER_IRQn);
  HAL_NVIC_DisableIRQ(DISCO_I2C4_EV_IRQn);
#endif
}

/**
  * @brief  Write values in registers of the device through BUS.
  * @param  DevAddr       Device address on Bus.
  * @param  MemAddSize Size of internal memory address.
  * @param  Reg        The target register start address to write.
  * @param  pData      The target register values to be written.
  * @param  Length     data length in bytes.
  * @retval BSP status.
  */
static int32_t I2C4_WriteReg(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length)
{  
  int32_t status = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Write(&hI2c4, DevAddr, Reg, MemAddSize, pData, Length, 10000) == HAL_OK)
  {
    status = BSP_ERROR_NONE;
  }
  
  return status;
}

/**
  * @brief  Read values in registers of the device through BUS.
  * @param  DevAddr       Device address on Bus.
  * @param  MemAddSize Size of internal memory address.
  * @param  Reg        The target register start address to read.
  * @param  pData      The target register values to be read.
  * @param  Length     data length in bytes.
  * @retval BSP status.
  */
static int32_t I2C4_ReadReg(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize, uint8_t *pData, uint16_t Length)
{  
  int32_t status = BSP_ERROR_BUS_FAILURE;

  if (HAL_I2C_Mem_Read(&hI2c4, DevAddr, Reg, MemAddSize, pData, Length, 10000) == HAL_OK)
  {
	status = BSP_ERROR_NONE;
  }
  
  return status;
}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
