#include "humidifier_service.h"

#include "app_config.h"
#include "relay_hw.h"
#include "sensor_service.h"
#include "water_level_service.h"

static Humidifier_Status_t s_humidifier_status = {
  HUMIDIFIER_STATE_OFF,
  0U,
  0U,
  0U,
  0U
};

static uint8_t s_humidifier_initialized = 0U;
static uint32_t s_last_control_tick = 0U;

/**
 * @brief 统一关闭执行器，并设置业务状态。
 * @param state 关闭后的业务状态。
 * @retval 无
 */
static void HumidifierService_SetOffState(Humidifier_State_t state)
{
  /* 关闭雾化继电器。 */
  MistRelay_Off();

  /* 关闭风扇继电器。 */
  FanRelay_Off();

  /* 记录当前状态。 */
  s_humidifier_status.state = state;

  /* 记录雾化输出已经关闭。 */
  s_humidifier_status.mist_on = 0U;

  /* 记录风扇输出已经关闭。 */
  s_humidifier_status.fan_on = 0U;
}

/**
 * @brief 统一开启雾化和风扇执行器。
 * @retval 无
 */
static void HumidifierService_SetOnState(void)
{
  /* 开启雾化继电器。 */
  MistRelay_On();

  /* 开启风扇继电器。 */
  FanRelay_On();

  /* 记录当前状态为正在加湿。 */
  s_humidifier_status.state = HUMIDIFIER_STATE_ON;

  /* 记录雾化输出已经开启。 */
  s_humidifier_status.mist_on = 1U;

  /* 记录风扇输出已经开启。 */
  s_humidifier_status.fan_on = 1U;
}

/**
 * @brief 初始化加湿控制服务，并确保上电默认关闭执行器。
 * @retval 无
 */
void HumidifierService_Init(void)
{
  /* 初始化已经验证过的继电器硬件驱动层。 */
  Relay_HW_Init();

  /* 先记录默认安全状态。 */
  s_humidifier_status.water_ok = 0U;
  s_humidifier_status.sensor_ok = 0U;

  /* 初始化后强制关闭雾化和风扇。 */
  HumidifierService_SetOffState(HUMIDIFIER_STATE_OFF);

  /* 记录当前服务已经初始化。 */
  s_humidifier_initialized = 1U;

  /* 清空最近一次控制更新时间。 */
  s_last_control_tick = 0U;
}

/**
 * @brief 按水位、传感器和湿度阈值更新雾化/风扇状态。
 * @retval 无
 */
void HumidifierService_Update(void)
{
  /* 获取当前 HAL 系统毫秒计数。 */
  uint32_t current_tick = HAL_GetTick();

  /* 未初始化时直接返回，避免继电器驱动未准备好就控制输出。 */
  if (s_humidifier_initialized == 0U)
  {
    return;
  }

  /* 控制周期未到时不重复写继电器，降低无意义 GPIO 翻转。 */
  if ((s_last_control_tick != 0U) &&
      ((current_tick - s_last_control_tick) < APP_CONTROL_UPDATE_INTERVAL_MS))
  {
    return;
  }

  /* 记录本次控制判断时间。 */
  s_last_control_tick = current_tick;

  /* 从水位服务获取当前是否有水。 */
  s_humidifier_status.water_ok = WaterLevelService_IsWaterOk();

  /* 从传感器服务获取当前温湿度数据是否有效。 */
  s_humidifier_status.sensor_ok = SensorService_IsDataValid();

#if APP_WATER_LEVEL_ENABLE
  /* 缺水保护优先级最高，任何湿度下都必须关闭执行器。 */
  if (s_humidifier_status.water_ok == 0U)
  {
    HumidifierService_SetOffState(HUMIDIFIER_STATE_WATER_LOW);
    return;
  }
#endif

#if (APP_MIST_ENABLE_WHEN_SENSOR_ERROR == 0U)
  /* 传感器无效时不允许雾化和风扇继续运行。 */
  if (s_humidifier_status.sensor_ok == 0U)
  {
    HumidifierService_SetOffState(HUMIDIFIER_STATE_SENSOR_ERROR);
    return;
  }
#endif

  /* 湿度低于下限时，开启雾化和风扇。 */
  if (SensorService_GetHumidity() < APP_HUMIDITY_LOW_THRESHOLD)
  {
    HumidifierService_SetOnState();
    return;
  }

  /* 湿度高于上限时，关闭雾化和风扇。 */
  if (SensorService_GetHumidity() > APP_HUMIDITY_HIGH_THRESHOLD)
  {
    HumidifierService_SetOffState(HUMIDIFIER_STATE_OFF);
    return;
  }

  /* 中间回差区间保持执行器状态，但把错误状态恢复为正常 ON/OFF 状态。 */
  if ((s_humidifier_status.mist_on != 0U) && (s_humidifier_status.fan_on != 0U))
  {
    s_humidifier_status.state = HUMIDIFIER_STATE_ON;
  }
  else
  {
    s_humidifier_status.state = HUMIDIFIER_STATE_OFF;
  }
}

/**
 * @brief 强制关闭雾化和风扇。
 * @retval 无
 */
void HumidifierService_ForceOff(void)
{
  if (s_humidifier_initialized == 0U)
  {
    return;
  }

  HumidifierService_SetOffState(HUMIDIFIER_STATE_OFF);
}

/**
 * @brief 获取雾化输出状态。
 * @retval 1 表示雾化开启，0 表示雾化关闭。
 */
uint8_t HumidifierService_IsMistOn(void)
{
  return s_humidifier_status.mist_on;
}

/**
 * @brief 获取风扇输出状态。
 * @retval 1 表示风扇开启，0 表示风扇关闭。
 */
uint8_t HumidifierService_IsFanOn(void)
{
  return s_humidifier_status.fan_on;
}

/**
 * @brief 获取当前加湿业务状态。
 * @retval 当前加湿业务状态。
 */
Humidifier_State_t HumidifierService_GetState(void)
{
  return s_humidifier_status.state;
}

/**
 * @brief 获取完整加湿器状态快照。
 * @retval 当前状态快照。
 */
Humidifier_Status_t HumidifierService_GetStatus(void)
{
  return s_humidifier_status;
}
