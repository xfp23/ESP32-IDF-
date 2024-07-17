**此頻道只提供编程方法,不提供知识点注解**
# 版权信息 
© 2024 未经许可不得复制、修改或分发。 此文献为 [小風的藏書閣](https://t.me/xfp2333) 所有。

# ESP32 - S3
# LED PWM控制器

- ** 占空比功率对应值计算 ： (2 ^ 分辨率) * 想要输出的功率百分比 = 对应数值

## 简介
- LED 控制器 (LEDC) 主要用于控制 LED，也可产生 PWM 信号用于其他设备的控制。该控制器有 8 路通道，可以产生独立的波形，驱动 RGB LED 等设备。

- LEDC 通道共有两组，分别为 8 路高速通道和 8 路低速通道。高速通道模式在硬件中实现，可以自动且无干扰地改变 PWM 占空比。低速通道模式下，PWM 占空比需要由软件中的驱动器改变。每组通道都可以使用不同的时钟源。

- LED PWM 控制器可在无需 CPU 干预的情况下自动改变占空比，实现亮度和颜色渐变。
- 设置 LEDC 通道分三步完成。注意，与 ESP32 不同，ESP32-S3 仅支持设置通道为低速模式。

| 时钟名称     | 时钟频率    | 时钟功能                                 |
| ------------ | ----------- | ---------------------------------------- |
| APB_CLK      | 80 MHz      | /                                        |
| RC_FAST_CLK  | ~ 20 MHz    | 支持动态调频（DFS）功能，支持 Light-sleep 模式 |
| XTAL_CLK     | 40 MHz      | 支持动态调频（DFS）功能                  |

## 环境配置

1. 包含以下头文件
```c
#include "driver/ledc.h"
```
## 定时器配置

- 要设置定时器，可调用函数 ledc_timer_config()，并将包括如下配置参数的数据结构 ***ledc_timer_config_t***

```c
typedef struct {
    ledc_mode_t speed_mode;                /*!< LEDC速度模式，高速模式（仅在esp32上存在）或低速模式 */
    ledc_timer_bit_t duty_resolution;      /*!< LEDC通道的占空比分辨率 */
    ledc_timer_t  timer_num;               /*!< 通道的定时器源（0 - LEDC_TIMER_MAX-1） */
    uint32_t freq_hz;                      /*!< LEDC定时器频率（赫兹） */
    ledc_clk_cfg_t clk_cfg;                /*!< 配置LEDC源时钟，来自ledc_clk_cfg_t。
                                                注意，LEDC_USE_RC_FAST_CLK和LEDC_USE_XTAL_CLK是
                                                非定时器特定的时钟源。不能让一个LEDC定时器使用
                                                RC_FAST_CLK作为时钟源，而另一个LEDC定时器使用XTAL_CLK
                                                作为时钟源。除了esp32和esp32s2外，所有芯片都没有
                                                定时器特定的时钟源，这意味着所有定时器的时钟源
                                                必须是相同的。 */
    bool deconfigure;                      /*!< 将此字段设置为取消配置之前已配置的LEDC定时器
                                                注意，它不会检查要取消配置的定时器是否
                                                绑定到任何通道。此外，必须先暂停定时器
                                                才能取消配置。
                                                当设置此字段时，duty_resolution、freq_hz、clk_cfg字段将被忽略。 */
} ledc_timer_config_t;
//ledc_timer_config_t::deconfigure 将指定定时器重置必须配置此项为 true

typedef enum {
    LEDC_INTR_DISABLE = 0,    /*!< 禁用LEDC中断 */
    LEDC_INTR_FADE_END,       /*!< 启用LEDC中断 */
    LEDC_INTR_MAX,
} ledc_intr_type_t;

typedef enum {
#if SOC_LEDC_SUPPORT_HS_MODE
    LEDC_HIGH_SPEED_MODE = 0, /*!< LEDC高速模式 */
#endif
    LEDC_LOW_SPEED_MODE,      /*!< LEDC低速模式 */
    LEDC_SPEED_MODE_MAX,      /*!< LEDC速度限制 */
} ledc_mode_t;


typedef enum {
    LEDC_TIMER_0 = 0, /*!< LEDC定时器 0 */
    LEDC_TIMER_1,     /*!< LEDC定时器 1 */
    LEDC_TIMER_2,     /*!< LEDC定时器 2 */
    LEDC_TIMER_3,     /*!< LEDC定时器 3 */
    LEDC_TIMER_MAX,   /*!< LEDC定时器的最大数量 */
} ledc_timer_t;


typedef enum {
    LEDC_TIMER_1_BIT = 1,   /*!< LEDC PWM duty resolution of  1 bits */
    LEDC_TIMER_2_BIT,       /*!< LEDC PWM duty resolution of  2 bits */
    LEDC_TIMER_3_BIT,       /*!< LEDC PWM duty resolution of  3 bits */
    LEDC_TIMER_4_BIT,       /*!< LEDC PWM duty resolution of  4 bits */
    LEDC_TIMER_5_BIT,       /*!< LEDC PWM duty resolution of  5 bits */
    LEDC_TIMER_6_BIT,       /*!< LEDC PWM duty resolution of  6 bits */
    LEDC_TIMER_7_BIT,       /*!< LEDC PWM duty resolution of  7 bits */
    LEDC_TIMER_8_BIT,       /*!< LEDC PWM duty resolution of  8 bits */
    LEDC_TIMER_9_BIT,       /*!< LEDC PWM duty resolution of  9 bits */
    LEDC_TIMER_10_BIT,      /*!< LEDC PWM duty resolution of 10 bits */
    LEDC_TIMER_11_BIT,      /*!< LEDC PWM duty resolution of 11 bits */
    LEDC_TIMER_12_BIT,      /*!< LEDC PWM duty resolution of 12 bits */
    LEDC_TIMER_13_BIT,      /*!< LEDC PWM duty resolution of 13 bits */
    LEDC_TIMER_14_BIT,      /*!< LEDC PWM duty resolution of 14 bits */
#if SOC_LEDC_TIMER_BIT_WIDTH > 14
    LEDC_TIMER_15_BIT,      /*!< LEDC PWM duty resolution of 15 bits */
    LEDC_TIMER_16_BIT,      /*!< LEDC PWM duty resolution of 16 bits */
    LEDC_TIMER_17_BIT,      /*!< LEDC PWM duty resolution of 17 bits */
    LEDC_TIMER_18_BIT,      /*!< LEDC PWM duty resolution of 18 bits */
    LEDC_TIMER_19_BIT,      /*!< LEDC PWM duty resolution of 19 bits */
    LEDC_TIMER_20_BIT,      /*!< LEDC PWM duty resolution of 20 bits */
#endif
    LEDC_TIMER_BIT_MAX,
} ledc_timer_bit_t;
typedef enum {
    LEDC_AUTO_CLK = 0,                              /*!< 在初始化定时器时，根据给定的分辨率和占空比参数自动选择源时钟 */
    LEDC_USE_APB_CLK = SOC_MOD_CLK_APB,             /*!< 选择APB作为源时钟 */
    LEDC_USE_RC_FAST_CLK = SOC_MOD_CLK_RC_FAST,     /*!< 选择RC_FAST作为源时钟 */
    LEDC_USE_XTAL_CLK = SOC_MOD_CLK_XTAL,           /*!< 选择XTAL作为源时钟 */

    LEDC_USE_RTC8M_CLK __attribute__((deprecated("请使用'LEDC_USE_RC_FAST_CLK'替代"))) = LEDC_USE_RC_FAST_CLK,   /*!< 'LEDC_USE_RC_FAST_CLK'的别名，此选项已弃用 */
} soc_periph_ledc_clk_src_legacy_t;

//应用配置
esp_err_t ledc_timer_config(const ledc_timer_config_t *timer_conf);
/**
*@brief 辅助函数，用于查找 ledc_timer_config（） 的最大可能占空比分辨率（以位为单位）
*@param 源时钟频率，单位为 Hz。
*@param 定时器频率，单位为Hz
*/
uint32_t ledc_find_suitable_duty_resolution(uint32_t src_clk_freq, uint32_t timer_freq)

```

2.代码示例：
```c
#define LEDC_TIMER              LEDC_TIMER_0           // 使用LEDC定时器0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE    // 使用低速模式
#define LEDC_OUTPUT_IO          (5)                    // 输出到GPIO 5
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT      // 设置占空比分辨率为13位
#define LEDC_FREQUENCY          (4000)                 // 设置频率为4 kHz


    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES, //分辨率
        .freq_hz          = LEDC_FREQUENCY, 
        .clk_cfg          = LEDC_AUTO_CLK //时钟源
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
```
- 如果 ESP32-S3 的定时器选用了 RC_FAST_CLK 作为其时钟源，驱动会通过内部校准来得知这个时钟源的实际频率。这样确保了输出 PWM 信号频率的精准性。
- ESP32-S3 的所有定时器共用一个时钟源。因此 ESP32-S3 不支持给不同的定时器配置不同的时钟源。

## 通道配置

- 配置所需的通道 ***ledc_channel_t***  调用 ***ledc_channel_config()*** 应用配置。

1. 原型：
```c
typedef struct {
    int gpio_num;                   /*!< LEDC 输出的 GPIO 编号，如果你想使用 GPIO16，则 gpio_num = 16 */
    ledc_mode_t speed_mode;         /*!< LEDC 速度模式，高速模式（仅存在于 ESP32）或低速模式 */
    ledc_channel_t channel;         /*!< LEDC 通道 (0 - LEDC_CHANNEL_MAX-1) */
    ledc_intr_type_t intr_type;     /*!< 配置中断，淡入淡出中断启用或淡入淡出中断禁用 */
    ledc_timer_t timer_sel;         /*!< 选择通道的定时器源 (0 - LEDC_TIMER_MAX-1) */
    uint32_t duty;                  /*!< LEDC 通道占空比，设置范围为 [0, (2**duty_resolution)] */
    int hpoint;                     /*!< LEDC 通道高点值，范围为 [0, (2**duty_resolution)-1] */
    struct {
        unsigned int output_invert: 1;/*!< 启用 (1) 或禁用 (0) GPIO 输出反转 */
    } flags;                        /*!< LEDC 标志 */

} ledc_channel_config_t;

typedef enum {
    LEDC_INTR_DISABLE = 0,    /*!< Disable LEDC interrupt */
    LEDC_INTR_FADE_END,       /*!< Enable LEDC interrupt */
    LEDC_INTR_MAX,
} ledc_intr_type_t;

esp_err_t ledc_channel_config(const ledc_channel_config_t *ledc_conf);
esp_err_t ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty);
esp_err_t ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel);
```
2. 代码示例：
```c
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
```

## 改变pwm信号

1. API函数原型：

```c
/**
*@brief LEDC 设置占空比 此功能不会更改此通道的 hpoint 值
*@param  pwm输出模式， LEDC_LOW_SPEED_MODE,      !< LEDC低速模式  LEDC_HIGH_SPEED_MODE = 0, !< LEDC高速模式
*@param pwm通道
*@param pwm占空比
*/
esp_err_t ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty);

/**
* @brief 调用此函数可激活 LEDC 更新的参数。ledc_set_duty后，我们需要调用此函数来更新设置。新的LEDC参数要到下一个PWM周期才会生效。
* @param pwm输出模式， LEDC_LOW_SPEED_MODE,      !< LEDC低速模式  LEDC_HIGH_SPEED_MODE = 0, !< LEDC高速模式
* @param pwm通道
*/
esp_err_t ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel);
```

2. 应用示例：
```c
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (5) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096

ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY));
ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
```

## 改变pwm频率

- API原型：

```c
/**
* @brief 设置信道频率 （Hz）
* @param speed_mode -- 选择具有指定速度模式的 LEDC 通道组
* @param timer_num -- LEDC 定时器索引 （0-3），从ledc_timer_t中选择
* @param freq_hz -- 设置 LEDC 频率
* @return 
*/
esp_err_t ledc_set_freq(ledc_mode_t speed_mode, ledc_timer_t timer_num, uint32_t freq_hz);

/**
* @brief LEDC 获取信道频率 （Hz）
* @param speed_mode -- 选择具有指定速度模式的 LEDC 通道组。请注意，并非所有目标都支持高速模式。
* @param timer_num -- LEDC 定时器索引 （0-3），从ledc_timer_t中选择
* @param 当前 LEDC 频率
*/
uint32_t ledc_get_freq(ledc_mode_t speed_mode, ledc_timer_t timer_num);
```

## 频率和占空比分辨率支持范围

- PWM 频率为 5 kHz 时，占空比分辨率最大可为 13 位。
- LED PWM 控制器可用于生成频率较高的信号，足以为数码相机模组等其他设备提供时钟。此时，最大频率可为 40 MHz，占空比分辨率为 1 位。也就是说，占空比固定为 50%，无法调整。
- LED PWM 控制器 API 会在设定的频率和占空比分辨率超过 LED PWM 控制器硬件范围时报错。例如，试图将频率设置为 20 MHz、占空比分辨率设置为 3 位时