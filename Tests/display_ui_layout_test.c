#include "display_ui_layout.h"

#include <assert.h>

static void test_layout_matches_172_by_320_screen(void)
{
  assert(DISPLAY_UI_SCREEN_W == 172);
  assert(DISPLAY_UI_SCREEN_H == 320);
}

static void test_layout_uses_three_full_width_cards(void)
{
  assert(DISPLAY_UI_DATA_X == 6);
  assert(DISPLAY_UI_DATA_Y == 52);
  assert(DISPLAY_UI_DATA_W == 160);
  assert(DISPLAY_UI_DATA_H == 72);

  assert(DISPLAY_UI_MODE_X == 6);
  assert(DISPLAY_UI_MODE_Y == 136);
  assert(DISPLAY_UI_MODE_W == 160);
  assert(DISPLAY_UI_MODE_H == 52);

  assert(DISPLAY_UI_TARGET_X == 6);
  assert(DISPLAY_UI_TARGET_Y == 200);
  assert(DISPLAY_UI_TARGET_W == 160);
  assert(DISPLAY_UI_TARGET_H == 104);
}

static void test_requested_micro_adjustments_are_kept(void)
{
  assert(DISPLAY_UI_TITLE_Y == 15);
  assert(DISPLAY_UI_MODE_FIRST_Y == 20);
  assert(DISPLAY_UI_MODE_REST_Y == 20);
  assert(DISPLAY_UI_MODE_FIRST_H == 24);
  assert(DISPLAY_UI_MODE_REST_H == 24);
  assert(DISPLAY_UI_TARGET_FIRST_Y == 13);
  assert(DISPLAY_UI_TARGET_REST_Y == 13);
  assert(DISPLAY_UI_TARGET_FIRST_H == 24);
  assert(DISPLAY_UI_TARGET_REST_H == 24);
  assert(DISPLAY_UI_TARGET_SCALE_MAX_X == 109);
}

static void test_chinese_labels_have_full_card_width(void)
{
  assert(DISPLAY_UI_TITLE_W == 172);
  assert(DISPLAY_UI_DATA_LINE_W == 160);
  assert((DISPLAY_UI_MODE_FIRST_W + DISPLAY_UI_MODE_REST_W) == 160);
  assert((DISPLAY_UI_TARGET_FIRST_W + DISPLAY_UI_TARGET_REST_W) == 160);
}

static void test_split_labels_touch_at_shared_boundary(void)
{
  assert((DISPLAY_UI_MODE_FIRST_X + DISPLAY_UI_MODE_FIRST_W) == DISPLAY_UI_MODE_REST_X);
  assert((DISPLAY_UI_TARGET_FIRST_X + DISPLAY_UI_TARGET_FIRST_W) == DISPLAY_UI_TARGET_REST_X);
}

int main(void)
{
  test_layout_matches_172_by_320_screen();
  test_layout_uses_three_full_width_cards();
  test_requested_micro_adjustments_are_kept();
  test_chinese_labels_have_full_card_width();
  test_split_labels_touch_at_shared_boundary();

  return 0;
}
