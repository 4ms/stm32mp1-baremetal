
#ifndef LCD_H_
#define LCD_H_

#include "stm32mp1xx_hal.h"
#include "otm8009a.h"

#define LCD_OTM8009A_ID  ((uint32_t) 0)

void LCD_Init(uint8_t, uint32_t);
void LCD_Reset();
uint16_t LCD_GetXSize();
uint16_t LCD_GetYSize();
void Set_Framebuffer(uint32_t fb);
void Error_Handler();


#endif /* LCD_H_ */
