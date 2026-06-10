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
