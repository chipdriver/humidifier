#ifndef __WATER_LEVEL_SERVICE_H
#define __WATER_LEVEL_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_config.h"
#include "main.h"

#ifndef WATER_LEVEL_HAS_WATER_LEVEL
#define WATER_LEVEL_HAS_WATER_LEVEL    GPIO_PIN_RESET
#endif

typedef enum
{
  WATER_LEVEL_OK = 0,
  WATER_LEVEL_LOW = 1,
  WATER_LEVEL_UNKNOWN = 2
} WaterLevel_Status_t;

void WaterLevelService_Init(void);
void WaterLevelService_Update(void);

GPIO_PinState WaterLevelService_GetRawLevel(void);
WaterLevel_Status_t WaterLevelService_GetStatus(void);

uint8_t WaterLevelService_IsWaterOk(void);
uint8_t WaterLevelService_IsWaterLow(void);

#ifdef __cplusplus
}
#endif

#endif /* __WATER_LEVEL_SERVICE_H */
