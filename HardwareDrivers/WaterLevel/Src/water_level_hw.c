#include "water_level_hw.h"

/**
 * @brief 初始化浮球水位开关 PA0，并启用内部上拉。
 * @retval 无
 */
void WaterLevel_HW_Init(void)
{
  /* 定义并清零 HAL GPIO 初始化结构体。 */
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* 使用 PA0 之前，先使能 GPIOA 外设时钟。 */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* 选择 PA0 作为浮球水位输入引脚。 */
  GPIO_InitStruct.Pin = WATER_LEVEL_PIN;

  /* PA0 只读取电平，所以配置为输入模式。 */
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;

  /* 浮球另一端接 GND，内部上拉可以让断开时读到高电平。 */
  GPIO_InitStruct.Pull = GPIO_PULLUP;

  /* 输入模式不需要配置速度，这里保留低速默认值。 */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  /* 将 PA0 配置写入 GPIOA。 */
  HAL_GPIO_Init(WATER_LEVEL_PORT, &GPIO_InitStruct);
}

/**
 * @brief 读取 PA0 当前原始电平。
 * @retval GPIO_PIN_SET 表示高电平，GPIO_PIN_RESET 表示低电平。
 */
GPIO_PinState WaterLevel_HW_ReadRaw(void)
{
  /* 本层只返回原始电平，不判断有水或缺水。 */
  return HAL_GPIO_ReadPin(WATER_LEVEL_PORT, WATER_LEVEL_PIN);
}
