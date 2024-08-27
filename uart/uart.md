**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息

© 2024 . 未经许可不得复制、修改或分发。 此文献为 [小風的藏書閣](https://t.me/xfp2333)  所有。


# 通用异步接收器/发送器(UART)编程指南

# 简介

- 通用异步接收器/发送器 (UART) 属于一种硬件功能，通过使用 RS232、RS422、RS485 等常见异步串行通信接口来处理通信时序要求和数据帧。UART 是实现不同设备之间全双工或半双工数据交换的一种常用且经济的方式。
- ESP32-S3 芯片有 3 个 UART 控制器（也称为端口），每个控制器都有一组相同的寄存器以简化编程并提高灵活性。
- 每个 UART 控制器可以独立配置**波特率**、**数据位长度**、**位顺序**、**停止位位数**、**奇偶校验位**等参数。

# 环境配置

1. 包含头文件：
```c
#include "driver/uart.h"
```

2.CMAKE配置：
```c
REQUIRES driver
```

# 设置通信参数

- 设置波特率
- 数据位
- 停止位

1. 一次性配置所有参数,调用函数 **uart_param_config()** 并向其传递 **uart_config_t** 结构体，**uart_config_t** 结构体应包含所有必要的参数。

- 结构类型声明:
```c
typedef struct {
    int baud_rate;                      /*!< UART 波特率 */
    uart_word_length_t data_bits;       /*!< UART 数据位长度 */
    uart_parity_t parity;               /*!< UART 校验模式 */
    uart_stop_bits_t stop_bits;         /*!< UART 停止位 */
    uart_hw_flowcontrol_t flow_ctrl;    /*!< UART 硬件流控制模式 (cts/rts) */
    uint8_t rx_flow_ctrl_thresh;        /*!< UART 硬件 RTS 阈值 */
    union {
        uart_sclk_t source_clk;             /*!< UART 源时钟选择 */
#if (SOC_UART_LP_NUM >= 1)
        lp_uart_sclk_t lp_source_clk;       /*!< 低功耗 UART 源时钟选择 */
#endif
    };
} uart_config_t;

//设置数据位结构：
typedef enum {
    UART_DATA_5_BITS   = 0x0,    /*!< 字长：5位 */
    UART_DATA_6_BITS   = 0x1,    /*!< 字长：6位 */
    UART_DATA_7_BITS   = 0x2,    /*!< 字长：7位 */
    UART_DATA_8_BITS   = 0x3,    /*!< 字长：8位 */
    UART_DATA_BITS_MAX = 0x4,
} uart_word_length_t;

//校验位设置：
typedef enum {
    UART_PARITY_DISABLE  = 0x0,  /*!< 禁用 UART 校验 */
    UART_PARITY_EVEN     = 0x2,  /*!< 启用 UART 偶校验 */
    UART_PARITY_ODD      = 0x3   /*!< 启用 UART 奇校验 */
} uart_parity_t;

//停止位设置：
typedef enum {
    UART_STOP_BITS_1   = 0x1,  /*!< 停止位：1位 */
    UART_STOP_BITS_1_5 = 0x2,  /*!< 停止位：1.5位 */
    UART_STOP_BITS_2   = 0x3,  /*!< 停止位：2位 */
    UART_STOP_BITS_MAX = 0x4,
} uart_stop_bits_t;

//硬件流控制设置：
typedef enum {
    UART_HW_FLOWCTRL_DISABLE = 0x0,   /*!< 禁用硬件流控制 */
    UART_HW_FLOWCTRL_RTS     = 0x1,   /*!< 启用 RX 硬件流控制 (rts) */
    UART_HW_FLOWCTRL_CTS     = 0x2,   /*!< 启用 TX 硬件流控制 (cts) */
    UART_HW_FLOWCTRL_CTS_RTS = 0x3,   /*!< 启用硬件流控制 */
    UART_HW_FLOWCTRL_MAX     = 0x4,
} uart_hw_flowcontrol_t;


//时钟源选择：
typedef enum {
    UART_SCLK_APB = SOC_MOD_CLK_APB,     /*!< UART 源时钟是 APB 时钟 */
    UART_SCLK_RTC = SOC_MOD_CLK_RC_FAST, /*!< UART 源时钟是 RC_FAST */
    UART_SCLK_XTAL = SOC_MOD_CLK_XTAL,   /*!< UART 源时钟是 XTAL */
    UART_SCLK_DEFAULT = SOC_MOD_CLK_APB, /*!< UART 源时钟默认选择是 APB */
} soc_periph_uart_clk_src_legacy_t;

```

2. 示例代码：
```c
//选择硬件串口编号
const uart_port_t uart_num = UART_NUM_2;
uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
    .rx_flow_ctrl_thresh = 122,
};
//应用配置
ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
```

## 分步依次配置每个参数：

- 单独配置特定参数的函数

| **配置参数**        | **函数**                                           |
|--------------------|------------------------------------------------------|
| ***波特率***       | uart_set_baudrate()                                |
| ***传输位***       | 调用 `uart_set_word_length()` 设置 `uart_word_length_t` |
| ***奇偶控制***     | 调用 `uart_set_parity()` 设置 `uart_parity_t`        |
| ***停止位***       | 调用 `uart_set_stop_bits()` 设置 `uart_stop_bits_t`  |
| ***硬件流控模式*** | 调用 `uart_set_hw_flow_ctrl()` 设置 `uart_hw_flowcontrol_t` |
| ***通信模式***     | 调用 `uart_set_mode()` 设置 `uart_mode_t`            |

[函数原型](funtcion.c)


# 设置通信管脚

- 通信参数设置完成后，可以配置其他 UART 设备连接的 GPIO 管脚。调用函数 uart_set_pin()，指定配置 Tx、Rx、RTS 和 CTS 信号的 GPIO 管脚编号。如要为特定信号保留当前分配的管脚编号，可传递宏 `UART_PIN_NO_CHANGE`。
- 请为不使用的管脚都指定为宏 `UART_PIN_NO_CHANGE`。
1. 示例代码
```
// Set UART pins(TX: IO4, RX: IO5, RTS: IO18, CTS: IO19)
ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, 4, 5, 18, 19));
ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, 4, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
```
# 安装驱动程序

1. 调用`uart_driver_install()`安装驱动程序并指定以下参数:
- Tx 环形缓冲区的大小
- Rx 环形缓冲区的大小
- 事件队列句柄和大小
- 分配中断的标志

```c
/**
 * @brief 安装串口驱动
 * @param 串口号 0 1 2
 * @param UART RX环形缓冲区大小。
 * @param UART TX 环形缓冲区大小。如果设置为零，驱动程序将不使用 TX 缓冲区，TX 函数将阻止任务，直到所有数据都发送出去。
 * @param UART 事件句柄队列大小 
 * @param UART 事件队列句柄地址，成功后，此处将编写一个新的队列句柄，以提供对 UART 事件的访问。如果设置为 NULL，驱动程序将不会使用事件队列。
 * @param 中断分配标志，一个或多个，请勿在此处设置 ESP_INTR_FLAG_IRAM（驱动程序的 ISR 处理程序不在 IRAM 中）
 * @return ESP调试信息
 */
esp_err_t uart_driver_install(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t *uart_queue, int intr_alloc_flags);
```
2. 代码示例:
```c
// 设置带有事件队列的 UART 缓冲 IO
const int uart_buffer_size = (1024 * 2);
QueueHandle_t uart_queue;
// 在此处使用事件队列安装 UART 驱动程序
ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, uart_buffer_size, \
                                        uart_buffer_size, 10, &uart_queue, 0));

```

# 运行UART通信

**串行通信由每个UART控制器的有限状态机(FSM)控制**

- 发送数据的过程分为以下几步:
1. 将数据写入TX FIFO缓冲区
2. FSM序列化数据
3. FSM发送数据

- 接收数据的过程分为以下几步：
1. FSM 处理且并行化传入的串行流
2. FSM 将数据写入 Rx FIFO 缓冲区
3. 从 Rx FIFO 缓冲区读取数据

***应用程序仅会通过 `uart_write_bytes()` 和 `uart_read_bytes()` 从特定缓冲区写入或读取数据，其余工作由 FSM 完成***。

# 发送数据:

1. 函数原型:

```c
/**
 * @brief 从给定的缓冲区和长度向UART端口发送数据
 * @param 数据缓冲区地址
 * @param 要发送的数据长度
 * @return 推送到 TX FIFO 的字节数
 */
int uart_write_bytes(uart_port_t uart_num, const void *src, size_t size);

/**
 * @brief 从给定的缓冲区和长度向UART端口发送数据
 * 仅当 UART TX 缓冲区未启用时，才应使用此函数。
 * @param 串口号
 * @param 发送数据缓冲地址
 * @param 要发送的数据长度
 * @return 推送到 TX FIFO 的字节数
 */
int uart_tx_chars(uart_port_t uart_num, const char *buffer, uint32_t len)；

```

2. 示例代码:

```c
//写数据到串口
char* test_str = "This is a test string.\n";
uart_write_bytes(uart_num, (const char*)test_str, strlen(test_str));

/*函数 uart_write_bytes_with_break() 与 uart_write_bytes() 类似，但在传输结束时会添加串行中断信号。
“串行中断信号”意味着 Tx 线保持低电平的时间长于一个数据帧。*/
// 将数据写入UART，以中断信号结束。
uart_write_bytes_with_break(uart_num, "test break\n",strlen("test break\n"), 100);

/*“配套”函数 uart_wait_tx_done() 用于监听 Tx FIFO 缓冲区的状态，并在缓冲区为空时返回。*/
//等待数据包发送
const uart_port_t uart_num = UART_NUM_2;
ESP_ERROR_CHECK(uart_wait_tx_done(uart_num, 100)); // wait timeout is 100 RTOS ticks (TickType_t)
```


# 接收数据

- 一旦 UART 接收了数据，并将其保存在 ***Rx FIFO*** 缓冲区中，就需要使用函数 `uart_read_bytes()` 检索数据。读取数据之前，调用 `uart_get_buffered_data_len()` 能够查看 Rx FIFO 缓冲区中可用的字节数。

1. 函数原型:
```c

/**
 * @brief 从串口读字节
 * @param 串口号
 * @param 指向缓冲区的指针
 * @param 数据长度
 * @param ticks_to_wait -- sTimeout，RTOS 滴答计数
 * @return 从 UART 缓冲区读取的字节数
 */
int uart_read_bytes(uart_port_t uart_num, void *buf, uint32_t length, TickType_t ticks_to_wait);

/**
 * @brief UART获取RX环形缓冲区缓存数据长度
 * @param 串口号
 * @param 接收数据长度的size_t指针
 * @return ESP调试信息
 */
esp_err_t uart_get_buffered_data_len(uart_port_t uart_num, size_t *size);

/**
 * @brief 如果不再需要 Rx FIFO 缓冲区中的数据，可以调用 uart_flush() 清空缓冲区
 * @param 串口号
 * @return ESP调试信息 
 */
esp_err_t uart_flush(uart_port_t uart_num);

/**
 * @brief 清除输入缓冲区，丢弃环形缓冲区中的所有数据
 * @param 串口号
 * @return ESP调试信息
 */
esp_err_t uart_flush_input(uart_port_t uart_num);

```
2. 示例代码:
```c
// Read data from UART.
const uart_port_t uart_num = UART_NUM_2;
uint8_t data[128];
int length = 0;
ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*)&length));
length = uart_read_bytes(uart_num, data, length, 100);
```

# 通信方式选择

- UART 控制器支持多种通信模式，使用函数 `uart_set_mode()` 可以选择模式。选择特定模式后，UART 驱动程序将处理已连接 UART 设备的相应行为。例如，使用 RTS 线控制 RS485 驱动芯片，能够实现半双工 RS485 通信。

1. 原型
```c

/***
 * @brief 设置串口模式 当驱动程序对象初始化时，必须在 uart_driver_install（） 之后执行此函数
 * @param 串口号
 * @param 串口工作模式
 * @return ESP调试信息
 */
esp_err_t uart_set_mode(uart_port_t uart_num, uart_mode_t mode);

typedef enum {
    UART_MODE_UART = 0x00,                      /*!< 模式：常规 UART 模式 */
    UART_MODE_RS485_HALF_DUPLEX = 0x01,         /*!< 模式：RTS 引脚控制的半双工 RS485 UART 模式 */
    UART_MODE_IRDA = 0x02,                      /*!< 模式：IRDA UART 模式 */
    UART_MODE_RS485_COLLISION_DETECT = 0x03,    /*!< 模式：RS485 碰撞检测 UART 模式（用于测试目的） */
    UART_MODE_RS485_APP_CTRL = 0x04,            /*!< 模式：应用控制的 RS485 UART 模式（用于测试目的） */
} uart_mode_t;

```

2. 调用示例:
```c
// Setup UART in rs485 half duplex mode
ESP_ERROR_CHECK(uart_set_mode(uart_num, UART_MODE_RS485_HALF_DUPLEX));
```

# 删除驱动程序

- 如不再需要与 uart_driver_install() 建立通信，则可调用 uart_driver_delete() 删除驱动程序，释放已分配的资源。

1. 原型:
```c
/**
 * @brief 删除串口驱动
 * @param  串口号
 * @return ESP调试信息
 */
esp_err_t uart_driver_delete(uart_port_t uart_num);
```

------------------------------------------------------------------------------------------------------------------------------------------------
[UART通信示例](uart_example.c)

# 宏指令

- API 还定义了一些宏指令。例如，UART_HW_FIFO_LEN 定义了硬件 FIFO 缓冲区的长度，UART_BITRATE_MAX 定义了 UART 控制器支持的最大波特率。

# RS485

## 接口连接选项

### 电路A：冲突检测电路

```md

        VCC ---------------+
                           |
                   +-------x-------+
        RXD <------| R             |
                   |              B|----------<> B
        TXD ------>| D    ADM483   |
ESP                |               |     RS485 bus side
        RTS ------>| DE            |
                   |              A|----------<> A
              +----| /RE           |
              |    +-------x-------+
              |            |
             GND          GND

```

### 电路B：无冲突检测的手动切换发射器/接收器

```md
        VCC ---------------+
                           |
                   +-------x-------+
        RXD <------| R             |
                   |              B|-----------<> B
        TXD ------>| D    ADM483   |
ESP                |               |     RS485 bus side
        RTS --+--->| DE            |
              |    |              A|-----------<> A
              +----| /RE           |
                   +-------x-------+
                           |
                          GND
```

### 电路c：自动切换发射器/接收器

```md
 VCC1 <-------------------+-----------+           +-------------------+----> VCC2
               10K ____   |           |           |                   |
              +---|____|--+       +---x-----------x---+    10K ____   |
              |                   |                   |   +---|____|--+
RX <----------+-------------------| RXD               |   |
                   10K ____       |                  A|---+---------------<> A (+)
              +-------|____|------| PV    ADM2483     |   |    ____  120
              |   ____            |                   |   +---|____|---+  RS485 bus side
      VCC1 <--+--|____|--+------->| DE                |                |
              10K        |        |                  B|---+------------+--<> B (-)
                      ---+    +-->| /RE               |   |    ____
         10K          |       |   |                   |   +---|____|---+
        ____       | /-C      +---| TXD               |    10K         |
TX >---|____|--+_B_|/   NPN   |   |                   |                |
                   |\         |   +---x-----------x---+                |
                   | \-E      |       |           |                    |
                      |       |       |           |                    |
                     GND1    GND1    GND1        GND2                 GND2
```