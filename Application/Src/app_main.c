#include "app_main.h"
#include "display_service.h"
#include "lv_port_disp.h"
#include "lvgl.h"

/**
 * @brief 应用层主流程入口。
 * @retval 无
 */
void App_Main(void)
{
  /* 初始化显示相关业务服务。 */
  DisplayService_Init();

  lv_init();
  lv_tick_set_cb(HAL_GetTick);
  LV_Port_Disp_Init();

  lv_obj_t *label = lv_label_create(lv_screen_active());
  lv_label_set_text(label, "Hello LVGL");
  lv_obj_center(label);

  while (1)
  {
    lv_timer_handler();
    HAL_Delay(5U);
  }
}
