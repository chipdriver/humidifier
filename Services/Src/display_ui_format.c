#include "display_ui_format.h"

#include <stdio.h>

static int DisplayUI_FloatToX10(float value)
{
  if (value >= 0.0f)
  {
    return (int)((value * 10.0f) + 0.5f);
  }

  return (int)((value * 10.0f) - 0.5f);
}

static void DisplayUI_FormatX10(char *buffer, uint32_t buffer_size, int value_x10, const char *suffix)
{
  int abs_x10 = value_x10;

  if (buffer == (char *)0)
  {
    return;
  }

  if (abs_x10 < 0)
  {
    abs_x10 = -abs_x10;
    (void)snprintf(buffer, buffer_size, "-%d.%d%s", abs_x10 / 10, abs_x10 % 10, suffix);
  }
  else
  {
    (void)snprintf(buffer, buffer_size, "%d.%d%s", abs_x10 / 10, abs_x10 % 10, suffix);
  }
}

void DisplayUI_FormatTemperature(char *buffer, uint32_t buffer_size, float temperature)
{
  char value_text[12] = {0};

  if (buffer == (char *)0)
  {
    return;
  }

  DisplayUI_FormatX10(value_text, sizeof(value_text), DisplayUI_FloatToX10(temperature), "C");
  (void)snprintf(buffer, buffer_size, "温度: %s", value_text);
}

void DisplayUI_FormatHumidity(char *buffer, uint32_t buffer_size, float humidity)
{
  char value_text[12] = {0};

  if (buffer == (char *)0)
  {
    return;
  }

  DisplayUI_FormatX10(value_text, sizeof(value_text), DisplayUI_FloatToX10(humidity), "%");
  (void)snprintf(buffer, buffer_size, "湿度: %s", value_text);
}

const char *DisplayUI_GetSensorErrorText(void)
{
  return "传感器异常";
}
