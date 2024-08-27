**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息

© 2024 . 未经许可不得复制、修改或分发。 此文献为 [小風的藏書閣](https://t.me/xfp2333)  所有。

# I2C Master 编程指南 (新版API) 

# 简介
- I2C 是一种串行、同步、多设备、半双工通信协议，允许多个主从设备在同一总线上共存。I2C 使用两条双向开漏线：串行数据线 （SDA） 和串行时钟线 （SCL），由电阻器上拉。
- ESP32-S3 有 2 个 I2C 控制器（也称为端口），负责处理 I2C 总线上的通信。单个 I2C 控制器可以是主控制器，也可以是从控制器。
- I2C 从设备具有 7 位地址或 10 位地址。ESP32-S3 支持 I2C 标准模式 （Sm） 和快速模式 （Fm），最高可达 **100KHz** 和 **400KHz**。
- 主模式下 SCL 的时钟频率不应大于 **400 KHz**
- ***SCL的频率受上拉电阻和导线电容的影响。因此，强烈建议用户选择合适的上拉电阻，使频率准确。上拉电阻的推荐值通常在 1K 欧姆到 10K 欧姆之间。***
- ***上拉电阻应越小（但不小于 1 KOhms）。事实上，大电阻器会降低电流，从而增加时钟开关时间并降低频率。我们通常建议 2 KOhms 到 5 KOhms 的范围***

# 环境配置
- 包含下列头文件
```c
#include "driver/i2c_master.h"
```
- CMAKE配置
```c
REQUIRES driver
```
# IIC 资源分配
- I2C 主总线和 I2C 从总线（如果支持）都由 ***i2c_bus_handle_t*** 驱动程序表示。可用端口在资源池中进行管理，该资源池根据请求分配空闲端口。
- I2C 主站基于总线设备模型设计。因此 ***i2c_master_bus_config_t*** ， ***i2c_device_config_t*** 需要分别分配 I2C 主总线实例和 I2C 设备实例。

1. 主总线类型定义：
```c
//句柄类型
i2c_master_bus_handle_t;
//I2C 主总线需要以下指定的 i2c_master_bus_config_t 配置：
typedef struct {
    i2c_port_num_t i2c_port;              /*!< I2C 端口号，-1 表示自动选择 */
    gpio_num_t sda_io_num;                /*!< I2C SDA 信号的 GPIO 号，内部上拉 */
    gpio_num_t scl_io_num;                /*!< I2C SCL 信号的 GPIO 号，内部上拉 */
    i2c_clock_source_t clk_source;        /*!< I2C 主总线的时钟源，同组的通道必须使用相同的时钟源 */
    uint8_t glitch_ignore_cnt;            /*!< 线路上的毛刺周期小于该值时，可以被滤除，典型值为 7（单位：I2C 模块时钟周期）*/
    int intr_priority;                    /*!< I2C 中断优先级，如果设置为 0，驱动程序将选择默认优先级（1、2、3） */
    size_t trans_queue_depth;             /*!< 内部传输队列的深度，增加此值可以支持在后台挂起更多传输，仅在异步传输中有效。（通常为 max_device_num * 每次传输）*/
    struct {
        uint32_t enable_internal_pullup:1;   /*!< 启用内部上拉。注意：这不足以在高速频率下拉起总线。如果可能，建议使用适当的外部上拉 */
    } flags;                              /*!< I2C 主配置标志 */
} i2c_master_bus_config_t;

//时钟源枚举类型:
typedef enum {
    I2C_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,
    I2C_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST,
    I2C_CLK_SRC_DEFAULT = SOC_MOD_CLK_XTAL,
} soc_periph_i2c_clk_src_t;

/**
 * @brief 配置iic 总线
 * @param 总线配置结构地址
 * @param 总线配置句柄
 * @return ESP调试信息
 */
esp_err_t i2c_new_master_bus(const i2c_master_bus_config_t *bus_config, i2c_master_bus_handle_t *ret_bus_handle);
```

- **如果控制器时钟源被选为 I2C_CLK_SRC_XTAL ，则驱动程序不会为其安装电源管理锁，只要源时钟仍能提供足够的分辨率，它就更适合低功耗应用。**

2. 从总线类型定义
```c
//句柄类型：
i2c_master_dev_handle_t

typedef struct {
    i2c_addr_bit_len_t dev_addr_length;         /*!< 选择从设备的地址长度。 */
    uint16_t device_address;                    /*!< I2C 设备的原始地址。（不带读/写位的 7/10 位地址） */
    uint32_t scl_speed_hz;                      /*!< I2C SCL 线频率。 */
    uint32_t scl_wait_us;                       /*!< 超时时间值。（单位：微秒）。请注意，此值不应太小，以便能够正确处理拉伸/干扰。如果设置为 0，则表示使用默认寄存器值。*/
    struct {
        uint32_t disable_ack_check: 1;          /*!< 禁用 ACK 检查。如果设置为 false，则表示启用 ACK 检查，当检测到 NACK 时，事务将停止并且 API 返回错误。 */
    } flags;                                    /*!< I2C 设备配置标志 */
} i2c_device_config_t;

typedef enum {
    I2C_ADDR_BIT_LEN_7 = 0,       /*!< I2C 地址长度为 7 位 */
#if SOC_I2C_SUPPORT_10BIT_ADDR
    I2C_ADDR_BIT_LEN_10 = 1,      /*!< I2C 地址长度为 10 位 */
#endif
} i2c_addr_bit_len_t;

/**
 * @brief 分配 I2C 设备实例并挂载到主总线
 * @param 主总线句柄
 * @param 从设备配置结构地址
 * @param 从设备句柄地址
 * @return ESP调试信息
 */
esp_err_t i2c_master_bus_add_device(i2c_master_bus_handle_t bus_handle, const i2c_device_config_t *dev_config, i2c_master_dev_handle_t *ret_handle);
```

3. 上述描述代码示例：

```c
#include "driver/i2c_master.h"

i2c_master_bus_config_t i2c_mst_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = TEST_I2C_PORT,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
};

i2c_master_bus_handle_t bus_handle;
ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

i2c_device_config_t dev_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = 0x58,
    .scl_speed_hz = 100000,
};

i2c_master_dev_handle_t dev_handle;
ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
```

# 卸载 I2C 主总线和设备

- 如果不再需要以前安装的 I2C 总线或设备，建议通过调用 i2c_master_bus_rm_device() 或 i2c_del_master_bus() 来回收资源，以便释放底层硬件。

# 安装 I2C 从设备
- 一般我们不把i2c当从机，所以此处不做关于i2c从机的教程，若想了解，请[查看此文件](iic_slv.md)

# I2C 主写入

- 成功安装 I2C 主总线后，您只需调用 i2c_master_transmit() 以将数据写入从设备即可;
```c
/**
 * @brief 在 I2C 总线上执行写入事务。事务将一直进行，直到它完成或达到提供的超时\
 * @param 由创建的 i2c_master_bus_add_device I2C 主设备句柄。
 * @param 要在 I2C 总线上发送的数据字节
 * @param 写入缓冲区的大小（以字节为单位）
 * @param 等待超时，以毫秒为单位。注意：-1 表示永远等待。
 * @return ESP调试信息
 */
esp_err_t i2c_master_transmit(i2c_master_dev_handle_t i2c_dev, const uint8_t *write_buffer, size_t write_size, int xfer_timeout_ms);
```
- 数据写入从服务器的简单示例：
```c
#define DATA_LENGTH 100
i2c_master_bus_config_t i2c_mst_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = I2C_PORT_NUM_0,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .glitch_ignore_cnt = 7,
};
i2c_master_bus_handle_t bus_handle;

ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

i2c_device_config_t dev_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = 0x58,
    .scl_speed_hz = 100000,
};

i2c_master_dev_handle_t dev_handle;
ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, data_wr, DATA_LENGTH, -1));
```

# I2C主机读取
- 调用 i2c_master_receive() 从设备读取数据即可。

```c
//读取设备函数原型:

/**
 * @param 在 I2C 总线上执行读取事务。事务将一直进行，直到它完成或达到提供的超时。
 * @param 主机句柄
 * @param 从 i2c 总线接收的数据字节
 * @param 读取缓冲区的大小（以字节为单位）。
 * @param 等待超时，以毫秒为单位。注意：-1 表示永远等待
 */
esp_err_t i2c_master_receive(i2c_master_dev_handle_t i2c_dev, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms);
```

- 从从服务器读取数据的简单示例：

```c
#define DATA_LENGTH 100
i2c_master_bus_config_t i2c_mst_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = I2C_PORT_NUM_0,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .glitch_ignore_cnt = 7,
};
i2c_master_bus_handle_t bus_handle;

ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

i2c_device_config_t dev_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = 0x58,
    .scl_speed_hz = 100000,
};

i2c_master_dev_handle_t dev_handle;
ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

i2c_master_receive(dev_handle, data_rd, DATA_LENGTH, -1);
```

# I2C 主机写入和读取 
- 一些 I2C 设备在从中读取数据之前需要写入配置，因此，称为 i2c_master_transmit_receive() 接口可以提供帮助

- 函数原型：
```c
/**
 * @brief 在 I2C 总线上执行写读事务。事务将一直进行，直到它完成或达到提供的超时
 * @param  I2C 主设备句柄。
 * @param  要在 I2C 总线上发送的数据字节。
 * @param 写入缓冲区的大小（以字节为单位）
 * @param 从 i2c 总线接收的数据字节。
 * @param  读取缓冲区的大小（以字节为单位）。
 * @param 等待超时，以毫秒为单位。注意：-1 表示永远等待
 * @return ESP调试信息
 */
esp_err_t i2c_master_transmit_receive(i2c_master_dev_handle_t i2c_dev, const uint8_t *write_buffer, size_t write_size, uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms);
```

 - 从从属写入和读取的简单示例：

 ```c
 i2c_device_config_t dev_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = 0x58,
    .scl_speed_hz = 100000,
};

i2c_master_dev_handle_t dev_handle;
ESP_ERROR_CHECK(i2c_master_bus_add_device(I2C_PORT_NUM_0, &dev_cfg, &dev_handle));
uint8_t buf[20] = {0x20};
uint8_t buffer[2];
ESP_ERROR_CHECK(i2c_master_transmit_receive(i2c_bus_handle, buf, sizeof(buf), buffer, 2, -1));
 ```

 [oled128644pin](oled128644pin.md)