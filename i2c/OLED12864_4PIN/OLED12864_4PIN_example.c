#include "oled.h"

static char * TAG = "IIC";
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO, // 18
        .scl_io_num = I2C_MASTER_SCL_IO, // 19
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}


void app_main(void)
{
    // IIC总线主机初始化
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");

    // OLED屏幕初始化
    OLED_Init();

    // 显示汉字
    OLED_ShowCHinese(0 * 18, 0, 0);
    OLED_ShowCHinese(1 * 18, 0, 1);
    OLED_ShowCHinese(2 * 18, 0, 2);

    // 显示单个字符
    OLED_ShowChar(0, 2, 'Q', 16);

    // 显示字符串
    OLED_ShowString(0, 4, "Fairy tale", 16);

    // 显示数字
    OLED_ShowNum(0, 6, 8266, 6, 16);

    // 删除IIC设备
    // ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
    // ESP_LOGI(TAG, "I2C unitialized successfully");
}


