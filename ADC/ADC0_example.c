#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

// 定义日志标签
static const char *TAG = "ADC_EXAMPLE";

// 定义ADC通道
#define EXAMPLE_ADC1_CHAN0 ADC_CHANNEL_0
#define EXAMPLE_ADC1_CHAN1 ADC_CHANNEL_3

void app_main(void)
{
    // 定义变量存储ADC原始读取值
    int adc_raw[2] = {0};

    // 初始化ADC单元实例
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    // 配置ADC通道
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN0, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, EXAMPLE_ADC1_CHAN1, &config));

    // 持续读取ADC转换结果并打印
    while (1) {
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN0, &adc_raw[0]));
        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN0, adc_raw[0]);

        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN1, &adc_raw[1]));
        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN1, adc_raw[1]);

        // 计算电压值
        const float Vmax = 3.3;  // 假设最大测量电压为3.3V
        const int Dmax = 4095;   // 对应12位位宽，2^12 - 1 = 4095

        float voltage[2];
        voltage[0] = adc_raw[0] * Vmax / Dmax;
        voltage[1] = adc_raw[1] * Vmax / Dmax;

        ESP_LOGI(TAG, "ADC%d Channel[%d] Voltage: %.2f V", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN0, voltage[0]);
        ESP_LOGI(TAG, "ADC%d Channel[%d] Voltage: %.2f V", ADC_UNIT_1 + 1, EXAMPLE_ADC1_CHAN1, voltage[1]);

        // 延时1秒
        vTaskDelay(pdMS_TO_TICKS(1000)); // 延时1秒
    }

    // 回收ADC资源
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle));
}
