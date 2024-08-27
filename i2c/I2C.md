**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息

# I2C Master 编程指南 (旧版API) 
© 2024 . 未经许可不得复制、修改或分发。 此文献为 [小風的藏書閣](https://t.me/xfp2333)  所有。

- 由于乐鑫官方更新了iic的API,[点击此处查看新版API教程](i2c_new.md)
**IIC分为主机和从机模式**

 - 此文档为主机模式，若要参考从机模式，请跳转 [IIC 从机模式](/i2c/I2C0.md)
 - ESP32 I2C 端口可配置，一般支持两路I2C的使用
 - I2C的时钟频率可选，有100khz和400khz

# 编程实现

## 环境配置

1. 包含以下头文件

```c
#include "driver/i2c.h"
#include "sdkconfig.h"
```

2.cmake配置

```c
REQUIRES  driver
```

## 初始化

1. 配置驱动程序

```c
{
    int i2c_master_port = I2C_MASTER_NUM;//I2C总线号

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,        //仅限主机模式
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,//启用上拉电阻
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,//时钟频率，有100k和400k可选
        .clk_flags = 0,                       //可选项，可以使用I2C_SCLK_SRC_FLAG_* 标志选择I2C时钟源
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
 * @param data 发送的数据缓冲区 可以是数组
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
//调用示例：
          i2c_master_write_to_device(I2C_MASTER_NUM, MPU9250_SENSOR_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
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

//使用示例：
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1  ·                          /*!< I2C nack value */
i2c_cmd_handle_t cmd = i2c_cmd_link_create();
i2c_master_start(cmd);
//最后一个参数ACK信号，如果读取到的数据长度大于1 ,表示读取到了有效值，那么就发送低电平(0x00)表示读取到了信号，反之发送高电平0x01表示有误
 i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
```

## IIC其他函数原型

[IIC API函数](i2c_function.h)
[IIC 类型定义](i2c_type.h)

## 应用示例项目
[OLED12864显示](OLED12864_4PIN/OLED12864_4PIN_example.c)
[MPU6050数据读取](MPU6050/main.c)

## 常用IIC设备数据手册
- [oled12864](/PDF/OLED12864.pdf)
- [MPU6050](/PDF/MPU6050.pdf)
- [VL53L0X](/PDF/VL53L0X.pdf)
- [LM75](/PDF/LM75.pdf)