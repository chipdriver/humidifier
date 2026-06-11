#include "display_ui_format.h"

#include <assert.h>
#include <string.h>

static void test_format_temperature_uses_short_full_line_and_c_unit(void)
{
  char text[24] = {0};

  DisplayUI_FormatTemperature(text, sizeof(text), 25.64f);

  assert(strcmp(text, "温度: 25.6C") == 0);
}

static void test_format_humidity_uses_short_full_line_and_percent(void)
{
  char text[24] = {0};

  DisplayUI_FormatHumidity(text, sizeof(text), 63.24f);

  assert(strcmp(text, "湿度: 63.2%") == 0);
}

static void test_error_text_matches_dashboard_copy(void)
{
  assert(strcmp(DisplayUI_GetSensorErrorText(), "传感器异常") == 0);
}

int main(void)
{
  test_format_temperature_uses_short_full_line_and_c_unit();
  test_format_humidity_uses_short_full_line_and_percent();
  test_error_text_matches_dashboard_copy();

  return 0;
}
