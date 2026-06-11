#include "app_main.h"
#include "display_service.h"
#include "sensor_service.h"

/**
 * @brief 应用层主流程入口。
 * @retval 无
 */
void App_Main(void)
{
  /* 初始化显示相关业务服务。 */
  DisplayService_Init();

  /* 初始化温湿度传感器业务服务。 */
  if (SensorService_Init() != SENSOR_SERVICE_OK)
  {
    /* 如果传感器初始化失败，就在屏幕上显示错误。 */
    DisplayService_ShowSensorError();

    /* 停在错误循环中，但仍保持 LVGL 定时任务刷新。 */
    while (1)
    {
      DisplayService_Update();
      HAL_Delay(5U);
    }
  }

  /* 初始化成功后先尝试读取一次，避免上电后长时间没有有效显示数据。 */
  (void)SensorService_Update();

  while (1)
  {
    /* 按默认周期更新温湿度缓存。 */
    (void)SensorService_UpdatePeriodic();

    /* 把当前温湿度缓存显示到 LCD。 */
    DisplayService_Update();

    /* LVGL 需要高频调用 timer handler，真实传感器读取周期仍由 SensorService 控制。 */
    HAL_Delay(5U);
  }
}
