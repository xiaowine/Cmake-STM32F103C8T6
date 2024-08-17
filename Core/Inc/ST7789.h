#ifndef ST7789_H
#define ST7789_H

#include "stm32f1xx_hal.h"

// Function prototypes
void ST7789_Init(GPIO_TypeDef* CS_Port, uint16_t CS_Pin, GPIO_TypeDef* DC_Port, uint16_t DC_Pin, GPIO_TypeDef* RST_Port, uint16_t RST_Pin, SPI_HandleTypeDef* hspi);
void ST7789_SendCommand(uint8_t cmd);
void ST7789_SendData(uint8_t data);
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ST7789_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void ST7789_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7789_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7789_FillScreen(uint16_t color);

#endif // ST7789_H