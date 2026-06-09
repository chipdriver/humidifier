#include "display_service.h"
#include "st7789.h"

/**
 * @brief 初始化显示服务。
 * @retval 无
 */
void DisplayService_Init(void)
{
  /* 显示服务通过设备驱动层初始化 ST7789 屏幕。 */
  ST7789_Init();
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
