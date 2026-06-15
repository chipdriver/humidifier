#include "humidifier_symbol_font.h"

#include <assert.h>
#include <stdint.h>

static void test_symbol_font_covers_dashboard_copy(void)
{
  static const uint32_t required_codepoints[] = {
    0x2103U,
    0x4F20U,
    0x4F5CU,
    0x4FDDU,
    0x52A0U,
    0x52A8U,
    0x5668U,
    0x5883U,
    0x5B9CU,
    0x5DE5U,
    0x5E38U,
    0x5EA6U,
    0x5F02U,
    0x5F0FU,
    0x611FU,
    0x6301U,
    0x667AU,
    0x6807U,
    0x6A21U,
    0x6E29U,
    0x6E7FU,
    0x73AFU,
    0x76EEU,
    0x80FDU,
    0x8212U,
    0x8282U,
    0x81EAU,
    0x8C03U,
    0x9002U,
    0xFF0CU
  };

  for (uint32_t i = 0U; i < (sizeof(required_codepoints) / sizeof(required_codepoints[0])); i++)
  {
    assert(HumidifierSymbolFont_HasGlyph(required_codepoints[i]) != 0U);
  }
}

static void test_symbol_font_uses_lvgl_static_bitmap_path(void)
{
  const lv_font_t *font = HumidifierSymbolFont_Get16();
  lv_font_glyph_dsc_t glyph_dsc = {0};
  const void *bitmap;

  assert(font != (const lv_font_t *)0);
  assert(font->static_bitmap == 1U);
  assert(font->get_glyph_dsc(font, &glyph_dsc, 0x667AU, 0U) == true);
  assert(glyph_dsc.adv_w == 16U);
  assert(glyph_dsc.format == LV_FONT_GLYPH_FORMAT_A8);
  assert(glyph_dsc.stride == 16U);

  glyph_dsc.req_raw_bitmap = 1U;
  bitmap = font->get_glyph_bitmap(&glyph_dsc, (lv_draw_buf_t *)0);
  assert(bitmap != (const void *)0);
}

static const uint8_t *get_symbol_bitmap(uint32_t codepoint, lv_font_glyph_dsc_t *glyph_dsc)
{
  const lv_font_t *font = HumidifierSymbolFont_Get16();

  assert(font != (const lv_font_t *)0);
  assert(font->get_glyph_dsc(font, glyph_dsc, codepoint, 0U) == true);
  assert(glyph_dsc->box_w == 16U);
  assert(glyph_dsc->box_h == 16U);
  assert(glyph_dsc->stride == 16U);

  glyph_dsc->req_raw_bitmap = 1U;
  return (const uint8_t *)font->get_glyph_bitmap(glyph_dsc, (lv_draw_buf_t *)0);
}

static void assert_symbol_glyph_has_clear_outer_bounds(uint32_t codepoint)
{
  lv_font_glyph_dsc_t glyph_dsc = {0};
  const uint8_t *bitmap = get_symbol_bitmap(codepoint, &glyph_dsc);

  assert(bitmap != (const uint8_t *)0);

  for (uint32_t x = 0U; x < 16U; x++)
  {
    assert(bitmap[x] == 0U);
    assert(bitmap[(15U * 16U) + x] == 0U);
  }

  for (uint32_t y = 0U; y < 16U; y++)
  {
    assert(bitmap[y * 16U] == 0U);
    assert(bitmap[(y * 16U) + 15U] == 0U);
  }
}

static void test_split_line_anchor_glyphs_are_not_edge_clipped(void)
{
  lv_font_glyph_dsc_t glyph_dsc = {0};
  const uint8_t *gong_bitmap;
  const uint8_t *mu_bitmap;

  assert_symbol_glyph_has_clear_outer_bounds(0x5DE5U);
  assert_symbol_glyph_has_clear_outer_bounds(0x76EEU);

  gong_bitmap = get_symbol_bitmap(0x5DE5U, &glyph_dsc);
  assert(gong_bitmap[(2U * 16U) + 3U] != 0U);
  assert(gong_bitmap[(7U * 16U) + 7U] != 0U);
  assert(gong_bitmap[(13U * 16U) + 12U] != 0U);

  mu_bitmap = get_symbol_bitmap(0x76EEU, &glyph_dsc);
  assert(mu_bitmap[(2U * 16U) + 3U] != 0U);
  assert(mu_bitmap[(7U * 16U) + 3U] != 0U);
  assert(mu_bitmap[(9U * 16U) + 8U] != 0U);
  assert(mu_bitmap[(13U * 16U) + 12U] != 0U);
}

int main(void)
{
  test_symbol_font_covers_dashboard_copy();
  test_symbol_font_uses_lvgl_static_bitmap_path();
  test_split_line_anchor_glyphs_are_not_edge_clipped();

  return 0;
}
