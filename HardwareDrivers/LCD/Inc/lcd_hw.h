#ifndef __LCD_HW_H
#define __LCD_HW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define LCD_RESET_PORT   GPIOB
#define LCD_RESET_PIN    GPIO_PIN_10

#define LCD_DC_PORT      GPIOB
#define LCD_DC_PIN       GPIO_PIN_11

#define LCD_CS_PORT      GPIOB
#define LCD_CS_PIN       GPIO_PIN_12

#define LCD_SCK_PORT     GPIOB
#define LCD_SCK_PIN      GPIO_PIN_13

#define LCD_MOSI_PORT    GPIOB
#define LCD_MOSI_PIN     GPIO_PIN_15

void LCD_HW_GPIO_Init(void);

void LCD_RESET_High(void);
void LCD_RESET_Low(void);

void LCD_DC_High(void);
void LCD_DC_Low(void);

void LCD_CS_High(void);
void LCD_CS_Low(void);

void LCD_SCK_High(void);
void LCD_SCK_Low(void);

void LCD_MOSI_High(void);
void LCD_MOSI_Low(void);

void LCD_SPI_WriteBit(uint8_t bit);
void LCD_SPI_WriteByte(uint8_t data);

void LCD_Reset(void);
void LCD_WriteCmd(uint8_t cmd);
void LCD_WriteData(uint8_t data);
void LCD_WriteDataBuffer(uint8_t *data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* __LCD_HW_H */
