#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/adc_types.h"
#include "esp_adc/adc_continuous.h"

#define ADC_BUFFER_SIZE 4  // 1 个通道，每个通道 4 字节

adc_continuous_handle_t conti_handle;
uint8_t adc_buffer[ADC_BUFFER_SIZE];

void adc_init(void) {
    adc_continuous_handle_cfg_t conti_initer = {
        .conv_frame_size = ADC_BUFFER_SIZE,  // 配置缓冲区大小
        .max_store_buf_size = 1024
    };
    adc_continuous_new_handle(&conti_initer, &conti_handle);

    adc_digi_pattern_config_t adc_digi_arr[] = {
        {
            .atten = ADC_ATTEN_DB_11,       // 11dB衰减
            .bit_width = ADC_BITWIDTH_12,   // 输出12bit
            .channel = ADC_CHANNEL_0,       // 通道0
            .unit = ADC_UNIT_1              // ADC1
        }
    };

    adc_continuous_config_t conti_config = {
        .adc_pattern = adc_digi_arr,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE2,
        .pattern_num = 1,  // 使用 1 个通道
        .sample_freq_hz = 20000
    };
    adc_continuous_config(conti_handle, &conti_config);
    adc_continuous_start(conti_handle);
}

void app_main(void) {
    adc_init();
    
    while (1) {
        // 从缓冲区读取 ADC 数据
        size_t bytes_read;
        esp_err_t err = adc_continuous_read(conti_handle, adc_buffer, ADC_BUFFER_SIZE, &bytes_read, portMAX_DELAY);

        if (err == ESP_OK) {
            if (bytes_read == ADC_BUFFER_SIZE) {
                // 提取通道 0 的数据
                int result = (((uint16_t)adc_buffer[1] & 0x0F) << 8) | adc_buffer[0];
                
                // 打印结果
                printf("Channel 0: %d\n", result);
            } else {
                printf("Error: Unexpected bytes_read size %d\n", (int)bytes_read);
            }
        } else {
            printf("Error reading ADC data: %s\n", esp_err_to_name(err));
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS); // 每秒打印一次
    }
}
