
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
    OLED_ShowString(60, 2, buf, 16); // 在特定位置显示倒计时数字
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

esp_err_t OLED_WR_Byte(uint8_t data, uint8_t cmd_)
{
    int ret;
    uint8_t write_buf[2] = {((cmd_ == OLED_CMD) ? (0x00) : (0x40)), data};

    ret = i2c_master_write_to_device(0, OLED_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT);

    if (ret != ESP_OK) {
        printf("I2C Write Failed: %d\n", ret);
    }

    return ret;
}


/*
 * SPDX-License-Identifier: Apache-2.0
 * 版权声明：2015-2022 Espressif Systems (Shanghai) CO LTD
 * SPDX-文件标识符：Apache-2.0
 * 版权所有：2015-2022 上海乐鑫信息科技有限公司
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

/**
 * @brief I2C端口号，可以是 I2C_NUM_0 ~ (I2C_NUM_MAX-1)。
 */
typedef enum {
    I2C_NUM_0 = 0,              /*!< I2C端口0 */
#if SOC_I2C_NUM >= 2
    I2C_NUM_1,                  /*!< I2C端口1 */
#endif /* SOC_I2C_NUM >= 2 */
#if SOC_LP_I2C_NUM >= 1
    LP_I2C_NUM_0,               /*< 低功耗I2C端口0 */
#endif /* SOC_LP_I2C_NUM >= 1 */
    I2C_NUM_MAX,                /*!< 最大I2C端口数 */
} i2c_port_t;

/**
 * @brief 计算I2C总线时序的数据结构。
 */
typedef struct {
    uint16_t clkm_div;          /*!< I2C核心时钟分频器 */
    uint16_t scl_low;           /*!< I2C时钟SCL低电平周期 */
    uint16_t scl_high;          /*!< I2C时钟SCL高电平周期 */
    uint16_t scl_wait_high;     /*!< I2C时钟SCL等待高电平周期 */
    uint16_t sda_hold;          /*!< I2C数据线SDA保持时间 */
    uint16_t sda_sample;        /*!< I2C数据线SDA采样时间 */
    uint16_t setup;             /*!< I2C起始和停止条件设置周期 */
    uint16_t hold;              /*!< I2C起始和停止条件保持周期 */
    uint16_t tout;              /*!< I2C总线超时周期 */
} i2c_hal_clk_config_t;

typedef enum {
#if SOC_I2C_SUPPORT_SLAVE
    I2C_MODE_SLAVE = 0,   /*!< I2C从机模式 */
#endif
    I2C_MODE_MASTER,      /*!< I2C主机模式 */
    I2C_MODE_MAX,
} i2c_mode_t;

typedef enum {
    I2C_MASTER_WRITE = 0,   /*!< I2C写入数据 */
    I2C_MASTER_READ,        /*!< I2C读取数据 */
} i2c_rw_t;

typedef enum {
    I2C_DATA_MODE_MSB_FIRST = 0,  /*!< I2C数据MSB优先 */
    I2C_DATA_MODE_LSB_FIRST = 1,  /*!< I2C数据LSB优先 */
    I2C_DATA_MODE_MAX
} i2c_trans_mode_t;

typedef enum {
    I2C_ADDR_BIT_7 = 0,    /*!< I2C从机模式下的7位地址 */
    I2C_ADDR_BIT_10,       /*!< I2C从机模式下的10位地址 */
    I2C_ADDR_BIT_MAX,
} i2c_addr_mode_t;

typedef enum {
    I2C_MASTER_ACK = 0x0,        /*!< 每字节读取时I2C应答 */
    I2C_MASTER_NACK = 0x1,       /*!< 每字节读取时I2C不应答 */
    I2C_MASTER_LAST_NACK = 0x2,   /*!< 最后一个字节时I2C不应答 */
    I2C_MASTER_ACK_MAX,
} i2c_ack_type_t;

/**
 * @brief 时序配置结构体。用于I2C内部复位。
 */
typedef struct {
    int high_period; /*!< 高电平时间 */
    int low_period; /*!< 低电平时间 */
    int wait_high_period; /*!< 等待高电平时间 */
    int rstart_setup; /*!< 重启设置时间 */
    int start_hold; /*!< 启动保持时间 */
    int stop_setup; /*!< 停止设置时间 */
    int stop_hold; /*!< 停止保持时间 */
    int sda_sample; /*!< SDA采样时间 */
    int sda_hold; /*!< SDA保持时间 */
    int timeout; /*!< 超时值 */
} i2c_hal_timing_config_t;

/**
 * @brief I2C组时钟源
 */
typedef soc_periph_i2c_clk_src_t i2c_clock_source_t;

#ifdef __cplusplus
}
#endif
