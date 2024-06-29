#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "stdio.h"

#define GPIO_INPUT_IO      18   // 定义用于输入的GPIO引脚
#define ESP_INTR_FLAG_DEFAULT 0

static const char *TAG = "GPIO_INTR";

// 中断处理函数
static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;
    // 在这里处理中断事件
    //ESP_LOGI(TAG, "GPIO[%d] intr, val: %d\n", gpio_num, gpio_get_level(gpio_num));
}

// 配置GPIO输入并启用中断
void app_main(void) {
    // 配置GPIO引脚
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;      // 中断类型：下降沿触发
    io_conf.pin_bit_mask = (1ULL << GPIO_INPUT_IO); // 配置引脚掩码
    io_conf.mode = GPIO_MODE_INPUT;             // 设置为输入模式
    io_conf.pull_up_en = 1;                     // 启用上拉
    gpio_config(&io_conf);

    // 安装GPIO中断服务
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    // 为特定GPIO引脚注册中断处理函数
    gpio_isr_handler_add(GPIO_INPUT_IO, gpio_isr_handler, (void*) GPIO_INPUT_IO);

    // 主循环
    while (1) {
        // 可以在这里添加其他逻辑
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
