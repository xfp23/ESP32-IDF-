**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息

© 2024 . 未经许可不得复制、修改或分发。 此文献为 **DXG工作室**  所有。

# 简介
- [spi知识点](spi_aknownladge.md)
- 串行外设接口 (SPI) 是一种同步串行接口，可用于与外围设备进行通信。ESP32-S3 芯片集成了四个 SPI 控制
器：
1. SPI0
2. SPI1
3. 通用 SPI2，即 GP-SPI2
4. 通用 SPI3，即 GP-SPI3 

- 为了多路复用来自 SPI 主机、SPI flash 等不同驱动的设备，每个 SPI 总线上都配有 SPI 总线锁。驱动程序可以通过对锁实施仲裁，将设备连接到总线上。
- SPI1 总线的后台服务为高速缓存。在设备操作开始前，总线锁可以禁用高速缓存，并在设备释放锁后将其再次启用。高速缓存处于禁用状态时，让出当前任务的执行权毫无意义，因此，该情况下 SPI1 总线上的任何设备都无法使用 ISR。

- SPI 主机驱动允许总线上连接多个设备（共享单个 `ESP32-S3 SPI` 外设）。每个设备仅由一个任务访问时，驱动程序线程安全。反之，若多个任务尝试访问同一 SPI 设备，则驱动程序 非线程安全。此时，建议执行以下任一操作：

重构应用程序，确保每个 SPI 外设在同一时间仅由一个任务访问。使用 `spi_bus_config_t::isr_cpu_id` 将 `SPI ISR` 注册到与 `SPI` 外设相关任务相同的内核，以确保线程安全。

使用 `xSemaphoreCreateMutex` 为共享设备添加互斥锁。

## SPI传输事务:

SPI 总线传输事务由五个阶段构成，详见下表（任意阶段均可跳过）。

| **阶段名称**         | **描述**                                                                                   |
|----------------------|--------------------------------------------------------------------------------------------|
| **命令阶段 (Command)** | 在此阶段，主机向总线发送命令字段，长度为 0-16 位。                                          |
| **地址阶段 (Address)** | 在此阶段，主机向总线发送地址字段，长度为 0-32 位。                                          |
| **Dummy 阶段**       | 此阶段可自行配置，用于适配时序要求。                                                        |
| **写入阶段 (Write)**  | 此阶段主机向设备传输数据，这些数据在紧随命令阶段（可选）和地址阶段（可选）之后。从电平的角度来看，数据与命令没有区别。|
| **读取阶段 (Read)**   | 此阶段主机读取设备数据。                                                                   |

# 环境配置

1. 添加以下头文件:
```c
#include "driver/spi_common.h"
```

2. CMAKE配置:
```
REQUIRES driver
```

# 初始化配置

传输事务属性由总线配置结构体 `spi_bus_config_t`、设备配置结构体 `spi_device_interface_config_t` 和传输事务配置结构体 `spi_transaction_t` 共同决定。

1. 配置SPI总线
```c

typedef struct {
    union {
      int mosi_io_num;    ///< 主机输出从机输入（=spi_d）信号的 GPIO 引脚编号，若未使用则为 -1。
      int data0_io_num;   ///< 在四路/八路模式下，spi data0 信号的 GPIO 引脚编号，若未使用则为 -1。
    };
    union {
      int miso_io_num;    ///< 主机输入从机输出（=spi_q）信号的 GPIO 引脚编号，若未使用则为 -1。
      int data1_io_num;   ///< 在四路/八路模式下，spi data1 信号的 GPIO 引脚编号，若未使用则为 -1。
    };
    int sclk_io_num;      ///< SPI 时钟信号的 GPIO 引脚编号，若未使用则为 -1。
    union {
      int quadwp_io_num;  ///< WP（写保护）信号的 GPIO 引脚编号，若未使用则为 -1。
      int data2_io_num;   ///< 在四路/八路模式下，spi data2 信号的 GPIO 引脚编号，若未使用则为 -1。
    };
    union {
      int quadhd_io_num;  ///< HD（保持）信号的 GPIO 引脚编号，若未使用则为 -1。
      int data3_io_num;   ///< 在四路/八路模式下，spi data3 信号的 GPIO 引脚编号，若未使用则为 -1。
    };
    int data4_io_num;     ///< 在八路模式下，spi data4 信号的 GPIO 引脚编号，若未使用则为 -1。
    int data5_io_num;     ///< 在八路模式下，spi data5 信号的 GPIO 引脚编号，若未使用则为 -1。
    int data6_io_num;     ///< 在八路模式下，spi data6 信号的 GPIO 引脚编号，若未使用则为 -1。
    int data7_io_num;     ///< 在八路模式下，spi data7 信号的 GPIO 引脚编号，若未使用则为 -1。
    int max_transfer_sz;  ///< 最大传输大小（以字节为单位）。如果启用 DMA 且此值为 0，则默认为 4092 字节；如果禁用 DMA，则默认为 `SOC_SPI_MAXIMUM_BUFFER_SIZE`。
    uint32_t flags;       ///< 驱动程序要检查的总线能力。为 ``SPICOMMON_BUSFLAG_*`` 标志的或值。
    esp_intr_cpu_affinity_t  isr_cpu_id;    ///< 选择注册 SPI 中断服务程序 (ISR) 的 CPU 核心。
    int intr_flags;       /**< 设置总线中断标志以确定优先级和 IRAM 属性，详见 ``esp_intr_alloc.h``。请注意，驱动程序会忽略 EDGE、INTRDISABLED 属性。
                           *  请注意，如果设置了 ESP_INTR_FLAG_IRAM 标志，则驱动程序的所有回调及其调用的函数都应放在 IRAM 中。
                           */
} spi_bus_config_t;

//成员isr_cu_id
typedef enum {
    ESP_INTR_CPU_AFFINITY_AUTO, ///< 将外设中断安装到任意 CPU 核心，由中断分配器当前运行的 CPU 核心决定。
    ESP_INTR_CPU_AFFINITY_0,    ///< 将外设中断安装到 CPU 核心 0。
    ESP_INTR_CPU_AFFINITY_1,    ///< 将外设中断安装到 CPU 核心 1。
} esp_intr_cpu_affinity_t;

//成员flags
#define SPICOMMON_BUSFLAG_SLAVE         0          ///< 以从模式初始化 I/O。
#define SPICOMMON_BUSFLAG_MASTER        (1<<0)     ///< 以主模式初始化 I/O。
#define SPICOMMON_BUSFLAG_IOMUX_PINS    (1<<1)     ///< 检查是否使用 IOMUX 引脚。或者表示引脚是通过 IO 多路复用器配置的，而不是通过 GPIO 矩阵配置的。
#define SPICOMMON_BUSFLAG_GPIO_PINS     (1<<2)     ///< 强制信号通过 GPIO 矩阵路由。或者表示引脚是通过 GPIO 矩阵路由的。
#define SPICOMMON_BUSFLAG_SCLK          (1<<3)     ///< 检查是否存在 SCLK 引脚。或者表示时钟线已初始化。
#define SPICOMMON_BUSFLAG_MISO          (1<<4)     ///< 检查是否存在 MISO 引脚。或者表示 MISO 线已初始化。
#define SPICOMMON_BUSFLAG_MOSI          (1<<5)     ///< 检查是否存在 MOSI 引脚。或者表示 MOSI 线已初始化。
#define SPICOMMON_BUSFLAG_DUAL          (1<<6)     ///< 检查 MOSI 和 MISO 引脚是否可以输出。或者表示总线能够在 DIO 模式下工作。
#define SPICOMMON_BUSFLAG_WPHD          (1<<7)     ///< 检查是否存在 WP 和 HD 引脚。或者表示 WP 和 HD 引脚已初始化。
#define SPICOMMON_BUSFLAG_QUAD          (SPICOMMON_BUSFLAG_DUAL|SPICOMMON_BUSFLAG_WPHD)     ///< 检查是否存在 MOSI/MISO/WP/HD 引脚作为输出。或者表示总线能够在 QIO 模式下工作。
#define SPICOMMON_BUSFLAG_IO4_IO7       (1<<8)     ///< 检查是否存在 IO4~IO7 引脚。或者表示 IO4~IO7 引脚已初始化。
#define SPICOMMON_BUSFLAG_OCTAL         (SPICOMMON_BUSFLAG_QUAD|SPICOMMON_BUSFLAG_IO4_IO7)  ///< 检查是否存在 MOSI/MISO/WP/HD/SPIIO4/SPIIO5/SPIIO6/SPIIO7 引脚作为输出。或者表示总线能够在八路模式下工作。
#define SPICOMMON_BUSFLAG_NATIVE_PINS   SPICOMMON_BUSFLAG_IOMUX_PINS ///< 使用原生引脚。


esp_err_t spi_bus_initialize(spi_host_device_t host_id, const spi_bus_config_t *bus_config, spi_dma_chan_t dma_chan);

typedef enum {
// SPI1 只能在 ESP32 上用作 GPSPI
    SPI1_HOST=0,    ///< SPI1
    SPI2_HOST=1,    ///< SPI2
#if SOC_SPI_PERIPH_NUM > 2
    SPI3_HOST=2,    ///< SPI3
#endif
    SPI_HOST_MAX,   ///< 无效的主机值
} spi_host_device_t;


/**
 * @brief SPI DMA 通道
 */
typedef enum {
  SPI_DMA_DISABLED = 0,     ///< 不为 SPI 启用 DMA
#if CONFIG_IDF_TARGET_ESP32
  SPI_DMA_CH1      = 1,     ///< 启用 DMA，选择 DMA 通道 1
  SPI_DMA_CH2      = 2,     ///< 启用 DMA，选择 DMA 通道 2
#endif
  SPI_DMA_CH_AUTO  = 3,     ///< 启用 DMA，通道由驱动程序自动选择
} spi_common_dma_t;


```
- 应用示例:
```c
#define PIN_NUM_MISO GPIO_NUM_1
#define PIN_NUM_MOSI GPIO_NUM_2
#define PIN_NUM_SCLK GPIO_NUM_3
spi_bus_config_t bus_config = {
    .mosi_io_num = PIN_NUM_MOSI, //主机输出引脚
    .miso_io_num = PIN_NUM_MISO, //主机输入引脚
    .sclk_io_num = PIN_NUM_SCLK，//时钟信号引脚
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .flags = SPICOMMON_BUSFLAG_MASTER,//主机模式下工作
    .max_transfer_sz = EXAMPLE_LCD_H_RES * 80 * sizeof(uint16_t), //< 最大传输大小（以字节为单位）
};

//应用配置:

spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO);

```
2. 向总线添加设备:

```c
//句柄类型
spi_device_handle_t;

typedef struct {
    uint8_t command_bits;           ///< 命令阶段的默认位数（0-16 位），当未使用 ``SPI_TRANS_VARIABLE_CMD`` 时使用，否则忽略。
    uint8_t address_bits;           ///< 地址阶段的默认位数（0-64 位），当未使用 ``SPI_TRANS_VARIABLE_ADDR`` 时使用，否则忽略。
    uint8_t dummy_bits;             ///< 在地址和数据阶段之间插入的虚拟位数。
    uint8_t mode;                   /**< SPI 模式，表示 (CPOL, CPHA) 配置的组合：
                                         - 0: (0, 0)
                                         - 1: (0, 1)
                                         - 2: (1, 0)
                                         - 3: (1, 1)
                                     */
    spi_clock_source_t clock_source;///< 选择 SPI 时钟源，默认为 `SPI_CLK_SRC_DEFAULT`。
    uint16_t duty_cycle_pos;        ///< 正时钟的占空比，以 1/256 为增量（128 = 50%/50% 占空比）。将其设置为 0（=不设置）等同于设置为 128。
    uint16_t cs_ena_pretrans;       ///< CS 线在传输前应该激活的 SPI 位周期数（0-16）。仅在半双工传输中有效。
    uint8_t cs_ena_posttrans;       ///< CS 线在传输后应保持激活的 SPI 位周期数（0-16）。
    int clock_speed_hz;             ///< SPI 时钟速度（以 Hz 为单位）。由 `clock_source` 派生。 – 在主机模式下：时钟频率可达 80 MHz
                                    //  从机模式下：时钟频率可达60MHz
    int input_delay_ns;             /**< 从机的数据有效时间最大值。包括从 SCLK 到 MISO 有效的时间，
        以及从从机到主机可能存在的时钟延迟。驱动程序使用此值在 MISO 在线上准备好之前提供额外的
        延迟。除非您知道需要延迟，否则请保持为 0。为了在高频（超过 8MHz）下获得更好的时序性能，
        建议设置合适的值。
        */
    int spics_io_num;               ///< 此设备的 CS GPIO 引脚编号，如果未使用则为 -1。
    uint32_t flags;                 ///< SPI_DEVICE_* 标志的按位或值。
    int queue_size;                 ///< 传输队列大小。此设置决定了在同一时间内可以“在空中”的传输数量（通过 spi_device_queue_trans 排队但尚未通过 spi_device_get_trans_result 完成的传输）。
    transaction_cb_t pre_cb;   /**< 在传输开始前调用的回调函数。
                                 *
                                 *  这个回调函数在中断上下文中被调用，
                                 *  为了获得最佳性能，应该放在 IRAM 中，
                                 *  详情请参阅 SPI 主设备文档中的“传输速度”部分。
                                 *  如果不这样做，当驱动程序使用 ESP_INTR_FLAG_IRAM 初始化时，
                                 *  在闪存操作期间回调函数可能会崩溃。
                                 */
    transaction_cb_t post_cb;  /**< 在传输完成后调用的回调函数。
                                 *
                                 *  这个回调函数在中断上下文中被调用，
                                 *  为了获得最佳性能，应该放在 IRAM 中，
                                 *  详情请参阅 SPI 主设备文档中的“传输速度”部分。
                                 *  如果不这样做，当驱动程序使用 ESP_INTR_FLAG_IRAM 初始化时，
                                 *  在闪存操作期间回调函数可能会崩溃。
                                 */
} spi_device_interface_config_t;

typedef enum {
    SPI_CLK_SRC_DEFAULT = SOC_MOD_CLK_APB,      /*!< Select APB as SPI source clock */
    SPI_CLK_SRC_APB = SOC_MOD_CLK_APB,          /*!< Select APB as SPI source clock */
} soc_periph_spi_clk_src_t;

esp_err_t spi_bus_add_device(spi_host_device_t host_id, const spi_device_interface_config_t *dev_config, spi_device_handle_t *handle);
```

- 应用示例:
```c
#define PIN_NUM_CS GPIO_NUM_10

spi_device_handle_t dev_handle;
spi_device_interface_config_t device_config = {
    .clock_source = SPI_CLK_SRC_DEFAULT,
    .command_bits = 0,
    .address_bits = 0,
    .dummy_bits = 0,
    .clock_speed_hz = 10000,//10k,最大到达80MHz
    .mode = 0,
    .spics_io_num = PIN_NUM_CS,//片选引脚
    /*回调函数框架: void spi_pre_cb(spi_transaction_t *trans)
    {
        int dc = (int)trans->user;
        //中断内容
    }*/
    .pre_cb = precallback;// 在传输开始前调用的回调函数。应该放在 IRAM 中, 发送数据之前控制dc引脚 ，DC引脚需要配置为推挽
    .post_cb = post_callback;//数据传输完后的回调函数
    .queue_size = 7;
}
    spi_bus_add_device(SPI2_HOST,&device_config,&dev_handle);

    //如果设备有复位等功能管脚，使用gpio_config_t 进行引脚配置，扩展

```

# 传输数据

## 设备获取总线的使用权:

```c
/**
 * @brief 设备获取总线使用权
 * @param 设备句柄
 * @param portMAX_DELAY
 */
esp_err_t spi_device_acquire_bus(spi_device_handle_t device, TickType_t wait);
```

- 使用示例:
```c

spi_device_acquire_bus(dev_handle,portMAX_DELAY);

```

## 发送与接收数据

### 发送数据
1. 结构:
```c
struct spi_transaction_t {
    uint32_t flags;                 ///< SPI_TRANS_* 标志的按位或值。
    uint16_t cmd;                   /**< 命令数据，长度在 spi_device_interface_config_t 的 ``command_bits`` 中设置。
                                      *
                                      *  <b>注意：这个字段在 ESP-IDF 2.1 及之前版本中被称为 "command"，在 ESP-IDF 3.0 中已被重写为新的用法。</b>
                                      *
                                      *  示例：写入 0x0123 并将 command_bits=12 设置为发送命令 0x12, 0x3_（在以前的版本中，可能需要写入 0x3_12）。
                                      */
    uint64_t addr;                  /**< 地址数据，长度在 spi_device_interface_config_t 的 ``address_bits`` 中设置。
                                      *
                                      *  <b>注意：这个字段在 ESP-IDF 2.1 及之前版本中被称为 "address"，在 ESP-IDF 3.0 中已被重写为新的用法。</b>
                                      *
                                      *  示例：写入 0x123400 并将 address_bits=24 设置为发送地址 0x12, 0x34, 0x00（在以前的版本中，可能需要写入 0x12340000）。
                                      */
    size_t length;                  ///< 数据总长度，以位为单位。
    size_t rxlength;                ///< 接收的数据总长度，在全双工模式下不应大于 ``length``（0 默认设置为 ``length`` 的值）。
    void *user;                     ///< 用户定义的变量。可以用于存储例如事务 ID。
    union {
        const void *tx_buffer;      ///< 发送缓冲区的指针，如果没有 MOSI 阶段则为 NULL。
        uint8_t tx_data[4];         ///< 如果设置了 SPI_TRANS_USE_TXDATA，则直接从此变量发送数据。
    };
    union {
        void *rx_buffer;            ///< 接收缓冲区的指针，如果没有 MISO 阶段则为 NULL。如果使用 DMA，则按 4 字节单元写入。
        uint8_t rx_data[4];         ///< 如果设置了 SPI_TRANS_USE_RXDATA，数据将直接接收到此变量中。
    };
} ;        // 接收数据应从 32 位对齐的地址开始，以避免 DMA 问题。

//flags字段成员
#define SPI_TRANS_MODE_DIO            (1<<0)  ///< 以 2 位模式发送/接收数据
#define SPI_TRANS_MODE_QIO            (1<<1)  ///< 以 4 位模式发送/接收数据
#define SPI_TRANS_USE_RXDATA          (1<<2)  ///< 将数据接收到 `spi_transaction_t` 的 `rx_data` 成员中，而不是接收到 `rx_buffer` 指向的内存中
#define SPI_TRANS_USE_TXDATA          (1<<3)  ///< 使用 `spi_transaction_t` 的 `tx_data` 成员发送数据，而不是使用 `tx_buffer` 中的数据。当使用此宏时，不要设置 `tx_buffer`
#define SPI_TRANS_MODE_DIOQIO_ADDR    (1<<4)  ///< 以 DIO/QIO 模式发送地址
#define SPI_TRANS_VARIABLE_CMD        (1<<5)  ///< 使用 `spi_transaction_ext_t` 中的 `command_bits`，而不是 `spi_device_interface_config_t` 中的默认值
#define SPI_TRANS_VARIABLE_ADDR       (1<<6)  ///< 使用 `spi_transaction_ext_t` 中的 `address_bits`，而不是 `spi_device_interface_config_t` 中的默认值
#define SPI_TRANS_VARIABLE_DUMMY      (1<<7)  ///< 使用 `spi_transaction_ext_t` 中的 `dummy_bits`，而不是 `spi_device_interface_config_t` 中的默认值
#define SPI_TRANS_CS_KEEP_ACTIVE      (1<<8)  ///< 在数据传输后保持 CS（片选）信号激活
#define SPI_TRANS_MULTILINE_CMD       (1<<9)  ///< 在命令阶段使用与数据阶段相同的数据线（否则，命令阶段仅使用一根数据线）
#define SPI_TRANS_MODE_OCT            (1<<10) ///< 以 8 位模式发送/接收数据
#define SPI_TRANS_MULTILINE_ADDR      SPI_TRANS_MODE_DIOQIO_ADDR ///< 在地址阶段使用与数据阶段相同的数据线（否则，地址阶段仅使用一根数据线）
#define SPI_TRANS_DMA_BUFFER_ALIGN_MANUAL   (1<<11) ///< 默认情况下，如果 DMA 缓冲区不满足硬件对齐或 DMA 支持要求，驱动程序会自动重新分配缓冲区。此标志用于禁用此功能，在这种情况下，你需要自行处理对齐问题，否则驱动程序会返回错误 ESP_ERR_INVALID_ARG


/**
 * @brief 此函数来自头文件<string.h> 将一块内存区域设置为指定值的函数
 * @param 指向要填充的内存块的地址。
 * @param 要设定的指定值
 * @param 要设置的字节数
 * @return 返回指向 ptr 的指针。
 */
void *memset(void *ptr, int value, size_t num);


/**
 * @brief 向总线发送数据
 * @param 设备句柄
 * @param  spi_transaction_t 结构变量
 * @return ESP调试信息
 */
esp_err_t SPI_MASTER_ISR_ATTR spi_device_polling_transmit(spi_device_handle_t handle, spi_transaction_t* trans_desc);
```

- 应用示例:

```c

uint8_t cmd;//假如这是要发送的数据
spi_transaction_t t;
meset(&t,0,sizeof(t));

t.length = 8; //要发送的数据长度，以位为单位，八位也就是一字节
t.tx_buffer = &cmd;//要发送的内容的缓冲区地址
t.rx_buffer = NULL; //接受数据的缓冲区地址
t.user = (void *)0; //此处用来回调函数获取DC指令
t.flags = SPI_TRANS_CS_KEEP_ACTIVE;
spi_device_polling_transmit(dev_handle,&t);
```
### 接收数据

- 接收数据和发送数据共用一套结构体，所以此处不列举了

1. 和发送数据一样，接收数据也先要获取总线使用权:

```c
spi_device_acquire_bus(dev_handle,portMAX_DELAY);

```

2. 配置结构体:

- 此处直接给代码

```c
spi_transaction_t t;
meset(&t,0,sizeof(t));
t.length = 8*2;//要接收的数据长度 单位: 位
t.flags = SPI_TRANS_USE_RXDATA; //将接收到的数据存在了结构成员 rx_data 中
t.user = (void *)1;
spi_device_polling_transmit(dev_handle,&t);
```

## 释放总线:

```c
void SPI_MASTER_ISR_ATTR spi_device_release_bus(spi_device_t *dev);
```

# 示例

- [SPI驱动TFT屏幕](SPITFT.c)
