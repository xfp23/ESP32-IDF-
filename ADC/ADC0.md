**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息

© 2024 . 未经许可不得复制、修改或分发。 此文献为 [小風的藏書閣](https://t.me/xfp2333) 所有。

# 模数转换器 (ADC) 单次转换模式驱动
- ESP32 有 两 个 ADC 单元，可以在以下场景使用：

1. 生成 ADC 单次转换结果

2. 生成连续 ADC 转换结果

3. 本指南介绍了 ADC 单次转换模式。

- **ESP32的ADC（模数转换器）可以工作在单次转换模式和连续转换模式，它们之间的主要区别如下：**

***单次转换模式：***

- 在单次转换模式下，ADC执行一次转换后自动停止。这意味着你需要显式地触发每次转换，并且每次转换后ADC会进入休眠状态以节省能量。
单次转换适合于需要在特定时间点测量并获取数据的应用场景，例如定时测量传感器数据或者事件驱动的数据采集。
连续转换模式：

- 在连续转换模式下，ADC会持续进行转换，每次转换完成后立即开始下一次转换。这种模式下，ADC不会自动停止，除非显式地中止转换。
连续转换适合需要持续监测并实时获取数据的应用，例如音频采集、数据记录或者实时控制系统。
区别总结：
触发方式：单次转换模式需要显式触发每次转换，而连续转换模式在启动后会自动持续转换。
功耗：单次转换模式相对于连续转换模式能够节省能量，因为它只在需要时才进行转换。
应用场景：根据应用的实时性需求和能耗要求选择合适的模式

# 编程实现

## 环境配置

- 所需头文件
```c
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
```
- cmake配置

```c
 REQUIRES  esp_adc 
```
## 资源分配

**ADC 单次转换模式驱动基于 ESP32 SAR ADC 模块实现，不同的 ESP 芯片可能拥有不同数量的独立 ADC。对于单次转换模式驱动而言，ADC 实例以 adc_oneshot_unit_handle_t 表示。**

1. 资源分配示例：
```c
adc_oneshot_unit_handle_t adc1_handle;
adc_oneshot_unit_init_cfg_t init_config1 = {
    .unit_id = ADC_UNIT_1,
    .ulp_mode = ADC_ULP_MODE_DISABLE,
};
ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));//绑定句柄与结构
```

2. 回收ADC资源：
```c
ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
```

## 配置ADC单元实例

- 配置两个ADC通道：

```c
adc_oneshot_chan_cfg_t config = {
    .bitwidth = ADC_BITWIDTH_DEFAULT,  // 使用默认的ADC位宽，通常为12位
    .atten = ADC_ATTEN_DB_12,          // 设置衰减值为12 dB，对应的电压范围为0 ~ 3.6V
};
ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN0, &config));
ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN1, &config));

```


## 读取ADC转换结果

```c
/**
* adc_raw: 数组用于存储ADC转换后的原始数据。
* adc_oneshot_read: 函数用于读取指定通道的ADC转换结果。它将ADC转换后的原始数据存储在提供的指针变量中。
* ESP_ERROR_CHECK: 这个宏用于检查函数返回的错误码并处理错误。如果读取操作失败，将会记录错误信息。
 */
int adc_raw[2];
ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN0, &adc_raw[0]));
ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN0, adc_raw[0]);

ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN1, &adc_raw[1]));
ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN1, adc_raw[1]);

```

- 通过该函数获取的 ADC 转换结果为原始数据。可以使用以下公式，根据 ADC 原始结果计算电压：
```c

/**
*@brief 参数说明:
*Vout数字输出结果，代表电压。
*Vmax 可测量的最大模拟输入电压，与 ADC 衰减相关
*Dmax 输出 ADC 原始数字读取结果的最大值，即 2^位宽，位宽即之前配置的 adc_digi_pattern_config_t::bit_width
*/
Vout = Dout * Vmax / Dmax       (1)

```

## 相关类型定义

```c
/**
 * @brief ADC单元
 */
typedef enum {
    ADC_UNIT_1,        ///< SAR ADC 1
    ADC_UNIT_2,        ///< SAR ADC 2
} adc_unit_t;

/**
 * @brief ADC通道
 */
typedef enum {
    ADC_CHANNEL_0,     ///< ADC通道0
    ADC_CHANNEL_1,     ///< ADC通道1
    ADC_CHANNEL_2,     ///< ADC通道2
    ADC_CHANNEL_3,     ///< ADC通道3
    ADC_CHANNEL_4,     ///< ADC通道4
    ADC_CHANNEL_5,     ///< ADC通道5
    ADC_CHANNEL_6,     ///< ADC通道6
    ADC_CHANNEL_7,     ///< ADC通道7
    ADC_CHANNEL_8,     ///< ADC通道8
    ADC_CHANNEL_9,     ///< ADC通道9
} adc_channel_t;

/**
 * @brief ADC衰减参数。不同参数决定了ADC的测量范围。
 */
typedef enum {
    ADC_ATTEN_DB_0   = 0,  ///< 无输入衰减，ADC可测量到约
    ADC_ATTEN_DB_2_5 = 1,  ///< ADC的输入电压将被衰减，测量范围扩展约2.5 dB (1.33倍)
    ADC_ATTEN_DB_6   = 2,  ///< ADC的输入电压将被衰减，测量范围扩展约6 dB (2倍)
    ADC_ATTEN_DB_11  = 3,  ///< ADC的输入电压将被衰减，测量范围扩展约11 dB (3.55倍)
} adc_atten_t;

typedef enum {
    ADC_BITWIDTH_DEFAULT = 0, ///< 默认ADC输出位宽，将选择支持的最大位宽
    ADC_BITWIDTH_9  = 9,      ///< ADC输出位宽为9位
    ADC_BITWIDTH_10 = 10,     ///< ADC输出位宽为10位
    ADC_BITWIDTH_11 = 11,     ///< ADC输出位宽为11位
    ADC_BITWIDTH_12 = 12,     ///< ADC输出位宽为12位
    ADC_BITWIDTH_13 = 13,     ///< ADC输出位宽为13位
} adc_bitwidth_t;

typedef enum {
    ADC_ULP_MODE_DISABLE = 0, ///< 禁用ADC ULP模式
    ADC_ULP_MODE_FSM     = 1, ///< ADC由ULP FSM控制
    ADC_ULP_MODE_RISCV   = 2, ///< ADC由ULP RISCV控制
} adc_ulp_mode_t;

/**
 * @brief ADC数字控制器(DMA模式)工作模式。
 */
typedef enum {
    ADC_CONV_SINGLE_UNIT_1 = 1,  ///< 仅使用ADC1进行转换
    ADC_CONV_SINGLE_UNIT_2 = 2,  ///< 仅使用ADC2进行转换
    ADC_CONV_BOTH_UNIT     = 3,  ///< 同时使用ADC1和ADC2进行转换
    ADC_CONV_ALTER_UNIT    = 7,  ///< 轮流使用ADC1和ADC2进行转换。例如：ADC1 -> ADC2 -> ADC1 -> ADC2 .....
} adc_digi_convert_mode_t;

/**
 * @brief ADC数字控制器(DMA模式)输出数据格式选项。
 */
typedef enum {
    ADC_DIGI_OUTPUT_FORMAT_TYPE1,   ///< 参见 `adc_digi_output_data_t.type1`
    ADC_DIGI_OUTPUT_FORMAT_TYPE2,   ///< 参见 `adc_digi_output_data_t.type2`
} adc_digi_output_format_t;

#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
typedef soc_periph_adc_digi_clk_src_t    adc_oneshot_clk_src_t;     ///< 数字控制器单次模式的时钟源类型
typedef soc_periph_adc_digi_clk_src_t    adc_continuous_clk_src_t;  ///< 数字控制器连续模式的时钟源类型
#elif SOC_ADC_RTC_CTRL_SUPPORTED
typedef soc_periph_adc_rtc_clk_src_t     adc_oneshot_clk_src_t;     ///< RTC控制器单次模式的时钟源类型
typedef soc_periph_adc_digi_clk_src_t    adc_continuous_clk_src_t;  ///< 数字控制器连续模式的时钟源类型
#endif

/**
 * @brief ADC数字控制器模式配置
 */
typedef struct {
    uint8_t atten;      ///< 此ADC通道的衰减
    uint8_t channel;    ///< ADC通道
    uint8_t unit;       ///< ADC单元
    uint8_t bit_width;  ///< ADC输出位宽
} adc_digi_pattern_config_t;

/**
 * @brief ADC IIR滤波器ID
 */
typedef enum {
    ADC_DIGI_IIR_FILTER_0,  ///< 滤波器0
    ADC_DIGI_IIR_FILTER_1,  ///< 滤波器1
} adc_digi_iir_filter_t;

/**
 * @brief IIR滤波器系数
 */
typedef enum {
    ADC_DIGI_IIR_FILTER_COEFF_2,     ///< 滤波系数为2
    ADC_DIGI_IIR_FILTER_COEFF_4,     ///< 滤波系数为4
    ADC_DIGI_IIR_FILTER_COEFF_8,     ///< 滤波系数为8
    ADC_DIGI_IIR_FILTER_COEFF_16,    ///< 滤波系数为16
    ADC_DIGI_IIR_FILTER_COEFF_64,    ///< 滤波系数为64
} adc_digi_iir_filter_coeff_t;

```

## 单次转换使用示例

[ADC单次转换示例](ADC0_example.c)
