#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// 定义 GPIO 引脚号
#define GPIO_OUTPUT_IN_SEL ((1ULL << GPIO_NUM_0) | (1ULL) << GPIO_NUM_1)

#define LED0 GPIO_NUM_0
#define LED GPIO_NUM_1

// 定义任务栈大小
#define TASK_STACK_SIZE 2048

// GPIO 初始化函数
static void gpio_init()
{
    gpio_config_t io_conf1;
   

    // 配置 GPIO 为输出模式
    io_conf1.intr_type = GPIO_INTR_DISABLE;
    io_conf1.mode = GPIO_MODE_OUTPUT;
    io_conf1.pin_bit_mask = GPIO_OUTPUT_IN_SEL;
    io_conf1.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf1.pull_up_en = GPIO_PULLUP_DISABLE;

    // 应用配置
    gpio_config(&io_conf1);
}

// 控制 GPIO 输出任务
void gpio_task(void *pvParameter)
{
    while (1) {
        // 输出高电平
        gpio_set_level(LED, 1);
        gpio_set_level(LED0, 1);
        //printf("GPIO[%d] set to HIGH\n", LED);
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 秒钟延时

        // 输出低电平
        gpio_set_level(LED, 0);
        gpio_set_level(LED0, 0);
        //printf("GPIO[%d] set to LOW\n", LED);
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 秒钟延时
    }
}

void app_main()
{
    // 初始化 GPIO
    gpio_init();

    // 创建任务来控制 GPIO 输出
    xTaskCreate(gpio_task, "gpio_task", TASK_STACK_SIZE, NULL, 5, NULL);
}
