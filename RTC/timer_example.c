#include <stdio.h>
#include <driver/gptimer.h>
#include "freertos/FreeRTOS.h"
#include <driver/gpio.h>
#include <driver/gptimer.h>


bool test_alarm_cb_t(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    gpio_set_level(GPIO_NUM_1, !gpio_get_level(GPIO_NUM_1));
    return true;
}

void led_init()
{
    gpio_config_t io_config = {0};
    io_config.mode = GPIO_MODE_INPUT_OUTPUT;
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_config.pull_up_en = GPIO_PULLUP_DISABLE;
    io_config.pin_bit_mask = 1ULL<<1;
    gpio_config(&io_config);
}

void timer_init()
{
    gptimer_handle_t gptimer_handle = {0};
    gptimer_config_t config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
    };
    gptimer_new_timer(&config, &gptimer_handle);
    
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 1000000,//一秒
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_set_alarm_action(gptimer_handle, &alarm_config);
    const gptimer_event_callbacks_t cbs = {
        .on_alarm = test_alarm_cb_t,
    };
    gptimer_register_event_callbacks(gptimer_handle, &cbs, NULL);
    gptimer_enable(gptimer_handle);
    gptimer_start(gptimer_handle);
}

void app_main(void)
{
    led_init();
    timer_init();
    while (1) {
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }

}