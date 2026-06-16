#ifndef __AHT20_H
#define __AHT20_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "i2c_sw.h"

typedef enum
{
  AHT20_OK = 0,
  AHT20_ERROR = 1
} AHT20_Status_t;

typedef struct
{
  float temperature;
  float humidity;
} AHT20_Data_t;

#define AHT20_I2C_ADDR             0x38U

#define AHT20_CMD_STATUS           0x71U
#define AHT20_CMD_INIT             0xBEU
#define AHT20_CMD_TRIGGER          0xACU
#define AHT20_CMD_SOFT_RESET       0xBAU

#define AHT20_INIT_PARAM_1         0x08U
#define AHT20_INIT_PARAM_2         0x00U

#define AHT20_TRIGGER_PARAM_1      0x33U
#define AHT20_TRIGGER_PARAM_2      0x00U

#define AHT20_STATUS_BUSY_MASK     0x80U
#define AHT20_STATUS_CAL_MASK      0x08U

AHT20_Status_t AHT20_Init(void);
AHT20_Status_t AHT20_ReadStatus(uint8_t *status);

uint8_t AHT20_IsBusy(uint8_t status);
uint8_t AHT20_IsCalibrated(uint8_t status);

AHT20_Status_t AHT20_SoftReset(void);
AHT20_Status_t AHT20_TriggerMeasurement(void);

AHT20_Status_t AHT20_ReadRaw(uint8_t raw[6]);
AHT20_Status_t AHT20_ParseRaw(const uint8_t raw[6], AHT20_Data_t *data);
AHT20_Status_t AHT20_ReadData(AHT20_Data_t *data);

AHT20_Status_t AHT20_TestRead(float *temperature, float *humidity);

#ifdef __cplusplus
}
#endif

#endif /* __AHT20_H */
