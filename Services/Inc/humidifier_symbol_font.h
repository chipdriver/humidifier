#ifndef __HUMIDIFIER_SYMBOL_FONT_H
#define __HUMIDIFIER_SYMBOL_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

const lv_font_t *HumidifierSymbolFont_Get16(void);
uint8_t HumidifierSymbolFont_HasGlyph(uint32_t unicode);

#ifdef __cplusplus
}
#endif

#endif /* __HUMIDIFIER_SYMBOL_FONT_H */
