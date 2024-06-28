/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "soc/soc_caps.h"
#include "hal/gpio_types.h"
#include "esp_rom_gpio.h"
#include "driver/gpio_etm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_PIN_COUNT                      (SOC_GPIO_PIN_COUNT)
/// Check whether it is a valid GPIO number
#define GPIO_IS_VALID_GPIO(gpio_num)        ((gpio_num >= 0) && \
                                              (((1ULL << (gpio_num)) & SOC_GPIO_VALID_GPIO_MASK) != 0))
/// Check whether it can be a valid GPIO number of output mode
#define GPIO_IS_VALID_OUTPUT_GPIO(gpio_num) ((gpio_num >= 0) && \
                                              (((1ULL << (gpio_num)) & SOC_GPIO_VALID_OUTPUT_GPIO_MASK) != 0))
/// Check whether it can be a valid digital I/O pad
#define GPIO_IS_VALID_DIGITAL_IO_PAD(gpio_num) ((gpio_num >= 0) && \
                                                 (((1ULL << (gpio_num)) & SOC_GPIO_VALID_DIGITAL_IO_PAD_MASK) != 0))

typedef intr_handle_t gpio_isr_handle_t;

/**
 * @brief GPIO interrupt handler
 *
 * @param arg User registered data
 */
typedef void (*gpio_isr_t)(void *arg);

/**
 * @brief Configuration parameters of GPIO pad for gpio_config function
 */
typedef struct {
    uint64_t pin_bit_mask;          /*!< GPIO 引脚：设置位掩码，每个位映射到一个 GPIO              */
    gpio_mode_t mode;               /*!< GPIO mode: set input/output mode                     */
    gpio_pullup_t pull_up_en;       /*!< GPIO pull-up                                         */
    gpio_pulldown_t pull_down_en;   /*!< GPIO pull-down                                       */
    gpio_int_type_t intr_type;      /*!< GPIO interrupt type                                  */
#if SOC_GPIO_SUPPORT_PIN_HYS_FILTER
    gpio_hys_ctrl_mode_t hys_ctrl_mode;       /*!< GPIO hysteresis: hysteresis filter on slope input    */
#endif
} gpio_config_t;

/**
 * @brief GPIO常用配置
 *
 * 配置GPIO的模式，上拉，下拉，IntrType
 *
 * @param pGPIOConfig 指向 GPIO 配置结构体的指针
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 *
 */
esp_err_t gpio_config(const gpio_config_t *pGPIOConfig);

/**
 * @brief 将 GPIO 重置为默认状态（选择 GPIO 功能、启用上拉并禁用输入和输出）。
 *
 * @param gpio_num GPIO 编号。
 *
 * @note 此函数还将该引脚的 IOMUX 配置为 GPIO
 * 功能，并断开通过 GPIO 配置的任何其他外设输出
 *       矩阵。
 *
 * @return 始终返回 ESP_OK。
 */
esp_err_t gpio_reset_pin(gpio_num_t gpio_num);

/**
 * @brief GPIO设置中断触发类型
 *
 * @param gpio_num GPIO 编号。如果您想设置例如触发类型对于 GPIO16，gpio_num 应为 GPIO_NUM_16 (16)；
 * @param intr_type 中断类型，从gpio_int_type_t中选择
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 *
 */
esp_err_t gpio_set_intr_type(gpio_num_t gpio_num, gpio_int_type_t intr_type);

/**
 * @brief 启用GPIO模块中断信号
 *
 * @note ESP32: 当使用 ADC 或启用睡眠模式的 Wi-Fi 和蓝牙时，请不要使用 GPIO36 和 GPIO39 的中断。
 * 请参考`adc1_get_raw`的注释。
 * 请参阅 <a href="https://espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_en.pdf">ESP32 ECO 和错误解决方法</a>的第 3.11 节了解此问题的描述。

 *
 * @param gpio_num GPIO 编号。如果您想启用中断，例如GPIO16，gpio_num 应为 GPIO_NUM_16 (16)；
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 *
 */
esp_err_t gpio_intr_enable(gpio_num_t gpio_num);

/**
 * @brief 禁用GPIO模块中断信号
 *
 * @note 当在 ISR 上下文中禁用缓存时，通过启用“CONFIG_GPIO_CTRL_FUNC_IN_IRAM”，允许执行此函数
 *
 * @param gpio_num GPIO 编号。如果你想禁用例如中断GPIO16，gpio_num 应为 GPIO_NUM_16 (16)；
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 *
 */
esp_err_t gpio_intr_disable(gpio_num_t gpio_num);

/**
 * @brief GPIO设置输出电平
 *
 * @note 当在 ISR 上下文中禁用缓存时，通过启用“CONFIG_GPIO_CTRL_FUNC_IN_IRAM”，允许执行此函数
 *
 * @param gpio_num GPIO 编号。如果您想设置例如的输出电平GPIO16，gpio_num 应为 GPIO_NUM_16 (16)；
 * @param level 输出级别。 0：低； 1：高
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG GPIO 编号错误
 *
 */
esp_err_t gpio_set_level(gpio_num_t gpio_num, uint32_t level);

/**
 * @brief GPIO 获取输入电平
 *
 * @warning 如果焊盘未配置为输入（或输入和输出），则返回值始终为 0。
 *
 * @param gpio_num GPIO 编号。如果你想获得例如的逻辑级别引脚 GPIO16，gpio_num 应为 GPIO_NUM_16 (16)；
 *
 * @返回
 * - 0 GPIO输入电平为0
 * - 1 GPIO输入电平为1
 *
 */
int gpio_get_level(gpio_num_t gpio_num);

/**
 * @brief GPIO 设置方向
 *
 * 配置GPIO方向，如output_only、input_only、output_and_input
 *
 * @param gpio_num 配置GPIO管脚编号，应该是GPIO编号。如果你想设置例如方向GPIO16，gpio_num 应为 GPIO_NUM_16 (16)；
 * @param mode GPIO方向
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG GPIO 错误
 *
 */
esp_err_t gpio_set_direction(gpio_num_t gpio_num, gpio_mode_t mode);

/**
 * @brief 配置GPIO上拉/下拉电阻
 *
 * @note ESP32：只有支持输入和输出的引脚才具有集成的上拉和下拉电阻。仅输入 GPIO 34-39 则没有。
 *
 * @param gpio_num GPIO 编号。如果您想设置上拉或下拉模式，例如GPIO16，gpio_num 应为 GPIO_NUM_16 (16)；
 * @param pull GPIO 上拉/下拉模式。
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG : 参数错误
 *
 */
esp_err_t gpio_set_pull_mode(gpio_num_t gpio_num, gpio_pull_mode_t pull);

/**
 * @brief 启用GPIO唤醒功能。
 *
 * @param gpio_num GPIO 编号。
 *
 * @param intr_type GPIO 唤醒类型。只能使用 GPIO_INTR_LOW_LEVEL 或 GPIO_INTR_HIGH_LEVEL。
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t gpio_wakeup_enable(gpio_num_t gpio_num, gpio_int_type_t intr_type);

/**
 * @brief Disable GPIO wake-up function.
 *
 * @param gpio_num GPIO number
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t gpio_wakeup_disable(gpio_num_t gpio_num);

/**
 * @brief 注册GPIO中断处理程序，该处理程序是一个ISR。
 * 处理程序将附加到运行此函数的同一 CPU 核心。
 *
 * 每当任何 GPIO 中断发生时，都会调用此 ISR 函数。看
 * 替代 gpio_install_isr_service() 和
 * gpio_isr_handler_add() API以获得驱动程序支持
 * 每个 GPIO ISR。
 *
 * @param fn 中断处理函数。
 * @param arg 处理函数的参数
 * @param intr_alloc_flags 用于分配中断的标志。一个或多个（ORred）
 * ESP_INTR_FLAG_* 值。有关更多信息，请参阅 esp_intr_alloc.h。
 * @param handle 返回句柄的指针。如果非 NULL，则此处将返回中断句柄。
 *
 * \逐字嵌入：rst：前导星号
 * 要禁用或删除 ISR，请将返回的句柄传递给:doc:`中断分配函数 </api-reference/system/intr_alloc>`。
 * \逐字结束
 *
 * @返回
 * - ESP_OK 成功；
 * - ESP_ERR_INVALID_ARG GPIO 错误
 * - ESP_ERR_NOT_FOUND 未找到具有指定标志的空闲中断
 */
esp_err_t gpio_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags, gpio_isr_handle_t *handle);

/**
  * @brief 在 GPIO 上启用上拉。
  *
  * @param gpio_num GPIO编号
  *
  * @返回
  * - ESP_OK 成功
  * - ESP_ERR_INVALID_ARG 参数错误
  */
esp_err_t gpio_pullup_en(gpio_num_t gpio_num);

/**
  * @brief Disable pull-up on GPIO.
  *
  * @param gpio_num GPIO number
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_INVALID_ARG Parameter error
  */
esp_err_t gpio_pullup_dis(gpio_num_t gpio_num);

/**
  * @brief Enable pull-down on GPIO.
  *
  * @param gpio_num GPIO number
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_INVALID_ARG Parameter error
  */
esp_err_t gpio_pulldown_en(gpio_num_t gpio_num);

/**
  * @brief Disable pull-down on GPIO.
  *
  * @param gpio_num GPIO number
  *
  * @return
  *     - ESP_OK Success
  *     - ESP_ERR_INVALID_ARG Parameter error
  */
esp_err_t gpio_pulldown_dis(gpio_num_t gpio_num);

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
esp_err_t gpio_install_isr_service(int intr_alloc_flags);

/**
  * @brief Uninstall the driver's GPIO ISR service, freeing related resources.
  */
void gpio_uninstall_isr_service(void);

/**
  * @brief 为相应的 GPIO 引脚添加 ISR 处理程序。
  *
  * 使用gpio_install_isr_service()后调用该函数
  * 安装驱动程序的 GPIO ISR 处理程序服务。
  *
  * 引脚 ISR 处理程序不再需要用 IRAM_ATTR 声明，
  * 除非您在分配时传递 ESP_INTR_FLAG_IRAM 标志
  * gpio_install_isr_service() 中的 ISR。
  *
  * 该 ISR 处理程序将从 ISR 中调用。于是就有了一个栈
  * 大小限制（可在 menuconfig 中配置为“ISR 堆栈大小”）。这
  * 与全局 GPIO 中断处理程序相比，限制较小，因为
  * 到额外的间接级别。
  *
  * @param gpio_num GPIO编号
  * @param isr_handler 对应GPIO编号的ISR处理函数。
  * @param args ISR 处理程序的参数。
  *
  * @返回
  * - ESP_OK 成功
  * - ESP_ERR_INVALID_STATE 错误状态，ISR 服务尚未初始化。
  * - ESP_ERR_INVALID_ARG 参数错误
  */
esp_err_t gpio_isr_handler_add(gpio_num_t gpio_num, gpio_isr_t isr_handler, void *args);

/**
  * @brief 删除相应 GPIO 引脚的 ISR 处理程序。
  *
  * @param gpio_num GPIO编号
  *
  * @返回
  * - ESP_OK 成功
  * - ESP_ERR_INVALID_STATE 错误状态，ISR 服务尚未初始化。
  * - ESP_ERR_INVALID_ARG 参数错误
  */
esp_err_t gpio_isr_handler_remove(gpio_num_t gpio_num);

/**
  * @brief 设置GPIO pad驱动能力
  *
  * @param gpio_num GPIO编号，仅支持输出GPIO
  * @param Strength 打击垫的驱动能力
  *
  * @返回
  * - ESP_OK 成功
  * - ESP_ERR_INVALID_ARG 参数错误
  */
esp_err_t gpio_set_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t strength);

/**
  * @brief 获取GPIO pad驱动能力
  *
  * @param gpio_num GPIO编号，仅支持输出GPIO
  * @param Strength 接受焊盘驱动能力的指针
  *
  * @返回
  * - ESP_OK 成功
  * - ESP_ERR_INVALID_ARG 参数错误
  */
esp_err_t gpio_get_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t *strength);

/**
  * @brief 启用 gpio pad 保持功能。
  *
  * 当 GPIO 设置为保持时，其状态在该时刻被锁存，并且当内部
  * 信号或IO MUX/GPIO配置被修改（包括输入使能、输出使能、输出值、
  * 功能和驱动强度值）。该函数可用于在芯片启动时保留 GPIO 的状态
  * 或系统复位，例如，当触发看门狗超时或深度睡眠事件时。
  *
  * 该功能在输入和输出模式下均有效，并且仅适用于具有输出功能的GPIO。
  * 如果启用此功能：
  * 输出模式下：GPIO 的输出电平将被锁定且无法更改。
  * 输入模式下：输入读取值仍能反映输入信号的变化。
  *
  * 但是，在 ESP32/S2/C3/S3/C2 上，此功能不能用于在 Deep-sleep 期间保持数字 GPIO 的状态。
  * 即使启用此功能，当芯片从唤醒状态唤醒时，数字 GPIO 也会重置为默认状态。
  * 沉睡。如果您想在深度睡眠期间保持数字 GPIO 的状态，请调用 `gpio_deep_sleep_hold_en`。
  *
  * 断电或调用 `gpio_hold_dis` 将禁用此功能。
  *
  * @param gpio_num GPIO编号，仅支持有输出功能的GPIO
  *
  * @返回
  * - ESP_OK 成功
  * - ESP_ERR_NOT_SUPPORTED 不支持 pad 保持功能
  */
esp_err_t gpio_hold_en(gpio_num_t gpio_num);

/**
  * @brief 禁用 gpio pad 保持功能。
  *
  * 当芯片从 Deep-sleep 唤醒时，gpio 将被设置为默认模式，因此，gpio 将输出
  * 调用此函数时的默认级别。如果您不希望电平发生变化，则应将 GPIO 配置为
  * 调用此函数之前的已知状态。
  * 例如
  * 如果在 Deep-sleep 期间将 gpio18 保持为高电平，则在芯片被唤醒并调用 `gpio_hold_dis` 后，
  * gpio18将输出低电平（因为gpio18默认为输入模式）。如果你不想要这种行为，
  * 在调用 `gpio_hold_dis` 之前，您应该将 gpio18 配置为输出模式并将其设置为高电平。
  *
  * @param gpio_num GPIO编号，仅支持有输出功能的GPIO
  *
  * @返回
  * - ESP_OK 成功
  * - ESP_ERR_NOT_SUPPORTED 不支持 pad 保持功能
  */
esp_err_t gpio_hold_dis(gpio_num_t gpio_num);

#if !SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP
/**
  * @brief 在深度睡眠期间启用所有数字 GPIO 垫保持功能。
  *
  * 启用此功能使所有数字 GPIO 板在深度睡眠期间处于保持状态。每个焊盘的状态
  * 保留的是其活动配置（不是 pad 的睡眠配置！）。
  *
  * 请注意，此焊盘保持功能仅在芯片处于深度睡眠模式时有效。当芯片处于活动模式时，
  * 即使您调用了该函数，数字GPIO状态也可以自由改变。
  *
  * 调用该API后，数字gpio深度睡眠保持功能将在每次睡眠过程中起作用。你
  * 应调用 `gpio_deep_sleep_hold_dis` 来禁用此功能。
  */
void gpio_deep_sleep_hold_en(void);

/**
  * @brief Disable all digital gpio pads hold function during Deep-sleep.
  */
void gpio_deep_sleep_hold_dis(void);
#endif //!SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP

/**
  * @brief Set pad input to a peripheral signal through the IOMUX.
  * @param gpio_num GPIO number of the pad.
  * @param signal_idx Peripheral signal id to input. One of the ``*_IN_IDX`` signals in ``soc/gpio_sig_map.h``.
  */
void gpio_iomux_in(uint32_t gpio_num, uint32_t signal_idx);

/**
  * @brief Set peripheral output to an GPIO pad through the IOMUX.
  * @param gpio_num gpio_num GPIO number of the pad.
  * @param func The function number of the peripheral pin to output pin.
  *        One of the ``FUNC_X_*`` of specified pin (X) in ``soc/io_mux_reg.h``.
  * @param oen_inv True if the output enable needs to be inverted, otherwise False.
  */
void gpio_iomux_out(uint8_t gpio_num, int func, bool oen_inv);

#if SOC_GPIO_SUPPORT_FORCE_HOLD
/**
  * @brief 强制按住所有数字和 rtc gpio 垫。
  *
  * GPIO 强制保持，无论芯片处于活动模式还是睡眠模式。
  *
  * 此功能将立即导致所有焊盘锁存输入使能、输出使能、
  * 输出值、功能和驱动强度值。
  *
  * @warning 此函数还将保留闪存和 UART 引脚。因此，这个函数以及之后运行的所有代码
  *（直到调用 `gpio_force_unhold_all` 来禁用此功能），必须放置在内部 RAM 中作为保存闪存
  * 引脚将停止 SPI 闪存操作，按住 UART 引脚将停止任何 UART 记录。
  * */
esp_err_t gpio_force_hold_all(void);

/**
  * @brief Force unhold all digital and rtc gpio pads.
  * */
esp_err_t gpio_force_unhold_all(void);
#endif

/**
  * @brief 启用SLP_SEL以在lightsleep状态下自动更改GPIO状态。
  * @param gpio_num pad 的 GPIO 编号。
  *
  * @返回
  * - ESP_OK 成功
  *
  */
esp_err_t gpio_sleep_sel_en(gpio_num_t gpio_num);

/**
  * @brief 禁用 SLP_SEL 以在 Lightsleep 状态下自动更改 GPIO 状态。
  * @param gpio_num pad 的 GPIO 编号。
  *
  * @返回
  * - ESP_OK 成功
  */
esp_err_t gpio_sleep_sel_dis(gpio_num_t gpio_num);

/**
 * @brief GPIO 在睡眠时设置方向
 *
 * 配置GPIO方向，如output_only、input_only、output_and_input
 *
 * @param gpio_num 配置GPIO管脚编号，应该是GPIO编号。如果你想设置例如方向GPIO16，gpio_num 应为 GPIO_NUM_16 (16)；
 * @param mode GPIO方向
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG GPIO 错误
 */
esp_err_t gpio_sleep_set_direction(gpio_num_t gpio_num, gpio_mode_t mode);

/**
 * @brief 配置休眠时GPIO上拉/下拉电阻
 *
 * @note ESP32：只有支持输入和输出的引脚才具有集成的上拉和下拉电阻。仅输入 GPIO 34-39 则没有。
 *
 * @param gpio_num GPIO 编号。如果您想设置上拉或下拉模式，例如GPIO16，gpio_num 应为 GPIO_NUM_16 (16)；
 * @param pull GPIO 上拉/下拉模式。
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG : 参数错误
 */
esp_err_t gpio_sleep_set_pull_mode(gpio_num_t gpio_num, gpio_pull_mode_t pull);

#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP

#define GPIO_IS_DEEP_SLEEP_WAKEUP_VALID_GPIO(gpio_num)    ((gpio_num >= 0) && \
                                                          (((1ULL << (gpio_num)) & SOC_GPIO_DEEP_SLEEP_WAKE_VALID_GPIO_MASK) != 0))

/**
 * @brief 启用GPIO深度睡眠唤醒功能。
 *
 * @param gpio_num GPIO 编号。
 *
 * @param intr_type GPIO 唤醒类型。只能使用 GPIO_INTR_LOW_LEVEL 或 GPIO_INTR_HIGH_LEVEL。
 *
 * @note 由 SDK 调用。用户不应直接在 APP 中调用此函数。
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t gpio_deep_sleep_wakeup_enable(gpio_num_t gpio_num, gpio_int_type_t intr_type);

/**
 * @brief 禁用GPIO深度睡眠唤醒功能。
 *
 * @param gpio_num GPIO编号
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t gpio_deep_sleep_wakeup_disable(gpio_num_t gpio_num);

#endif

#ifdef __cplusplus
}
#endif
