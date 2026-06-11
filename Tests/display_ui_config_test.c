#include "lv_conf.h"

#if LV_FONT_SOURCE_HAN_SANS_SC_16_CJK != 0
#error "The full SourceHanSans 16 CJK subset must stay disabled; use the minimal humidifier fallback font instead."
#endif

#if LV_FONT_MONTSERRAT_20 != 1
#error "Montserrat 20 must stay enabled for single-line numeric values."
#endif

#if LV_FONT_MONTSERRAT_16 != 1
#error "Montserrat 16 must stay enabled for compact target scale labels."
#endif

int main(void)
{
  return 0;
}
