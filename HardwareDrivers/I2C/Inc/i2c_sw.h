#ifndef __I2C_SW_H
#define __I2C_SW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define I2C_SW_SCL_PORT    GPIOB
#define I2C_SW_SCL_PIN     GPIO_PIN_6

#define I2C_SW_SDA_PORT    GPIOB
#define I2C_SW_SDA_PIN     GPIO_PIN_7

typedef enum
{
  I2C_SW_OK = 0,
  I2C_SW_ERROR = 1
} I2C_SW_Status_t;

void I2C_SW_GPIO_Init(void);

void I2C_SW_SCL_High(void);
void I2C_SW_SCL_Low(void);

void I2C_SW_SDA_High(void);
void I2C_SW_SDA_Low(void);

uint8_t I2C_SW_SDA_Read(void);

void I2C_SW_Start(void);
void I2C_SW_Stop(void);

void I2C_SW_WriteBit(uint8_t bit);
uint8_t I2C_SW_ReadBit(void);

I2C_SW_Status_t I2C_SW_WaitAck(void);
void I2C_SW_Ack(void);
void I2C_SW_NAck(void);

I2C_SW_Status_t I2C_SW_WriteByte(uint8_t data);
uint8_t I2C_SW_ReadByte(uint8_t ack);

I2C_SW_Status_t I2C_SW_Write(uint8_t dev_addr_7bit, const uint8_t *data, uint16_t len);
I2C_SW_Status_t I2C_SW_Read(uint8_t dev_addr_7bit, uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __I2C_SW_H */
