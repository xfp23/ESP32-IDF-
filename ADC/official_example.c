/*
* @file 官方ADC示例
/
/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_adc/adc_continuous.h"

// 定义ADC单元和转换模式
#define EXAMPLE_ADC_UNIT                    ADC_UNIT_1
#define _EXAMPLE_ADC_UNIT_STR(unit)         #unit
#define EXAMPLE_ADC_UNIT_STR(unit)          _EXAMPLE_ADC_UNIT_STR(unit)
#define EXAMPLE_ADC_CONV_MODE               ADC_CONV_SINGLE_UNIT_1
#define EXAMPLE_ADC_ATTEN                   ADC_ATTEN_DB_0
#define EXAMPLE_ADC_BIT_WIDTH               SOC_ADC_DIGI_MAX_BITWIDTH

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
#define EXAMPLE_ADC_OUTPUT_TYPE             ADC_DIGI_OUTPUT_FORMAT_TYPE1
#define EXAMPLE_ADC_GET_CHANNEL(p_data)     ((p_data)->type1.channel)
#define EXAMPLE_ADC_GET_DATA(p_data)        ((p_data)->type1.data)
#else
#define EXAMPLE_ADC_OUTPUT_TYPE             ADC_DIGI_OUTPUT_FORMAT_TYPE2
#define EXAMPLE_ADC_GET_CHANNEL(p_data)     ((p_data)->type2.channel)
#define EXAMPLE_ADC_GET_DATA(p_data)        ((p_data)->type2.data)
#endif

#define EXAMPLE_READ_LEN                    256

#if CONFIG_IDF_TARGET_ESP32
// ESP32的ADC通道
static adc_channel_t channel[2] = {ADC_CHANNEL_6, ADC_CHANNEL_7};
#else
// 其他平台的ADC通道
static adc_channel_t channel[2] = {ADC_CHANNEL_2, ADC_CHANNEL_3};
#endif

static TaskHandle_t s_task_handle;
static const char *TAG = "EXAMPLE";

// ADC转换完成回调函数
static bool IRAM_ATTR s_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    BaseType_t mustYield = pdFALSE;
    // 通知任务ADC连续驱动已完成足够数量的转换
    vTaskNotifyGiveFromISR(s_task_handle, &mustYield);

    return (mustYield == pdTRUE);
}

// 初始化ADC连续模式
static void continuous_adc_init(adc_channel_t *channel, uint8_t channel_num, adc_continuous_handle_t *out_handle)
{
    adc_continuous_handle_t handle = NULL;

    // 配置ADC连续模式句柄
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 1024,
        .conv_frame_size = EXAMPLE_READ_LEN,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));

    // 配置ADC数字控制器
    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = 20 * 1000,
        .conv_mode = EXAMPLE_ADC_CONV_MODE,
        .format = EXAMPLE_ADC_OUTPUT_TYPE,
    };

    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    dig_cfg.pattern_num = channel_num;
    for (int i = 0; i < channel_num; i++) {
        adc_pattern[i].atten = EXAMPLE_ADC_ATTEN;
        adc_pattern[i].channel = channel[i] & 0x7;
        adc_pattern[i].unit = EXAMPLE_ADC_UNIT;
        adc_pattern[i].bit_width = EXAMPLE_ADC_BIT_WIDTH;

        ESP_LOGI(TAG, "adc_pattern[%d].atten is :%"PRIx8, i, adc_pattern[i].atten);
        ESP_LOGI(TAG, "adc_pattern[%d].channel is :%"PRIx8, i, adc_pattern[i].channel);
        ESP_LOGI(TAG, "adc_pattern[%d].unit is :%"PRIx8, i, adc_pattern[i].unit);
    }
    dig_cfg.adc_pattern = adc_pattern;
    ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));

    *out_handle = handle;
}

void app_main(void)
{
    esp_err_t ret;
    uint32_t ret_num = 0;
    uint8_t result[EXAMPLE_READ_LEN] = {0};
    memset(result, 0xcc, EXAMPLE_READ_LEN);

    s_task_handle = xTaskGetCurrentTaskHandle();

    adc_continuous_handle_t handle = NULL;
    continuous_adc_init(channel, sizeof(channel) / sizeof(adc_channel_t), &handle);

    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = s_conv_done_cb,
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(handle, &cbs, NULL));
    ESP_ERROR_CHECK(adc_continuous_start(handle));

    while (1) {

        /**
         * 这是展示如何使用ADC连续模式驱动程序事件回调的方法。
         * 当任务中的数据处理速度快时，这里的 `ulTaskNotifyTake` 会阻塞。
         * 但在此示例中，数据处理（打印）速度较慢，所以几乎不会在此阻塞。
         *
         * 不使用此事件回调（通知此任务），你仍然可以在循环中调用 `adc_continuous_read()`，
         * 并且可以选择是否带有一定的阻塞超时。
         */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        char unit[] = EXAMPLE_ADC_UNIT_STR(EXAMPLE_ADC_UNIT);

        while (1) {
            ret = adc_continuous_read(handle, result, EXAMPLE_READ_LEN, &ret_num, 0);
            if (ret == ESP_OK) {
                ESP_LOGI("TASK", "ret is %x, ret_num is %"PRIu32" bytes", ret, ret_num);
                for (int i = 0; i < ret_num; i += SOC_ADC_DIGI_RESULT_BYTES) {
                    adc_digi_output_data_t *p = (adc_digi_output_data_t*)&result[i];
                    uint32_t chan_num = EXAMPLE_ADC_GET_CHANNEL(p);
                    uint32_t data = EXAMPLE_ADC_GET_DATA(p);
                    /* 检查通道号是否有效，如果通道号超过最大通道，则数据无效 */
                    if (chan_num < SOC_ADC_CHANNEL_NUM(EXAMPLE_ADC_UNIT)) {
                        ESP_LOGI(TAG, "Unit: %s, Channel: %"PRIu32", Value: %"PRIx32, unit, chan_num, data);
                    } else {
                        ESP_LOGW(TAG, "Invalid data [%s_%"PRIu32"_%"PRIx32"]", unit, chan_num, data);
                    }
                }
                /**
                 * 因为打印速度较慢，所以每次调用 `ulTaskNotifyTake` 会立即返回。
                 * 为避免任务看门狗超时，在此添加一个延迟。当你替换数据处理方式时，
                 * 通常不需要这个延迟（因为此任务会阻塞一段时间）。
                 */
                vTaskDelay(1);
            } else if (ret == ESP_ERR_TIMEOUT) {
                // 尝试读取 `EXAMPLE_READ_LEN`，直到API返回超时，这意味着没有可用数据
                break;
            }
        }
    }

    ESP_ERROR_CHECK(adc_continuous_stop(handle));
    ESP_ERROR_CHECK(adc_continuous_deinit(handle));
}
  
