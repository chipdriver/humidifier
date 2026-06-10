#include "st7789.h"

#define ST7789_CMD_SWRESET   0x01
#define ST7789_CMD_SLPOUT    0x11
#define ST7789_CMD_NORON     0x13
#define ST7789_CMD_INVOFF    0x20
#define ST7789_CMD_INVON     0x21
#define ST7789_CMD_DISPON    0x29
#define ST7789_CMD_CASET     0x2A
#define ST7789_CMD_RASET     0x2B
#define ST7789_CMD_RAMWR     0x2C
#define ST7789_CMD_MADCTL    0x36
#define ST7789_CMD_COLMOD    0x3A

#define ST7789_COLMOD_16BIT  0x55
#define ST7789_MADCTL_VALUE  0x00
#define ST7789_USE_INVERSION 1

static const uint8_t s_font_5x7[][5] = {
  {0x00, 0x00, 0x00, 0x00, 0x00}, /* space */
  {0x00, 0x00, 0x5F, 0x00, 0x00}, /* ! */
  {0x00, 0x07, 0x00, 0x07, 0x00}, /* " */
  {0x14, 0x7F, 0x14, 0x7F, 0x14}, /* # */
  {0x24, 0x2A, 0x7F, 0x2A, 0x12}, /* $ */
  {0x23, 0x13, 0x08, 0x64, 0x62}, /* % */
  {0x36, 0x49, 0x55, 0x22, 0x50}, /* & */
  {0x00, 0x05, 0x03, 0x00, 0x00}, /* ' */
  {0x00, 0x1C, 0x22, 0x41, 0x00}, /* ( */
  {0x00, 0x41, 0x22, 0x1C, 0x00}, /* ) */
  {0x14, 0x08, 0x3E, 0x08, 0x14}, /* * */
  {0x08, 0x08, 0x3E, 0x08, 0x08}, /* + */
  {0x00, 0x50, 0x30, 0x00, 0x00}, /* , */
  {0x08, 0x08, 0x08, 0x08, 0x08}, /* - */
  {0x00, 0x60, 0x60, 0x00, 0x00}, /* . */
  {0x20, 0x10, 0x08, 0x04, 0x02}, /* / */
  {0x3E, 0x51, 0x49, 0x45, 0x3E}, /* 0 */
  {0x00, 0x42, 0x7F, 0x40, 0x00}, /* 1 */
  {0x42, 0x61, 0x51, 0x49, 0x46}, /* 2 */
  {0x21, 0x41, 0x45, 0x4B, 0x31}, /* 3 */
  {0x18, 0x14, 0x12, 0x7F, 0x10}, /* 4 */
  {0x27, 0x45, 0x45, 0x45, 0x39}, /* 5 */
  {0x3C, 0x4A, 0x49, 0x49, 0x30}, /* 6 */
  {0x01, 0x71, 0x09, 0x05, 0x03}, /* 7 */
  {0x36, 0x49, 0x49, 0x49, 0x36}, /* 8 */
  {0x06, 0x49, 0x49, 0x29, 0x1E}, /* 9 */
  {0x00, 0x36, 0x36, 0x00, 0x00}, /* : */
  {0x00, 0x56, 0x36, 0x00, 0x00}, /* ; */
  {0x08, 0x14, 0x22, 0x41, 0x00}, /* < */
  {0x14, 0x14, 0x14, 0x14, 0x14}, /* = */
  {0x00, 0x41, 0x22, 0x14, 0x08}, /* > */
  {0x02, 0x01, 0x51, 0x09, 0x06}, /* ? */
  {0x32, 0x49, 0x79, 0x41, 0x3E}, /* @ */
  {0x7E, 0x11, 0x11, 0x11, 0x7E}, /* A */
  {0x7F, 0x49, 0x49, 0x49, 0x36}, /* B */
  {0x3E, 0x41, 0x41, 0x41, 0x22}, /* C */
  {0x7F, 0x41, 0x41, 0x22, 0x1C}, /* D */
  {0x7F, 0x49, 0x49, 0x49, 0x41}, /* E */
  {0x7F, 0x09, 0x09, 0x09, 0x01}, /* F */
  {0x3E, 0x41, 0x49, 0x49, 0x7A}, /* G */
  {0x7F, 0x08, 0x08, 0x08, 0x7F}, /* H */
  {0x00, 0x41, 0x7F, 0x41, 0x00}, /* I */
  {0x20, 0x40, 0x41, 0x3F, 0x01}, /* J */
  {0x7F, 0x08, 0x14, 0x22, 0x41}, /* K */
  {0x7F, 0x40, 0x40, 0x40, 0x40}, /* L */
  {0x7F, 0x02, 0x0C, 0x02, 0x7F}, /* M */
  {0x7F, 0x04, 0x08, 0x10, 0x7F}, /* N */
  {0x3E, 0x41, 0x41, 0x41, 0x3E}, /* O */
  {0x7F, 0x09, 0x09, 0x09, 0x06}, /* P */
  {0x3E, 0x41, 0x51, 0x21, 0x5E}, /* Q */
  {0x7F, 0x09, 0x19, 0x29, 0x46}, /* R */
  {0x46, 0x49, 0x49, 0x49, 0x31}, /* S */
  {0x01, 0x01, 0x7F, 0x01, 0x01}, /* T */
  {0x3F, 0x40, 0x40, 0x40, 0x3F}, /* U */
  {0x1F, 0x20, 0x40, 0x20, 0x1F}, /* V */
  {0x3F, 0x40, 0x38, 0x40, 0x3F}, /* W */
  {0x63, 0x14, 0x08, 0x14, 0x63}, /* X */
  {0x07, 0x08, 0x70, 0x08, 0x07}, /* Y */
  {0x61, 0x51, 0x49, 0x45, 0x43}, /* Z */
  {0x00, 0x7F, 0x41, 0x41, 0x00}, /* [ */
  {0x02, 0x04, 0x08, 0x10, 0x20}, /* \ */
  {0x00, 0x41, 0x41, 0x7F, 0x00}, /* ] */
  {0x04, 0x02, 0x01, 0x02, 0x04}, /* ^ */
  {0x40, 0x40, 0x40, 0x40, 0x40}, /* _ */
  {0x00, 0x01, 0x02, 0x04, 0x00}, /* ` */
  {0x20, 0x54, 0x54, 0x54, 0x78}, /* a */
  {0x7F, 0x48, 0x44, 0x44, 0x38}, /* b */
  {0x38, 0x44, 0x44, 0x44, 0x20}, /* c */
  {0x38, 0x44, 0x44, 0x48, 0x7F}, /* d */
  {0x38, 0x54, 0x54, 0x54, 0x18}, /* e */
  {0x08, 0x7E, 0x09, 0x01, 0x02}, /* f */
  {0x0C, 0x52, 0x52, 0x52, 0x3E}, /* g */
  {0x7F, 0x08, 0x04, 0x04, 0x78}, /* h */
  {0x00, 0x44, 0x7D, 0x40, 0x00}, /* i */
  {0x20, 0x40, 0x44, 0x3D, 0x00}, /* j */
  {0x7F, 0x10, 0x28, 0x44, 0x00}, /* k */
  {0x00, 0x41, 0x7F, 0x40, 0x00}, /* l */
  {0x7C, 0x04, 0x18, 0x04, 0x78}, /* m */
  {0x7C, 0x08, 0x04, 0x04, 0x78}, /* n */
  {0x38, 0x44, 0x44, 0x44, 0x38}, /* o */
  {0x7C, 0x14, 0x14, 0x14, 0x08}, /* p */
  {0x08, 0x14, 0x14, 0x18, 0x7C}, /* q */
  {0x7C, 0x08, 0x04, 0x04, 0x08}, /* r */
  {0x48, 0x54, 0x54, 0x54, 0x20}, /* s */
  {0x04, 0x3F, 0x44, 0x40, 0x20}, /* t */
  {0x3C, 0x40, 0x40, 0x20, 0x7C}, /* u */
  {0x1C, 0x20, 0x40, 0x20, 0x1C}, /* v */
  {0x3C, 0x40, 0x30, 0x40, 0x3C}, /* w */
  {0x44, 0x28, 0x10, 0x28, 0x44}, /* x */
  {0x0C, 0x50, 0x50, 0x50, 0x3C}, /* y */
  {0x44, 0x64, 0x54, 0x4C, 0x44}, /* z */
  {0x00, 0x08, 0x36, 0x41, 0x00}, /* { */
  {0x00, 0x00, 0x7F, 0x00, 0x00}, /* | */
  {0x00, 0x41, 0x36, 0x08, 0x00}, /* } */
  {0x08, 0x08, 0x2A, 0x1C, 0x08}  /* ~ */
};

/**
 * @brief 向 ST7789 写入 1 个 16 位数值，高字节在前。
 * @param value 要写入的 16 位数值。
 * @retval 无
 */
static void ST7789_WriteU16(uint16_t value)
{
  /* 先发送高 8 位。 */
  LCD_WriteData((uint8_t)(value >> 8));

  /* 再发送低 8 位。 */
  LCD_WriteData((uint8_t)(value & 0x00FFU));
}

/**
 * @brief 初始化 ST7789 屏幕控制器。
 * @retval 无
 */
void ST7789_Init(void)
{
  /* 初始化上一层硬件驱动使用的 GPIO 引脚。 */
  LCD_HW_GPIO_Init();

  /* 通过 RESET 引脚复位屏幕。 */
  LCD_Reset();

  /* 发送退出睡眠模式命令。 */
  LCD_WriteCmd(ST7789_CMD_SLPOUT);

  /* 等待屏幕从睡眠模式中稳定退出。 */
  HAL_Delay(120U);

  /* 发送正常显示模式开启命令。 */
  LCD_WriteCmd(ST7789_CMD_NORON);

  /* 发送颜色格式设置命令。 */
  LCD_WriteCmd(ST7789_CMD_COLMOD);

  /* 设置为 RGB565 / 16-bit 颜色格式。 */
  LCD_WriteData(ST7789_COLMOD_16BIT);

  /* 发送扫描方向设置命令。 */
  LCD_WriteCmd(ST7789_CMD_MADCTL);

  /* 写入当前版本使用的扫描方向参数。 */
  LCD_WriteData(ST7789_MADCTL_VALUE);

  /* 判断是否启用显示反色模式。 */
#if ST7789_USE_INVERSION
  /* 发送显示反色开启命令。 */
  LCD_WriteCmd(ST7789_CMD_INVON);
#else
  /* 发送显示反色关闭命令。 */
  LCD_WriteCmd(ST7789_CMD_INVOFF);
#endif

  /* 发送开启显示命令。 */
  LCD_WriteCmd(ST7789_CMD_DISPON);

  /* 等待显示开启后稳定。 */
  HAL_Delay(20U);

  /* 初始化完成后，默认把整屏清为黑色。 */
  ST7789_FillScreen(ST7789_COLOR_BLACK);
}

/**
 * @brief 设置 ST7789 接下来写入显存的窗口区域。
 * @param x0 窗口左上角 X 坐标。
 * @param y0 窗口左上角 Y 坐标。
 * @param x1 窗口右下角 X 坐标。
 * @param y1 窗口右下角 Y 坐标。
 * @retval 无
 */
void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  /* 如果起点 X 超出屏幕范围，就限制到屏幕最右侧像素。 */
  if (x0 >= ST7789_WIDTH)
  {
    x0 = ST7789_WIDTH - 1U;
  }

  /* 如果终点 X 超出屏幕范围，就限制到屏幕最右侧像素。 */
  if (x1 >= ST7789_WIDTH)
  {
    x1 = ST7789_WIDTH - 1U;
  }

  /* 如果起点 Y 超出屏幕范围，就限制到屏幕最底部像素。 */
  if (y0 >= ST7789_HEIGHT)
  {
    y0 = ST7789_HEIGHT - 1U;
  }

  /* 如果终点 Y 超出屏幕范围，就限制到屏幕最底部像素。 */
  if (y1 >= ST7789_HEIGHT)
  {
    y1 = ST7789_HEIGHT - 1U;
  }

  /* 如果 X 起点大于 X 终点，就交换这两个坐标。 */
  if (x0 > x1)
  {
    uint16_t temp = x0;
    x0 = x1;
    x1 = temp;
  }

  /* 如果 Y 起点大于 Y 终点，就交换这两个坐标。 */
  if (y0 > y1)
  {
    uint16_t temp = y0;
    y0 = y1;
    y1 = temp;
  }

  /* 计算加入屏幕 X 偏移后的列起点。 */
  uint16_t x_start = x0 + ST7789_X_OFFSET;

  /* 计算加入屏幕 X 偏移后的列终点。 */
  uint16_t x_end = x1 + ST7789_X_OFFSET;

  /* 计算加入屏幕 Y 偏移后的行起点。 */
  uint16_t y_start = y0 + ST7789_Y_OFFSET;

  /* 计算加入屏幕 Y 偏移后的行终点。 */
  uint16_t y_end = y1 + ST7789_Y_OFFSET;

  /* 发送列地址设置命令。 */
  LCD_WriteCmd(ST7789_CMD_CASET);

  /* 发送列起点，高字节在前。 */
  ST7789_WriteU16(x_start);

  /* 发送列终点，高字节在前。 */
  ST7789_WriteU16(x_end);

  /* 发送行地址设置命令。 */
  LCD_WriteCmd(ST7789_CMD_RASET);

  /* 发送行起点，高字节在前。 */
  ST7789_WriteU16(y_start);

  /* 发送行终点，高字节在前。 */
  ST7789_WriteU16(y_end);

  /* 发送写显存命令，后续数据会写入这个窗口区域。 */
  LCD_WriteCmd(ST7789_CMD_RAMWR);
}

void ST7789_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  ST7789_SetAddressWindow(x0, y0, x1, y1);
}

/**
 * @brief 向 ST7789 写入 1 个 RGB565 颜色。
 * @param color 要写入的 RGB565 颜色值。
 * @retval 无
 */
void ST7789_WriteColor(uint16_t color)
{
  /* RGB565 颜色先发送高 8 位。 */
  LCD_WriteData((uint8_t)(color >> 8));

  /* RGB565 颜色再发送低 8 位。 */
  LCD_WriteData((uint8_t)(color & 0x00FFU));
}

void ST7789_WritePixels(uint16_t *colors, uint32_t len)
{
  for (uint32_t index = 0U; index < len; index++)
  {
    ST7789_WriteColor(colors[index]);
  }
}

/**
 * @brief 在指定坐标画 1 个像素点。
 * @param x 像素点 X 坐标。
 * @param y 像素点 Y 坐标。
 * @param color 像素点 RGB565 颜色值。
 * @retval 无
 */
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  /* 如果 X 坐标越界，直接返回。 */
  if (x >= ST7789_WIDTH)
  {
    return;
  }

  /* 如果 Y 坐标越界，直接返回。 */
  if (y >= ST7789_HEIGHT)
  {
    return;
  }

  /* 设置写入窗口为当前这 1 个像素。 */
  ST7789_SetAddressWindow(x, y, x, y);

  /* 写入这个像素的 RGB565 颜色。 */
  ST7789_WriteColor(color);
}

/**
 * @brief 填充一个矩形区域。
 * @param x 矩形左上角 X 坐标。
 * @param y 矩形左上角 Y 坐标。
 * @param w 矩形宽度。
 * @param h 矩形高度。
 * @param color 要填充的 RGB565 颜色值。
 * @retval 无
 */
void ST7789_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
  /* 如果矩形宽度为 0，直接返回。 */
  if (w == 0U)
  {
    return;
  }

  /* 如果矩形高度为 0，直接返回。 */
  if (h == 0U)
  {
    return;
  }

  /* 如果矩形起点 X 已经在屏幕外，直接返回。 */
  if (x >= ST7789_WIDTH)
  {
    return;
  }

  /* 如果矩形起点 Y 已经在屏幕外，直接返回。 */
  if (y >= ST7789_HEIGHT)
  {
    return;
  }

  /* 如果矩形宽度超过右边界，就裁剪到屏幕右边界。 */
  if ((uint32_t)x + (uint32_t)w > ST7789_WIDTH)
  {
    w = ST7789_WIDTH - x;
  }

  /* 如果矩形高度超过下边界，就裁剪到屏幕下边界。 */
  if ((uint32_t)y + (uint32_t)h > ST7789_HEIGHT)
  {
    h = ST7789_HEIGHT - y;
  }

  /* 计算矩形右下角 X 坐标。 */
  uint16_t x1 = x + w - 1U;

  /* 计算矩形右下角 Y 坐标。 */
  uint16_t y1 = y + h - 1U;

  /* 设置接下来要写入的显存窗口。 */
  ST7789_SetAddressWindow(x, y, x1, y1);

  /* 计算矩形区域内一共有多少个像素。 */
  uint32_t pixel_count = (uint32_t)w * (uint32_t)h;

  /* 按像素数量循环写入同一个颜色。 */
  for (uint32_t index = 0U; index < pixel_count; index++)
  {
    /* 写入当前像素的 RGB565 颜色。 */
    ST7789_WriteColor(color);
  }
}

/**
 * @brief 使用指定颜色填充整个屏幕。
 * @param color 要填充的 RGB565 颜色值。
 * @retval 无
 */
void ST7789_FillScreen(uint16_t color)
{
  /* 从屏幕左上角开始，按屏幕宽高填充整个区域。 */
  ST7789_FillRect(0U, 0U, ST7789_WIDTH, ST7789_HEIGHT, color);
}

/**
 * @brief 在指定坐标绘制 1 个 5x7 ASCII 字符。
 * @param x 字符左上角 X 坐标。
 * @param y 字符左上角 Y 坐标。
 * @param ch 要绘制的 ASCII 字符。
 * @param color 字符前景色。
 * @param bg_color 字符背景色。
 * @retval 无
 */
void ST7789_DrawChar(uint16_t x, uint16_t y, char ch, uint16_t color, uint16_t bg_color)
{
  /* 字符宽度为 5 列，额外留 1 列空白作为字符间距。 */
  const uint16_t char_width = 6U;

  /* 字符高度为 7 行，额外留 1 行空白作为行间距。 */
  const uint16_t char_height = 8U;

  /* 如果字符整体已经超出屏幕右边界，就直接返回。 */
  if (x >= ST7789_WIDTH)
  {
    return;
  }

  /* 如果字符整体已经超出屏幕下边界，就直接返回。 */
  if (y >= ST7789_HEIGHT)
  {
    return;
  }

  /* 非可打印 ASCII 字符统一按空格显示。 */
  if ((ch < ' ') || (ch > '~'))
  {
    ch = ' ';
  }

  /* 计算当前字符在 5x7 字库表中的索引。 */
  const uint8_t *bitmap = s_font_5x7[(uint8_t)ch - (uint8_t)' '];

  /* 逐列绘制字符和背景。 */
  for (uint16_t col = 0U; col < char_width; col++)
  {
    /* 字体表只有前 5 列，第 6 列固定作为空白间距。 */
    uint8_t column_bits = (col < 5U) ? bitmap[col] : 0U;

    /* 逐行绘制当前列的每一个像素。 */
    for (uint16_t row = 0U; row < char_height; row++)
    {
      /* 计算当前像素的屏幕 X 坐标。 */
      uint16_t pixel_x = x + col;

      /* 计算当前像素的屏幕 Y 坐标。 */
      uint16_t pixel_y = y + row;

      /* 如果当前像素超出屏幕边界，就跳过。 */
      if ((pixel_x >= ST7789_WIDTH) || (pixel_y >= ST7789_HEIGHT))
      {
        continue;
      }

      /* 判断字库当前 bit 是否需要画前景色。 */
      if ((row < 7U) && ((column_bits & (uint8_t)(1U << row)) != 0U))
      {
        /* 当前 bit 为 1，画字符前景色。 */
        ST7789_DrawPixel(pixel_x, pixel_y, color);
      }
      else
      {
        /* 当前 bit 为 0，画背景色，避免旧字符残留。 */
        ST7789_DrawPixel(pixel_x, pixel_y, bg_color);
      }
    }
  }
}

/**
 * @brief 在指定坐标绘制一个 ASCII 字符串。
 * @param x 字符串左上角 X 坐标。
 * @param y 字符串左上角 Y 坐标。
 * @param str 指向以 '\0' 结尾的 ASCII 字符串。
 * @param color 字符前景色。
 * @param bg_color 字符背景色。
 * @retval 无
 */
void ST7789_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg_color)
{
  /* 参数指针不能为空。 */
  if (str == (const char *)0)
  {
    return;
  }

  /* 按 6 像素宽度逐字符绘制。 */
  while (*str != '\0')
  {
    /* 如果下一个字符已经超出屏幕右侧，就停止绘制。 */
    if (x >= ST7789_WIDTH)
    {
      return;
    }

    /* 绘制当前字符。 */
    ST7789_DrawChar(x, y, *str, color, bg_color);

    /* X 坐标右移 6 像素，准备绘制下一个字符。 */
    x = (uint16_t)(x + 6U);

    /* 指向下一个字符。 */
    str++;
  }
}

/**
 * @brief 循环显示几种纯色，用于烧录后肉眼验证屏幕。
 * @retval 无
 */
void ST7789_TestColorCycle(void)
{
  /* 全屏显示红色。 */
  ST7789_FillScreen(ST7789_COLOR_RED);

  /* 红色保持 500 ms。 */
  HAL_Delay(500U);

  /* 全屏显示绿色。 */
  ST7789_FillScreen(ST7789_COLOR_GREEN);

  /* 绿色保持 500 ms。 */
  HAL_Delay(500U);

  /* 全屏显示蓝色。 */
  ST7789_FillScreen(ST7789_COLOR_BLUE);

  /* 蓝色保持 500 ms。 */
  HAL_Delay(500U);

  /* 全屏显示白色。 */
  ST7789_FillScreen(ST7789_COLOR_WHITE);

  /* 白色保持 500 ms。 */
  HAL_Delay(500U);

  /* 全屏显示黑色。 */
  ST7789_FillScreen(ST7789_COLOR_BLACK);

  /* 黑色保持 500 ms。 */
  HAL_Delay(500U);
}
