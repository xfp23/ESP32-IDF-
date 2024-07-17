#include "driver/i2c.h"
#include "esp_err.h"

void init_VL53L0X()
{
#if defined(IIC_CONFIG)

i2c_config_t conf = {
    .mode = VL53_IIC_NUM,
    .sda_io_num = VL53_SDA_PIN,
    .scl_io_num = VL53_SCL_PIN,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = VL53_FREQ,
    .clk_flags = 0,
};
i2c_param_config(VL53_IIC_NUM,&conf);
i2c_driver_install(VL53_IIC_NUM,conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
#endif
}

/**
 * @brief 绑定设备配置
 */
void init_VL53L0X_DEV(VL53L0X_DEV * dev)
{
    dev->init = init_VL53L0X;
    dev->read0 = read_single_vl530x;
    dev->read1 = read_continu_vl530x;
}

int read_single_vl530x()
{
    int data[2];
}