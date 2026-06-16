#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define APP_HUMIDITY_LOW_THRESHOLD        65.0f
#define APP_HUMIDITY_HIGH_THRESHOLD       75.0f

#define APP_SENSOR_UPDATE_INTERVAL_MS     2000U
#define APP_DISPLAY_UPDATE_INTERVAL_MS    500U
#define APP_CONTROL_UPDATE_INTERVAL_MS    500U

#define APP_WATER_LEVEL_ENABLE            1U

#define APP_MIST_ENABLE_WHEN_SENSOR_ERROR 0U

/* 当前实测：PA0 = 1 表示有水，PA0 = 0 表示缺水。 */
#define WATER_LEVEL_HAS_WATER_LEVEL       GPIO_PIN_SET

#ifdef __cplusplus
}
#endif

#endif /* __APP_CONFIG_H */
