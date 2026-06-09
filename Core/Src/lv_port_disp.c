#include "lv_port_disp.h"
#include "st7789.h"

#define LCD_HOR_RES    ST7789_WIDTH
#define LCD_VER_RES    ST7789_HEIGHT
#define LCD_BUF_LINES  20U

static uint16_t s_lvgl_draw_buf[LCD_HOR_RES * LCD_BUF_LINES];

static void LVGL_Flush_Callback(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
  uint16_t width = (uint16_t)(area->x2 - area->x1 + 1);
  uint16_t height = (uint16_t)(area->y2 - area->y1 + 1);
  uint32_t pixel_count = (uint32_t)width * (uint32_t)height;

  ST7789_SetWindow((uint16_t)area->x1,
                   (uint16_t)area->y1,
                   (uint16_t)area->x2,
                   (uint16_t)area->y2);
  ST7789_WritePixels((uint16_t *)px_map, pixel_count);

  lv_display_flush_ready(disp);
}

void LV_Port_Disp_Init(void)
{
  lv_display_t *disp = lv_display_create(LCD_HOR_RES, LCD_VER_RES);

  lv_display_set_flush_cb(disp, LVGL_Flush_Callback);
  lv_display_set_buffers(disp,
                         s_lvgl_draw_buf,
                         NULL,
                         sizeof(s_lvgl_draw_buf),
                         LV_DISPLAY_RENDER_MODE_PARTIAL);
}
