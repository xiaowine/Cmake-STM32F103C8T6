#include "st7789.h"
#include <stdio.h>
#include <stdlib.h>

// Static variables to store GPIO ports, pins, and SPI handle
static GPIO_TypeDef* CS_Port;
static uint16_t CS_Pin;
static GPIO_TypeDef* DC_Port;
static uint16_t DC_Pin;
static GPIO_TypeDef* RST_Port;
static uint16_t RST_Pin;
static SPI_HandleTypeDef* hspi;

void ST7789_Init(GPIO_TypeDef* csPort, uint16_t csPin, GPIO_TypeDef* dcPort, uint16_t dcPin, GPIO_TypeDef* rstPort, uint16_t rstPin, SPI_HandleTypeDef* hspiHandle)
{
    // Store the GPIO ports, pins, and SPI handle
    CS_Port = csPort;
    CS_Pin = csPin;
    DC_Port = dcPort;
    DC_Pin = dcPin;
    RST_Port = rstPort;
    RST_Pin = rstPin;
    hspi = hspiHandle;

    printf("Initializing ST7789...\n");

    HAL_GPIO_WritePin(RST_Port, RST_Pin, GPIO_PIN_RESET); // Reset the display
    HAL_Delay(5);
    HAL_GPIO_WritePin(RST_Port, RST_Pin, GPIO_PIN_SET);
    HAL_Delay(120);

    ST7789_SendCommand(0x36);
    ST7789_SendData(0x00);

    ST7789_SendCommand(0x3A);
    ST7789_SendData(0x05);

    ST7789_SendCommand(0xB2);
    ST7789_SendData(0x0C);
    ST7789_SendData(0x0C);
    ST7789_SendData(0x00);
    ST7789_SendData(0x33);
    ST7789_SendData(0x33);

    ST7789_SendCommand(0xB7);
    ST7789_SendData(0x35);

    ST7789_SendCommand(0xBB);
    ST7789_SendData(0x19);

    ST7789_SendCommand(0xC0);
    ST7789_SendData(0x2C);

    ST7789_SendCommand(0xC2);
    ST7789_SendData(0x01);

    ST7789_SendCommand(0xC3);
    ST7789_SendData(0x12);

    ST7789_SendCommand(0xC4);
    ST7789_SendData(0x20);

    ST7789_SendCommand(0xC6);
    ST7789_SendData(0x0F);

    ST7789_SendCommand(0xD0);
    ST7789_SendData(0xA4);
    ST7789_SendData(0xA1);

    ST7789_SendCommand(0xE0);
    ST7789_SendData(0xD0);
    ST7789_SendData(0x04);
    ST7789_SendData(0x0D);
    ST7789_SendData(0x11);
    ST7789_SendData(0x13);
    ST7789_SendData(0x2B);
    ST7789_SendData(0x3F);
    ST7789_SendData(0x54);
    ST7789_SendData(0x4C);
    ST7789_SendData(0x18);
    ST7789_SendData(0x0D);
    ST7789_SendData(0x0B);
    ST7789_SendData(0x1F);
    ST7789_SendData(0x23);

    ST7789_SendCommand(0xE1);
    ST7789_SendData(0xD0);
    ST7789_SendData(0x04);
    ST7789_SendData(0x0C);
    ST7789_SendData(0x11);
    ST7789_SendData(0x13);
    ST7789_SendData(0x2C);
    ST7789_SendData(0x3F);
    ST7789_SendData(0x44);
    ST7789_SendData(0x51);
    ST7789_SendData(0x2F);
    ST7789_SendData(0x1F);
    ST7789_SendData(0x1F);
    ST7789_SendData(0x20);
    ST7789_SendData(0x23);

    ST7789_SendCommand(0x21);

    ST7789_SendCommand(0x11);
    HAL_Delay(120);

    ST7789_SendCommand(0x29);

    printf("ST7789 Initialized.\n");
}

void ST7789_SendCommand(uint8_t cmd)
{
    printf("Sending command: 0x%02X\n", cmd);
    HAL_GPIO_WritePin(DC_Port, DC_Pin, GPIO_PIN_RESET); // DC pin low for command
    HAL_GPIO_WritePin(CS_Port, CS_Pin, GPIO_PIN_RESET); // CS pin low
    HAL_SPI_Transmit(hspi, &cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(CS_Port, CS_Pin, GPIO_PIN_SET); // CS pin high
}

void ST7789_SendData(uint8_t data)
{
    printf("Sending data: 0x%02X\n", data);
    HAL_GPIO_WritePin(DC_Port, DC_Pin, GPIO_PIN_SET); // DC pin high for data
    HAL_GPIO_WritePin(CS_Port, CS_Pin, GPIO_PIN_RESET); // CS pin low
    HAL_SPI_Transmit(hspi, &data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(CS_Port, CS_Pin, GPIO_PIN_SET); // CS pin high
}

void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    printf("Drawing pixel at (%d, %d) with color 0x%04X\n", x, y, color);
    ST7789_SetAddressWindow(x, y, x, y);
    uint8_t data[] = {color >> 8, color & 0xFF};
    ST7789_SendData(data[0]);
    ST7789_SendData(data[1]);
}

void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    printf("Setting address window: (%d, %d) to (%d, %d)\n", x0, y0, x1, y1);
    ST7789_SendCommand(0x2A);
    ST7789_SendData(x0 >> 8);
    ST7789_SendData(x0 & 0xFF);
    ST7789_SendData(x1 >> 8);
    ST7789_SendData(x1 & 0xFF);

    ST7789_SendCommand(0x2B);
    ST7789_SendData(y0 >> 8);
    ST7789_SendData(y0 & 0xFF);
    ST7789_SendData(y1 >> 8);
    ST7789_SendData(y1 & 0xFF);

    ST7789_SendCommand(0x2C);
}

void ST7789_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    printf("Drawing line from (%d, %d) to (%d, %d) with color 0x%04X\n", x0, y0, x1, y1, color);
    int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy;

    while (1)
    {
        ST7789_DrawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int16_t e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void ST7789_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    printf("Drawing rectangle at (%d, %d) with width %d and height %d and color 0x%04X\n", x, y, w, h, color);
    ST7789_DrawLine(x, y, x + w - 1, y, color);
    ST7789_DrawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
    ST7789_DrawLine(x + w - 1, y + h - 1, x, y + h - 1, color);
    ST7789_DrawLine(x, y + h - 1, x, y, color);
}

void ST7789_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    printf("Filling rectangle at (%d, %d) with width %d and height %d and color 0x%04X\n", x, y, w, h, color);
    for (uint16_t i = y; i < y + h; i++)
    {
        for (uint16_t j = x; j < x + w; j++)
        {
            ST7789_DrawPixel(j, i, color);
        }
    }
}

void ST7789_FillScreen(uint16_t color)
{
    printf("Filling screen with color 0x%04X\n", color);
    ST7789_FillRectangle(0, 0, 240, 320, color);
}