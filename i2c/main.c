#include "oled.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>
#include <stdio.h>

#define I2C_MASTER_SCL_IO GPIO_NUM_9        /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO GPIO_NUM_8        /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM 0                    /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ 100000           /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0         /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0         /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 100

static char * TAG = "i2c";

static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void OLED_DrawProgressBar(uint8_t progress)
{
    uint8_t width = (progress * 128) / 100; // 计算进度条长度
    OLED_Set_Pos(0, 4); // 在中间第4页绘制进度条
    for (uint8_t i = 0; i < 128; i++)
    {
        if (i < width)
        {
            OLED_WR_Byte(0xFF, OLED_DATA); // 进度条部分
        }
        else
        {
            OLED_WR_Byte(0x00, OLED_DATA); // 剩余部分
        }
    }
}

void OLED_ShowCountdown(uint8_t seconds)
{
    char buf[4]; // 增加缓冲区大小以避免警告
    snprintf(buf, sizeof(buf), "%02d", seconds); // 格式化倒计时秒数
    OLED_ShowString(60, 2, buf, 1); // 在特定位置显示倒计时数字
}

void app_main(void)
{
    // IIC总线主机初始化
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");

    // OLED屏幕初始化
    OLED_Init();

    // 显示 "RTC" 字符串
    OLED_ShowString(50,0, "RTC", 8);

    // 开始 60 秒倒计时
    for (uint8_t i = 60; i > 0; i--)
    {
        OLED_ShowCountdown(i);
        vTaskDelay(pdMS_TO_TICKS(1000)); // 延迟1秒钟
    }

    // 删除IIC设备
    // ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
    // ESP_LOGI(TAG, "I2C unitialized successfully");
}
