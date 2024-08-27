#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "hal/adc_types.h"
#include "esp_adc/adc_continuous.h"

adc_continuous_handle_t conti_handle;

int adc_num;                    // 缓冲区大小
int result1 = 0, result2 = 0;          
uint8_t* adc_val;               // 缓冲区

bool adc_callback(adc_continuous_handle_t handle, const adc_continuous_evt_data_t* edata, void* user_data) {
    adc_num = edata->size;                                          // 获取缓冲区的大小
    adc_val = edata->conv_frame_buffer;                             // 获取转换结果
    if (adc_num == 8) {                                             // 将转换结果(4个byte)合成一个int
        result1 = (((uint16_t)adc_val[1] & 0x0F) << 8) | adc_val[0];   // 12-bit 数据，高4位清零
        result2 = (((uint16_t)adc_val[5] & 0x0F) << 8) | adc_val[4];   //根据选取的位宽处理
        return true;
    }
    return false;
}

void adc_init(void) {
    adc_continuous_handle_cfg_t conti_initer = {
        .conv_frame_size = 8,               //根据通道选择，每个通道占用4字节，那么这个示例用了两个通道，就是2*4个字节
        .max_store_buf_size = 1024          // 比2*4大就行
    };
    adc_continuous_new_handle(&conti_initer, &conti_handle);

    adc_digi_pattern_config_t adc_digi_arr[] = {
        {
            .atten = ADC_ATTEN_DB_11,       // 11dB衰减
            .bit_width = ADC_BITWIDTH_12,   // 输出12bit
            .channel = ADC_CHANNEL_0,       // 通道0
            .unit = ADC_UNIT_1              // ADC1
        },{
            .atten = ADC_ATTEN_DB_11,   
            .bit_width = ADC_BITWIDTH_12,
            .channel = ADC_CHANNEL_1,       // 通道1
            .unit = ADC_UNIT_1              
        }
    };
    adc_continuous_config_t conti_config = {
        .adc_pattern = adc_digi_arr,                // 配置的通道数组
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,        // 只使用ADC1
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE2,     // 输出格式Type2
        .pattern_num = 2,                           // 使用的通道数
        .sample_freq_hz = 20000                     // 采样频率
    };
    adc_continuous_config(conti_handle, &conti_config);

    adc_continuous_evt_cbs_t conti_evt = {
        .on_conv_done = adc_callback,               // 绑定转换完毕后的回调函数
    };
    adc_continuous_register_event_callbacks(conti_handle, &conti_evt, NULL);
    
    adc_continuous_start(conti_handle);             // 开启连续转换
}

void app_main(void) {
    adc_init();
    while (1) {
        // 打印每秒钟的结果
        printf("result1 is %d, result2 is %d\r\n", result1, result2);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // 每秒打印一次
    }
}
