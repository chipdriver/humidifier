#ifndef __SENSOR_SERVICE_H
#define __SENSOR_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "app_config.h"
#include "aht20.h"

typedef enum
{
  SENSOR_SERVICE_OK = 0,
  SENSOR_SERVICE_ERROR = 1,
  SENSOR_SERVICE_NOT_INIT = 2,
  SENSOR_SERVICE_READ_FAIL = 3
} SensorService_Status_t;

typedef struct
{
  float temperature;
  float humidity;
  uint8_t valid;
} SensorService_Data_t;

SensorService_Status_t SensorService_Init(void);
SensorService_Status_t SensorService_Update(void);
SensorService_Status_t SensorService_UpdatePeriodic(void);

SensorService_Status_t SensorService_GetData(SensorService_Data_t *data);

float SensorService_GetTemperature(void);
float SensorService_GetHumidity(void);

uint8_t SensorService_IsDataValid(void);
uint8_t SensorService_IsInitialized(void);

SensorService_Status_t SensorService_TestRead(float *temperature, float *humidity);

#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_SERVICE_H */
