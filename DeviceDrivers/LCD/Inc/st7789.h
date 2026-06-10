#ifndef __ST7789_H
#define __ST7789_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "lcd_hw.h"

#define ST7789_WIDTH        172
#define ST7789_HEIGHT       320

#define ST7789_X_OFFSET     34
#define ST7789_Y_OFFSET     0

#define ST7789_COLOR_BLACK   0x0000
#define ST7789_COLOR_WHITE   0xFFFF
#define ST7789_COLOR_RED     0xF800
#define ST7789_COLOR_GREEN   0x07E0
#define ST7789_COLOR_BLUE    0x001F
#define ST7789_COLOR_YELLOW  0xFFE0
#define ST7789_COLOR_CYAN    0x07FF
#define ST7789_COLOR_MAGENTA 0xF81F

void ST7789_Init(void);
void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ST7789_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ST7789_WriteColor(uint16_t color);
void ST7789_WritePixels(uint16_t *colors, uint32_t len);
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7789_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7789_FillScreen(uint16_t color);
void ST7789_DrawChar(uint16_t x, uint16_t y, char ch, uint16_t color, uint16_t bg_color);
void ST7789_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg_color);
void ST7789_TestColorCycle(void);

#ifdef __cplusplus
}
#endif

#endif /* __ST7789_H */
