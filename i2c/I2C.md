**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息

© 2024 . 未经许可不得复制、修改或分发。 此文献为 [小風的藏書閣](https://t.me/xfp2333) 所有。

**IIC分为主机和从机模式**

 - 此文档为主机模式，若要参考从机模式，请跳转 [IIC 从机模式](/i2c/I2C0.md)

# 编程实现

## 环境配置

1. 包含以下头文件

```c
#include "driver/i2c.h"
```

2.cmake配置

```c
REQUIRES  driver
```

## 初始化

```c
{
    int i2c_master_port = I2C_MASTER_NUM;//I2C总线号

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,//时钟频率，有100k和400k可选
    };

    i2c_param_config(i2c_master_port, &conf);//配置i2c总线

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);//i2c设备安装
}
```

## 向总线发送数据
```c
/**
 * @brief 向连接到特定I2C端口的设备写入数据。
 *        此函数是`i2c_master_start()`、`i2c_master_write_byte()`...、`i2c_master_stop()`函数的便捷缩写。
 *
 * @param i2c_num I2C端口号
 * @param dev_addr 设备地址
 * @param data 发送的数据缓冲区
 * @param size 发送的数据长度（字节数）
 * @param ticks_to_wait 等待传输完成的最长时间（以时钟节拍为单位）#define I2C_MASTER_TIMEOUT 1000 / portTICK_PERIOD_MS
 *
 * @return
 *     - ESP_OK 成功
 *     - ESP_ERR_INVALID_ARG 参数错误
 *     - ESP_ERR_INVALID_STATE 与I2C主机相关的错误
 *     - ESP_FAIL I2C主机传输失败
 *     - ESP_ERR_TIMEOUT I2C主机超时
 */
esp_err_t i2c_master_write_to_device(i2c_port_t i2c_num, uint8_t dev_addr, uint8_t *data, size_t size, TickType_t ticks_to_wait);
```
## 向总线读数据

```c
/**
 * @brief 执行I2C主机读操作。
 *
 * @param cmd_handle I2C总线操作的命令句柄。
 * @param data 用于存储接收数据的缓冲区指针。
 * @param data_len 数据缓冲区的长度。
 * @param ack 读取数据后发送的应答类型。
 *
 * @return
 *     - ESP_OK: 成功
 *     - ESP_ERR_INVALID_ARG: 参数错误
 *     - ESP_ERR_INVALID_STATE: 无效状态
 *     - ESP_FAIL: 一般失败
 */
esp_err_t i2c_master_read(i2c_cmd_handle_t cmd_handle, uint8_t *data, size_t data_len, i2c_ack_type_t ack);

```

## 上述类型定义
```c
/**
 * @brief I2C 端口号，可以是 I2C_NUM_0 ~ (I2C_NUM_MAX-1)。
 */
typedef enum {
    I2C_NUM_0 = 0,              /*!< I2C 端口 0 */
#if SOC_I2C_NUM >= 2
    I2C_NUM_1,                  /*!< I2C 端口 1 */
#endif /* SOC_I2C_NUM >= 2 */
#if SOC_LP_I2C_NUM >= 1
    LP_I2C_NUM_0,               /*< LP_I2C 端口 0 */
#endif /* SOC_LP_I2C_NUM >= 1 */
    I2C_NUM_MAX,                /*!< 最大支持的 I2C 端口数 */
} i2c_port_t;

/**
 * @brief 用于计算 I2C 总线时序的数据结构。
 */
typedef struct {
    uint16_t clkm_div;          /*!< I2C 核心时钟分频器 */
    uint16_t scl_low;           /*!< I2C SCL 低电平周期 */
    uint16_t scl_high;          /*!< I2C SCL 高电平周期 */
    uint16_t scl_wait_high;     /*!< I2C SCL 等待高电平周期 */
    uint16_t sda_hold;          /*!< I2C SDA 保持时间 */
    uint16_t sda_sample;        /*!< I2C SDA 采样时间 */
    uint16_t setup;             /*!< I2C 启动和停止条件设置周期 */
    uint16_t hold;              /*!< I2C 启动和停止条件保持周期 */
    uint16_t tout;              /*!< I2C 总线超时周期 */
} i2c_hal_clk_config_t;

typedef enum {
#if SOC_I2C_SUPPORT_SLAVE
    I2C_MODE_SLAVE = 0,   /*!< I2C 从模式 */
#endif
    I2C_MODE_MASTER,      /*!< I2C 主模式 */
    I2C_MODE_MAX,
} i2c_mode_t;

typedef enum {
    I2C_MASTER_WRITE = 0,   /*!< I2C 写数据 */
    I2C_MASTER_READ,        /*!< I2C 读数据 */
} i2c_rw_t;

typedef enum {
    I2C_DATA_MODE_MSB_FIRST = 0,  /*!< I2C 数据高位优先 */
    I2C_DATA_MODE_LSB_FIRST = 1,  /*!< I2C 数据低位优先 */
    I2C_DATA_MODE_MAX
} i2c_trans_mode_t;

typedef enum {
    I2C_ADDR_BIT_7 = 0,    /*!< I2C 从模式下的 7 位地址 */
    I2C_ADDR_BIT_10,       /*!< I2C 从模式下的 10 位地址 */
    I2C_ADDR_BIT_MAX,
} i2c_addr_mode_t;

typedef enum {
    I2C_MASTER_ACK = 0x0,        /*!< I2C 每个字节读取时的应答 */
    I2C_MASTER_NACK = 0x1,       /*!< I2C 每个字节读取时的非应答 */
    I2C_MASTER_LAST_NACK = 0x2,   /*!< I2C 最后一个字节的非应答 */
    I2C_MASTER_ACK_MAX,
} i2c_ack_type_t;

/**
 * @brief 时序配置结构体。用于内部 I2C 复位。
 */
typedef struct {
    int high_period; /*!< 高电平时间 */
    int low_period; /*!< 低电平时间 */
    int wait_high_period; /*!< 等待高电平时间 */
    int rstart_setup; /*!< 重启设置时间 */
    int start_hold; /*!< 启动保持时间 */
    int stop_setup; /*!< 停止设置时间 */
    int stop_hold; /*!< 停止保持时间 */
    int sda_sample; /*!< SDA 采样时间 */
    int sda_hold; /*!< SDA 保持时间 */
    int timeout; /*!< 超时值 */
} i2c_hal_timing_config_t;
```
## IIC其他函数原型

[其他IIC API函数](i2c_function.h)

## 应用示例项目
[OLED12864显示](OLED12864_4PIN/OLED12864_4PIN_example.c)
