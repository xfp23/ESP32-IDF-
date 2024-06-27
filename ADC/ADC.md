**本人只提供idf编程方法，不提供知识点注解**

# 版权信息

© 2024 [xfp23]. 未经许可不得复制、修改或分发。
此文献为 GitHub 作者 **xfp23** 所有。


# ESP32 ADC使用指南

- ESP32 芯片集成了模数转换器 (ADC)，支持测量特定模拟 IO 管脚的模拟信号。此外，ADC 还支持直接内存访问 (DMA) 功能，高效获取 ADC 转换结果。
- ESP32 具有 两 个 ADC 单元，可应用于以下场景：
1. 成单次 ADC 转换结果
2. 生成连续 ADC 转换结果
3. 此文档为连续转换

# 编程实现

## 环境配置
1.包含的头文件：

```c
#include "hal/adc_types.h"
#include "esp_adc/adc_oneshot.h"
```
2.cmake配置：

```c
REQUIRES esp_adc
```

## ADC设置

### 初始化ADC连续转换模式驱动

***ADC 连续转换模式驱动基于 ESP32 SAR ADC 模块实现，不同的 ESP 目标芯片可能拥有不同数量的独立 ADC***
1. 配置分配资源结构体 **ADC驱动**
  ```c
adc_continuous_handle_cfg_t adc_config={
.max_store_buf_size,//以字节单位设置最大缓冲池大小，驱动将ADC转换结果保存到缓冲池中。缓冲池已满（数据溢出），自动清零、
.conv_frame_size,//以字节为单位设置ADC转换帧大小
.flags//设置可以改变驱动程序行为的标志
}
```
2. 经过上述结构配置后，调用函数设定
```c
adc_continuous_nes_handle(&adc_config);
```

**完整代码示例**
```c
adc_continuous_handle_cfg_t adc_config = {
    .max_store_buf_size = 1024,
    .conv_frame_size = 100,
};
ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config));

//此函数回收驱动
ESP_ERROR_CHECK(adc_continuous_deinit());
```

### 配置ADC

**初始化驱动之后，进行以下操作**

1.配置 **adc_continuous_config_t** 结构体

```c
adc_continuous_config_t esp_adc_cfg = {
    .pattern_num = 1, // 要使用的ADC通道数量
    .adc_pattern = { // 每个要使用的ADC通道配置
        {
            .atten = ADC_ATTEN_DB_0,      // 该通道的ADC衰减
            .channel = ADC_CHANNEL_0,     // ADC通道号
            .unit = ADC_UNIT_1,           // IO所属的单元
            .bit_width = ADC_WIDTH_BIT_12 // 原始转换结果的位宽
        } ......
    },
    .sample_freq_hz = 1000,                // 频率 (Hz)
    .conv_mode = ADC_CONV_SINGLE_UNIT_1,   // 连续转换模式
    .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1 // 转换模式结果的输出格式
};

/*
对于 IO 对应的 ADC 通道号，请参阅 技术参考手册 获取 ADC IO 管脚的详细信息。另外，可以使用 adc_continuous_io_to_channel() 和
adc_continuous_channel_to_io() 获取 ADC 通道和 ADC IO 的对应关系。
*/

adc_continous_config(&esp_adc_cfg);
```

2.上述结构的枚举类型：(**来自乐鑫官方**)

```c
/**
 * @brief ADC 单元
 */
typedef enum {
    ADC_UNIT_1,        ///< SAR ADC 1
    ADC_UNIT_2,        ///< SAR ADC 2
} adc_unit_t;

/**
 * @brief ADC 通道
 */
typedef enum {
    ADC_CHANNEL_0,     ///< ADC 通道 0
    ADC_CHANNEL_1,     ///< ADC 通道 1
    ADC_CHANNEL_2,     ///< ADC 通道 2
    ADC_CHANNEL_3,     ///< ADC 通道 3
    ADC_CHANNEL_4,     ///< ADC 通道 4
    ADC_CHANNEL_5,     ///< ADC 通道 5
    ADC_CHANNEL_6,     ///< ADC 通道 6
    ADC_CHANNEL_7,     ///< ADC 通道 7
    ADC_CHANNEL_8,     ///< ADC 通道 8
    ADC_CHANNEL_9,     ///< ADC 通道 9
} adc_channel_t;

/**
 * @brief ADC 衰减参数。不同的参数决定 ADC 的测量范围。
 */
typedef enum {
    ADC_ATTEN_DB_0   = 0,  ///< 无输入衰减，ADC 最大可测量约 1.1V
    ADC_ATTEN_DB_2_5 = 1,  ///< ADC 的输入电压将被衰减，测量范围扩展约 2.5 dB (1.33 倍)
    ADC_ATTEN_DB_6   = 2,  ///< ADC 的输入电压将被衰减，测量范围扩展约 6 dB (2 倍)
    ADC_ATTEN_DB_11  = 3,  ///< ADC 的输入电压将被衰减，测量范围扩展约 11 dB (3.55 倍)
} adc_atten_t;

/**
 * @brief ADC 位宽
 */
typedef enum {
    ADC_BITWIDTH_DEFAULT = 0, ///< 默认 ADC 输出位宽，将选择最大支持的宽度
    ADC_BITWIDTH_9  = 9,      ///< ADC 输出位宽为 9 位
    ADC_BITWIDTH_10 = 10,     ///< ADC 输出位宽为 10 位
    ADC_BITWIDTH_11 = 11,     ///< ADC 输出位宽为 11 位
    ADC_BITWIDTH_12 = 12,     ///< ADC 输出位宽为 12 位
    ADC_BITWIDTH_13 = 13,     ///< ADC 输出位宽为 13 位
} adc_bitwidth_t;

/**
 * @brief ADC ULP 模式
 */
typedef enum {
    ADC_ULP_MODE_DISABLE = 0, ///< ADC ULP 模式禁用
    ADC_ULP_MODE_FSM     = 1, ///< ADC 由 ULP FSM 控制
    ADC_ULP_MODE_RISCV   = 2, ///< ADC 由 ULP RISCV 控制
} adc_ulp_mode_t;

/**
 * @brief ADC 数字控制器 (DMA 模式) 工作模式
 */
typedef enum {
    ADC_CONV_SINGLE_UNIT_1 = 1,  ///< 仅使用 ADC1 进行转换
    ADC_CONV_SINGLE_UNIT_2 = 2,  ///< 仅使用 ADC2 进行转换
    ADC_CONV_BOTH_UNIT     = 3,  ///< 同时使用 ADC1 和 ADC2 进行转换
    ADC_CONV_ALTER_UNIT    = 7,  ///< 交替使用 ADC1 和 ADC2 进行转换，例如 ADC1 -> ADC2 -> ADC1 -> ADC2 .....
} adc_digi_convert_mode_t;

/**
 * @brief ADC 数字控制器 (DMA 模式) 输出数据格式选项
 */
typedef enum {
    ADC_DIGI_OUTPUT_FORMAT_TYPE1,   ///< 参见 `adc_digi_output_data_t.type1`
    ADC_DIGI_OUTPUT_FORMAT_TYPE2,   ///< 参见 `adc_digi_output_data_t.type2`
} adc_digi_output_format_t;

```

### ADC控制

1. **启动和停止**
   ```c
   adc_continuous_start();//启动ADC
   adc_continuous_stop();//停止ADC

   adc_continous_read();//获取通道的转换结果
   ```

2. **注册事件回调**
   
   1.调用 adc_continuous_register_event_callbacks()，可以将自己的函数链接到驱动程序的 ISR 中。
   2.通过 adc_continuous_evt_cbs_t 可查看所有支持的事件回调。

  3.adc_continuous_evt_cbs_t::on_conv_done：当一个转换帧完成时，触发此事件。

  4.adc_continuous_evt_cbs_t::on_pool_ovf：当内部缓冲池已满时，触发此事件，新的转换结果将丢失。

  5.由于上述回调函数在 ISR 中调用，请确保回调函数适合在 ISR 上下文中运行，且这些回调不应涉及阻塞逻辑。回调函数的原型在 adc_continuous_callback_t 中声明。

 6.在调用 adc_continuous_register_event_callbacks() 时，还可以通过参数 user_data 注册自己的上下文，该用户数据将直接传递给回调函数。

 ```c
// 回调函数，用于处理转换完成事件
void adc_conv_done_callback(adc_continuous_handle_t handle, const adc_event_data_t *event_data, void *user_data) {
    // 确认事件类型为转换完成
    if (event_data->type == ADC_EVENT_CONTINUOUS_CONV_DONE) {
        // 声明缓冲区用于存储读取的ADC数据
        uint8_t result_buffer[1024];
        size_t result_size = 0;
        
        // 读取ADC数据
        esp_err_t ret = adc_continuous_read(handle, result_buffer, sizeof(result_buffer), &result_size, 0);
        if (ret == ESP_OK) {
            ESP_LOGI("ADC", "读取 ADC 数据，大小: %d", result_size);
            // 处理读取到的 ADC 数据
        } else {
            ESP_LOGE("ADC", "读取 ADC 数据失败: %s", esp_err_to_name(ret));
        }
    }
}

// 回调函数，用于处理缓冲池溢出事件
void adc_pool_ovf_callback(adc_continuous_handle_t handle, const adc_event_data_t *event_data, void *user_data) {
    // 确认事件类型为缓冲池溢出
    if (event_data->type == ADC_EVENT_CONTINUOUS_POOL_OVF) {
        ESP_LOGW("ADC", "缓冲池已满，新的转换结果将丢失");
        // 处理缓冲池溢出情况，例如清空缓冲区或调整采样率
    }
}
// 定义事件回调结构体
    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = adc_conv_done_callback, // 注册转换完成事件的回调函数
        .on_pool_ovf = adc_pool_ovf_callback,   // 注册缓冲池溢出事件的回调函数
    };
// 注册事件回调函数
    ret = adc_continuous_register_event_callbacks(adc_handle, &cbs, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE("ADC", "注册事件回调函数失败: %s", esp_err_to_name(ret));
        adc_continuous_deinit(adc_handle);
        return;
    }
```
