#include "aht20.h"

/**
 * @brief 初始化 AHT20 温湿度传感器。
 * @retval AHT20_OK 表示初始化成功，AHT20_ERROR 表示初始化失败。
 */
AHT20_Status_t AHT20_Init(void)
{
  /* 定义变量保存 AHT20 的状态字。 */
  uint8_t status = 0U;

  /* 初始化上一层软件 I2C 使用的 GPIO。 */
  I2C_SW_GPIO_Init();

  /* AHT20 上电后需要等待一段时间，保证芯片内部稳定。 */
  HAL_Delay(40U);

  /* 读取 AHT20 当前状态字。 */
  if (AHT20_ReadStatus(&status) != AHT20_OK)
  {
    /* 状态字读取失败，说明当前总线通信不正常。 */
    return AHT20_ERROR;
  }

  /* 判断状态字中的校准使能位是否已经置 1。 */
  if (AHT20_IsCalibrated(status) == 0U)
  {
    /* AHT20 初始化命令固定为 0xBE 0x08 0x00。 */
    uint8_t init_cmd[3] = {
      AHT20_CMD_INIT,
      AHT20_INIT_PARAM_1,
      AHT20_INIT_PARAM_2
    };

    /* 通过软件 I2C 发送初始化命令。 */
    if (I2C_SW_Write(AHT20_I2C_ADDR, init_cmd, 3U) != I2C_SW_OK)
    {
      /* 初始化命令发送失败，直接返回错误。 */
      return AHT20_ERROR;
    }

    /* 发送初始化命令后等待 AHT20 内部完成校准准备。 */
    HAL_Delay(10U);

    /* 再次读取状态字，用来确认校准使能位是否已经置 1。 */
    if (AHT20_ReadStatus(&status) != AHT20_OK)
    {
      /* 第二次状态字读取失败，返回错误。 */
      return AHT20_ERROR;
    }

    /* 如果校准使能位仍然不是 1，则认为初始化失败。 */
    if (AHT20_IsCalibrated(status) == 0U)
    {
      /* 校准使能位没有置位，返回错误给上层。 */
      return AHT20_ERROR;
    }
  }

  /* 状态检查通过，AHT20 初始化成功。 */
  return AHT20_OK;
}

/**
 * @brief 读取 AHT20 状态字。
 * @param status 指向状态字存储变量的指针。
 * @retval AHT20_OK 表示读取成功，AHT20_ERROR 表示读取失败或参数错误。
 */
AHT20_Status_t AHT20_ReadStatus(uint8_t *status)
{
  /* 定义状态读取命令。 */
  uint8_t cmd = AHT20_CMD_STATUS;

  /* 参数指针不能为空，否则后面无法保存读取结果。 */
  if (status == (uint8_t *)0)
  {
    /* 传入空指针，直接返回错误。 */
    return AHT20_ERROR;
  }

  /* 先向 AHT20 发送状态读取命令 0x71。 */
  if (I2C_SW_Write(AHT20_I2C_ADDR, &cmd, 1U) != I2C_SW_OK)
  {
    /* 命令发送失败，返回错误。 */
    return AHT20_ERROR;
  }

  /* 再从 AHT20 读取 1 个字节状态数据。 */
  if (I2C_SW_Read(AHT20_I2C_ADDR, status, 1U) != I2C_SW_OK)
  {
    /* 状态数据读取失败，返回错误。 */
    return AHT20_ERROR;
  }

  /* 命令发送和状态读取都成功。 */
  return AHT20_OK;
}

/**
 * @brief 判断 AHT20 是否处于忙状态。
 * @param status AHT20 状态字。
 * @retval 1 表示正在测量，0 表示空闲。
 */
uint8_t AHT20_IsBusy(uint8_t status)
{
  /* Busy 位是状态字 bit7，掩码为 0x80。 */
  if ((status & AHT20_STATUS_BUSY_MASK) != 0U)
  {
    /* Busy 位为 1，表示 AHT20 正在测量或内部处理。 */
    return 1U;
  }

  /* Busy 位为 0，表示 AHT20 当前空闲。 */
  return 0U;
}

/**
 * @brief 判断 AHT20 是否已经校准使能。
 * @param status AHT20 状态字。
 * @retval 1 表示已校准，0 表示未校准。
 */
uint8_t AHT20_IsCalibrated(uint8_t status)
{
  /* Calibration Enable 位是状态字 bit3，掩码为 0x08。 */
  if ((status & AHT20_STATUS_CAL_MASK) != 0U)
  {
    /* 校准使能位为 1，表示 AHT20 已经处于可正常测量状态。 */
    return 1U;
  }

  /* 校准使能位为 0，表示还需要发送初始化命令。 */
  return 0U;
}

/**
 * @brief 对 AHT20 发送软件复位命令。
 * @retval AHT20_OK 表示命令发送成功，AHT20_ERROR 表示命令发送失败。
 */
AHT20_Status_t AHT20_SoftReset(void)
{
  /* AHT20 软件复位命令为 0xBA。 */
  uint8_t cmd = AHT20_CMD_SOFT_RESET;

  /* 通过软件 I2C 发送软件复位命令。 */
  if (I2C_SW_Write(AHT20_I2C_ADDR, &cmd, 1U) != I2C_SW_OK)
  {
    /* 命令发送失败，返回错误。 */
    return AHT20_ERROR;
  }

  /* 软件复位后等待 AHT20 内部重新启动。 */
  HAL_Delay(20U);

  /* 命令发送成功，并且等待完成。 */
  return AHT20_OK;
}

/**
 * @brief 触发 AHT20 进行一次温湿度测量。
 * @retval AHT20_OK 表示命令发送成功，AHT20_ERROR 表示命令发送失败。
 */
AHT20_Status_t AHT20_TriggerMeasurement(void)
{
  /* AHT20 触发测量命令固定为 0xAC 0x33 0x00。 */
  uint8_t trigger_cmd[3] = {
    AHT20_CMD_TRIGGER,
    AHT20_TRIGGER_PARAM_1,
    AHT20_TRIGGER_PARAM_2
  };

  /* 通过软件 I2C 发送触发测量命令。 */
  if (I2C_SW_Write(AHT20_I2C_ADDR, trigger_cmd, 3U) != I2C_SW_OK)
  {
    /* 命令发送失败，返回错误。 */
    return AHT20_ERROR;
  }

  /* 命令发送成功。 */
  return AHT20_OK;
}

/**
 * @brief 触发测量并读取 AHT20 的 6 字节原始数据。
 * @param raw 指向 6 字节原始数据缓冲区的指针。
 * @retval AHT20_OK 表示读取成功，AHT20_ERROR 表示读取失败、忙状态或参数错误。
 */
AHT20_Status_t AHT20_ReadRaw(uint8_t raw[6])
{
  /* 原始数据缓冲区不能为空，否则无法保存 6 字节测量结果。 */
  if (raw == (uint8_t *)0)
  {
    /* 传入空指针，直接返回错误。 */
    return AHT20_ERROR;
  }

  /* 先发送触发测量命令。 */
  if (AHT20_TriggerMeasurement() != AHT20_OK)
  {
    /* 触发测量失败，直接返回错误。 */
    return AHT20_ERROR;
  }

  /* 第一版使用固定 80 ms 等待，给 AHT20 留足测量时间。 */
  HAL_Delay(80U);

  /* 从 AHT20 连续读取 6 字节原始数据。 */
  if (I2C_SW_Read(AHT20_I2C_ADDR, raw, 6U) != I2C_SW_OK)
  {
    /* 原始数据读取失败，返回错误。 */
    return AHT20_ERROR;
  }

  /* raw[0] 是状态字，读完后再次检查 Busy 位。 */
  if (AHT20_IsBusy(raw[0]) != 0U)
  {
    /* Busy 位仍然为 1，说明数据还没有准备好。 */
    return AHT20_ERROR;
  }

  /* 6 字节数据读取完成，并且状态字显示数据可用。 */
  return AHT20_OK;
}

/**
 * @brief 解析 AHT20 的 6 字节原始数据为温度和湿度。
 * @param raw 指向 6 字节原始数据缓冲区的指针。
 * @param data 指向温湿度结果结构体的指针。
 * @retval AHT20_OK 表示解析成功，AHT20_ERROR 表示参数错误。
 */
AHT20_Status_t AHT20_ParseRaw(const uint8_t raw[6], AHT20_Data_t *data)
{
  /* 定义变量保存 20 bit 湿度原始值。 */
  uint32_t humidity_raw;

  /* 定义变量保存 20 bit 温度原始值。 */
  uint32_t temperature_raw;

  /* 原始数据指针不能为空。 */
  if (raw == (const uint8_t *)0)
  {
    /* 传入空指针，直接返回错误。 */
    return AHT20_ERROR;
  }

  /* 结果结构体指针不能为空。 */
  if (data == (AHT20_Data_t *)0)
  {
    /* 传入空指针，直接返回错误。 */
    return AHT20_ERROR;
  }

  /* 湿度原始值由 raw[1]、raw[2] 和 raw[3] 高 4 位组成。 */
  humidity_raw =
    ((uint32_t)raw[1] << 12) |
    ((uint32_t)raw[2] << 4) |
    ((uint32_t)(raw[3] >> 4));

  /* 温度原始值由 raw[3] 低 4 位、raw[4] 和 raw[5] 组成。 */
  temperature_raw =
    (((uint32_t)raw[3] & 0x0FU) << 16) |
    ((uint32_t)raw[4] << 8) |
    ((uint32_t)raw[5]);

  /* 1048576.0f 等于 2^20，湿度换算结果单位为 %RH。 */
  data->humidity = ((float)humidity_raw / 1048576.0f) * 100.0f;

  /* 1048576.0f 等于 2^20，温度换算结果单位为摄氏度。 */
  data->temperature = ((float)temperature_raw / 1048576.0f) * 200.0f - 50.0f;

  /* 原始数据解析完成。 */
  return AHT20_OK;
}

/**
 * @brief 读取并解析 AHT20 温湿度数据。
 * @param data 指向温湿度结果结构体的指针。
 * @retval AHT20_OK 表示读取成功，AHT20_ERROR 表示读取失败或参数错误。
 */
AHT20_Status_t AHT20_ReadData(AHT20_Data_t *data)
{
  /* 定义 6 字节缓冲区保存 AHT20 原始数据。 */
  uint8_t raw[6] = {0U};

  /* 结果结构体指针不能为空。 */
  if (data == (AHT20_Data_t *)0)
  {
    /* 传入空指针，直接返回错误。 */
    return AHT20_ERROR;
  }

  /* 先读取 AHT20 原始数据。 */
  if (AHT20_ReadRaw(raw) != AHT20_OK)
  {
    /* 原始数据读取失败，返回错误。 */
    return AHT20_ERROR;
  }

  /* 再把原始数据解析为温度和湿度。 */
  if (AHT20_ParseRaw(raw, data) != AHT20_OK)
  {
    /* 解析失败，返回错误。 */
    return AHT20_ERROR;
  }

  /* 读取和解析都成功。 */
  return AHT20_OK;
}

/**
 * @brief 测试读取一次 AHT20 温湿度，并拆成两个 float 输出。
 * @param temperature 指向温度输出变量的指针，单位为摄氏度。
 * @param humidity 指向湿度输出变量的指针，单位为 %RH。
 * @retval AHT20_OK 表示读取成功，AHT20_ERROR 表示读取失败或参数错误。
 */
AHT20_Status_t AHT20_TestRead(float *temperature, float *humidity)
{
  /* 定义结构体保存一次完整的温湿度读取结果。 */
  AHT20_Data_t data = {0};

  /* 温度输出指针不能为空。 */
  if (temperature == (float *)0)
  {
    /* 传入空指针，直接返回错误。 */
    return AHT20_ERROR;
  }

  /* 湿度输出指针不能为空。 */
  if (humidity == (float *)0)
  {
    /* 传入空指针，直接返回错误。 */
    return AHT20_ERROR;
  }

  /* 调用标准读取接口，获取一次温湿度数据。 */
  if (AHT20_ReadData(&data) != AHT20_OK)
  {
    /* 读取失败，返回错误。 */
    return AHT20_ERROR;
  }

  /* 把结构体中的温度复制到调用者传入的温度变量。 */
  *temperature = data.temperature;

  /* 把结构体中的湿度复制到调用者传入的湿度变量。 */
  *humidity = data.humidity;

  /* 温湿度读取和输出都成功。 */
  return AHT20_OK;
}
