/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           USB_Device/DFU_Standalone/USB_Device/App/usbd_desc.h
 * @author         MCD Application Team
 * @brief          Header for usbd_desc.c file.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_DESC__H__
#define __USBD_DESC__H__
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_def.h"

#define DEVICE_ID1 (UID_BASE)
#define DEVICE_ID2 (UID_BASE + 0x4)
#define DEVICE_ID3 (UID_BASE + 0x8)

#define USB_SIZ_STRING_SERIAL 0x1A

extern USBD_DescriptorsTypeDef DFU_Desc;

#ifdef __cplusplus
}
#endif

#endif /* __USBD_DESC__H__ */
