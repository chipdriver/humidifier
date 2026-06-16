#include "water_level_service.h"

#include "water_level_hw.h"

static GPIO_PinState s_water_level_raw = GPIO_PIN_SET;
static WaterLevel_Status_t s_water_level_status = WATER_LEVEL_UNKNOWN;
static uint8_t s_water_level_initialized = 0U;

/**
 * @brief 初始化水位服务，并读取一次当前水位状态。
 * @retval 无
 */
void WaterLevelService_Init(void)
{
#if APP_WATER_LEVEL_ENABLE
  /* 初始化硬件驱动层 PA0 输入上拉。 */
  WaterLevel_HW_Init();

  /* 标记水位服务已经初始化。 */
  s_water_level_initialized = 1U;

  /* 初始化后立即读取一次，避免上层拿到 UNKNOWN。 */
  WaterLevelService_Update();
#else
  /* 如果临时关闭水位检测，默认按有水处理，便于单独调试传感器或屏幕。 */
  s_water_level_raw = WATER_LEVEL_HAS_WATER_LEVEL;
  s_water_level_status = WATER_LEVEL_OK;
  s_water_level_initialized = 1U;
#endif
}

/**
 * @brief 更新水位状态缓存。
 * @retval 无
 */
void WaterLevelService_Update(void)
{
#if APP_WATER_LEVEL_ENABLE
  /* 未初始化时不直接读 GPIO，状态保持 UNKNOWN。 */
  if (s_water_level_initialized == 0U)
  {
    s_water_level_status = WATER_LEVEL_UNKNOWN;
    return;
  }

  /* 调用硬件驱动层读取 PA0 原始电平。 */
  s_water_level_raw = WaterLevel_HW_ReadRaw();

  /* 只通过 WATER_LEVEL_HAS_WATER_LEVEL 宏判断有水电平，方便后续实测后改宏。 */
  if (s_water_level_raw == WATER_LEVEL_HAS_WATER_LEVEL)
  {
    s_water_level_status = WATER_LEVEL_OK;
  }
  else
  {
    s_water_level_status = WATER_LEVEL_LOW;
  }
#else
  /* 关闭水位检测时，上层始终看到水位正常。 */
  s_water_level_raw = WATER_LEVEL_HAS_WATER_LEVEL;
  s_water_level_status = WATER_LEVEL_OK;
#endif
}

/**
 * @brief 获取最近一次读取到的 PA0 原始电平。
 * @retval GPIO_PIN_SET 表示高电平，GPIO_PIN_RESET 表示低电平。
 */
GPIO_PinState WaterLevelService_GetRawLevel(void)
{
  return s_water_level_raw;
}

/**
 * @brief 获取当前水位业务状态。
 * @retval WATER_LEVEL_OK 表示有水，WATER_LEVEL_LOW 表示缺水，WATER_LEVEL_UNKNOWN 表示未知。
 */
WaterLevel_Status_t WaterLevelService_GetStatus(void)
{
  return s_water_level_status;
}

/**
 * @brief 判断当前水位是否允许加湿。
 * @retval 1 表示水位正常，0 表示缺水或未知。
 */
uint8_t WaterLevelService_IsWaterOk(void)
{
  if (s_water_level_status == WATER_LEVEL_OK)
  {
    return 1U;
  }

  return 0U;
}

/**
 * @brief 判断当前是否缺水。
 * @retval 1 表示缺水，0 表示有水或未知。
 */
uint8_t WaterLevelService_IsWaterLow(void)
{
  if (s_water_level_status == WATER_LEVEL_LOW)
  {
    return 1U;
  }

  return 0U;
}
