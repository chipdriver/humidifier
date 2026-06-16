#include "display_service.h"

#include "app_config.h"
#include "humidifier_service.h"
#include "sensor_service.h"
#include "st7789.h"
#include "water_level_service.h"

#include <stdio.h>

#define DISPLAY_TEXT_X             6U
#define DISPLAY_TITLE_Y            8U
#define DISPLAY_LINE_1_Y           36U
#define DISPLAY_LINE_2_Y           60U
#define DISPLAY_LINE_3_Y           84U
#define DISPLAY_LINE_4_Y           108U
#define DISPLAY_LINE_5_Y           132U
#define DISPLAY_LINE_6_Y           156U
#define DISPLAY_LINE_H             14U
#define DISPLAY_TEXT_BUFFER_SIZE   32U

static uint8_t s_display_ready = 0U;
static uint32_t s_last_display_tick = 0U;

/**
 * @brief 将 float 数值转换为四舍五入后的 x10 整数。
 * @param value 原始 float 数值。
 * @retval 放大 10 倍后的整数。
 */
static int32_t DisplayService_FloatToX10(float value)
{
  if (value >= 0.0f)
  {
    return (int32_t)((value * 10.0f) + 0.5f);
  }

  return (int32_t)((value * 10.0f) - 0.5f);
}

/**
 * @brief 用整数 x10 格式化一行温湿度文本，避免 float printf。
 * @param buffer 输出缓冲区。
 * @param buffer_size 输出缓冲区大小。
 * @param label 行标题。
 * @param value_x10 放大 10 倍后的数值。
 * @param unit 单位字符串。
 * @retval 无
 */
static void DisplayService_FormatX10Line(char *buffer,
                                         uint32_t buffer_size,
                                         const char *label,
                                         int32_t value_x10,
                                         const char *unit)
{
  int32_t abs_x10 = value_x10;
  const char *sign = "";

  /* 参数无效时直接返回，避免访问空指针。 */
  if ((buffer == (char *)0) || (label == (const char *)0) || (unit == (const char *)0))
  {
    return;
  }

  /* 负数单独处理符号，保证小数位始终为正。 */
  if (abs_x10 < 0)
  {
    sign = "-";
    abs_x10 = -abs_x10;
  }

  /* 只用整数格式化，不依赖 STM32 printf 的 float 支持。 */
  (void)snprintf(buffer,
                 buffer_size,
                 "%s %s%ld.%ld %s",
                 label,
                 sign,
                 (long)(abs_x10 / 10),
                 (long)(abs_x10 % 10),
                 unit);
}

/**
 * @brief 清除一行显示区域。
 * @param y 行起始 Y 坐标。
 * @retval 无
 */
static void DisplayService_ClearLine(uint16_t y)
{
  /* 用黑色矩形覆盖整行，避免较短文本留下旧字符。 */
  ST7789_FillRect(0U, y, ST7789_WIDTH, DISPLAY_LINE_H, ST7789_COLOR_BLACK);
}

/**
 * @brief 绘制一行英文状态文本。
 * @param y 行起始 Y 坐标。
 * @param text 要显示的字符串。
 * @param color RGB565 前景色。
 * @retval 无
 */
static void DisplayService_DrawLine(uint16_t y, const char *text, uint16_t color)
{
  /* 先清除本行旧内容。 */
  DisplayService_ClearLine(y);

  /* 再绘制新的 ASCII 文本。 */
  ST7789_DrawString(DISPLAY_TEXT_X, (uint16_t)(y + 2U), text, color, ST7789_COLOR_BLACK);
}

/**
 * @brief 把加湿器状态枚举转换为英文显示文本。
 * @param state 加湿器状态枚举。
 * @retval 状态英文文本。
 */
static const char *DisplayService_GetStateText(Humidifier_State_t state)
{
  switch (state)
  {
    case HUMIDIFIER_STATE_ON:
      return "ON";

    case HUMIDIFIER_STATE_WATER_LOW:
      return "WATER LOW";

    case HUMIDIFIER_STATE_SENSOR_ERROR:
      return "SENSOR ERR";

    case HUMIDIFIER_STATE_OFF:
    default:
      return "OFF";
  }
}

/**
 * @brief 初始化 LCD 显示服务。
 * @retval 无
 */
void DisplayService_Init(void)
{
  /* 初始化 ST7789 设备驱动，内部会初始化 LCD GPIO 并清屏。 */
  ST7789_Init();

  /* 标记显示服务可用。 */
  s_display_ready = 1U;

  /* 清空最近一次刷新时间，让第一次 Update 可以正常刷新。 */
  s_last_display_tick = 0U;
}

/**
 * @brief 显示上电启动页面。
 * @retval 无
 */
void DisplayService_ShowBootScreen(void)
{
  if (s_display_ready == 0U)
  {
    return;
  }

  /* 启动页先整屏清黑。 */
  ST7789_FillScreen(ST7789_COLOR_BLACK);

  /* 显示基础启动信息。 */
  ST7789_DrawString(DISPLAY_TEXT_X, DISPLAY_TITLE_Y, "STM32G474 Humidifier", ST7789_COLOR_CYAN, ST7789_COLOR_BLACK);
  ST7789_DrawString(DISPLAY_TEXT_X, DISPLAY_LINE_1_Y, "Booting...", ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);
  ST7789_DrawString(DISPLAY_TEXT_X, DISPLAY_LINE_2_Y, "Mist: OFF", ST7789_COLOR_GREEN, ST7789_COLOR_BLACK);
  ST7789_DrawString(DISPLAY_TEXT_X, DISPLAY_LINE_3_Y, "Fan : OFF", ST7789_COLOR_GREEN, ST7789_COLOR_BLACK);
}

/**
 * @brief 显示正常主页面。
 * @retval 无
 */
void DisplayService_ShowMainPage(void)
{
  SensorService_Data_t sensor_data = {0};
  Humidifier_Status_t humidifier_status = HumidifierService_GetStatus();
  WaterLevel_Status_t water_status = WaterLevelService_GetStatus();
  char line[DISPLAY_TEXT_BUFFER_SIZE] = {0};

  if (s_display_ready == 0U)
  {
    return;
  }

  /* 固定标题行。 */
  DisplayService_DrawLine(DISPLAY_TITLE_Y, "STM32G474 Humidifier", ST7789_COLOR_CYAN);

  /* 温湿度有效时显示数值，无效时显示占位。 */
  if (SensorService_GetData(&sensor_data) == SENSOR_SERVICE_OK)
  {
    DisplayService_FormatX10Line(line,
                                 sizeof(line),
                                 "Temp:",
                                 DisplayService_FloatToX10(sensor_data.temperature),
                                 "C");
    DisplayService_DrawLine(DISPLAY_LINE_1_Y, line, ST7789_COLOR_WHITE);

    DisplayService_FormatX10Line(line,
                                 sizeof(line),
                                 "Humi:",
                                 DisplayService_FloatToX10(sensor_data.humidity),
                                 "%");
    DisplayService_DrawLine(DISPLAY_LINE_2_Y, line, ST7789_COLOR_WHITE);
  }
  else
  {
    DisplayService_DrawLine(DISPLAY_LINE_1_Y, "Temp: --.- C", ST7789_COLOR_WHITE);
    DisplayService_DrawLine(DISPLAY_LINE_2_Y, "Humi: --.- %", ST7789_COLOR_WHITE);
  }

  /* 显示水位状态。 */
  if (water_status == WATER_LEVEL_OK)
  {
    DisplayService_DrawLine(DISPLAY_LINE_3_Y, "Water: OK", ST7789_COLOR_GREEN);
  }
  else if (water_status == WATER_LEVEL_LOW)
  {
    DisplayService_DrawLine(DISPLAY_LINE_3_Y, "Water: LOW", ST7789_COLOR_RED);
  }
  else
  {
    DisplayService_DrawLine(DISPLAY_LINE_3_Y, "Water: UNKNOWN", ST7789_COLOR_YELLOW);
  }

  /* 显示雾化和风扇输出状态。 */
  (void)snprintf(line, sizeof(line), "Mist: %s", (humidifier_status.mist_on != 0U) ? "ON" : "OFF");
  DisplayService_DrawLine(DISPLAY_LINE_4_Y, line, (humidifier_status.mist_on != 0U) ? ST7789_COLOR_GREEN : ST7789_COLOR_WHITE);

  (void)snprintf(line, sizeof(line), "Fan : %s", (humidifier_status.fan_on != 0U) ? "ON" : "OFF");
  DisplayService_DrawLine(DISPLAY_LINE_5_Y, line, (humidifier_status.fan_on != 0U) ? ST7789_COLOR_GREEN : ST7789_COLOR_WHITE);

  /* 显示控制状态。 */
  (void)snprintf(line, sizeof(line), "State: %s", DisplayService_GetStateText(humidifier_status.state));
  DisplayService_DrawLine(DISPLAY_LINE_6_Y, line, ST7789_COLOR_WHITE);
}

/**
 * @brief 显示传感器异常页面。
 * @retval 无
 */
void DisplayService_ShowSensorError(void)
{
  if (s_display_ready == 0U)
  {
    return;
  }

  DisplayService_ShowMainPage();
  DisplayService_DrawLine(DISPLAY_LINE_6_Y, "State: SENSOR ERR", ST7789_COLOR_RED);
}

/**
 * @brief 显示缺水保护页面。
 * @retval 无
 */
void DisplayService_ShowWaterLow(void)
{
  if (s_display_ready == 0U)
  {
    return;
  }

  DisplayService_ShowMainPage();
  DisplayService_DrawLine(DISPLAY_LINE_6_Y, "State: WATER LOW", ST7789_COLOR_RED);
}

/**
 * @brief 兼容旧接口，显示当前传感器和主状态页面。
 * @retval 无
 */
void DisplayService_ShowSensorData(void)
{
  DisplayService_ShowMainPage();
}

/**
 * @brief 周期刷新显示页面。
 * @retval 无
 */
void DisplayService_Update(void)
{
  uint32_t current_tick = HAL_GetTick();
  Humidifier_Status_t humidifier_status;

  if (s_display_ready == 0U)
  {
    return;
  }

  /* 按配置周期刷新 LCD，避免软件 SPI 过于频繁地刷屏。 */
  if ((s_last_display_tick != 0U) &&
      ((current_tick - s_last_display_tick) < APP_DISPLAY_UPDATE_INTERVAL_MS))
  {
    return;
  }

  /* 记录本次刷新时间。 */
  s_last_display_tick = current_tick;

  /* 根据安全优先级选择页面。 */
  humidifier_status = HumidifierService_GetStatus();

  if (humidifier_status.water_ok == 0U)
  {
    DisplayService_ShowWaterLow();
  }
  else if (humidifier_status.sensor_ok == 0U)
  {
    DisplayService_ShowSensorError();
  }
  else
  {
    DisplayService_ShowMainPage();
  }
}

/**
 * @brief 执行 LCD 纯色循环测试。
 * @retval 无
 */
void DisplayService_TestColorCycle(void)
{
  ST7789_TestColorCycle();
}
