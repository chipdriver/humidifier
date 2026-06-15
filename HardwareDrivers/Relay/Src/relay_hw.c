#include "relay_hw.h"

/**
 * @brief 初始化继电器控制侧 GPIO，并确保两个继电器默认关闭。
 * @retval None
 */
void Relay_HW_Init(void)
{
  /* 定义并清零 HAL GPIO 初始化结构体。 */
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* 使用 PA1 之前，先使能 GPIOA 外设时钟。 */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* 使用 PB0 之前，先使能 GPIOB 外设时钟。 */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* 在配置为输出前，先把 PA1 的输出锁存值设置为释放电平。 */
  HAL_GPIO_WritePin(MIST_RELAY_PORT, MIST_RELAY_PIN, RELAY_INACTIVE_LEVEL);

  /* 在配置为输出前，先把 PB0 的输出锁存值设置为释放电平。 */
  HAL_GPIO_WritePin(FAN_RELAY_PORT, FAN_RELAY_PIN, RELAY_INACTIVE_LEVEL);

  /* 选择雾化继电器控制引脚 PA1。 */
  GPIO_InitStruct.Pin = MIST_RELAY_PIN;

  /* 将 PA1 配置为推挽输出模式。 */
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  /* PA1 不使用内部上拉或下拉电阻。 */
  GPIO_InitStruct.Pull = GPIO_NOPULL;

  /* 继电器控制不需要高速翻转，使用低速即可。 */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  /* 将 PA1 的配置写入 GPIOA。 */
  HAL_GPIO_Init(MIST_RELAY_PORT, &GPIO_InitStruct);

  /* 选择风扇继电器控制引脚 PB0。 */
  GPIO_InitStruct.Pin = FAN_RELAY_PIN;

  /* 将 PB0 配置为推挽输出模式。 */
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  /* PB0 不使用内部上拉或下拉电阻。 */
  GPIO_InitStruct.Pull = GPIO_NOPULL;

  /* 继电器控制不需要高速翻转，使用低速即可。 */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  /* 将 PB0 的配置写入 GPIOB。 */
  HAL_GPIO_Init(FAN_RELAY_PORT, &GPIO_InitStruct);

  /* 初始化完成后，再统一关闭一次，确保两个继电器默认释放。 */
  Relay_HW_AllOff();
}

/**
 * @brief 开启雾化继电器，使继电器吸合。
 * @retval None
 */
void MistRelay_On(void)
{
  /* PA1 输出有效电平，高电平触发雾化继电器吸合。 */
  HAL_GPIO_WritePin(MIST_RELAY_PORT, MIST_RELAY_PIN, RELAY_ACTIVE_LEVEL);
}

/**
 * @brief 关闭雾化继电器，使继电器释放。
 * @retval None
 */
void MistRelay_Off(void)
{
  /* PA1 输出无效电平，低电平让雾化继电器释放。 */
  HAL_GPIO_WritePin(MIST_RELAY_PORT, MIST_RELAY_PIN, RELAY_INACTIVE_LEVEL);
}

/**
 * @brief 开启风扇继电器，使继电器吸合。
 * @retval None
 */
void FanRelay_On(void)
{
  /* PB0 输出有效电平，高电平触发风扇继电器吸合。 */
  HAL_GPIO_WritePin(FAN_RELAY_PORT, FAN_RELAY_PIN, RELAY_ACTIVE_LEVEL);
}

/**
 * @brief 关闭风扇继电器，使继电器释放。
 * @retval None
 */
void FanRelay_Off(void)
{
  /* PB0 输出无效电平，低电平让风扇继电器释放。 */
  HAL_GPIO_WritePin(FAN_RELAY_PORT, FAN_RELAY_PIN, RELAY_INACTIVE_LEVEL);
}

/**
 * @brief 关闭所有继电器。
 * @retval None
 */
void Relay_HW_AllOff(void)
{
  /* 关闭雾化继电器。 */
  MistRelay_Off();

  /* 关闭风扇继电器。 */
  FanRelay_Off();
}

/**
 * @brief 只测试继电器控制侧的吸合和释放动作。
 * @retval None
 */
void Relay_HW_TestControlSide(void)
{
  /* 测试开始前先关闭全部继电器。 */
  Relay_HW_AllOff();

  /* 保持全部关闭 1000ms，便于观察默认释放状态。 */
  HAL_Delay(1000U);

  /* 开启雾化继电器，观察继电器是否吸合或指示灯是否变化。 */
  MistRelay_On();

  /* 雾化继电器保持吸合 1000ms。 */
  HAL_Delay(1000U);

  /* 关闭雾化继电器，观察继电器是否释放。 */
  MistRelay_Off();

  /* 雾化继电器保持释放 1000ms。 */
  HAL_Delay(1000U);

  /* 开启风扇继电器，观察继电器是否吸合或指示灯是否变化。 */
  FanRelay_On();

  /* 风扇继电器保持吸合 1000ms。 */
  HAL_Delay(1000U);

  /* 关闭风扇继电器，观察继电器是否释放。 */
  FanRelay_Off();

  /* 风扇继电器保持释放 1000ms。 */
  HAL_Delay(1000U);

  /* 测试结束后再次关闭全部继电器。 */
  Relay_HW_AllOff();
}
