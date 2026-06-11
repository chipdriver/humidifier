#ifndef __DISPLAY_UI_FORMAT_H
#define __DISPLAY_UI_FORMAT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void DisplayUI_FormatTemperature(char *buffer, uint32_t buffer_size, float temperature);
void DisplayUI_FormatHumidity(char *buffer, uint32_t buffer_size, float humidity);

const char *DisplayUI_GetSensorErrorText(void);

#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_UI_FORMAT_H */
