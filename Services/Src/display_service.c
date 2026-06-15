#include "display_service.h"

#include "display_ui_format.h"
#include "display_ui_layout.h"
#include "humidifier_symbol_font.h"
#include "lv_port_disp.h"
#include "sensor_service.h"
#include "st7789.h"

#define UI_TARGET_HUMIDITY      60
#define UI_TARGET_HUMIDITY_MIN  30
#define UI_TARGET_HUMIDITY_MAX  90
#define UI_LINE_TEXT_SIZE       24U

#define UI_COLOR_BG             0x03070CU
#define UI_COLOR_CARD_BG        0x07111FU
#define UI_COLOR_CARD_BORDER    0x245070U
#define UI_COLOR_TEXT_MAIN      0xF4F9FFU
#define UI_COLOR_BLUE           0x3F86FFU
#define UI_COLOR_GREEN          0x76D94CU
#define UI_COLOR_BAR_BG         0x2D343CU
#define UI_COLOR_ERROR          0xFF4D4DU

#if (DISPLAY_UI_SCREEN_W != ST7789_WIDTH) || (DISPLAY_UI_SCREEN_H != ST7789_HEIGHT)
#error "Display UI layout must match ST7789 panel resolution."
#endif

static lv_obj_t *s_temp_label;
static lv_obj_t *s_humi_label;
static lv_obj_t *s_mode_first_label;
static lv_obj_t *s_mode_rest_label;
static lv_obj_t *s_target_first_label;
static lv_obj_t *s_target_rest_label;
static lv_obj_t *s_target_bar;

static lv_font_t s_ui_font_16;
static lv_font_t s_num_font;
static uint8_t s_display_ready;

static lv_color_t DisplayService_Color(uint32_t color)
{
  return lv_color_hex(color);
}

static const lv_font_t *DisplayService_GetUIFont(void)
{
  return &s_ui_font_16;
}

static const lv_font_t *DisplayService_GetNumFont(void)
{
  return &s_num_font;
}

static void DisplayService_InitFonts(void)
{
#if LV_FONT_SOURCE_HAN_SANS_SC_16_CJK
  s_ui_font_16 = lv_font_source_han_sans_sc_16_cjk;
#else
  s_ui_font_16 = lv_font_montserrat_14;
#endif
  s_ui_font_16.fallback = HumidifierSymbolFont_Get16();

#if LV_FONT_MONTSERRAT_16
  s_num_font = lv_font_montserrat_16;
#else
  s_num_font = lv_font_montserrat_14;
#endif
  s_num_font.fallback = HumidifierSymbolFont_Get16();
}

static void DisplayService_PrepareObject(lv_obj_t *obj)
{
  lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scrollbar_mode(obj, LV_SCROLLBAR_MODE_OFF);
}

static lv_obj_t *DisplayService_CreateCard(lv_obj_t *parent, int32_t x, int32_t y, int32_t width, int32_t height)
{
  lv_obj_t *card = lv_obj_create(parent);

  lv_obj_remove_style_all(card);
  lv_obj_set_pos(card, x, y);
  lv_obj_set_size(card, width, height);
  lv_obj_set_style_bg_color(card, DisplayService_Color(UI_COLOR_CARD_BG), 0);
  lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(card, DisplayService_Color(UI_COLOR_CARD_BORDER), 0);
  lv_obj_set_style_border_width(card, 1, 0);
  lv_obj_set_style_border_opa(card, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(card, DISPLAY_UI_CARD_RADIUS, 0);
  lv_obj_set_style_pad_all(card, 0, 0);
  lv_obj_set_style_pad_left(card, 0, 0);
  lv_obj_set_style_pad_right(card, 0, 0);
  lv_obj_set_style_pad_top(card, 0, 0);
  lv_obj_set_style_pad_bottom(card, 0, 0);
  DisplayService_PrepareObject(card);

  return card;
}

static lv_obj_t *DisplayService_CreateLabel(lv_obj_t *parent,
                                            const char *text,
                                            uint32_t color,
                                            const lv_font_t *font,
                                            int32_t x,
                                            int32_t y,
                                            int32_t width,
                                            int32_t height,
                                            lv_text_align_t align)
{
  lv_obj_t *label = lv_label_create(parent);

  lv_label_set_text(label, text);
  lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_CLIP);
  lv_obj_set_pos(label, x, y);
  lv_obj_set_size(label, width, height);
  lv_obj_set_style_bg_opa(label, LV_OPA_TRANSP, 0);
  lv_obj_set_style_text_color(label, DisplayService_Color(color), 0);
  lv_obj_set_style_text_font(label, font, 0);
  lv_obj_set_style_text_align(label, align, 0);
  lv_obj_set_style_text_letter_space(label, 0, 0);
  lv_obj_set_style_text_line_space(label, 0, 0);
  DisplayService_PrepareObject(label);

  return label;
}

static void DisplayService_SetDefaultValues(void)
{
  lv_label_set_text(s_temp_label, "温度: 25.6C");
  lv_label_set_text(s_humi_label, "湿度: 63.2%");
  lv_label_set_text(s_mode_first_label, "工");
  lv_label_set_text(s_mode_rest_label, "作: 自动");
  lv_obj_set_style_text_color(s_mode_first_label, DisplayService_Color(UI_COLOR_GREEN), 0);
  lv_obj_set_style_text_color(s_mode_rest_label, DisplayService_Color(UI_COLOR_GREEN), 0);
  lv_label_set_text(s_target_first_label, "目");
  lv_label_set_text(s_target_rest_label, "标: 60%");
  lv_bar_set_value(s_target_bar, UI_TARGET_HUMIDITY, LV_ANIM_OFF);
}

static void DisplayService_CreateUI(void)
{
  lv_obj_t *screen = lv_screen_active();
  lv_obj_t *data_card;
  lv_obj_t *mode_card;
  lv_obj_t *target_card;

  lv_obj_remove_style_all(screen);
  lv_obj_set_size(screen, DISPLAY_UI_SCREEN_W, DISPLAY_UI_SCREEN_H);
  lv_obj_set_style_bg_color(screen, DisplayService_Color(UI_COLOR_BG), 0);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
  lv_obj_set_style_pad_all(screen, 0, 0);
  DisplayService_PrepareObject(screen);

  (void)DisplayService_CreateLabel(screen,
                                   "智能加湿器",
                                   UI_COLOR_TEXT_MAIN,
                                   DisplayService_GetUIFont(),
                                   DISPLAY_UI_TITLE_X,
                                   DISPLAY_UI_TITLE_Y,
                                   DISPLAY_UI_TITLE_W,
                                   DISPLAY_UI_TITLE_H,
                                   LV_TEXT_ALIGN_CENTER);

  data_card = DisplayService_CreateCard(screen,
                                        DISPLAY_UI_DATA_X,
                                        DISPLAY_UI_DATA_Y,
                                        DISPLAY_UI_DATA_W,
                                        DISPLAY_UI_DATA_H);
  s_temp_label = DisplayService_CreateLabel(data_card,
                                            "温度: 25.6C",
                                            UI_COLOR_BLUE,
                                            DisplayService_GetUIFont(),
                                            DISPLAY_UI_TEMP_LINE_X,
                                            DISPLAY_UI_TEMP_LINE_Y,
                                            DISPLAY_UI_DATA_LINE_W,
                                            DISPLAY_UI_DATA_LINE_H,
                                            LV_TEXT_ALIGN_CENTER);
  s_humi_label = DisplayService_CreateLabel(data_card,
                                            "湿度: 63.2%",
                                            UI_COLOR_GREEN,
                                            DisplayService_GetUIFont(),
                                            DISPLAY_UI_HUMI_LINE_X,
                                            DISPLAY_UI_HUMI_LINE_Y,
                                            DISPLAY_UI_DATA_LINE_W,
                                            DISPLAY_UI_DATA_LINE_H,
                                            LV_TEXT_ALIGN_CENTER);

  mode_card = DisplayService_CreateCard(screen,
                                        DISPLAY_UI_MODE_X,
                                        DISPLAY_UI_MODE_Y,
                                        DISPLAY_UI_MODE_W,
                                        DISPLAY_UI_MODE_H);
  s_mode_first_label = DisplayService_CreateLabel(mode_card,
                                                  "工",
                                                  UI_COLOR_GREEN,
                                                  DisplayService_GetUIFont(),
                                                  DISPLAY_UI_MODE_FIRST_X,
                                                  DISPLAY_UI_MODE_FIRST_Y,
                                                  DISPLAY_UI_MODE_FIRST_W,
                                                  DISPLAY_UI_MODE_FIRST_H,
                                                  LV_TEXT_ALIGN_RIGHT);
  s_mode_rest_label = DisplayService_CreateLabel(mode_card,
                                                 "作: 自动",
                                                 UI_COLOR_GREEN,
                                                 DisplayService_GetUIFont(),
                                                 DISPLAY_UI_MODE_REST_X,
                                                 DISPLAY_UI_MODE_REST_Y,
                                                 DISPLAY_UI_MODE_REST_W,
                                                 DISPLAY_UI_MODE_REST_H,
                                                 LV_TEXT_ALIGN_LEFT);

  target_card = DisplayService_CreateCard(screen,
                                          DISPLAY_UI_TARGET_X,
                                          DISPLAY_UI_TARGET_Y,
                                          DISPLAY_UI_TARGET_W,
                                          DISPLAY_UI_TARGET_H);
  s_target_first_label = DisplayService_CreateLabel(target_card,
                                                    "目",
                                                    UI_COLOR_TEXT_MAIN,
                                                    DisplayService_GetUIFont(),
                                                    DISPLAY_UI_TARGET_FIRST_X,
                                                    DISPLAY_UI_TARGET_FIRST_Y,
                                                    DISPLAY_UI_TARGET_FIRST_W,
                                                    DISPLAY_UI_TARGET_FIRST_H,
                                                    LV_TEXT_ALIGN_RIGHT);
  s_target_rest_label = DisplayService_CreateLabel(target_card,
                                                   "标: 60%",
                                                   UI_COLOR_TEXT_MAIN,
                                                   DisplayService_GetUIFont(),
                                                   DISPLAY_UI_TARGET_REST_X,
                                                   DISPLAY_UI_TARGET_REST_Y,
                                                   DISPLAY_UI_TARGET_REST_W,
                                                   DISPLAY_UI_TARGET_REST_H,
                                                   LV_TEXT_ALIGN_LEFT);

  s_target_bar = lv_bar_create(target_card);
  lv_obj_set_pos(s_target_bar, DISPLAY_UI_TARGET_BAR_X, DISPLAY_UI_TARGET_BAR_Y);
  lv_obj_set_size(s_target_bar, DISPLAY_UI_TARGET_BAR_W, DISPLAY_UI_TARGET_BAR_H);
  lv_bar_set_range(s_target_bar, UI_TARGET_HUMIDITY_MIN, UI_TARGET_HUMIDITY_MAX);
  lv_bar_set_value(s_target_bar, UI_TARGET_HUMIDITY, LV_ANIM_OFF);
  lv_obj_set_style_bg_color(s_target_bar, DisplayService_Color(UI_COLOR_BAR_BG), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(s_target_bar, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_bg_color(s_target_bar, DisplayService_Color(UI_COLOR_BLUE), LV_PART_INDICATOR);
  lv_obj_set_style_bg_opa(s_target_bar, LV_OPA_COVER, LV_PART_INDICATOR);
  lv_obj_set_style_radius(s_target_bar, 4, LV_PART_MAIN);
  lv_obj_set_style_radius(s_target_bar, 4, LV_PART_INDICATOR);
  DisplayService_PrepareObject(s_target_bar);

  (void)DisplayService_CreateLabel(target_card,
                                   "30%",
                                   UI_COLOR_TEXT_MAIN,
                                   DisplayService_GetNumFont(),
                                   DISPLAY_UI_TARGET_SCALE_MIN_X,
                                   DISPLAY_UI_TARGET_SCALE_Y,
                                   DISPLAY_UI_TARGET_SCALE_W,
                                   DISPLAY_UI_TARGET_SCALE_H,
                                   LV_TEXT_ALIGN_LEFT);
  (void)DisplayService_CreateLabel(target_card,
                                   "60%",
                                   UI_COLOR_TEXT_MAIN,
                                   DisplayService_GetNumFont(),
                                   DISPLAY_UI_TARGET_SCALE_MID_X,
                                   DISPLAY_UI_TARGET_SCALE_Y,
                                   DISPLAY_UI_TARGET_SCALE_W,
                                   DISPLAY_UI_TARGET_SCALE_H,
                                   LV_TEXT_ALIGN_CENTER);
  (void)DisplayService_CreateLabel(target_card,
                                   "90%",
                                   UI_COLOR_TEXT_MAIN,
                                   DisplayService_GetNumFont(),
                                   DISPLAY_UI_TARGET_SCALE_MAX_X,
                                   DISPLAY_UI_TARGET_SCALE_Y,
                                   DISPLAY_UI_TARGET_SCALE_W,
                                   DISPLAY_UI_TARGET_SCALE_H,
                                   LV_TEXT_ALIGN_RIGHT);
}

void DisplayService_Init(void)
{
  ST7789_Init();

  lv_init();
  lv_tick_set_cb(HAL_GetTick);
  LV_Port_Disp_Init();

  DisplayService_InitFonts();
  DisplayService_CreateUI();
  s_display_ready = 1U;
  DisplayService_SetDefaultValues();

  (void)lv_timer_handler();
}

void DisplayService_ShowBootScreen(void)
{
  if (s_display_ready == 0U)
  {
    return;
  }

  DisplayService_SetDefaultValues();
}

void DisplayService_ShowSensorData(void)
{
  SensorService_Data_t sensor_data = {0};
  char temp_text[UI_LINE_TEXT_SIZE] = {0};
  char humi_text[UI_LINE_TEXT_SIZE] = {0};

  if (s_display_ready == 0U)
  {
    return;
  }

  if (SensorService_GetData(&sensor_data) != SENSOR_SERVICE_OK)
  {
    DisplayService_ShowSensorError();
    return;
  }

  DisplayUI_FormatTemperature(temp_text, sizeof(temp_text), sensor_data.temperature);
  DisplayUI_FormatHumidity(humi_text, sizeof(humi_text), sensor_data.humidity);

  lv_label_set_text(s_temp_label, temp_text);
  lv_label_set_text(s_humi_label, humi_text);
  lv_label_set_text(s_mode_first_label, "工");
  lv_label_set_text(s_mode_rest_label, "作: 自动");
  lv_obj_set_style_text_color(s_mode_first_label, DisplayService_Color(UI_COLOR_GREEN), 0);
  lv_obj_set_style_text_color(s_mode_rest_label, DisplayService_Color(UI_COLOR_GREEN), 0);
  lv_label_set_text(s_target_first_label, "目");
  lv_label_set_text(s_target_rest_label, "标: 60%");
  lv_bar_set_value(s_target_bar, UI_TARGET_HUMIDITY, LV_ANIM_OFF);
}

void DisplayService_ShowSensorError(void)
{
  if (s_display_ready == 0U)
  {
    return;
  }

  lv_label_set_text(s_temp_label, "温度: --.-C");
  lv_label_set_text(s_humi_label, "湿度: --.-%");
  lv_label_set_text(s_mode_first_label, "");
  lv_label_set_text(s_mode_rest_label, DisplayUI_GetSensorErrorText());
  lv_obj_set_style_text_color(s_mode_first_label, DisplayService_Color(UI_COLOR_ERROR), 0);
  lv_obj_set_style_text_color(s_mode_rest_label, DisplayService_Color(UI_COLOR_ERROR), 0);
  lv_label_set_text(s_target_first_label, "目");
  lv_label_set_text(s_target_rest_label, "标: 60%");
  lv_bar_set_value(s_target_bar, UI_TARGET_HUMIDITY, LV_ANIM_OFF);
}

void DisplayService_Update(void)
{
  if (s_display_ready == 0U)
  {
    return;
  }

  DisplayService_ShowSensorData();
  (void)lv_timer_handler();
}

void DisplayService_TestColorCycle(void)
{
  ST7789_TestColorCycle();

  if (s_display_ready != 0U)
  {
    lv_obj_invalidate(lv_screen_active());
  }
}
