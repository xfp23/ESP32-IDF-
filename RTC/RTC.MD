**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息

© 2024 . 未经许可不得复制、修改或分发。 此文献为 [小風的藏書閣](https://t.me/xfp2333)  所有。

# 通用定时器编程指南

# 简介
- 通用定时器是 ESP32-S3 定时器组外设的驱动程序。ESP32-S3 硬件定时器分辨率高，具有灵活的报警功能。定时器内部计数器达到特定目标数值的行为被称为定时器报警。定时器报警时将调用用户注册的不同定时器回调函数。

- 通用定时器通常在以下场景中使用：

1. 如同挂钟一般自由运行，随时随地获取高分辨率时间戳；

2. 生成周期性警报，定期触发事件；

3. 生成一次性警报，在目标时间内响应。

# 环境配置

1. 包含下列头文件：
```c
#include "driver/gptimer.h"
```
2. CMAKE配置：
```c
REQUIRES driver
```

# 资源分配

- **不同的 ESP 芯片可能有不同数量的独立定时器组，每组内也可能有若干个独立定时器**
- 通用定时器实例由 ***gptimer_handle_t*** 表示。可用硬件资源汇集在资源池内，由后台驱动程序管理，无需考虑硬件所属的定时器以及定时器组

1. 结构体配置：
```c
//定时器句柄类型：
gptimer_handle_t;

//结构类型：
typedef struct {
    gptimer_clock_source_t clk_src;      /*!< GPTimer 时钟源 */
    gptimer_count_direction_t direction; /*!< 计数方向 */
    uint32_t resolution_hz;              /*!< 计数器分辨率（工作频率），单位为 Hz，
                                              因此，每个计数滴答的步长等于 (1 / resolution_hz) 秒 */
    int intr_priority;                   /*!< GPTimer 中断优先级，
                                              如果设置为 0，驱动程序将尝试分配相对较低优先级的中断（1,2,3） */
    struct {
        uint32_t intr_shared: 1;         /*!< 如果设置为 true，定时器中断号可以与其他外设共享 */
    } flags;                             /*!< GPTimer 配置标志 */
} gptimer_config_t;

//结构成员类型：

//gptimer_clock_source_t类型枚举值，枚举了时钟源：
typedef enum {
    GPTIMER_CLK_SRC_APB = SOC_MOD_CLK_APB,     /*!< 选择 APB 作为源时钟 */
    GPTIMER_CLK_SRC_XTAL = SOC_MOD_CLK_XTAL,   /*!< 选择 XTAL 作为源时钟 */
    GPTIMER_CLK_SRC_DEFAULT = SOC_MOD_CLK_APB, /*!< 选择 APB 作为默认选择 */
} soc_periph_gptimer_clk_src_t;

typedef enum {
    GPTIMER_COUNT_DOWN, /*!< 递减计数值 */
    GPTIMER_COUNT_UP,   /*!< 递增计数值 */
} gptimer_count_direction_t;


/**
* @brief 配置定时器
* @param 定时器配置结构地址
* @param 定时器句柄地址
* @return ESP调试信息
*/
esp_err_t gptimer_new_timer(const gptimer_config_t *config, gptimer_handle_t *ret_timer)
```

2. 配置定时器代码示例：
```c
gptimer_handle_t gptimer = NULL;
gptimer_config_t timer_config = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
};
ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
```


# 设置获取计数值

- 创建通用定时器时，内部计数器将默认重置为零。计数值可以通过 gptimer_set_raw_count() 异步更新。最大计数值取决于硬件定时器的位宽，这也会在 SOC 宏 SOC_TIMER_GROUP_COUNTER_BIT_WIDTH 中有所反映。当更新活动定时器的原始计数值时，定时器将立即从新值开始计数。

计数值可以随时通过 ***gptimer_get_raw_count()*** 获取。

```c
/**
* @brief 获取定时器计数值
* @param 要获取的定时器句柄
* @param 存储计数值的地址 uint64_t
* @return ESP调试信息
*/
esp_err_t gptimer_get_raw_count(gptimer_handle_t timer, uint64_t *value)
```

# 设置报警动作

- 对于大多数通用定时器使用场景而言，应在启动定时器之前设置警报动作，但不包括简单的挂钟场景，该场景仅需自由运行的定时器。

```c

//结构类型:

//gptimer_alarm_config_t::alarm_count 和 gptimer_alarm_config_t::reload_count 不能设置为相同的值，因为警报值和重载值相同时没有意义。
typedef struct {
    uint64_t alarm_count;  /*!< 报警目标计数值 */
    uint64_t reload_count; /*!< 报警重新加载计数值，仅当 `auto_reload_on_alarm` 设置为 true 时有效 */
    struct {
        uint32_t auto_reload_on_alarm: 1; /*!< 在报警事件发生时由硬件立即重新加载计数值 */
    } flags;                              /*!< 报警配置标志 */
} gptimer_alarm_config_t;

/**
* @brief 配置定时器
* @param 定时器句柄
* @param 定时器报警配置结构
* @return ESP调试信息
*/
esp_err_t gptimer_set_alarm_action(gptimer_handle_t timer, const gptimer_alarm_config_t *config);
```

# 注册事件回调函数

- 定时器启动后，可动态产生特定事件（如“警报事件”）。如需在事件发生时调用某些函数，请通过 **gptimer_register_event_callbacks()** 将函数挂载到中断服务例程 (ISR)。***gptimer_event_callbacks_t*** 中列出了所有支持的事件回调函数

1. 结构类型:
```c
/**
 * @brief 支持的 GPTimer 回调函数组
 * @note 回调函数都在 ISR 环境下运行
 * @note 当 CONFIG_GPTIMER_ISR_IRAM_SAFE 启用时，回调函数本身及其调用的函数应放在 IRAM 中
 */
typedef struct {
    gptimer_alarm_cb_t on_alarm; /*!< 定时器报警回调函数 */
} gptimer_event_callbacks_t;
//设置警报事件的回调函数。由于此函数在 ISR 上下文中调用，必须确保该函数不会试图阻塞（例如，确保仅从函数内调用具有 ISR 后缀的 FreeRTOS API）。

/**
* @brief 注册定时器中断回调函数
* @param 定时器配置结构，第一个中提到的
* @param 定时器中断结构，第三个提到的
* @param 用户数据，直接提供给函数
* @return ESP调试信息   
*/
esp_err_t gptimer_register_event_callbacks(gptimer_handle_t timer, const gptimer_event_callbacks_t *cbs, void *user_data);
```

# 使能和禁用定时器

在对定时器进行 IO 控制之前，需要先调用 gptimer_enable() 使能定时器：
- 此函数将把定时器驱动程序的状态从 init 切换为 enable。
- 如果 gptimer_register_event_callbacks() 已经延迟安装中断服务，此函数将使能中断服务。
- 如果选择了特定的时钟源（例如 APB 时钟），此函数将获取适当的电源管理锁。
- 调用 gptimer_disable() 会进行相反的操作，即将定时器驱动程序恢复到 init 状态，禁用中断服务并释放电源管理锁。

```c
/**
* @brief 使能定时器
* @param 要使能的定时器句柄
* @return ESP调试信息
*/
esp_err_t gptimer_enable(gptimer_handle_t timer);

```

# 启动和停止定时器

- 调用 gptimer_start() 可以使内部计数器开始工作
- gptimer_stop() 停止计数器
```c
/**
* @brief 启动定时器
* @param 定时器句柄
*/
esp_err_t gptimer_start(gptimer_handle_t timer);
esp_err_t gptimer_stop(gptimer_handle_t timer);
```

# 将定时器作为挂钟启动

- 示例代码：
```c
ESP_ERROR_CHECK(gptimer_enable(gptimer)); //使能定时器
ESP_ERROR_CHECK(gptimer_start(gptimer)); // 运行定时器
//随机获取时间戳
uint64_t count;
ESP_ERROR_CHECK(gptimer_get_raw_count(gptimer, &count));
```


[定时器示例程序](timer_example.c)

# IARM安全

- 默认情况下，当 cache 因写入或擦除 flash 等原因而被禁用时，通用定时器的中断服务将会延迟，造成警报中断无法及时执行。在实时应用程序中通常需要避免这一情况发生。

- 调用 Kconfig 选项 CONFIG_GPTIMER_ISR_IRAM_SAFE 可实现如下功能：

- 即使禁用 cache 也可使能正在运行的中断

- 将 ISR 使用的所有函数放入 IRAM 2

- 将驱动程序对象放入 DRAM（以防意外映射到 PSRAM）

- 这将允许中断在 cache 禁用时运行，但会增加 IRAM 使用量。

- 调用另一 Kconfig 选项 CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM 也可将常用的 IO 控制功能放入 IRAM，以便这些函数在 cache 禁用时也能执行。常用的 IO 控制功能如下：

1. gptimer_start()

2. gptimer_stop()

3. gptimer_get_raw_count()

4. gptimer_set_raw_count()

4. gptimer_set_alarm_action()

# 线程安全
- 驱动提供的所有 API 都是线程安全的。使用时，可以直接从不同的 RTOS 任务中调用此类函数，无需额外锁保护。以下这些函数还支持在中断上下文中运行。

- gptimer_start()

- gptimer_stop()

- gptimer_get_raw_count()

- gptimer_set_raw_count()

- gptimer_get_captured_count()

- gptimer_set_alarm_action()
