**本人只提供idf编程方法，不提供知识点注解**

# 版权信息

© 2024  未经许可不得复制、修改或分发。
 此文献为 **DXG工作室**  所有。

# ADC连续转换模式编程指南

- 若想查看单次转换模式,[点击此处跳转](ADC0.md)
- [此处查看有关ADC的知识点](ADC_POINT.md)
# 简介

ESP32-S3 芯片集成了模数转换器 (ADC)，支持测量特定模拟 IO 管脚的模拟信号。此外，ADC 还支持直接内存访问 (DMA) 功能，高效获取 ADC 转换结果。

ESP32-S3 具有 两 个 ADC 单元，可应用于以下场景：

生成单次 ADC 转换结果

生成连续 ADC 转换结果

本指南介绍了 ADC 连续转换模式。

# ADC连续转换模式概念:

ADC 连续转换模式驱动由多个转换帧组成。

转换帧：一个转换帧包含多个转换结果。转换帧大小以字节为单位，在 `adc_continuous_new_handle()` 中配置。

转换结果：一个转换结果包含多个字节，即 `SOC_ADC_DIGI_RESULT_BYTES`。转换结果的数据结构由 `adc_digi_output_data_t` 定义，包括 ADC 单元、ADC 通道以及原始数据

# 环境配置:

1. 包含头文件:

```c
#include "esp_adc/adc_continuous.h"
```

2. CAMKE配置

```c
REQUIRES esp_adc
```

# 资源分配

- 配置结构体 `adc_continuous_handle_cfg_t`，创建 ADC 连续转换模式驱动的句柄
- 完成 ADC 配置后，使用已设置的配置结构体 `adc_continuous_handle_cfg_t` 调用 `adc_continuous_new_handle()`
- 如果不再使用 ADC 连续转换模式驱动，请调用 `adc_continuous_deinit()` 将驱动去初始化。

1. 类型声明:

```c
//句柄类型:
adc_continuous_handle_t;

//结构定义:
typedef struct {
    uint32_t max_store_buf_size;    ///< 以字节为单位设置最大缓冲池的大小，驱动程序将 ADC 转换结果保存到该缓冲池中。缓冲池已满时，新的转换将丢失。
    uint32_t conv_frame_size;       ///< 转换帧大小，以字节为单位。应为 `SOC_ADC_DIGI_DATA_BYTES_PER_CONV` 的倍数。
    struct {
        uint32_t flush_pool: 1;     ///< 当池满时，刷新内部池。
    } flags;                        ///< 驱动标志
} adc_continuous_handle_cfg_t;

/**
 * @brief 应用adc连续转换配置
 * @param 连续转换配置结构地址
 * @param 连续转换句柄地址
 * @return ESP调试信息
 */
esp_err_t adc_continuous_new_handle(const adc_continuous_handle_cfg_t *hdl_config, adc_continuous_handle_t *ret_handle);

/**
 * @brief 取消初始化的ADC连续驱动器
 * @param adc句柄
 * @return ESP调试信息
 */
esp_err_t adc_continuous_deinit(adc_continuous_handle_t handle);
```

2. 示例代码:
```c
adc_continuous_handle_cfg_t adc_config = {
    .max_store_buf_size = 1024,
    .conv_frame_size = 100,
};
ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config,&adc_handle));
```

## IIR滤波器
- ADC 连续转换模式下支持使用两个 IIR 滤波器。请设置 `adc_continuous_iir_filter_config_t` 结构体并调用 `adc_new_continuous_iir_filter()`，以创建 ADC IIR 滤波器
- 在一个 ADC 通道上同时使用两个滤波器时，只有第一个滤波器会生效。
1. 类型声明:
```c
//句柄类型
adc_iir_filter_handle_t;

typedef struct {
    adc_unit_t unit;                    ///< ADC单元
    adc_channel_t channel;              ///< 要过滤的ADC通道。注意，对于ESP32S2，每个ADC单元在模式表中应只设置一个通道。详情请参阅编程指南。
    adc_digi_iir_filter_coeff_t coeff;  ///< ADC滤波系数
} adc_continuous_iir_filter_config_t;

//选择ADC单元:
typedef enum {
    ADC_UNIT_1,        ///< SAR ADC 1
    ADC_UNIT_2,        ///< SAR ADC 2
} adc_unit_t;

//选择ADC通道:
typedef enum {
    ADC_CHANNEL_0,     ///< ADC channel
    ADC_CHANNEL_1,     ///< ADC channel
    ADC_CHANNEL_2,     ///< ADC channel
    ADC_CHANNEL_3,     ///< ADC channel
    ADC_CHANNEL_4,     ///< ADC channel
    ADC_CHANNEL_5,     ///< ADC channel
    ADC_CHANNEL_6,     ///< ADC channel
    ADC_CHANNEL_7,     ///< ADC channel
    ADC_CHANNEL_8,     ///< ADC channel
    ADC_CHANNEL_9,     ///< ADC channel
} adc_channel_t;

//选择ADC滤波系数
typedef enum {
    ADC_DIGI_IIR_FILTER_COEFF_2,     ///< 滤波系数为 2
    ADC_DIGI_IIR_FILTER_COEFF_4,     ///< 滤波系数为 4
    ADC_DIGI_IIR_FILTER_COEFF_8,     ///< 滤波系数为 8
    ADC_DIGI_IIR_FILTER_COEFF_16,    ///< 滤波系数为 16
    ADC_DIGI_IIR_FILTER_COEFF_64,    ///< 滤波系数为 64
} adc_digi_iir_filter_coeff_t;

/**
 * @brief 创建新的滤波器
 * @param adc连续转换句柄
 * @param adc滤波器结构地址
 * @param adc滤波器句柄地址
 * @return ESP调试信息
 */
esp_err_t adc_new_continuous_iir_filter(adc_continuous_handle_t handle, const adc_continuous_iir_filter_config_t *config, adc_iir_filter_handle_t *ret_hdl);

/**
 * @brief 回收滤波器
 * @param 滤波器句柄
 * @return ESP调试信息
 */
esp_err_t adc_del_continuous_iir_filter(adc_iir_filter_handle_t filter_hdl);
```


# 监视器

- 当 ADC 在连续转换模式下运行时，支持使用 2 个监视器。你可以在运行中的 ADC 通道上设置一到两个监视器阈值，一旦转换结果超出阈值，监视器将在每个采样循环中触发中断
- 设置 `adc_monitor_config_t`，并调用 `adc_new_continuous_monitor()` 以创建 ADC 监视器

```c
//监视器句柄：
adc_monitor_handle_t；

typedef struct {
    adc_unit_t adc_unit;            /*!< 设置用于监视的 ADC 单元编号。 */
    adc_channel_t channel;          /*!< 设置用于监视的 ADC 通道编号。 */
    int32_t h_threshold;            /*!< 高阈值，转换结果大于此值将触发中断，如果不使用此阈值，则将其设置为 -1。 */
    int32_t l_threshold;            /*!< 低阈值，转换结果小于此值将触发中断，如果不使用此阈值，则将其设置为 -1。 */
} adc_monitor_config_t;

/**
 * @brief 创建一个新的adc监视器
 * @param 要监视的adc连续转换句柄
 * @param 监视器结构地址
 * @param 监视器句柄
 * @return ESP调试信息
 */
esp_err_t adc_new_continuous_monitor(adc_continuous_handle_t handle, const adc_monitor_config_t *monitor_cfg, adc_monitor_handle_t *ret_handle);

//启用监视器
esp_err_t adc_continuous_monitor_enable(adc_monitor_handle_t monitor_handle);

//禁用监视器
esp_err_t adc_continuous_monitor_disable(adc_monitor_handle_t monitor_handle);

//删除监视器，释放资源
esp_err_t adc_del_continuous_monitor(adc_monitor_handle_t monitor_handle);
/** @brief 注册用户回调函数，在adc结果超出阈值时，执行回调函数
 * @param 监视器句柄
 * @param 回调结构地址
 * @param NULL 一般都是空，我也不知道具体是干什么的，听官方说是给中断函数传入数据用，不知具体用法
 * @return ESP调试信息
 */
esp_err_t adc_continuous_monitor_register_event_callbacks(adc_monitor_handle_t monitor_handle, const adc_monitor_evt_cbs_t *cbs, void *user_data);

typedef struct {
    adc_monitor_evt_cb_t on_over_high_thresh;           /*!< ADC 监视高值中断回调函数 */
    adc_monitor_evt_cb_t on_below_low_thresh;           /*!< ADC 监视低值中断回调函数 */
} adc_monitor_evt_cbs_t;


//定义一个回调函数示例:
static bool IRAM_ATTR s_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    BaseType_t mustYield = pdFALSE;
    //Notify that ADC continuous driver has done enough number of conversions
    vTaskNotifyGiveFromISR(s_task_handle, &mustYield);

    return (mustYield == pdTRUE);
}
```
 
# 配置ADC

- 初始化 ADC 连续转换模式驱动后，设置 `adc_continuous_config_t` 配置 ADC IO，测量模拟信号
- [此处查看详细的通道对应IO](ADC_POINT.md)
```c
//句柄类型：

typedef struct {
    uint32_t pattern_num;                   ///< 使用的 ADC 通道数量
    adc_digi_pattern_config_t *adc_pattern; ///< 每个使用的 ADC 通道的配置列表 此处应该指向一个结构数组
    uint32_t sample_freq_hz;                /*!< 期望的 ADC 采样频率（单位：Hz）。请参考 `soc/soc_caps.h` 了解可用的采样频率范围 */
    adc_digi_convert_mode_t conv_mode;      ///< ADC DMA 转换模式，参见 `adc_digi_convert_mode_t`
    adc_digi_output_format_t format;        ///< ADC DMA 转换输出格式，参见 `adc_digi_output_format_t`
} adc_continuous_config_t;

//选择每个通道配置列表
typedef struct {
    uint8_t atten;      ///< 此 ADC 通道的衰减值
    uint8_t channel;    ///< ADC 通道编号
    uint8_t unit;       ///< ADC 单元编号
    uint8_t bit_width;  ///< ADC 输出位宽
} adc_digi_pattern_config_t;


//选择转换模式
typedef enum {
    ADC_CONV_SINGLE_UNIT_1 = 1,  ///< 仅使用 ADC1 进行转换
    ADC_CONV_SINGLE_UNIT_2 = 2,  ///< 仅使用 ADC2 进行转换
    ADC_CONV_BOTH_UNIT     = 3,  ///< 同时使用 ADC1 和 ADC2 进行转换
    ADC_CONV_ALTER_UNIT    = 7,  ///< 交替使用 ADC1 和 ADC2 进行转换。例如：ADC1 -> ADC2 -> ADC1 -> ADC2 .....
} adc_digi_convert_mode_t;

//转换输出格式
/**
 * @brief ADC digital controller (DMA mode) output data format option.
 */
typedef enum {
    ADC_DIGI_OUTPUT_FORMAT_TYPE1,   ///< 参见 `adc_digi_output_data_t.type1` 数据格式 默认的
    ADC_DIGI_OUTPUT_FORMAT_TYPE2,   ///< 参见 `adc_digi_output_data_t.type2` 数据格式
} adc_digi_output_format_t;

/**
 * @brief 配置ADC
 * @param ADC句柄
 * @param ADC配置结构地址
 * @return ESP调试信息
 */
esp_err_t adc_continuous_config(adc_continuous_handle_t handle, const adc_continuous_config_t *config);
```


2. 示例代码：

```c
    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = 20 * 1000,
        .conv_mode = EXAMPLE_ADC_CONV_MODE,
        .format = EXAMPLE_ADC_OUTPUT_TYPE,
    };
    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    dig_cfg.pattern_num = channel_num;
    for (int i = 0; i < channel_num; i++) {
        adc_pattern[i].atten = EXAMPLE_ADC_ATTEN;
        adc_pattern[i].channel = channel[i] & 0x7;
        adc_pattern[i].unit = EXAMPLE_ADC_UNIT;
        adc_pattern[i].bit_width = EXAMPLE_ADC_BIT_WIDTH;

        ESP_LOGI(TAG, "adc_pattern[%d].atten is :%"PRIx8, i, adc_pattern[i].atten);
        ESP_LOGI(TAG, "adc_pattern[%d].channel is :%"PRIx8, i, adc_pattern[i].channel);
        ESP_LOGI(TAG, "adc_pattern[%d].unit is :%"PRIx8, i, adc_pattern[i].unit);
    }
    dig_cfg.adc_pattern = adc_pattern;
    ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));
```

# ADC 控制

## 启动和停止
- 调用 `adc_continuous_start()`，将使 ADC 开始从配置好的 ADC 通道测量模拟信号，并生成转换结果。
- 调用 adc_continuous_stop() 则会停止 ADC 转换。
```c
//函数原型:
esp_err_t adc_continuous_start(adc_continuous_handle_t handle);//开始

esp_err_t adc_continuous_stop(adc_continuous_handle_t handle);//停止
```
## 注册事件回调
- 调用 `adc_continuous_register_event_callbacks()`，可以将自己的函数链接到驱动程序的 ISR 中。
- 通过 `adc_continuous_evt_cbs_t` 可查看所有支持的事件回调。

```c
typedef struct {
    adc_continuous_callback_t on_conv_done;    ///< 事件回调函数，当一个转换帧完成时调用。有关“转换帧”概念，请参阅本头文件中的 `Driver Backgrounds` 小节。
    adc_continuous_callback_t on_pool_ovf;     ///< 当内部缓冲池已满时，触发此事件，新的转换结果将丢失。
} adc_continuous_evt_cbs_t;

/**
 * @brief 注册回调函数
 * @param ADC句柄
 * @param 回调函数地址
 * @param NULL
 * @return ESP
 */
esp_err_t adc_continuous_register_event_callbacks(adc_continuous_handle_t handle, const adc_continuous_evt_cbs_t *cbs, void *user_data);

//回调函数声明:
static bool IRAM_ATTR s_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data);
```

- 由于上述回调函数在 ISR 中调用，请确保回调函数适合在 ISR 上下文中运行，且这些回调不应涉及阻塞逻辑。回调函数的原型在 adc_continuous_callback_t 中声明

## 读取转换结果
- adc_continuous_read() 可以获取 ADC 通道的转换结果。

```c
/**
 * @brief 读取adc结果。
 * @param ADC连续模式驱动器手柄。
 * @param 从ADC读取的转换结果缓冲区。
 * @param 从ADC读取的转换结果的预期长度以字节为单位。
 * @param 通过此API从ADC读取的转换结果的实际长度以字节为单位。
 * @param 等待通过此API获取数据的时间以毫秒为单位。
 * @return ESP
 * 
 */
esp_err_t adc_continuous_read(adc_continuous_handle_t handle, uint8_t *buf, uint32_t length_max, uint32_t *out_length, uint32_t timeout_ms);
```

- 从上述函数读取的 ADC 转换结果为原始数据。要根据 ADC 原始结果计算电压，可以使用以下公式:
```c
Vout = Dout * Vmax / Dmax       
```

| 符号     | 说明                                                                                                             |
|----------|------------------------------------------------------------------------------------------------------------------|
| Vout     | **数字输出结果，代表电压**                                                                                       |
| Dout输出 | ADC 原始数字读取结果                                                                                            |
| Vmax     | 可测量的最大模拟输入电压，与 ADC 衰减无关。请参考 [技术手册->片上传感器与模拟信号处理](/PDF/ESP32-S3.pdf)         |
| Dmax     | 输出 ADC 原始数字读取结果的最大值，即 \(2^{\text{位宽}}\)，位宽即之前配置的 `adc_digi_pattern_config_t::bit_width` |

- 若需进一步校准，将 ADC 原始结果转换为以 mV 为单位的电压数据,[ADC校准指南](ADC_calibration.md)。
## 函数原型
- [上述函数原型](/ADC/ADC_Functions.c)
- [ADC连续转换示例 1](adc_con.c)
- [ADC连续转换示例 2](adc_con2.c)