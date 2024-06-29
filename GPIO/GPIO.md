**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息

© 2024 . 未经许可不得复制、修改或分发。 此文献为 [小風的藏書閣](https://t.me/xfp2333) 所有。

# ESP32 GPIO使用指南

## 环境配置

1. 所需头文件
```c
#include "freertos/FreeRTOS.h"
//可选,此部分为rtos依赖库
#include "freertos/task.h"
#include "driver/gpio.h"
```

2. cmake 配置

```c
 REQUIRES driver
```

## 配置引脚
- 配置前需要定义所需引脚
```c
#define GPIO_OUTPUT_IN_SEL ((1ULL << GPIO_NUM_0) | (1ULL) << GPIO_NUM_1)
#define LED0 GPIO_NUM_0
#define LED GPIO_NUM_1
```

1.对下面结构进行配置
```c
gpio_config_t
```
2.配置示例

```c
    gpio_config_t io_conf1;

    // 配置 GPIO 为输出模式
    io_conf1.intr_type = GPIO_INTR_DISABLE;
    io_conf1.mode = GPIO_MODE_OUTPUT;
    io_conf1.pin_bit_mask = GPIO_OUTPUT_IN_SEL;
    io_conf1.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf1.pull_up_en = GPIO_PULLUP_DISABLE;

    // 应用配置
    gpio_config(&io_conf1);
```
## 控制GPIO
1.输出高低电平
```c
 gpio_set_level(LED, 1);//输出高电平
 gpio_set_level(LED, 0);//输出低电平
```
2.延时函数
```c
/*
* @brief esp32的延时是由FreeRTOS支持的,需要包含可选头文件
*/
vTaskDelay(pdMS_TO_TICKS(1000)); // 1 秒钟延时

```
2.读取gpio

```c
gpio_get_level();
```

## 外部中断的注册与触发

1.定义中断函数 **!中断函数不能有返回值**
```c
//中断处理函数
static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;
    // 在这里处理中断事件
    ESP_LOGI(TAG, "GPIO[%d] intr, val: %d\n", gpio_num, gpio_get_level(gpio_num));
}

```
2.配置引脚
- 参考上述标题 配置引脚 内容

3.注册中断服务
```c
/**
  * @brief 安装 GPIO 驱动程序的 ETS_GPIO_INTR_SOURCE ISR 处理程序服务，该服务允许每个引脚 GPIO 中断处理程序。
  *
  * 此函数与 gpio_isr_register() 不兼容 - 如果使用该函数，将为所有 GPIO 中断注册一个全局 ISR。如果使用此函数，ISR 服务将提供全局 GPIO ISR，并且通过 gpio_isr_handler_add() 函数注册各个引脚处理程序。
  *
  * @param intr_alloc_flags 用于分配中断的标志。一个或多个（ORred）
  * ESP_INTR_FLAG_* 值。有关更多信息，请参阅 esp_intr_alloc.h。
  *
  * @返回
  * - ESP_OK 成功
  * - ESP_ERR_NO_MEM 没有内存来安装此服务
  * - ESP_ERR_INVALID_STATE ISR 服务已安装。
  * - ESP_ERR_NOT_FOUND 未找到具有指定标志的空闲中断
  * - ESP_ERR_INVALID_ARG GPIO 错误
  */
  gpio_install_isr_service(0);//中断0
```
4.绑定中断引脚
```c
/*
*@brief 为特定GPIO引脚注册中断处理函数
*@param GPIO引脚编号
*@param 中断服务函数
*@param GPIO引脚编号
*/
gpio_isr_handler_add(GPIO_INPUT_IO, gpio_isr_handler, (void*) GPIO_INPUT_IO);
```


## 上述类型定义:
```c
typedef enum {
    GPIO_INTR_DISABLE = 0,     /*!< 禁用GPIO中断                                     */
    GPIO_INTR_POSEDGE = 1,     /*!< GPIO中断类型：上升沿触发                        */
    GPIO_INTR_NEGEDGE = 2,     /*!< GPIO中断类型：下降沿触发                        */
    GPIO_INTR_ANYEDGE = 3,     /*!< GPIO中断类型：上升沿和下降沿都触发              */
    GPIO_INTR_LOW_LEVEL = 4,   /*!< GPIO中断类型：输入低电平触发                    */
    GPIO_INTR_HIGH_LEVEL = 5,  /*!< GPIO中断类型：输入高电平触发                    */
    GPIO_INTR_MAX,
} gpio_int_type_t;

/** @cond */
#define GPIO_MODE_DEF_DISABLE         (0)
#define GPIO_MODE_DEF_INPUT           (BIT0)    ///< 输入的位掩码
#define GPIO_MODE_DEF_OUTPUT          (BIT1)    ///< 输出的位掩码
#define GPIO_MODE_DEF_OD              (BIT2)    ///< 开漏模式的位掩码
/** @endcond */

typedef enum {
    GPIO_MODE_DISABLE = GPIO_MODE_DEF_DISABLE,                                                         /*!< GPIO模式：禁用输入和输出                       */
    GPIO_MODE_INPUT = GPIO_MODE_DEF_INPUT,                                                             /*!< GPIO模式：仅输入模式                           */
    GPIO_MODE_OUTPUT = GPIO_MODE_DEF_OUTPUT,                                                           /*!< GPIO模式：仅输出模式                           */
    GPIO_MODE_OUTPUT_OD = ((GPIO_MODE_DEF_OUTPUT) | (GPIO_MODE_DEF_OD)),                               /*!< GPIO模式：开漏输出模式                         */
    GPIO_MODE_INPUT_OUTPUT_OD = ((GPIO_MODE_DEF_INPUT) | (GPIO_MODE_DEF_OUTPUT) | (GPIO_MODE_DEF_OD)), /*!< GPIO模式：开漏输出和输入模式                   */
    GPIO_MODE_INPUT_OUTPUT = ((GPIO_MODE_DEF_INPUT) | (GPIO_MODE_DEF_OUTPUT)),                         /*!< GPIO模式：输入输出模式                         */
} gpio_mode_t;

typedef enum {
    GPIO_PULLUP_DISABLE = 0x0,     /*!< 禁用GPIO上拉电阻 */
    GPIO_PULLUP_ENABLE = 0x1,      /*!< 启用GPIO上拉电阻 */
} gpio_pullup_t;

typedef enum {
    GPIO_PULLDOWN_DISABLE = 0x0,   /*!< 禁用GPIO下拉电阻 */
    GPIO_PULLDOWN_ENABLE = 0x1,    /*!< 启用GPIO下拉电阻 */
} gpio_pulldown_t;

typedef enum {
    GPIO_PULLUP_ONLY,               /*!< 仅上拉                  */
    GPIO_PULLDOWN_ONLY,             /*!< 仅下拉                  */
    GPIO_PULLUP_PULLDOWN,           /*!< 上拉和下拉              */
    GPIO_FLOATING,                  /*!< 悬空                    */
} gpio_pull_mode_t;

typedef enum {
    GPIO_DRIVE_CAP_0       = 0,    /*!< 弱驱动能力          */
    GPIO_DRIVE_CAP_1       = 1,    /*!< 较强驱动能力        */
    GPIO_DRIVE_CAP_2       = 2,    /*!< 中等驱动能力        */
    GPIO_DRIVE_CAP_DEFAULT = 2,    /*!< 中等驱动能力        */
    GPIO_DRIVE_CAP_3       = 3,    /*!< 最强驱动能力        */
    GPIO_DRIVE_CAP_MAX,
} gpio_drive_cap_t;

/**
 * @brief GPIO配置滞后特性的可用选项
 */
typedef enum {
    GPIO_HYS_CTRL_EFUSE     = 0,    /*!< 通过efuse控制输入滞后特性 */
    GPIO_HYS_SOFT_ENABLE    = 1,    /*!< 通过软件启用输入滞后特性 */
    GPIO_HYS_SOFT_DISABLE   = 2,    /*!< 通过软件禁用输入滞后特性 */
} gpio_hys_ctrl_mode_t;
```

## GPIO控制的其他函数原型
[其他gpio控制函数](gpio.h)
## 完整项目程序示例
- [外部中断](gpio_isr.c)
- [控制LED](gpio_led.c)
