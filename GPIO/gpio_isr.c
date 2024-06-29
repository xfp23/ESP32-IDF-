#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED GPIO_NUM_0
#define KEY GPIO_NUM_1

static const char* TAG = "GPIO";

// GPIO中断处理程序
void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    // 切换LED状态
    gpio_set_level(LED, !gpio_get_level(LED));
}

void init_gpio()
{
    // 配置LED引脚
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_level(LED, 0); // 默认熄灭

    // 配置按键引脚
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_NEGEDGE; // 下降沿中断
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << KEY);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    // 安装GPIO ISR处理程序
    gpio_install_isr_service(0); // 使用默认标志
    gpio_isr_handler_add(KEY, gpio_isr_handler, (void*) KEY);
}

void app_main(void)
{
    init_gpio();
    ESP_LOGI(TAG, "GPIO initialized and interrupt handler added.");

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // 空循环保持任务运行
    }
}
