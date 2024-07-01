#ifndef OLED_H
#define OLED_H

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"

#define OLED_CMD 0
#define OLED_DATA 1
#define OLED_ADDR 0x3C // OLED的IIC地址，逻辑分析仪读出的
#define I2C_MASTER_SCL_IO 4        /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO 5        /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM 0            /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ 100000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 100
#define portTICK_RATE_MS 1000

esp_err_t OLED_WR_Byte(uint8_t data, uint8_t cmd_);                                   //写命令
void OLED_Init(void);                                                                 //初始化屏幕
void OLED_Set_Pos(uint8_t x, uint8_t y);                                              //设置屏幕坐标
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size);             //显示单个字节
void OLED_Clear(void);                                                                //清屏
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size2);    //显示数字
uint32_t oled_pow(uint8_t m, uint8_t n);
void OLED_ShowString(uint8_t x, uint8_t y, char *chr, uint8_t Char_Size);             //显示字符串
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no);                              //显示汉字

#endif
