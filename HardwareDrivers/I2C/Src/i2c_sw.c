#include "i2c_sw.h"

/**
 * @brief 给软件 I2C bit 级时序提供一个很短的延时。
 * @retval 无
 */
static void I2C_SW_Delay(void)
{
  /* 使用 volatile 变量，避免编译器把空循环优化掉。 */
  volatile uint32_t i;

  /* 第一版使用简单 for 循环，后续可以按示波器结果微调次数。 */
  for (i = 0U; i < 50U; i++)
  {
    /* 执行一条 CPU 空操作指令，用来消耗少量时间。 */
    __NOP();
  }
}

/**
 * @brief 初始化软件 I2C 使用的 PB6/PB7 GPIO，并默认释放总线。
 * @retval 无
 */
void I2C_SW_GPIO_Init(void)
{
  /* 定义并清零 HAL GPIO 初始化结构体。 */
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* 使用 PB6/PB7 之前，先使能 GPIOB 外设时钟。 */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* 在切换为输出模式之前先把输出数据寄存器置高，减少上电时误拉低总线。 */
  HAL_GPIO_WritePin(I2C_SW_SCL_PORT, I2C_SW_SCL_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(I2C_SW_SDA_PORT, I2C_SW_SDA_PIN, GPIO_PIN_SET);

  /* 选择 PB6 和 PB7，分别作为软件 I2C 的 SCL 和 SDA。 */
  GPIO_InitStruct.Pin = I2C_SW_SCL_PIN | I2C_SW_SDA_PIN;

  /* I2C 总线是开漏结构，输出高表示释放总线，输出低才是真正拉低总线。 */
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;

  /* 第一版保留芯片内部上拉；即使模块外部带上拉，内部上拉也不会影响基本验证。 */
  GPIO_InitStruct.Pull = GPIO_PULLUP;

  /* 软件 I2C 时序引脚使用高速 GPIO。 */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  /* 将上面的配置写入 GPIOB。 */
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* 初始化完成后释放 SCL，总线由上拉电阻拉为高电平。 */
  I2C_SW_SCL_High();

  /* 初始化完成后释放 SDA，总线由上拉电阻拉为高电平。 */
  I2C_SW_SDA_High();
}

/**
 * @brief 释放 SCL，总线被上拉为高电平。
 * @retval 无
 */
void I2C_SW_SCL_High(void)
{
  /* 开漏输出写 1 表示不主动拉低 SCL。 */
  HAL_GPIO_WritePin(I2C_SW_SCL_PORT, I2C_SW_SCL_PIN, GPIO_PIN_SET);
}

/**
 * @brief 拉低 SCL。
 * @retval 无
 */
void I2C_SW_SCL_Low(void)
{
  /* 开漏输出写 0 表示主动把 SCL 拉到低电平。 */
  HAL_GPIO_WritePin(I2C_SW_SCL_PORT, I2C_SW_SCL_PIN, GPIO_PIN_RESET);
}

/**
 * @brief 释放 SDA，总线被上拉为高电平。
 * @retval 无
 */
void I2C_SW_SDA_High(void)
{
  /* 开漏输出写 1 表示不主动拉低 SDA。 */
  HAL_GPIO_WritePin(I2C_SW_SDA_PORT, I2C_SW_SDA_PIN, GPIO_PIN_SET);
}

/**
 * @brief 拉低 SDA。
 * @retval 无
 */
void I2C_SW_SDA_Low(void)
{
  /* 开漏输出写 0 表示主动把 SDA 拉到低电平。 */
  HAL_GPIO_WritePin(I2C_SW_SDA_PORT, I2C_SW_SDA_PIN, GPIO_PIN_RESET);
}

/**
 * @brief 读取 SDA 当前实际电平。
 * @retval 0 表示 SDA 为低电平，1 表示 SDA 为高电平。
 */
uint8_t I2C_SW_SDA_Read(void)
{
  /* 读取 GPIO 输入数据寄存器中的 SDA 实际电平。 */
  if (HAL_GPIO_ReadPin(I2C_SW_SDA_PORT, I2C_SW_SDA_PIN) == GPIO_PIN_SET)
  {
    /* HAL 读到 GPIO_PIN_SET，统一返回数字 1。 */
    return 1U;
  }

  /* HAL 没有读到 GPIO_PIN_SET，统一返回数字 0。 */
  return 0U;
}

/**
 * @brief 产生软件 I2C START 起始信号。
 * @retval 无
 */
void I2C_SW_Start(void)
{
  /* 先释放 SDA，保证起始信号之前 SDA 处于高电平。 */
  I2C_SW_SDA_High();

  /* 再释放 SCL，让 SCL 处于高电平。 */
  I2C_SW_SCL_High();

  /* 保持总线空闲状态一小段时间。 */
  I2C_SW_Delay();

  /* 当 SCL 为高时，SDA 从高变低，形成 START。 */
  I2C_SW_SDA_Low();

  /* 保持 START 条件一小段时间。 */
  I2C_SW_Delay();

  /* 拉低 SCL，准备后续发送或读取数据位。 */
  I2C_SW_SCL_Low();

  /* 保持 SCL 低电平一小段时间。 */
  I2C_SW_Delay();
}

/**
 * @brief 产生软件 I2C STOP 停止信号。
 * @retval 无
 */
void I2C_SW_Stop(void)
{
  /* 先拉低 SDA，为后面的低到高变化做准备。 */
  I2C_SW_SDA_Low();

  /* 保持 SDA 低电平一小段时间。 */
  I2C_SW_Delay();

  /* 释放 SCL，让 SCL 处于高电平。 */
  I2C_SW_SCL_High();

  /* 保持 SCL 高电平一小段时间。 */
  I2C_SW_Delay();

  /* 当 SCL 为高时，SDA 从低变高，形成 STOP。 */
  I2C_SW_SDA_High();

  /* 保持 STOP 条件一小段时间。 */
  I2C_SW_Delay();
}

/**
 * @brief 通过软件 I2C 写入 1 个 bit。
 * @param bit 要发送的 bit 值；0 表示发送 0，非 0 表示发送 1。
 * @retval 无
 */
void I2C_SW_WriteBit(uint8_t bit)
{
  /* SCL 低电平期间准备 SDA 数据，避免从机在高电平采样期看到毛刺。 */
  I2C_SW_SCL_Low();

  /* 判断当前要发送的 bit 是否为逻辑 1。 */
  if (bit != 0U)
  {
    /* 发送 1 时释放 SDA，由上拉电阻把总线拉高。 */
    I2C_SW_SDA_High();
  }
  else
  {
    /* 发送 0 时主动拉低 SDA。 */
    I2C_SW_SDA_Low();
  }

  /* 数据准备好后保持一小段时间。 */
  I2C_SW_Delay();

  /* 拉高 SCL，产生时钟高电平，从机在这个阶段采样 SDA。 */
  I2C_SW_SCL_High();

  /* 保持 SCL 高电平一小段时间。 */
  I2C_SW_Delay();

  /* 拉低 SCL，结束当前 bit 的时钟周期。 */
  I2C_SW_SCL_Low();

  /* 保持 SCL 低电平一小段时间，准备下一个 bit。 */
  I2C_SW_Delay();
}

/**
 * @brief 通过软件 I2C 读取 1 个 bit。
 * @retval 0 表示读到低电平，1 表示读到高电平。
 */
uint8_t I2C_SW_ReadBit(void)
{
  /* 定义变量保存当前读到的 bit。 */
  uint8_t bit_value;

  /* SCL 低电平期间释放 SDA，让从机能够驱动 SDA。 */
  I2C_SW_SCL_Low();
  I2C_SW_SDA_High();

  /* 给 SDA 释放和从机输出留一点建立时间。 */
  I2C_SW_Delay();

  /* 拉高 SCL，进入读数据采样窗口。 */
  I2C_SW_SCL_High();

  /* 保持 SCL 高电平一小段时间，让 SDA 电平稳定。 */
  I2C_SW_Delay();

  /* 读取 SDA 当前实际电平。 */
  bit_value = I2C_SW_SDA_Read();

  /* 拉低 SCL，结束当前 bit 的时钟周期。 */
  I2C_SW_SCL_Low();

  /* 保持 SCL 低电平一小段时间。 */
  I2C_SW_Delay();

  /* 返回读到的 bit，数值只会是 0 或 1。 */
  return bit_value;
}

/**
 * @brief 等待从机 ACK 应答。
 * @retval I2C_SW_OK 表示收到 ACK，I2C_SW_ERROR 表示没有收到 ACK。
 */
I2C_SW_Status_t I2C_SW_WaitAck(void)
{
  /* 定义状态变量，默认按未收到 ACK 处理。 */
  I2C_SW_Status_t status = I2C_SW_ERROR;

  /* 第 9 个时钟之前释放 SDA，让从机有机会拉低 SDA。 */
  I2C_SW_SDA_High();

  /* 给 SDA 释放和从机驱动留一点建立时间。 */
  I2C_SW_Delay();

  /* 拉高 SCL，进入 ACK 采样窗口。 */
  I2C_SW_SCL_High();

  /* 保持 SCL 高电平一小段时间，让 SDA 电平稳定。 */
  I2C_SW_Delay();

  /* ACK 的含义是从机在第 9 个时钟期间把 SDA 拉低。 */
  if (I2C_SW_SDA_Read() == 0U)
  {
    /* 读到 SDA 为低，表示收到 ACK。 */
    status = I2C_SW_OK;
  }

  /* 拉低 SCL，结束 ACK 时钟。 */
  I2C_SW_SCL_Low();

  /* 保持 SCL 低电平一小段时间。 */
  I2C_SW_Delay();

  /* 返回本次 ACK 采样结果。 */
  return status;
}

/**
 * @brief 主机向从机发送 ACK。
 * @retval 无
 */
void I2C_SW_Ack(void)
{
  /* SCL 低电平期间准备 ACK 数据位。 */
  I2C_SW_SCL_Low();

  /* ACK 是低电平，所以主机主动拉低 SDA。 */
  I2C_SW_SDA_Low();

  /* 保持 SDA 低电平一小段时间。 */
  I2C_SW_Delay();

  /* 拉高 SCL，把 ACK 位发送给从机。 */
  I2C_SW_SCL_High();

  /* 保持 SCL 高电平一小段时间。 */
  I2C_SW_Delay();

  /* 拉低 SCL，结束 ACK 位。 */
  I2C_SW_SCL_Low();

  /* 保持 SCL 低电平一小段时间。 */
  I2C_SW_Delay();

  /* 发送完 ACK 后释放 SDA，避免继续占用总线。 */
  I2C_SW_SDA_High();
}

/**
 * @brief 主机向从机发送 NACK。
 * @retval 无
 */
void I2C_SW_NAck(void)
{
  /* SCL 低电平期间准备 NACK 数据位。 */
  I2C_SW_SCL_Low();

  /* NACK 是高电平，所以主机释放 SDA。 */
  I2C_SW_SDA_High();

  /* 保持 SDA 高电平一小段时间。 */
  I2C_SW_Delay();

  /* 拉高 SCL，把 NACK 位发送给从机。 */
  I2C_SW_SCL_High();

  /* 保持 SCL 高电平一小段时间。 */
  I2C_SW_Delay();

  /* 拉低 SCL，结束 NACK 位。 */
  I2C_SW_SCL_Low();

  /* 保持 SCL 低电平一小段时间。 */
  I2C_SW_Delay();
}

/**
 * @brief 通过软件 I2C 写入 1 个字节，并等待从机 ACK。
 * @param data 要发送的字节，按高位在前的顺序发送。
 * @retval I2C_SW_OK 表示收到 ACK，I2C_SW_ERROR 表示没有收到 ACK。
 */
I2C_SW_Status_t I2C_SW_WriteByte(uint8_t data)
{
  /* 一共发送 8 位，从 bit7 一直发送到 bit0。 */
  for (uint8_t bit_index = 0U; bit_index < 8U; bit_index++)
  {
    /* 发送当前最高位。 */
    I2C_SW_WriteBit((uint8_t)(data & 0x80U));

    /* 左移 1 位，把下一位移动到最高位位置。 */
    data <<= 1U;
  }

  /* 8 位发送完成后读取第 9 个时钟上的 ACK。 */
  return I2C_SW_WaitAck();
}

/**
 * @brief 通过软件 I2C 读取 1 个字节，并按参数发送 ACK 或 NACK。
 * @param ack 1 表示读完后发送 ACK，0 表示读完后发送 NACK。
 * @retval 读取到的 8 位数据。
 */
uint8_t I2C_SW_ReadByte(uint8_t ack)
{
  /* 定义变量保存最终读到的字节。 */
  uint8_t data = 0U;

  /* 一共读取 8 位，从 bit7 一直读取到 bit0。 */
  for (uint8_t bit_index = 0U; bit_index < 8U; bit_index++)
  {
    /* 先左移 1 位，给即将读到的新 bit 腾出最低位位置。 */
    data <<= 1U;

    /* 读取 1 个 bit，并合并到 data 的最低位。 */
    data |= I2C_SW_ReadBit();
  }

  /* 判断调用者是否希望继续读取后续字节。 */
  if (ack != 0U)
  {
    /* 发送 ACK，告诉从机主机还要继续读取。 */
    I2C_SW_Ack();
  }
  else
  {
    /* 发送 NACK，告诉从机这是最后一个字节。 */
    I2C_SW_NAck();
  }

  /* 返回读到的完整字节。 */
  return data;
}

/**
 * @brief 向 7 位地址的 I2C 设备连续写入数据。
 * @param dev_addr_7bit 7 位设备地址，例如 AHT20 为 0x38。
 * @param data 指向待发送数据缓冲区的指针。
 * @param len 要发送的数据字节数量。
 * @retval I2C_SW_OK 表示写入流程收到所有 ACK，I2C_SW_ERROR 表示中途没有 ACK 或参数错误。
 */
I2C_SW_Status_t I2C_SW_Write(uint8_t dev_addr_7bit, const uint8_t *data, uint16_t len)
{
  /* 如果要发送的数据长度不为 0，则数据指针必须有效。 */
  if ((data == (const uint8_t *)0) && (len > 0U))
  {
    /* 参数无效，直接返回错误，避免后面访问空指针。 */
    return I2C_SW_ERROR;
  }

  /* 产生 START 信号，开始一次 I2C 写事务。 */
  I2C_SW_Start();

  /* 发送 7 位设备地址左移 1 位后的写地址，最低位 0 表示写方向。 */
  if (I2C_SW_WriteByte((uint8_t)(dev_addr_7bit << 1U)) != I2C_SW_OK)
  {
    /* 地址阶段没有收到 ACK，先发送 STOP 释放总线。 */
    I2C_SW_Stop();

    /* 返回错误给上层设备驱动。 */
    return I2C_SW_ERROR;
  }

  /* 按顺序发送数据缓冲区中的每一个字节。 */
  for (uint16_t index = 0U; index < len; index++)
  {
    /* 发送当前数据字节，并检查从机是否 ACK。 */
    if (I2C_SW_WriteByte(data[index]) != I2C_SW_OK)
    {
      /* 数据阶段没有收到 ACK，先发送 STOP 释放总线。 */
      I2C_SW_Stop();

      /* 返回错误给上层设备驱动。 */
      return I2C_SW_ERROR;
    }
  }

  /* 所有字节都发送并收到 ACK 后，发送 STOP 结束事务。 */
  I2C_SW_Stop();

  /* 返回成功状态。 */
  return I2C_SW_OK;
}

/**
 * @brief 从 7 位地址的 I2C 设备连续读取数据。
 * @param dev_addr_7bit 7 位设备地址，例如 AHT20 为 0x38。
 * @param data 指向接收数据缓冲区的指针。
 * @param len 要读取的数据字节数量。
 * @retval I2C_SW_OK 表示读取流程完成，I2C_SW_ERROR 表示地址阶段没有 ACK 或参数错误。
 */
I2C_SW_Status_t I2C_SW_Read(uint8_t dev_addr_7bit, uint8_t *data, uint16_t len)
{
  /* 如果读取长度为 0，不需要访问总线，直接按成功处理。 */
  if (len == 0U)
  {
    /* 0 长度读取没有实际事务，也不会产生 ACK 错误。 */
    return I2C_SW_OK;
  }

  /* 读取长度不为 0 时，接收缓冲区指针必须有效。 */
  if (data == (uint8_t *)0)
  {
    /* 参数无效，直接返回错误，避免后面访问空指针。 */
    return I2C_SW_ERROR;
  }

  /* 产生 START 信号，开始一次 I2C 读事务。 */
  I2C_SW_Start();

  /* 发送 7 位设备地址左移 1 位后的读地址，最低位 1 表示读方向。 */
  if (I2C_SW_WriteByte((uint8_t)((dev_addr_7bit << 1U) | 0x01U)) != I2C_SW_OK)
  {
    /* 地址阶段没有收到 ACK，先发送 STOP 释放总线。 */
    I2C_SW_Stop();

    /* 返回错误给上层设备驱动。 */
    return I2C_SW_ERROR;
  }

  /* 按顺序读取指定数量的数据字节。 */
  for (uint16_t index = 0U; index < len; index++)
  {
    /* 前 len-1 个字节读完发送 ACK，最后 1 个字节读完发送 NACK。 */
    uint8_t ack = (uint8_t)((index + 1U) < len);

    /* 读取当前字节，并按 ack 参数自动发送 ACK 或 NACK。 */
    data[index] = I2C_SW_ReadByte(ack);
  }

  /* 所有字节读取完成后，发送 STOP 结束事务。 */
  I2C_SW_Stop();

  /* 返回成功状态。 */
  return I2C_SW_OK;
}
