**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息

© 2024 . 未经许可不得复制、修改或分发。 此文献为 [小風的藏書閣](https://t.me/xfp2333) 所有。

# 模数转换器 (ADC) 单次转换模式驱动编程指南

# 简介

**模数转换器集成于芯片，支持测量特定模拟 IO 管脚的模拟信号。**

- ESP32-S3 有 两 个 ADC 单元，可以在以下场景使用：

- 生成 ADC 单次转换结果

- 生成连续 ADC 转换结果

本指南介绍了ADC单次转换模式，若想查看ADC连续转换，请[跳转此文档](ADC.md)

# 环境配置

1. 包含头文件:
```c
#include "hal/adc_types.h"
```

2. CMAKE配置：
```c
REQUIRES esp_adc
```

# 资源分配

ADC 单次转换模式驱动基于 ESP32-S3 SAR ADC 模块实现，不同的 ESP 芯片可能拥有不同数量的独立 ADC。对于单次转换模式驱动而言，ADC 实例以 `adc_oneshot_unit_handle_t` 表示

-  配置结构体 `adc_oneshot_unit_init_cfg_t`安装ADC实例

1. 类型定义:

```c

//句柄类型:
adc_oneshot_unit_handle_t;
typedef struct {
    adc_unit_t unit_id;             ///< ADC 单元
    adc_oneshot_clk_src_t clk_src;  ///< 时钟源
    adc_ulp_mode_t ulp_mode;        ///< 由 ULP 控制的 ADC，请参见 `adc_ulp_mode_t`
} adc_oneshot_unit_init_cfg_t;

//选择ADC单元:
typedef enum {
    ADC_UNIT_1,        ///< SAR ADC 1
    ADC_UNIT_2,        ///< SAR ADC 2
} adc_unit_t;

//选择ADC时钟源:

typedef soc_periph_adc_rtc_clk_src_t     adc_oneshot_clk_src_t;  
typedef enum {
    ADC_RTC_CLK_SRC_RC_FAST = SOC_MOD_CLK_RC_FAST,      /*!< 选择 RC_FAST 作为源时钟 */
    ADC_RTC_CLK_SRC_DEFAULT = SOC_MOD_CLK_RC_FAST,      /*!< 选择 RC_FAST 作为默认时钟选择 */
} soc_periph_adc_rtc_clk_src_t;

//低功耗选择：
typedef enum {
    ADC_ULP_MODE_DISABLE = 0, ///< ADC ULP 模式禁用
    ADC_ULP_MODE_FSM     = 1, ///< ADC 由 ULP FSM 控制 ULP FSM 可以在主 CPU 休眠时定期唤醒并执行一些预定义的任务，包括控制 ADC 进行数据采集。
    ADC_ULP_MODE_RISCV   = 2, ///< ADC 由 ULP RISCV 控制 ADC 由 ULP RISCV 控制。ULP RISCV 是一个更强大的处理器，可以执行更复杂的任务，包括控制 ADC 进行数据采集和处理
} adc_ulp_mode_t;


//应用配置:

esp_err_t adc_oneshot_new_unit(const adc_oneshot_unit_init_cfg_t *init_config, adc_oneshot_unit_handle_t *ret_unit);
```

2. 示例代码:

```c
adc_oneshot_unit_handle_t adc1_handle;
adc_oneshot_unit_init_cfg_t init_config1 = {
    .unit_id = ADC_UNIT_1,
    .ulp_mode = ADC_ULP_MODE_DISABLE,
};
ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
```

# 回收ADC单元实例

```c
ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
```

# 配置ADC单元实例

- 创建 ADC 单元实例后，请设置 `adc_oneshot_chan_cfg_t` 配置 ADC IO 以测量模拟信号
- 为使以上设置生效，请使用上述配置结构体调用 `adc_oneshot_config_channel()`，并指定要配置的 ADC 通道。函数 `adc_oneshot_config_channel()` 支持多次调用，以配置不同的 ADC 通道。驱动程序将在内部保存每个通道的配置。

- 由于ADC相对复杂，此处提供了[部分知识点](ADC_POINT.md)

- **EP32-S3 GPIO & 通道 对照表**：


|--------------------------------------------------------|
| 管脚名/信号名/GPIO | Sar_Mux | ADC 选择 (Sel)           |
|--------------------|---------|-------------------------|
| GPIO1              | 0       |                         |
| GPIO2              | 1       |                         |
| GPIO3              | 2       |                         |
| GPIO4              | 3       |                         |
| GPIO5              | 4       |                         |
| GPIO6              | 5       | **Sel=0,选择SAR ADC1**  |
| GPIO7              | 6       |                         |
| GPIO8              | 7       |                         |
| GPIO9              | 8       |                         |
| GPIO10             | 9       |                         |
| -------------------|---------|-------------------------|
| GPIO11             | 0       |                         |
| GPIO12             | 1       |                         |
| GPIO13             | 2       |                         |
| GPIO14             | 3       |                         |
| GPIO15             | 4       |                         |
| GPIO16             | 5       | **Sel=1,选择 SAR ADC2** |
| GPIO17             | 6       |                         |
| GPIO18             | 7       |                         |
| GPIO19             | 8       |                         |
| GPIO20             | 9       |                         |
|--------------------------------------------------------|

- **ADC转换与衰减**


1. 配置类型声明:

```c
typedef struct {
    adc_atten_t atten;              ///< ADC 衰减
    adc_bitwidth_t bitwidth;        ///< ADC 转换结果位数
} adc_oneshot_chan_cfg_t;


//选择ADC衰减信号
typedef enum {
    ADC_ATTEN_DB_0   = 0,  ///< 无输入衰减，ADC可以测量最高约为0 dB的输入电压
    ADC_ATTEN_DB_2_5 = 1,  ///< 输入电压会被衰减，测量范围扩展约为2.5 dB
    ADC_ATTEN_DB_6   = 2,  ///< 输入电压会被衰减，测量范围扩展约为6 dB
    ADC_ATTEN_DB_12  = 3,  ///< 输入电压会被衰减，测量范围扩展约为12 dB
    ADC_ATTEN_DB_11 __attribute__((deprecated)) = ADC_ATTEN_DB_12,  ///< 已弃用，与 `ADC_ATTEN_DB_12` 行为相同
} adc_atten_t;

//选择转换位数:
typedef enum {
    ADC_BITWIDTH_DEFAULT = 0, ///< 默认的 ADC 输出位宽，最大支持的位宽将被选择
    ADC_BITWIDTH_9  = 9,      ///< ADC 输出位宽为 9 位
    ADC_BITWIDTH_10 = 10,     ///< ADC 输出位宽为 10 位
    ADC_BITWIDTH_11 = 11,     ///< ADC 输出位宽为 11 位
    ADC_BITWIDTH_12 = 12,     ///< ADC 输出位宽为 12 位
    ADC_BITWIDTH_13 = 13,     ///< ADC 输出位宽为 13 位
} adc_bitwidth_t;

/**
* @brief 设置 ADC oneshot 模式所需的配置。
* @param adc单次转换句柄
* @param adc通道
* @param adc配置单元结构地址
* @return ESP调试信息
*/
esp_err_t adc_oneshot_config_channel(adc_oneshot_unit_handle_t handle, adc_channel_t channel, const adc_oneshot_chan_cfg_t *config);
```

2. 配置两个ADC通道：
```c
adc_oneshot_chan_cfg_t config = {
    .bitwidth = ADC_BITWIDTH_DEFAULT,
    .atten = ADC_ATTEN_DB_12,
};
ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_0, &config));
ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_1, &config));
```

# 读取转换结果

- 调用 `adc_oneshot_read()` 可以获取 ADC 通道的原始转换结果
1. 通过该函数获取的 ADC 转换结果为原始数据。可以使用以下公式，根据 ADC 原始结果计算电压：
```c
 Vout = Dout * Vmax / Dmax 
 ```
| 符号     | 说明                                                                                                             |
|----------|------------------------------------------------------------------------------------------------------------------|
| Vout     | **数字输出结果，代表电压**                                                                                       |
| Dout输出 | ADC 原始数字读取结果                                                                                            |
| Vmax     | 可测量的最大模拟输入电压，与 ADC 衰减无关。请参考 [技术手册->片上传感器与模拟信号处理](PDF/ESP32-S3.pdf)         |
| Dmax     | 输出 ADC 原始数字读取结果的最大值，即 \(2^{\text{位宽}}\)，位宽即之前配置的 `adc_digi_pattern_config_t::bit_width` |

 2. 函数原型:
 ```c
 /**
 * @brief 读取ADC转换
 * @param adc单次转换句柄
 * @param 要读取的adc通道
 * @param 存放结果的整数地址
 * @return ESP调试信息 
 */
esp_err_t adc_oneshot_read(adc_oneshot_unit_handle_t handle, adc_channel_t chan, int *out_raw);

 ```
 3. 若需进一步校准，将 ADC 原始结果转换为以 mV 为单位的电压数据，[ADC校准指南](ADC_calibration.md)。

 4. 代码示例:
 ```c
ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN0, &adc_raw[0][0]));
ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN0, adc_raw[0][0]);

ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN1, &adc_raw[0][1]));
ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN1, adc_raw[0][1]);
 ```


----------------------------------------------------------------------------

[ADC单次转换使用示例](/ADC/ADC0_example.c)
