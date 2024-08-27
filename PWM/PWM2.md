**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息

© 2024 . 未经许可不得复制、修改或分发。 此文献为 [小風的藏書閣](https://t.me/xfp2333)  所有。

# 电机控制脉宽调制器(MCPWM) (有待完善)

# 简介

**MCPWM 外设是一个多功能 PWM 生成器，集成多个子模块，在电力电子应用（如电机控制、数字电源等）中至关重要。MCPWM 外设通常适用于以下场景：**

- 数字电机控制，如有刷/无刷直流电机、RC 伺服电机

- 基于开关模式的数字电源转换

- 功率数模转换器 (Power DAC)，其中占空比等于 DAC 的模拟值

- 计算外部脉宽，并将其转换为其他模拟值，如速度、距离

- 为磁场定向控制 (FOC) 生成空间矢量调制 (SVPWM) 信号

## 模块介绍：
- MCPWM 定时器模块：最终输出 PWM 信号的时间基准。它也决定了其他子模块的事件时序。

- MCPWM 操作器模块：生成 PWM 波形的关键模块。它由其他子模块组成，如比较器、PWM 生成器、死区生成器和载波调制器。

- MCPWM 比较器模块：输入时间基准值，并不断与配置的阈值进行比较。当**定时器计数值等于任何一个阈值时**，生成一个比较事件，MCPWM 生成器随即相应更新其电平。

- MCPWM 生成器模块：根据 MCPWM 定时器、MCPWM 比较器等子模块触发的各种事件，生成一对独立或互补的 PWM 波形。

- MCPWM 故障检测模块：通过 GPIO 交换矩阵检测外部的故障情况。检测到故障信号时，MCPWM 操作器将强制所有生成器进入预先定义的状态，从而保护系统。

- MCPWM 同步模块：同步 MCPWM 定时器，以确保由不同的 MCPWM 生成器最终生成的 PWM 信号具有固定的相位差。可以通过 GPIO 交换矩阵和 MCPWM 定时器事件生成同步信号。

- 死区生成器模块：在此前生成的 PWM 边沿上插入额外的延迟。

- 载波模块：可通过 PWM 波形生成器和死区生成器，将一个高频载波信号调制为 PWM 波形，这是控制功率开关器件的必需功能。

- 制动控制：MCPWM 操作器支持配置检测到特定故障时生成器的制动控制方式。根据故障的严重程度，可以选择立即关闭或是逐周期调节 PWM 输出。

- MCPWM 捕获模块：独立子模块，不依赖于上述 MCPWM 操作器工作。**捕获模块包括一个专用的定时器和几个独立的通道**，每个通道都与 GPIO 相连。GPIO 上的脉冲触发捕获定时器以存储时间基准值，随后通过中断进行通知。此模块有助于更加**精准地测量脉宽**。此外，捕获定时器也可以通过 MCPWM 同步子模块进行同步
# 环境配置

- **添加以下头文件**
```c
#include "driver/mcpwm_timer.h"
```

- **CMAKE配置**
```c
REQUIRES driver
```

# 资源配置及初始化
- 资源配置及初始化 - 介绍各类 MCPWM 模块的分配，如定时器、操作器、比较器、生成器等。

## **步骤**

### MCPWM定时器

1. 配置 ***mcpwm_timer_config_t***结构体，结构体定义为：
```c
//定义句柄
mcpwm_timer_handle_t mcpwm;


//需要配置的结构体
typedef struct {
    int group_id;                        /*!<  指定 MCPWM 组 ID，范围为 [0,  - 1]。需注意，位于不同组的定时器彼此独立。*/
    mcpwm_timer_clock_source_t clk_src;  /*!< MCPWM定时器时钟源*/
    uint32_t resolution_hz;              /*!< 设置定时器的预期分辨率。内部驱动将根据时钟源和分辨率设置合适的分频器
                                              每个计数刻度的步长等于 (1 / resolution_hz) 秒*/
    mcpwm_timer_count_mode_t count_mode; /*!< 计数模式 */
    uint32_t period_ticks;               /*!<设置定时器的周期，以 Tick 为单位（通过 mcpwm_timer_config_t::resolution_hz 设置 Tick 分辨率 */
    int intr_priority;                   /*!如果设置为 0，则会分配一个默认优先级的中断，否则会使用指定的优先级 */

    struct {
        uint32_t update_period_on_empty: 1; /*!<  设置当定时器计数为零时是否更新周期值。 */
        uint32_t update_period_on_sync: 1;  /*!<设置当定时器接收同步信号时是否更新周期值*/
    } flags;                                /*!< 定时器的额外配置标志 */
} mcpwm_timer_config_t;

//选择时钟源枚举值
typedef enum {
    MCPWM_TIMER_CLK_SRC_PLL160M = SOC_MOD_CLK_PLL_F160M, /*!< 选择PLL_F160M作为源时钟 */
    MCPWM_TIMER_CLK_SRC_DEFAULT = SOC_MOD_CLK_PLL_F160M, /*!< 选择 PLL_F160M 作为默认时钟选择 */
} soc_periph_mcpwm_timer_clk_src_t;
//选择计数模式枚举值
typedef enum {
    MCPWM_TIMER_COUNT_MODE_PAUSE,   /*!< MCPWM 定时器暂停*/
    MCPWM_TIMER_COUNT_MODE_UP,      /*!< MCPWM 定时器向上计数 */
    MCPWM_TIMER_COUNT_MODE_DOWN,    /*!<MCPWM定时器倒向下计数 */
    MCPWM_TIMER_COUNT_MODE_UP_DOWN, /*!< MCPWM 定时器向上和向下计数*/
} mcpwm_timer_count_mode_t;

```
- 分配成功后，**mcpwm_new_timer()** 将返回一个指向已分配定时器的指针。否则，函数将返回错误代码。具体来说，当 MCPWM 组中没有空闲定时器时

- 调用 **mcpwm_del_timer()** 函数将释放已分配的定时器

2. 应用代码示例：
```c
mcpwm_timer_handle_t mcpwm;
mcpwm_timer_config_t mcpwm_config= {
    .group_id = 0,
    .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
    .resolution_hz = 100000, //时钟分频
    .count_mode = MCPEM_TIMER_COUNT_MODE_UP,
    .period_ticks = 1000     // 1000 ticks, 1ms 定时器的周期
    .intr_priority = 0
};
mcpwm_new_timer(&mcpwm_config,&mcpwm);//应用配置
```

### MCPWM操作器
1. 调用mcpwm_new_operator()函数，以配置结构体 ***mcpwm_operator_config_t*** 分配一个MCPWM操作器为对象。结构体定义为：
```c
//定义句柄:
mcpwm_oper_handle_t;

//结构体：
typedef struct {
    int group_id;                              /*!< 指定从哪个组分配MCPWM操作单元 */
    int intr_priority;                         /*!< MCPWM操作单元的中断优先级，
                                                    如果设置为0，驱动程序将尝试分配相对较低优先级的中断（1, 2, 3） */
    struct {
        uint32_t update_gen_action_on_tez: 1;  /*!< 是否在定时器计数到零时更新发生器动作 */
        uint32_t update_gen_action_on_tep: 1;  /*!< 是否在定时器计数到峰值时更新发生器动作 */
        uint32_t update_gen_action_on_sync: 1; /*!< 是否在同步事件时更新发生器动作 */
        uint32_t update_dead_time_on_tez: 1;   /*!< 是否在定时器计数到零时更新死区时间 */
        uint32_t update_dead_time_on_tep: 1;   /*!< 是否在定时器计数到峰值时更新死区时间 */
        uint32_t update_dead_time_on_sync: 1;  /*!< 是否在同步事件时更新死区时间 */
    } flags;                                   /*!< 操作单元的额外配置标志 */
} mcpwm_operator_config_t;

```

- **mcpwm_new_operator()** 将返回一个指向已分配操作器的指针。
- **mcpwm_del_operator()** 将释放已分配的操作器
#### 示例代码：
```c
mcpwm_oper_handle_t mcpwm_operator_handle;
mcpwm_operator_config_t mcpwm_operator_config = {
    .group_id = 0,//选择定时器组
};
 ESP_ERROR_CHECK(mcpwm_new_operator(&mcpwm_operator_config,&mcpwm_operator_handle));
```

### MCPWM比较器

1. 配置结构体 ***mcpwm_comparator_config_t*** 为参数，分配一个 MCPWM 比较器为对象
```c
//句柄类型： 
mcpwm_cmpr_handle_t；
typedef struct {
    int intr_priority;                  /*!< MCPWM比较器中断优先级，
                                             如果设置为0，驱动程序将尝试分配相对较低优先级的中断（1, 2, 3） */
    struct {
        uint32_t update_cmp_on_tez: 1;  /*!< 是否在定时器计数等于零（tez）时更新比较值 */
        uint32_t update_cmp_on_tep: 1;  /*!< 是否在定时器计数等于峰值（tep）时更新比较值 */
        uint32_t update_cmp_on_sync: 1; /*!< 是否在同步事件时更新比较值 */
    } flags;                            /*!< 比较器的额外配置标志 */
} mcpwm_comparator_config_t;

```
2. 应用代码示例：
```c
mcpwm_cmpr_handle_t mc_cmp;

mcpwm_comparator_config_t comparator_config = {
    .flags.update_cmp_on_tez = true,
};
/**
 * @brief MCPWM比较器应用配置
 * @param 操作器句柄
 * @param 比较器配置结构
 * @param 比较器句柄
 */
ESP_ERROR_CHECK(mcpwm_new_cmparator(&mcpwm_operator_handle,&comparator_config，&mc_cmp)); //应用配置
```

### MCPWM生成器

1. 配置***mcpwm_generator_config_t***结构
```c
//句柄类型:
mcpwm_gen_handle_t;

//结构:
typedef struct {
    int gen_gpio_num;           /*!< 用于输出PWM信号的GPIO编号 */
    struct {
        uint32_t invert_pwm: 1;   /*!< 是否反转PWM信号（通过GPIO矩阵完成） 设置是否反相 PWM 信号bcvg */
        uint32_t io_loop_back: 1; /*!< 设置是否启用回环模式。该模式仅用于调试，使用 GPIO 交换矩阵外设同时启用 GPIO 输入和输出。 */
        uint32_t io_od_mode: 1;   /*!< 将GPIO配置为开漏模式 */
        uint32_t pull_up: 1;      /*!< 是否内部上拉 */
        uint32_t pull_down: 1;    /*!< 是否内部下拉 */
    } flags;                      /*!< 生成器的额外配置标志 */
} mcpwm_generator_config_t;

```
- 分配成功后，mcpwm_new_generator() 将返回一个指向已分配生成器的指针。否则，函数将返回错误代码。
- 调用 mcpwm_del_generator() 函数将释放已分配的生成器。

2. 示例代码：
```c

//声明句柄：
mcpwm_gen_handle_t gen_handle;
//配置结构
mcpwm_generator_config_t gen_config = {
    .gen_gpio_num = GPIO_NUM_1;
};

/**
 * @brief 应用配置
 * @param MCPWM操作器句柄
 * @param MCPWM生成器配置结构
 * @param MCAPWM生成器句柄
 */
mcpwm_new_generator(&mcpwm_operator_handle,&gen_handle,&gen_config);

mcpwm_del_generator(mcpwm_gen_handle_t gen);//释放生成器资源
```

### ***MCPWM同步源***
- 调用 mcpwm_new_gpio_sync_src() 函数，以配置结构体 ***mcpwm_gpio_sync_src_config_t*** 为参数，分配一个 GPIO 同步源。

```c
//句柄类型：

//结构:
typedef struct {
    int group_id; /*!< MCPWM组ID */
    int gpio_num; /*!< 由同步源使用的GPIO编号 */
    struct {
        uint32_t active_neg: 1;   /*!< 是否在下降沿触发同步信号，默认情况下，同步信号的上升沿被视为有效 */
        uint32_t io_loop_back: 1; /*!< 用于调试/测试，GPIO输出的信号也将被反馈到输入路径 */
        uint32_t pull_up: 1;      /*!< 是否内部上拉 */
        uint32_t pull_down: 1;    /*!< 是否内部下拉 */
    } flags;                      /*!< GPIO同步源的额外配置标志 */
} mcpwm_gpio_sync_src_config_t;

```

### ***MCPWM 捕获定时器和通道***
 - 以配置结构体 ***mcpwm_capture_timer_config_t*** 为参数，分配一个捕获定时器。

 ```c
typedef struct {
    int group_id;                         /*!< 指定从哪个组分配捕获定时器 */
    mcpwm_capture_clock_source_t clk_src; /*!< MCPWM捕获定时器的时钟源 */
    uint32_t resolution_hz;               /*!< 捕获定时器的分辨率（单位：赫兹） */
} mcpwm_capture_timer_config_t;

 ```

 ### ***注册定时器事件回调***

 - MCPWM 定时器运行时会生成不同的事件。若有函数需在特定事件发生时调用，则应预先调用 **mcpwm_timer_register_event_callbacks()**，将所需函数挂载至中断服务程序 (ISR) 中。驱动中定时器回调函数原型声明为 ***mcpwm_timer_event_cb_t***

 ```c
 //指针原型：
 /**
 * @brief 回调函数类型
 * @param 定时器句柄，
 */
typedef bool (*mcpwm_timer_event_cb_t)(mcpwm_timer_handle_t timer, const mcpwm_timer_event_data_t *edata, void *user_ctx);
 ```

### 启用和禁用定时器

- 在对定时器进行 IO 控制前，需要预先调用 **mcpwm_timer_enable()** 函数启用定时器
- 调用 mcpwm_timer_disable() 会将定时器切换回 init 状态、禁用中断服务并释放电源管理锁
```c
/**
* @brief 启用定时器
* @param 定时器句柄
*/
esp_err_t mcpwm_timer_enable(mcpwm_timer_handle_t timer);

/**
* @brief 禁用定时器
* @param 定时器句柄
*/
esp_err_t mcpwm_timer_disable(mcpwm_timer_handle_t timer)；
```

### 启动和停止定时器

- 通过基本的 IO 控制，即可启动和停止定时器。使用不同的 ***mcpwm_timer_start_stop_cmd_t*** 命令调用 **mcpwm_timer_start_stop()** 便可立即启动定时器，或在发生特定事件时停止定时器。此外，还可以通过配置，让定时器仅计数一轮。也就是说，在计数达到峰值或零后，定时器自行停止

```c
typedef enum {
    MCPWM_TIMER_STOP_EMPTY,       /*!< MCPWM定时器在下一个计数达到零时停止 */
    MCPWM_TIMER_STOP_FULL,        /*!< MCPWM定时器在下一个计数达到峰值时停止 */
    MCPWM_TIMER_START_NO_STOP,    /*!< MCPWM定时器开始计数，并且不会停止，直到收到停止命令 */
    MCPWM_TIMER_START_STOP_EMPTY, /*!< MCPWM定时器开始计数，并且在下一个计数达到零时停止 */
    MCPWM_TIMER_START_STOP_FULL,  /*!< MCPWM定时器开始计数，并且在下一个计数达到峰值时停止 */
} mcpwm_timer_start_stop_cmd_t;

/**
* @brief 停止或启动定时器
* @param 定时器句柄
* @param 枚举的命令
*/
esp_err_t mcpwm_timer_start_stop(mcpwm_timer_handle_t timer, mcpwm_timer_start_stop_cmd_t command);
```

### 连接定时器和操作器

- 调用 **mcpwm_operator_connect_timer()** 函数，连接分配的 MCPWM 定时器和 MCPWM 操作器。连接后，操作器即可将定时器作为时基，生成所需的 PWM 波形。需注意，**MCPWM 定时器和操作器必须位于同一个组中**。

```c
/**
* @brief 链接定时器和操作器
* @param 操作器句柄
* @param 定时器句柄
*/
esp_err_t mcpwm_operator_connect_timer(mcpwm_oper_handle_t oper, mcpwm_timer_handle_t timer)
```

### 经典 PWM 波形生成器配置

- 生成波形为 对称波形 还是 不对称波形 取决于 MCPWM 定时器的计数模式。

- 波形对的 激活电平 取决于占空比较小的 PWM 波形的电平。

- PWM 波形的周期取决于定时器的周期和计数模式。

- PWM 波形的占空比取决于生成器的各种操作配置组合。

```c
static void gen_action_config(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb)
{
    // 设置gena的定时器事件动作
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(gena,
                    MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    
    // 设置gena的比较事件动作
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(gena,
                    MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW)));
    
    // 设置genb的定时器事件动作
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(genb,
                    MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    
    // 设置genb的比较事件动作
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(genb,
                    MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_LOW)));
}

```