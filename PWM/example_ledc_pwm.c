#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "hal/ledc_types.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "math.h"

#define BITS LEDC_TIMER_13_BIT
#define PWM_GPIO GPIO_NUM_5
#define PWM_CHANNEL LEDC_CHANNEL_0

void init_pwm();
uint32_t get_duty(int bits, int out_percent);
void update_pwm(int channel, uint32_t ledc_duty);

void app_main(void)
{
    init_pwm();
    while (1) {
        for (int i = 0; i <= 100; i++) {
            update_pwm(PWM_CHANNEL, get_duty(BITS, i));
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        for (int i = 100; i >= 0; i--) {
            update_pwm(PWM_CHANNEL, get_duty(BITS, i));
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

void init_pwm()
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .gpio_num = GPIO_NUM_5,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .duty = 0,
        .hpoint = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

uint32_t get_duty(int bits, int out_percent)
{
    return (uint32_t)((pow(2, bits) - 1) * (out_percent / 100.0));
}

void update_pwm(int channel, uint32_t ledc_duty)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, ledc_duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
}
