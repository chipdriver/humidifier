#include "display_service.h"
#include "sensor_service.h"
#include "st7789.h"
#include <stdio.h>

#define DISPLAY_TITLE_X        10U
#define DISPLAY_TITLE_Y        20U
#define DISPLAY_TEMP_X         10U
#define DISPLAY_TEMP_Y         80U
#define DISPLAY_HUMI_X         10U
#define DISPLAY_HUMI_Y         110U
#define DISPLAY_STATUS_X       10U
#define DISPLAY_STATUS_Y       150U
#define DISPLAY_TEXT_AREA_Y    60U
#define DISPLAY_TEXT_AREA_H    120U

/**
 * @brief 把浮点数放大 10 倍后转换为整数，便于不用 float printf 显示 1 位小数。
 * @param value 要转换的浮点数。
 * @retval 放大 10 倍后的整数值。
 */
static int DisplayService_FloatToX10(float value)
{
  /* 正数加 0.5，负数减 0.5，用来做简单四舍五入。 */
  if (value >= 0.0f)
  {
    return (int)((value * 10.0f) + 0.5f);
  }

  /* 负数路径使用 -0.5f，避免 -1.26 直接截断成 -12。 */
  return (int)((value * 10.0f) - 0.5f);
}

/**
 * @brief 生成带 1 位小数的整数格式字符串。
 * @param buffer 字符串输出缓冲区。
 * @param buffer_size 输出缓冲区大小。
 * @param prefix 显示前缀，例如 "Temp"。
 * @param value 要显示的浮点数。
 * @param suffix 显示后缀，例如 "C" 或 "%"。
 * @retval 无
 */
static void DisplayService_FormatValue(char *buffer, uint32_t buffer_size, const char *prefix, float value, const char *suffix)
{
  /* 把浮点数转换为放大 10 倍后的整数。 */
  int value_x10 = DisplayService_FloatToX10(value);

  /* 计算整数部分。 */
  int value_int = value_x10 / 10;

  /* 计算小数部分。 */
  int value_dec = value_x10 % 10;

  /* 如果小数部分为负数，就取正，显示时只让整数部分带负号。 */
  if (value_dec < 0)
  {
    value_dec = -value_dec;
  }

  /* 使用整数 snprintf，避免依赖 printf 的 float 支持。 */
  (void)snprintf(buffer, buffer_size, "%s: %d.%d %s", prefix, value_int, value_dec, suffix);
}

/**
 * @brief 初始化显示服务。
 * @retval 无
 */
void DisplayService_Init(void)
{
  /* 显示服务通过设备驱动层初始化 ST7789 屏幕。 */
  ST7789_Init();

  /* 初始化完成后先清为黑色背景。 */
  ST7789_FillScreen(ST7789_COLOR_BLACK);

  /* 显示简单启动页面。 */
  DisplayService_ShowBootScreen();
}

/**
 * @brief 显示启动页面。
 * @retval 无
 */
void DisplayService_ShowBootScreen(void)
{
  /* 清屏为黑色。 */
  ST7789_FillScreen(ST7789_COLOR_BLACK);

  /* 显示测试标题。 */
  ST7789_DrawString(DISPLAY_TITLE_X, DISPLAY_TITLE_Y, "AHT20 LCD Test", ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);

  /* 显示启动状态。 */
  ST7789_DrawString(DISPLAY_TITLE_X, DISPLAY_TITLE_Y + 20U, "Starting...", ST7789_COLOR_CYAN, ST7789_COLOR_BLACK);
}

/**
 * @brief 显示当前 SensorService 缓存中的温湿度数据。
 * @retval 无
 */
void DisplayService_ShowSensorData(void)
{
  /* 定义变量保存从传感器服务层获取到的数据。 */
  SensorService_Data_t sensor_data = {0};

  /* 定义温度显示行缓冲区。 */
  char temp_line[20] = {0};

  /* 定义湿度显示行缓冲区。 */
  char humi_line[20] = {0};

  /* 必须通过 SensorService 获取数据，不直接读取 AHT20。 */
  if (SensorService_GetData(&sensor_data) != SENSOR_SERVICE_OK)
  {
    /* 如果当前没有有效数据，就显示错误页面。 */
    DisplayService_ShowSensorError();

    /* 错误页面已经处理完成。 */
    return;
  }

  /* 清除温湿度显示区域，减少旧字符残留。 */
  ST7789_FillRect(0U, DISPLAY_TEXT_AREA_Y, ST7789_WIDTH, DISPLAY_TEXT_AREA_H, ST7789_COLOR_BLACK);

  /* 显示固定标题。 */
  ST7789_DrawString(DISPLAY_TITLE_X, DISPLAY_TITLE_Y, "AHT20 LCD Test", ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);

  /* 把温度格式化成 Temp: 25.6 C。 */
  DisplayService_FormatValue(temp_line, sizeof(temp_line), "Temp", sensor_data.temperature, "C");

  /* 把湿度格式化成 Humi: 63.2 %。 */
  DisplayService_FormatValue(humi_line, sizeof(humi_line), "Humi", sensor_data.humidity, "%");

  /* 绘制温度行。 */
  ST7789_DrawString(DISPLAY_TEMP_X, DISPLAY_TEMP_Y, temp_line, ST7789_COLOR_YELLOW, ST7789_COLOR_BLACK);

  /* 绘制湿度行。 */
  ST7789_DrawString(DISPLAY_HUMI_X, DISPLAY_HUMI_Y, humi_line, ST7789_COLOR_CYAN, ST7789_COLOR_BLACK);
}

/**
 * @brief 显示传感器错误页面。
 * @retval 无
 */
void DisplayService_ShowSensorError(void)
{
  /* 清屏为黑色。 */
  ST7789_FillScreen(ST7789_COLOR_BLACK);

  /* 显示固定标题。 */
  ST7789_DrawString(DISPLAY_TITLE_X, DISPLAY_TITLE_Y, "AHT20 LCD Test", ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);

  /* 显示传感器错误提示。 */
  ST7789_DrawString(DISPLAY_STATUS_X, DISPLAY_STATUS_Y, "Sensor Error", ST7789_COLOR_RED, ST7789_COLOR_BLACK);
}

/**
 * @brief 刷新显示服务内容。
 * @retval 无
 */
void DisplayService_Update(void)
{
  /* 第一版直接显示当前传感器缓存数据。 */
  DisplayService_ShowSensorData();
}

/**
 * @brief 执行显示服务的颜色循环测试。
 * @retval 无
 */
void DisplayService_TestColorCycle(void)
{
  /* 显示服务通过设备驱动层执行 ST7789 颜色循环测试。 */
  ST7789_TestColorCycle();
}
