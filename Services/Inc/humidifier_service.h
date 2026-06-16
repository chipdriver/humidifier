#ifndef __HUMIDIFIER_SERVICE_H
#define __HUMIDIFIER_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef enum
{
  HUMIDIFIER_STATE_OFF = 0,
  HUMIDIFIER_STATE_ON = 1,
  HUMIDIFIER_STATE_WATER_LOW = 2,
  HUMIDIFIER_STATE_SENSOR_ERROR = 3
} Humidifier_State_t;

typedef struct
{
  Humidifier_State_t state;
  uint8_t mist_on;
  uint8_t fan_on;
  uint8_t water_ok;
  uint8_t sensor_ok;
} Humidifier_Status_t;

void HumidifierService_Init(void);
void HumidifierService_Update(void);

void HumidifierService_ForceOff(void);

uint8_t HumidifierService_IsMistOn(void);
uint8_t HumidifierService_IsFanOn(void);

Humidifier_State_t HumidifierService_GetState(void);
Humidifier_Status_t HumidifierService_GetStatus(void);

#ifdef __cplusplus
}
#endif

#endif /* __HUMIDIFIER_SERVICE_H */
