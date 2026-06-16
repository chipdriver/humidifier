#include "app_main.h"
#include "display_service.h"
#include "humidifier_service.h"
#include "sensor_service.h"
#include "water_level_service.h"

/**
 * @brief 应用层主流程入口。
 * @retval 无
 */
void App_Main(void)
{
  /* 初始化显示相关业务服务。 */
  DisplayService_Init();

  /* 上电先显示启动页面，证明 LCD 已经亮起。 */
  DisplayService_ShowBootScreen();

  /* 尽早初始化继电器控制服务，确保雾化和风扇默认关闭。 */
  HumidifierService_Init();

  /* 初始化水位检测服务。 */
  WaterLevelService_Init();

  /* 初始化温湿度传感器业务服务。 */
  (void)SensorService_Init();

  /* 初始化成功后先尝试读取一次，避免上电后长时间没有有效显示数据。 */
  (void)SensorService_Update();

  /* 启动页保持一小段时间，便于肉眼确认系统已经启动。 */
  HAL_Delay(1000U);

  while (1)
  {
    /* 按默认周期更新温湿度缓存。 */
    (void)SensorService_UpdatePeriodic();

    /* 每次循环都更新水位，缺水保护要优先响应。 */
    WaterLevelService_Update();

    /* 根据水位和湿度更新雾化/风扇继电器状态。 */
    HumidifierService_Update();

    /* 按显示周期刷新 LCD 页面。 */
    DisplayService_Update();

    /* 主循环保留 100ms 节拍，控制和显示周期由各服务内部判断。 */
    HAL_Delay(100U);
  }
}
