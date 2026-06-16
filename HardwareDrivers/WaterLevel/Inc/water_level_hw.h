#ifndef __WATER_LEVEL_HW_H
#define __WATER_LEVEL_HW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define WATER_LEVEL_PORT    GPIOA
#define WATER_LEVEL_PIN     GPIO_PIN_0

void WaterLevel_HW_Init(void);
GPIO_PinState WaterLevel_HW_ReadRaw(void);

#ifdef __cplusplus
}
#endif

#endif /* __WATER_LEVEL_HW_H */
