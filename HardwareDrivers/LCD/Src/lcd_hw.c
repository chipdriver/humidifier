#include "lcd_hw.h"

/**
 * @brief 给软件 SPI 时序提供一个很短的延时。
 * @retval 无
 */
static void LCD_SPI_Delay(void)
{
  /* 使用 volatile 计数变量，避免编译器把延时循环优化掉。 */
  volatile uint32_t delay = 8U;

  /* 一直循环，直到计数值减到 0。 */
  while (delay > 0U)
  {
    /* 执行一条 CPU 空操作指令。 */
    __NOP();

    /* 延时计数值减 1。 */
    delay--;
  }
}

/**
 * @brief 初始化 LCD 使用的 GPIO 引脚，并设置默认电平。
 * @retval 无
 */
void LCD_HW_GPIO_Init(void)
{
  /* 定义并清零 HAL GPIO 初始化结构体。 */
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* 使用 PB10/PB11/PB12/PB13/PB15 之前，先使能 GPIOB 外设时钟。 */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* 选择 RESET、DC、CS 这 3 个控制引脚，准备配置为中速输出。 */
  GPIO_InitStruct.Pin = LCD_RESET_PIN | LCD_DC_PIN | LCD_CS_PIN;

  /* 将选中的引脚配置为推挽输出模式。 */
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  /* 不启用芯片内部上拉或下拉电阻。 */
  GPIO_InitStruct.Pull = GPIO_NOPULL;

  /* 控制引脚使用中速即可。 */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;

  /* 将上面的配置写入 GPIOB。 */
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* 选择 SCK 和 MOSI 这 2 个软件 SPI 引脚，准备配置为高速输出。 */
  GPIO_InitStruct.Pin = LCD_SCK_PIN | LCD_MOSI_PIN;

  /* 继续使用推挽输出模式。 */
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  /* 继续不启用芯片内部上拉或下拉电阻。 */
  GPIO_InitStruct.Pull = GPIO_NOPULL;

  /* SCK 和 MOSI 属于软件 SPI 时序引脚，建议使用高速。 */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  /* 将上面的配置写入 GPIOB。 */
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* 初始化完成后，RESET 默认保持高电平。 */
  LCD_RESET_High();

  /* 初始化完成后，CS 默认保持高电平，表示不选中屏幕。 */
  LCD_CS_High();

  /* 初始化完成后，DC 默认保持低电平，表示命令模式。 */
  LCD_DC_Low();

  /* 初始化完成后，SCK 默认保持低电平，软件 SPI 空闲低。 */
  LCD_SCK_Low();

  /* 初始化完成后，MOSI 默认保持低电平。 */
  LCD_MOSI_Low();
}

/**
 * @brief 将 LCD RESET 引脚设置为高电平。
 * @retval 无
 */
void LCD_RESET_High(void)
{
  /* 输出 RESET 高电平。 */
  HAL_GPIO_WritePin(LCD_RESET_PORT, LCD_RESET_PIN, GPIO_PIN_SET);
}

/**
 * @brief 将 LCD RESET 引脚设置为低电平。
 * @retval 无
 */
void LCD_RESET_Low(void)
{
  /* 输出 RESET 低电平。 */
  HAL_GPIO_WritePin(LCD_RESET_PORT, LCD_RESET_PIN, GPIO_PIN_RESET);
}

/**
 * @brief 将 LCD DC 引脚设置为高电平。
 * @retval 无
 */
void LCD_DC_High(void)
{
  /* 输出 DC 高电平，表示后面发送的是数据。 */
  HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
}

/**
 * @brief 将 LCD DC 引脚设置为低电平。
 * @retval 无
 */
void LCD_DC_Low(void)
{
  /* 输出 DC 低电平，表示后面发送的是命令。 */
  HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET);
}

/**
 * @brief 将 LCD CS 引脚设置为高电平。
 * @retval 无
 */
void LCD_CS_High(void)
{
  /* 输出 CS 高电平，取消选中屏幕。 */
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
}

/**
 * @brief 将 LCD CS 引脚设置为低电平。
 * @retval 无
 */
void LCD_CS_Low(void)
{
  /* 输出 CS 低电平，选中屏幕。 */
  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
}

/**
 * @brief 将 LCD SCK 引脚设置为高电平。
 * @retval 无
 */
void LCD_SCK_High(void)
{
  /* 输出软件 SPI 时钟高电平。 */
  HAL_GPIO_WritePin(LCD_SCK_PORT, LCD_SCK_PIN, GPIO_PIN_SET);
}

/**
 * @brief 将 LCD SCK 引脚设置为低电平。
 * @retval 无
 */
void LCD_SCK_Low(void)
{
  /* 输出软件 SPI 时钟低电平。 */
  HAL_GPIO_WritePin(LCD_SCK_PORT, LCD_SCK_PIN, GPIO_PIN_RESET);
}

/**
 * @brief 将 LCD MOSI 引脚设置为高电平。
 * @retval 无
 */
void LCD_MOSI_High(void)
{
  /* 输出软件 SPI MOSI 高电平。 */
  HAL_GPIO_WritePin(LCD_MOSI_PORT, LCD_MOSI_PIN, GPIO_PIN_SET);
}

/**
 * @brief 将 LCD MOSI 引脚设置为低电平。
 * @retval 无
 */
void LCD_MOSI_Low(void)
{
  /* 输出软件 SPI MOSI 低电平。 */
  HAL_GPIO_WritePin(LCD_MOSI_PORT, LCD_MOSI_PIN, GPIO_PIN_RESET);
}

/**
 * @brief 通过软件 SPI 向 LCD 写入 1 个 bit。
 * @param bit 要发送的 bit 值；0 表示发送 0，非 0 表示发送 1。
 * @retval 无
 */
void LCD_SPI_WriteBit(uint8_t bit)
{
  /* 判断当前要发送的 bit 是否为逻辑 1。 */
  if (bit != 0U)
  {
    /* 在 SCK 上升沿到来之前，先把 MOSI 准备为高电平。 */
    LCD_MOSI_High();
  }
  else
  {
    /* 在 SCK 上升沿到来之前，先把 MOSI 准备为低电平。 */
    LCD_MOSI_Low();
  }

  /* 拉高 SCK，产生上升沿，屏幕在这个边沿采样 MOSI。 */
  LCD_SCK_High();

  /* 保持 SCK 高电平一小段时间。 */
  LCD_SPI_Delay();

  /* 拉低 SCK，准备发送下一位数据。 */
  LCD_SCK_Low();

  /* 保持 SCK 低电平一小段时间。 */
  LCD_SPI_Delay();
}

/**
 * @brief 通过软件 SPI 向 LCD 写入 1 个字节。
 * @param data 要发送的字节，按高位在前的顺序发送。
 * @retval 无
 */
void LCD_SPI_WriteByte(uint8_t data)
{
  /* 一共发送 8 位，从 bit7 一直发送到 bit0。 */
  for (uint8_t bit_index = 0U; bit_index < 8U; bit_index++)
  {
    /* 发送当前最高位。 */
    LCD_SPI_WriteBit((uint8_t)(data & 0x80U));

    /* 左移 1 位，把下一位移动到最高位位置。 */
    data <<= 1U;
  }
}

/**
 * @brief 通过 RESET 引脚复位 LCD 模块。
 * @retval 无
 */
void LCD_Reset(void)
{
  /* 拉低 RESET，开始硬件复位脉冲。 */
  LCD_RESET_Low();

  /* RESET 低电平保持 20 ms。 */
  HAL_Delay(20U);

  /* 拉高 RESET，让屏幕退出复位状态。 */
  LCD_RESET_High();

  /* 等待 120 ms，让屏幕控制器内部稳定。 */
  HAL_Delay(120U);
}

/**
 * @brief 向 LCD 写入 1 个命令字节。
 * @param cmd 要发送的命令字节。
 * @retval 无
 */
void LCD_WriteCmd(uint8_t cmd)
{
  /* 拉低 DC，表示当前发送的是命令。 */
  LCD_DC_Low();

  /* 拉低 CS，选中屏幕。 */
  LCD_CS_Low();

  /* 通过软件 SPI 发送命令字节。 */
  LCD_SPI_WriteByte(cmd);

  /* 拉高 CS，取消选中屏幕。 */
  LCD_CS_High();
}

/**
 * @brief 向 LCD 写入 1 个数据字节。
 * @param data 要发送的数据字节。
 * @retval 无
 */
void LCD_WriteData(uint8_t data)
{
  /* 拉高 DC，表示当前发送的是数据。 */
  LCD_DC_High();

  /* 拉低 CS，选中屏幕。 */
  LCD_CS_Low();

  /* 通过软件 SPI 发送数据字节。 */
  LCD_SPI_WriteByte(data);

  /* 拉高 CS，取消选中屏幕。 */
  LCD_CS_High();
}

/**
 * @brief 向 LCD 连续写入多个数据字节。
 * @param data 指向待发送数据缓冲区的指针。
 * @param len 要发送的数据字节数量。
 * @retval 无
 */
void LCD_WriteDataBuffer(uint8_t *data, uint32_t len)
{
  /* 拉高 DC，表示当前发送的是数据。 */
  LCD_DC_High();

  /* 拉低 CS，并在整个数据缓冲区发送期间保持选中屏幕。 */
  LCD_CS_Low();

  /* 按顺序发送缓冲区中的每一个字节。 */
  for (uint32_t index = 0U; index < len; index++)
  {
    /* 通过软件 SPI 发送当前数据字节。 */
    LCD_SPI_WriteByte(data[index]);
  }

  /* 所有数据发送完成后，拉高 CS，取消选中屏幕。 */
  LCD_CS_High();
}
