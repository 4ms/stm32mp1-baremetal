/**
  ******************************************************************************
  * @file    stm32mp15xx_disco.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions to manage LEDs, 
  *          push-buttons and COM ports available on STM32MP15xx-DISCO 
  *          discovery board from STMicroelectronics.
  *
  @verbatim
  *
  @endverbatim
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
#include "stm32mp15xx_disco.h"

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup STM32MP15XX_DISCO
  * @{
  */

/** @defgroup STM32MP15XX_DISCO_LOW_LEVEL_Private_TypesDefinitions Private Typedef
  * @{
  */
typedef void (* BSP_EXTI_LineCallback)(void);
/**
  * @}
  */

/** @defgroup STM32MP15XX_DISCO_LOW_LEVEL_Private_Defines Private Def
  * @{
  */

/** @brief STM32MP15XX DISCO BSP Driver version number V2.0.0
  */
#define STM32MP15XX_DISCO_BSP_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define STM32MP15XX_DISCO_BSP_VERSION_SUB1   (0x02U) /*!< [23:16] sub1 version */
#define STM32MP15XX_DISCO_BSP_VERSION_SUB2   (0x00U) /*!< [15:8]  sub2 version */
#define STM32MP15XX_DISCO_BSP_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */
#define STM32MP15XX_DISCO_BSP_VERSION         ((STM32MP15XX_DISCO_BSP_VERSION_MAIN << 24)\
                                             |(STM32MP15XX_DISCO_BSP_VERSION_SUB1 << 16)\
                                             |(STM32MP15XX_DISCO_BSP_VERSION_SUB2 << 8 )\
                                             |(STM32MP15XX_DISCO_BSP_VERSION_RC))
/**
  * @}
  */


/** @defgroup STM32MP15XX_DISCO_LOW_LEVEL_Private_Variables Private Variables
  * @{
  */

static GPIO_TypeDef* LED_PORT[LEDn] = {LED6_GPIO_PORT,
                                       LED5_GPIO_PORT,
                                       LED7_GPIO_PORT,
                                       LED8_GPIO_PORT};

static const uint32_t LED_PIN[LEDn] = {LED6_PIN,
                                       LED5_PIN,
                                       LED7_PIN,
                                       LED8_PIN};


static GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {WAKEUP_BUTTON_GPIO_PORT,
                                             USER_BUTTON_GPIO_PORT,
                                             USER2_BUTTON_GPIO_PORT};

static const uint16_t BUTTON_PIN[BUTTONn] = {WAKEUP_BUTTON_PIN,
                                             USER_BUTTON_PIN,
                                             USER2_BUTTON_PIN};

static const uint16_t BUTTON_IRQn[BUTTONn] = {WAKEUP_BUTTON_EXTI_IRQn,
                                              USER_BUTTON_EXTI_IRQn,
                                              USER2_BUTTON_EXTI_IRQn};

#ifdef HAL_EXTI_MODULE_ENABLED
EXTI_HandleTypeDef   hExtiButtonHandle[BUTTONn];
#endif

#if (USE_BSP_COM_FEATURE == 1)
#ifdef HAL_UART_MODULE_ENABLED
static USART_TypeDef* COM_USART[COMn] = {COM1_UART};

static UART_HandleTypeDef hComHandle[COMn];

#if (USE_COM_LOG == 1)
static COM_TypeDef COM_ActiveLogPort = COM1;
#endif

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
static uint32_t IsComMspCbValid[COMn] = {0};
#endif

#endif
#endif /* (USE_BSP_COM_FEATURE == 1) */

/**
  * @}
  */


/** @defgroup STM32MP15XX_DISCO_LOW_LEVEL_Private_FunctionPrototypes Private Prototypes
  * @{
  */
#ifdef HAL_EXTI_MODULE_ENABLED
static void BUTTON_WAKEUP_EXTI_Callback(void);
static void BUTTON_USER_EXTI_Callback(void);
static void BUTTON_USER2_EXTI_Callback(void);
#endif

#if (USE_BSP_COM_FEATURE == 1)
#ifdef HAL_UART_MODULE_ENABLED
static void COM1_MspInit(UART_HandleTypeDef *huart);
static void COM1_MspDeInit(UART_HandleTypeDef *huart);
#endif
#endif /* (USE_BSP_COM_FEATURE == 1) */
/**
  * @}
  */

  
/** @addtogroup STM32MP15XX_DISCO_LOW_LEVEL_Exported_Functions Exported Functions
  * @{
  */ 

  /**
  * @brief  This method returns the STM32MP15XX DISCO BSP Driver revision
  * @retval version: 0xXYZR (8bits for each decimal, R for RC)
  */
uint32_t BSP_GetVersion(void)
{
  return STM32MP15XX_DISCO_BSP_VERSION;
}

/** @addtogroup STM32MP15XX_DISCO_LOW_LEVEL_Exported_Functions_Group1 LED Functions
  * @{
  */
/**
  * @brief  Configures LEDs.
  * @param  Led LED to be configured.
  *          This parameter can be one of the following values:
  *            @arg  LED5
  *            @arg  LED6
  *            @arg  LED7
  *            @arg  LED8
  */
int32_t BSP_LED_Init(Led_TypeDef Led)
{
int32_t  status = BSP_ERROR_NONE;
  GPIO_InitTypeDef  gpio_init_structure;

  /* Enable the GPIO_LED clock */
  if(Led == LED6)
  {
    LED6_GPIO_CLK_ENABLE();
  }
  else if (Led == LED5)
  {
    LED5_GPIO_CLK_ENABLE();
  }
  else if (Led == LED7)
  {
    LED7_GPIO_CLK_ENABLE();
  }
  else if (Led == LED8)
  {
    LED8_GPIO_CLK_ENABLE();
  }

    /* Configure the GPIO_LED pin */
  gpio_init_structure.Pin = LED_PIN[Led];
  gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  BSP_ENTER_CRITICAL_SECTION(LED_PORT[Led]);
  HAL_GPIO_Init(LED_PORT[Led], &gpio_init_structure);
  BSP_EXIT_CRITICAL_SECTION(LED_PORT[Led]);
  
  /* By default, turn off LED */
  BSP_LED_Off(Led);

return status;
}


/**
  * @brief  DeInit LEDs.
  * @param  Led LED to be configured.
  *          This parameter can be one of the following values:
  *            @arg  LED5
  *            @arg  LED6
  *            @arg  LED7
  *            @arg  LED8
  * @note Led DeInit does not disable the GPIO clock.
  */
int32_t BSP_LED_DeInit(Led_TypeDef Led)
{
int32_t  status = BSP_ERROR_NONE;
  GPIO_InitTypeDef  gpio_init_structure;
  /* Turn off LED */
  BSP_LED_Off(Led);
    /* Configure the GPIO_LED pin */
  gpio_init_structure.Pin = LED_PIN[Led];
  BSP_ENTER_CRITICAL_SECTION(LED_PORT[Led]);
  HAL_GPIO_DeInit(LED_PORT[Led], gpio_init_structure.Pin);
  BSP_EXIT_CRITICAL_SECTION(LED_PORT[Led]);

return status;
}

/**
  * @brief  Turns selected LED On.
  * @param  Led LED to be set on
  *          This parameter can be one of the following values:
  *            @arg  LED5
  *            @arg  LED6
  *            @arg  LED7
  *            @arg  LED8
  */
int32_t BSP_LED_On(Led_TypeDef Led)
{
  int32_t  status = BSP_ERROR_NONE;
  if((Led == LED5) || (Led == LED6))
  {
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
  }
  else if ((Led == LED7) || (Led == LED8))
  {
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
  }
  return status;
}

/**
  * @brief  Turns selected LED Off. 
  * @param  Led LED to be set off
  *          This parameter can be one of the following values:
  *            @arg  LED5
  *            @arg  LED6
  *            @arg  LED7
  *            @arg  LED8
  */
int32_t BSP_LED_Off(Led_TypeDef Led)
{
  int32_t  status = BSP_ERROR_NONE;
  if((Led == LED5) || (Led == LED6))
  {
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
  }
  else if ((Led == LED7) || (Led == LED8))
  {
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
  }

  return status;
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led LED to be toggled
  *          This parameter can be one of the following values:
  *            @arg  LED5
  *            @arg  LED6
  *            @arg  LED7
  *            @arg  LED8
  */
int32_t BSP_LED_Toggle(Led_TypeDef Led)
{
  int32_t  status = BSP_ERROR_NONE;
  /* Toggle GPIO Led Pin */
  HAL_GPIO_TogglePin(LED_PORT[Led], LED_PIN[Led]);
  return status;
}

/**
  * @brief  Get the state of selected LED.
  * @param  Led Specifies the Led.
  *   This parameter can be one of following parameters:
  *            @arg  LED5
  *            @arg  LED6
  *            @arg  LED7
  *            @arg  LED8
  * @retval 0 means off, 1 means on, negative value means error.
  */
int32_t BSP_LED_GetState(Led_TypeDef Led)
{
  int32_t status;

  status = (HAL_GPIO_ReadPin(LED_PORT[Led], LED_PIN[Led]) == GPIO_PIN_RESET) ? 1 : 0;

  return status;
}
/**
  * @}
  */

/** @addtogroup STM32MP15XX_DISCO_LOW_LEVEL_Exported_Functions_Group2 BUTTON Functions
  * @{
  */

/**
  * @brief  Configures button GPIO and EXTI Line.
  * @param  Button Button to be configured
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button 
  *            @arg  BUTTON_USER: User Push Button
  *            @arg  BUTTON_USER2: User2 Push Button
  * @param  ButtonMode Button mode
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_MODE_GPIO: Button will be used as simple IO
  *            @arg  BUTTON_MODE_EXTI: Button will be connected to EXTI line 
  *                                    with interrupt generation capability
  */
int32_t BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
  int32_t  status = BSP_ERROR_NONE;
  GPIO_InitTypeDef      gpio_init_structure;
  static uint32_t  BSP_BUTTON_IT_PRIORITY[BUTTONn] = {BSP_BUTTON_WAKEUP_IT_PRIORITY, BSP_BUTTON_USER_IT_PRIORITY, BSP_BUTTON_USER2_IT_PRIORITY};
#ifdef HAL_EXTI_MODULE_ENABLED
  static uint32_t  BUTTON_EXTI_LINE[BUTTONn]   = {WAKEUP_BUTTON_EXTI_LINE, USER_BUTTON_EXTI_LINE, USER2_BUTTON_EXTI_LINE};
  static BSP_EXTI_LineCallback  ButtonCallback[BUTTONn]     = {BUTTON_WAKEUP_EXTI_Callback, BUTTON_USER_EXTI_Callback, BUTTON_USER2_EXTI_Callback};
#endif

  /* Enable the BUTTON clock */
  switch(Button)
  {
  case BUTTON_WAKEUP:
    /* BUTTON_WAKEUP clock enable */
    WAKEUP_BUTTON_GPIO_CLK_ENABLE();
    break;
  case BUTTON_USER:
    /* BUTTON_USER clock enable */
    USER_BUTTON_GPIO_CLK_ENABLE();    
    break;
  case BUTTON_USER2:
    /* BUTTON_USER2 clock enable */
    USER2_BUTTON_GPIO_CLK_ENABLE();
    break;
  default:
    break;
  }

  if(ButtonMode == BUTTON_MODE_GPIO)
  {
    /* Configure Button pin as input */
    gpio_init_structure.Pin = BUTTON_PIN[Button];
    gpio_init_structure.Mode = GPIO_MODE_INPUT;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
    BSP_ENTER_CRITICAL_SECTION(BUTTON_PORT[Button]);
    HAL_GPIO_Init(BUTTON_PORT[Button], &gpio_init_structure);
    BSP_EXIT_CRITICAL_SECTION(BUTTON_PORT[Button]);
  }
  
  else /*(ButtonMode == BUTTON_MODE_EXTI) */
  {
    /* Configure Button pin as input with External interrupt */
    gpio_init_structure.Pin = BUTTON_PIN[Button];
    gpio_init_structure.Pull = GPIO_NOPULL; //GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_structure.Mode = GPIO_MODE_IT_RISING;
    BSP_ENTER_CRITICAL_SECTION(BUTTON_PORT[Button]);
    HAL_GPIO_Init(BUTTON_PORT[Button], &gpio_init_structure);
    BSP_EXIT_CRITICAL_SECTION(BUTTON_PORT[Button]);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
#ifdef HAL_EXTI_MODULE_ENABLED
    if (HAL_EXTI_GetHandle(&hExtiButtonHandle[Button], BUTTON_EXTI_LINE[Button]) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
    if (HAL_EXTI_RegisterCallback(&hExtiButtonHandle[Button], HAL_EXTI_RISING_CB_ID, ButtonCallback[Button]) != HAL_OK)
    {
      status = BSP_ERROR_PERIPH_FAILURE;
    }
#endif
#if defined (CORE_CA7)
        IRQ_SetPriority(BUTTON_IRQn[Button], 0x00);
        IRQ_Enable(BUTTON_IRQn[Button]);
#else
        HAL_NVIC_SetPriority((IRQn_Type)(BUTTON_IRQn[Button]), BSP_BUTTON_IT_PRIORITY[Button], 0x00);
        HAL_NVIC_EnableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
#endif

  }
  return status;
}

/**
  * @brief  Push Button DeInit.
  * @param  Button Button to be configured
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button 
  *            @arg  BUTTON_USER: User Push Button
  *            @arg  BUTTON_USER2: User2 Push Button
  * @note PB DeInit does not disable the GPIO clock
  */
int32_t BSP_PB_DeInit(Button_TypeDef Button)
{
  GPIO_InitTypeDef gpio_init_structure;
  gpio_init_structure.Pin = BUTTON_PIN[Button];

  /* Disable the BUTTON clock */
  switch(Button)
  {
  case BUTTON_WAKEUP:
    /* BUTTON_WAKEUP clock disable */
    WAKEUP_BUTTON_GPIO_CLK_DISABLE();
    break;
  case BUTTON_USER:
    /* BUTTON_USER clock disable */
    USER_BUTTON_GPIO_CLK_DISABLE();    
    break;
  case BUTTON_USER2:
    /* BUTTON_USER2 clock disable */
    USER2_BUTTON_GPIO_CLK_DISABLE(); 
    break;
  default:
    break;
  }
  
#if defined (CORE_CA7)
  IRQ_Disable(BUTTON_IRQn[Button]);
#else
  HAL_NVIC_DisableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
#endif

  BSP_ENTER_CRITICAL_SECTION(BUTTON_PORT[Button]);
  HAL_GPIO_DeInit(BUTTON_PORT[Button], gpio_init_structure.Pin);
  BSP_EXIT_CRITICAL_SECTION(BUTTON_PORT[Button]);

  return BSP_ERROR_NONE;
}


/**
  * @brief  Returns the selected button state.
  * @param  Button Button to be checked
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_WAKEUP: Wakeup Push Button 
  *            @arg  BUTTON_USER: User Push Button
  *            @arg  BUTTON_USER2: User2 Push Button
  * @retval The Button GPIO pin value (GPIO_PIN_RESET = button pressed)
  */
uint32_t BSP_PB_GetState(Button_TypeDef Button)
{
  return (int32_t) HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}

#ifdef HAL_EXTI_MODULE_ENABLED
/**
  * @brief  This function handles EXTI_LINE_0 interrupt request.
  * @retval None
  */
void BSP_PB_WAKEUP_EXTI_LINE_0_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hExtiButtonHandle[BUTTON_WAKEUP]);
}

/**
  * @brief  This function handles EXTI_LINE_13 interrupt request.
  * @retval None
  */
void BSP_PB_USER_EXTI_LINE_14_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hExtiButtonHandle[BUTTON_USER]);
}

/**
  * @brief  This function handles EXTI_LINE_14 interrupt request.
  * @retval None
  */
void BSP_PB_USER2_EXTI_LINE_13_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hExtiButtonHandle[BUTTON_USER2]);
}
#endif


/**
  * @}
  */

#if (USE_BSP_COM_FEATURE == 1)
/** @addtogroup STM32MP15XX_DISCO_LOW_LEVEL_Exported_Functions_Group3 COM Functions
  * @{
  */
/**
  * @brief  Configures COM port.
  * @param  COM COM port to be configured.
  *          This parameter can be one of the following values:
  *            @arg  COM1 
  * @param  COM_Init Pointer to a UART_HandleTypeDef structure that contains the
  *                  configuration information for the specified USART peripheral.
  * @retval BSP error code
  */
#ifdef HAL_UART_MODULE_ENABLED
int32_t BSP_COM_Init(COM_TypeDef COM, COM_InitTypeDef *COM_Init)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
    /* Init the UART Msp */
    COM1_MspInit(&hComHandle[COM]);
#else
    if(IsComMspCbValid[COM] == 0U)
    {
      if(BSP_COM_RegisterDefaultMspCallbacks(COM) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_MSP_FAILURE;
      }
    }
#endif

    if(MX_UART4_Init(&hComHandle[COM], COM_Init) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  DeInit COM port.
  * @param  COM COM port to be configured.
  *          This parameter can be one of the following values:
  *            @arg  COM1 
  * @retval BSP status
  */
int32_t BSP_COM_DeInit(COM_TypeDef COM)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* USART configuration */
    hComHandle[COM].Instance = COM_USART[COM];

#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
    COM1_MspDeInit(&hComHandle[COM]);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 0) */

    if(HAL_UART_DeInit(&hComHandle[COM]) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  Configures COM port.
  * @param  huart USART handle
  * @param  COM_Init Pointer to a UART_HandleTypeDef structure that contains the
  *                  configuration information for the specified USART peripheral.
  * @retval HAL error code
  */
__weak HAL_StatusTypeDef MX_UART4_Init(UART_HandleTypeDef *huart, COM_InitTypeDef *COM_Init)
{
  /* USART configuration */
 huart->Instance          = COM_USART[COM1];
 huart->Init.BaudRate     = COM_Init->BaudRate;
 huart->Init.Mode         = UART_MODE_TX_RX;
 huart->Init.Parity       = (uint32_t)COM_Init->Parity;
 huart->Init.WordLength   = COM_Init->WordLength;
 huart->Init.StopBits     = (uint32_t)COM_Init->StopBits;
 huart->Init.HwFlowCtl    = (uint32_t)COM_Init->HwFlowCtl;
 huart->Init.OverSampling = UART_OVERSAMPLING_8;

 return HAL_UART_Init(huart);
}

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
/**
  * @brief Register Default COM Msp Callbacks
  * @param  COM COM port to be configured.
  *          This parameter can be COM1
  * @retval BSP status
  */
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    __HAL_UART_RESET_HANDLE_STATE(&hComHandle[COM]);

    /* Register default MspInit/MspDeInit Callback */
    if(HAL_UART_RegisterCallback(&hComHandle[COM], HAL_UART_MSPINIT_CB_ID, COM1_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_UART_RegisterCallback(&hComHandle[COM], HAL_UART_MSPDEINIT_CB_ID, COM1_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsComMspCbValid[COM] = 1U;
    }
  }

  /* BSP status */
  return ret;
}

/**
  * @brief Register COM1 Msp Callback registering
  * @param  COM COM port to be configured.
  *          This parameter can be COM1
  * @param Callback     pointer to COM1 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_COM_RegisterMspCallbacks(COM_TypeDef COM, BSP_COM_Cb_t *Callback)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    __HAL_UART_RESET_HANDLE_STATE(&hComHandle[COM]);

    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_UART_RegisterCallback(&hComHandle[COM], HAL_UART_MSPINIT_CB_ID, Callback->pMspInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_UART_RegisterCallback(&hComHandle[COM], HAL_UART_MSPDEINIT_CB_ID, Callback->pMspDeInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsComMspCbValid[COM] = 1U;
    }
  }

  /* BSP status */
  return ret;
}
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

#if (USE_COM_LOG == 1)
/**
  * @brief  Select the active COM port.
  * @param  COM COM port to be activated.
  *          This parameter can be COM1
  * @retval BSP status
  */
int32_t BSP_COM_SelectLogPort(COM_TypeDef COM)
{
  if(COM_ActiveLogPort != COM)
  {
    COM_ActiveLogPort = COM;
  }
  return BSP_ERROR_NONE;
}

/**
  * @brief  Redirect console output to COM
  * @param  ch: character to send
  * @param  f: pointer to file (not used)
  * @retval The character received
  */
 #ifdef __GNUC__
 int __io_putchar (int ch)
 #else
 int fputc (int ch, FILE *f)
 #endif /* __GNUC__ */
{
  HAL_UART_Transmit (&hComHandle [COM_ActiveLogPort], (uint8_t *) &ch, 1, COM1_POLL_TIMEOUT);
  return ch;
}

/**
  * @brief  Get console input from COM
  * @param  f: pointer to file (not used)
  * @retval The character received
  */
#ifdef __GNUC__
int __io_getchar (void)
#else
int fgetc(FILE * f)
#endif /* __GNUC__ */
{
  uint8_t ch = 0;
  HAL_UART_Receive(&hComHandle[COM_ActiveLogPort], (uint8_t *)&ch, 1, COM1_POLL_TIMEOUT);
  return ch;
}
#endif /* USE_COM_LOG */

#endif /* HAL_UART_MODULE_ENABLED */
/**
  * @}
  */
#endif /* (USE_BSP_COM_FEATURE == 1) */

/**
  * @}
  */
  
/** @addtogroup STM32MP15XX_DISCO_LOW_LEVEL_Private_Functions Private Functions
  * @{
  */ 
  
/** @addtogroup STM32MP15XX_DISCO_LOW_LEVEL_Private_Functions_Group1 BUTTON Functions
  * @{
  */

/**
  * @brief  Wakeup EXTI line detection callbacks.
  * @retval None
  */
static void BUTTON_WAKEUP_EXTI_Callback(void)
{
  BSP_PB_Callback(BUTTON_WAKEUP);
}

/**
  * @brief  User EXTI line detection callbacks.
  * @retval None
  */
static void BUTTON_USER_EXTI_Callback(void)
{
  BSP_PB_Callback(BUTTON_USER);
}

/**
  * @brief  User2 EXTI line detection callbacks.
  * @retval None
  */
static void BUTTON_USER2_EXTI_Callback(void)
{
  BSP_PB_Callback(BUTTON_USER2);
}

/**
  * @brief  BSP Push Button callback
  * @param  Button Specifies the pin connected EXTI line
  * @retval None.
  */
__weak void BSP_PB_Callback(Button_TypeDef Button)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Button);

  /* This function should be implemented by the user application.
     It is called into this driver when an event on Button is triggered. */
}

/**
  * @}
  */  
  
#if (USE_BSP_COM_FEATURE == 1)
#ifdef HAL_UART_MODULE_ENABLED
/** @addtogroup STM32MP15XX_DISCO_LOW_LEVEL_Private_Functions_Group2 COM Functions
  * @{
  */
/**
  * @brief  Initializes UART MSP.
  * @param  huart UART handle
  * @retval None
  */
static void COM1_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef gpio_init_structure;
  /* Enable GPIO clock */
  COM1_TX_GPIO_CLK_ENABLE();
  COM1_RX_GPIO_CLK_ENABLE();

  /* Enable USART clock */
  COM1_CLK_ENABLE();

  /* Configure USART Tx as alternate function */
  gpio_init_structure.Pin = COM1_TX_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Alternate = COM1_TX_AF;
  BSP_ENTER_CRITICAL_SECTION(COM1_TX_GPIO_PORT);
  HAL_GPIO_Init(COM1_TX_GPIO_PORT, &gpio_init_structure);
  BSP_EXIT_CRITICAL_SECTION(COM1_TX_GPIO_PORT);

  /* Configure USART Rx as alternate function */
  gpio_init_structure.Pin = COM1_RX_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = COM1_RX_AF;
  BSP_ENTER_CRITICAL_SECTION(COM1_RX_GPIO_PORT);
  HAL_GPIO_Init(COM1_RX_GPIO_PORT, &gpio_init_structure);
  BSP_EXIT_CRITICAL_SECTION(COM1_RX_GPIO_PORT);
}

/**
  * @brief  Initialize USART6 Msp part
  * @param  huart UART handle
  * @retval None
  */
static void COM1_MspDeInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef gpio_init_structure;

  /* COM GPIO pin configuration */
  gpio_init_structure.Pin  = COM1_TX_PIN;
  BSP_ENTER_CRITICAL_SECTION(COM1_TX_GPIO_PORT);
  HAL_GPIO_DeInit(COM1_TX_GPIO_PORT, gpio_init_structure.Pin);
  BSP_EXIT_CRITICAL_SECTION(COM1_TX_GPIO_PORT);

  gpio_init_structure.Pin  = COM1_RX_PIN;
  BSP_ENTER_CRITICAL_SECTION(COM1_RX_GPIO_PORT);
  HAL_GPIO_DeInit(COM1_RX_GPIO_PORT, gpio_init_structure.Pin);
  BSP_EXIT_CRITICAL_SECTION(COM1_RX_GPIO_PORT);
  /* Disable USART clock */
  COM1_CLK_DISABLE();

}
#endif
#endif /* (USE_BSP_COM_FEATURE == 1) */

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
