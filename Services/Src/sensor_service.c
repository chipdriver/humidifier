#include "sensor_service.h"

static SensorService_Data_t s_sensor_data = {0};
static uint8_t s_sensor_initialized = 0U;
static uint32_t s_last_update_tick = 0U;

/**
 * @brief 初始化温湿度业务服务。
 * @retval SENSOR_SERVICE_OK 表示初始化成功，SENSOR_SERVICE_ERROR 表示初始化失败。
 */
SensorService_Status_t SensorService_Init(void)
{
  /* 初始化前先清空缓存温度。 */
  s_sensor_data.temperature = 0.0f;

  /* 初始化前先清空缓存湿度。 */
  s_sensor_data.humidity = 0.0f;

  /* 初始化前默认认为当前缓存数据无效。 */
  s_sensor_data.valid = 0U;

  /* 初始化前默认认为服务还没有成功初始化。 */
  s_sensor_initialized = 0U;

  /* 初始化前清空最近一次更新时间。 */
  s_last_update_tick = 0U;

  /* 调用设备驱动层初始化 AHT20。 */
  if (AHT20_Init() != AHT20_OK)
  {
    /* AHT20 初始化失败，服务层也返回初始化失败。 */
    return SENSOR_SERVICE_ERROR;
  }

  /* AHT20 初始化成功后，记录服务已经初始化。 */
  s_sensor_initialized = 1U;

  /* 服务初始化成功。 */
  return SENSOR_SERVICE_OK;
}

/**
 * @brief 立即读取一次 AHT20 温湿度并更新服务缓存。
 * @retval SENSOR_SERVICE_OK 表示读取成功，其他状态表示未初始化或读取失败。
 */
SensorService_Status_t SensorService_Update(void)
{
  /* 定义设备驱动层的数据结构，用来接收 AHT20 读取结果。 */
  AHT20_Data_t aht20_data = {0};

  /* 如果服务还没有初始化成功，不允许直接读取。 */
  if (s_sensor_initialized == 0U)
  {
    /* 返回未初始化状态，提醒上层先调用 SensorService_Init()。 */
    return SENSOR_SERVICE_NOT_INIT;
  }

  /* 调用 AHT20 设备驱动层读取一次温湿度数据。 */
  if (AHT20_ReadData(&aht20_data) != AHT20_OK)
  {
    /* 第一版读取失败后直接把缓存标记为无效。 */
    s_sensor_data.valid = 0U;

    /* 返回读取失败状态。 */
    return SENSOR_SERVICE_READ_FAIL;
  }

  /* 读取成功后，把最新温度保存到服务层缓存。 */
  s_sensor_data.temperature = aht20_data.temperature;

  /* 读取成功后，把最新湿度保存到服务层缓存。 */
  s_sensor_data.humidity = aht20_data.humidity;

  /* 读取成功后，把缓存数据标记为有效。 */
  s_sensor_data.valid = 1U;

  /* 本次读取和缓存更新成功。 */
  return SENSOR_SERVICE_OK;
}

/**
 * @brief 按固定周期更新温湿度数据。
 * @retval SENSOR_SERVICE_OK 表示未到时间或读取成功，其他状态表示未初始化或读取失败。
 */
SensorService_Status_t SensorService_UpdatePeriodic(void)
{
  /* 获取当前 HAL 系统毫秒计数。 */
  uint32_t current_tick = HAL_GetTick();

  /* 如果服务还没有初始化成功，不允许进入周期读取。 */
  if (s_sensor_initialized == 0U)
  {
    /* 返回未初始化状态，提醒上层先调用 SensorService_Init()。 */
    return SENSOR_SERVICE_NOT_INIT;
  }

  /* 如果距离上次读取还不到默认周期，就不访问传感器。 */
  if ((current_tick - s_last_update_tick) < SENSOR_SERVICE_UPDATE_INTERVAL_MS)
  {
    /* 未到更新时间，不读取也不算错误。 */
    return SENSOR_SERVICE_OK;
  }

  /* 时间到了，记录本次尝试读取的时间。 */
  s_last_update_tick = current_tick;

  /* 调用立即更新函数执行一次真实读取。 */
  return SensorService_Update();
}

/**
 * @brief 获取最近一次有效的温湿度缓存数据。
 * @param data 指向温湿度服务数据结构体的指针。
 * @retval SENSOR_SERVICE_OK 表示获取成功，SENSOR_SERVICE_ERROR 表示参数错误或数据无效。
 */
SensorService_Status_t SensorService_GetData(SensorService_Data_t *data)
{
  /* 输出参数不能为空。 */
  if (data == (SensorService_Data_t *)0)
  {
    /* 传入空指针，直接返回错误。 */
    return SENSOR_SERVICE_ERROR;
  }

  /* 如果当前缓存数据无效，就不把旧数据交给上层使用。 */
  if (s_sensor_data.valid == 0U)
  {
    /* 当前没有有效数据，返回错误。 */
    return SENSOR_SERVICE_ERROR;
  }

  /* 复制最近一次成功读取到的温度。 */
  data->temperature = s_sensor_data.temperature;

  /* 复制最近一次成功读取到的湿度。 */
  data->humidity = s_sensor_data.humidity;

  /* 复制当前数据有效标志。 */
  data->valid = s_sensor_data.valid;

  /* 数据复制完成。 */
  return SENSOR_SERVICE_OK;
}

/**
 * @brief 获取最近一次成功读取到的温度。
 * @retval 温度值，单位为摄氏度；没有有效数据时返回 0.0f。
 */
float SensorService_GetTemperature(void)
{
  /* 如果当前没有有效数据，第一版直接返回 0.0f。 */
  if (s_sensor_data.valid == 0U)
  {
    /* 没有有效温度缓存。 */
    return 0.0f;
  }

  /* 返回最近一次成功读取到的温度。 */
  return s_sensor_data.temperature;
}

/**
 * @brief 获取最近一次成功读取到的湿度。
 * @retval 湿度值，单位为 %RH；没有有效数据时返回 0.0f。
 */
float SensorService_GetHumidity(void)
{
  /* 如果当前没有有效数据，第一版直接返回 0.0f。 */
  if (s_sensor_data.valid == 0U)
  {
    /* 没有有效湿度缓存。 */
    return 0.0f;
  }

  /* 返回最近一次成功读取到的湿度。 */
  return s_sensor_data.humidity;
}

/**
 * @brief 判断当前温湿度缓存数据是否有效。
 * @retval 1 表示有效，0 表示无效。
 */
uint8_t SensorService_IsDataValid(void)
{
  /* 当前缓存有效时返回 1。 */
  if (s_sensor_data.valid != 0U)
  {
    /* 数据有效。 */
    return 1U;
  }

  /* 当前缓存无效。 */
  return 0U;
}

/**
 * @brief 判断温湿度业务服务是否初始化成功。
 * @retval 1 表示已经初始化成功，0 表示未初始化或初始化失败。
 */
uint8_t SensorService_IsInitialized(void)
{
  /* 初始化成功标志为非 0 时返回 1。 */
  if (s_sensor_initialized != 0U)
  {
    /* 服务已经初始化成功。 */
    return 1U;
  }

  /* 服务未初始化或初始化失败。 */
  return 0U;
}

/**
 * @brief 测试读取一次温湿度，并拆成两个 float 输出。
 * @param temperature 指向温度输出变量的指针，单位为摄氏度。
 * @param humidity 指向湿度输出变量的指针，单位为 %RH。
 * @retval SENSOR_SERVICE_OK 表示读取成功，其他状态表示参数错误、未初始化或读取失败。
 */
SensorService_Status_t SensorService_TestRead(float *temperature, float *humidity)
{
  /* 定义变量保存本次读取状态。 */
  SensorService_Status_t status;

  /* 温度输出参数不能为空。 */
  if (temperature == (float *)0)
  {
    /* 传入空指针，直接返回错误。 */
    return SENSOR_SERVICE_ERROR;
  }

  /* 湿度输出参数不能为空。 */
  if (humidity == (float *)0)
  {
    /* 传入空指针，直接返回错误。 */
    return SENSOR_SERVICE_ERROR;
  }

  /* 调用服务层立即更新接口读取一次温湿度。 */
  status = SensorService_Update();

  /* 如果读取没有成功，就直接把状态返回给调用者。 */
  if (status != SENSOR_SERVICE_OK)
  {
    /* 返回实际失败原因，例如未初始化或读取失败。 */
    return status;
  }

  /* 读取成功后，把当前缓存温度输出给调用者。 */
  *temperature = s_sensor_data.temperature;

  /* 读取成功后，把当前缓存湿度输出给调用者。 */
  *humidity = s_sensor_data.humidity;

  /* 测试读取成功。 */
  return SENSOR_SERVICE_OK;
}
