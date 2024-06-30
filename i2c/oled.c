#include "oled.h"
#include "oled_font.h"

#define I2C_MASTER_TIMEOUT 1000 / portTICK_PERIOD_MS

void OLED_Init(void)
{
    // 关闭显示
    OLED_WR_Byte(0xAE, OLED_CMD);
    // 设置低列地址
    OLED_WR_Byte(0x00, OLED_CMD);
    // 设置高列地址
    OLED_WR_Byte(0x10, OLED_CMD);
    // 设置起始行地址
    OLED_WR_Byte(0x40, OLED_CMD);
    // 设置页地址
    OLED_WR_Byte(0xB0, OLED_CMD);
    // 对比度控制
    OLED_WR_Byte(0x81, OLED_CMD);
    OLED_WR_Byte(0xFF, OLED_CMD); // 128
    // 设置段重映射
    OLED_WR_Byte(0xA1, OLED_CMD);
    // 正常/反向显示
    OLED_WR_Byte(0xA6, OLED_CMD);
    // 设置多路复用比率(1到64)
    OLED_WR_Byte(0xA8, OLED_CMD);
    OLED_WR_Byte(0x3F, OLED_CMD); // 1/32 duty
    // 设置COM扫描方向
    OLED_WR_Byte(0xC8, OLED_CMD);
    // 设置显示偏移
    OLED_WR_Byte(0xD3, OLED_CMD);
    OLED_WR_Byte(0x00, OLED_CMD);
    // 设置振荡分频
    OLED_WR_Byte(0xD5, OLED_CMD);
    OLED_WR_Byte(0x80, OLED_CMD);
    // 关闭区域颜色模式
    OLED_WR_Byte(0xD8, OLED_CMD);
    OLED_WR_Byte(0x05, OLED_CMD);
    // 设置预充电周期
    OLED_WR_Byte(0xD9, OLED_CMD);
    OLED_WR_Byte(0xF1, OLED_CMD);
    // 设置COM引脚配置
    OLED_WR_Byte(0xDA, OLED_CMD);
    OLED_WR_Byte(0x12, OLED_CMD);
    // 设置Vcomh
    OLED_WR_Byte(0xDB, OLED_CMD);
    OLED_WR_Byte(0x30, OLED_CMD);
    // 设置充电泵使能
    OLED_WR_Byte(0x8D, OLED_CMD);
    OLED_WR_Byte(0x14, OLED_CMD);
    // 开启OLED面板
    OLED_WR_Byte(0xAF, OLED_CMD);

    OLED_Clear(); // 清屏
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

void OLED_Set_Pos(uint8_t x, uint8_t y)
{
    OLED_WR_Byte(0xb0 + y, OLED_CMD);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte((x & 0x0f), OLED_CMD);
}

void OLED_Clear(void)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        OLED_WR_Byte(0xb0 + i, OLED_CMD);
        OLED_WR_Byte(0x00, OLED_CMD);
        OLED_WR_Byte(0x10, OLED_CMD);
        for (uint8_t n = 0; n < 128; n++)
        {
            OLED_WR_Byte(0, OLED_DATA);
        }
    }
}

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size)
{
    uint8_t c = chr - ' ';
    if (x > 127)
    {
        x = 0;
        y += (Char_Size == 16) ? 2 : 1;
    }

    if (Char_Size == 16)
    {
        OLED_Set_Pos(x, y);
        for (uint8_t i = 0; i < 8; i++)
        {
            OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
        }
        OLED_Set_Pos(x, y + 1);
        for (uint8_t i = 0; i < 8; i++)
        {
            OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
        }
    }
    else
    {
        OLED_Set_Pos(x, y);
        for (uint8_t i = 0; i < 6; i++)
        {
            OLED_WR_Byte(F6x8[c][i], OLED_DATA);
        }
    }
}

void OLED_ShowString(uint8_t x, uint8_t y, char *chr, uint8_t Char_Size)
{
    while (*chr != '\0')
    {
        OLED_ShowChar(x, y, *chr, Char_Size);
        x += (Char_Size == 16) ? 8 : 6;
        if (x > 120)
        {
            x = 0;
            y += (Char_Size == 16) ? 2 : 1;
        }
        chr++;
    }
}

void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no)
{
    uint8_t adder = 0;
    OLED_Set_Pos(x, y);
    for (uint8_t t = 0; t < 16; t++)
    {
        OLED_WR_Byte(Hzk[2 * no][t], OLED_DATA);
        adder++;
    }
    OLED_Set_Pos(x, y + 1);
    for (uint8_t t = 0; t < 16; t++)
    {
        OLED_WR_Byte(Hzk[2 * no + 1][t], OLED_DATA);
        adder++;
    }
}

uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--)
    {
        result *= m;
    }
    return result;
}

void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size2)
{
    uint8_t enshow = 0;
    for (uint8_t t = 0; t < len; t++)
    {
        uint8_t temp = (num / oled_pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                OLED_ShowChar(x + (size2 / 2) * t, y, ' ', size2);
                continue;
            }
            else
            {
                enshow = 1;
            }
        }
        OLED_ShowChar(x + (size2 / 2) * t, y, temp + '0', size2);
    }
}
