#include "oled.h"
#include "oled_font.h"

void OLED_Init(void)
{
    OLED_WR_Byte(0xAE, OLED_CMD); //--关闭显示 (Turn off display)
    OLED_WR_Byte(0x00, OLED_CMD); //--设置低列地址 (Set low column address)
    OLED_WR_Byte(0x10, OLED_CMD); //--设置高列地址 (Set high column address)
    OLED_WR_Byte(0x40, OLED_CMD); //--设置起始行地址 (Set start line address)
    OLED_WR_Byte(0xB0, OLED_CMD); //--设置页地址 (Set page address)
    OLED_WR_Byte(0x81, OLED_CMD); //--对比度控制 (Contrast control)
    OLED_WR_Byte(0xFF, OLED_CMD); //--128 (128)
    OLED_WR_Byte(0xA1, OLED_CMD); //--设置段重映射 (Set segment remap)
    OLED_WR_Byte(0xA6, OLED_CMD); //--正常/反向显示 (Normal/Reverse display)
    OLED_WR_Byte(0xA8, OLED_CMD); //--设置多路复用比率(1到64) (Set multiplex ratio(1 to 64))
    OLED_WR_Byte(0x3F, OLED_CMD); //--1/32占空比 (1/32 duty)
    OLED_WR_Byte(0xC8, OLED_CMD); //--设置COM扫描方向 (Set COM scan direction)
    OLED_WR_Byte(0xD3, OLED_CMD); //--设置显示偏移 (Set display offset)
    OLED_WR_Byte(0x00, OLED_CMD); //
    OLED_WR_Byte(0xD5, OLED_CMD); //--设置振荡分频 (Set oscillator division)
    OLED_WR_Byte(0x80, OLED_CMD); //
    OLED_WR_Byte(0xD8, OLED_CMD); //--关闭区域颜色模式 (Set area color mode off)
    OLED_WR_Byte(0x05, OLED_CMD); //
    OLED_WR_Byte(0xD9, OLED_CMD); //--设置预充电周期 (Set Pre-Charge Period)
    OLED_WR_Byte(0xF1, OLED_CMD); //
    OLED_WR_Byte(0xDA, OLED_CMD); //--设置COM引脚配置 (Set COM pin configuration)
    OLED_WR_Byte(0x12, OLED_CMD);
    OLED_WR_Byte(0xDB, OLED_CMD); //--设置Vcomh (Set Vcomh)
    OLED_WR_Byte(0x30, OLED_CMD); //
    OLED_WR_Byte(0x8D, OLED_CMD); //--设置充电泵使能 (Set charge pump enable)
    OLED_WR_Byte(0x14, OLED_CMD); //
    OLED_WR_Byte(0xAF, OLED_CMD); //--开启OLED面板 (Turn on OLED panel)
    OLED_Clear(); //--清屏 (Clear screen)
}

esp_err_t OLED_WR_Byte(uint8_t data, uint8_t cmd_)
{
    int ret;

    uint8_t write_buf[2] = {((cmd_ == 1) ? (0x40) : (0x00)), data};

    ret = i2c_master_write_to_device(I2C_MASTER_NUM, OLED_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);

    return ret;
}


/**
 * @description: OLED 清屏
 * @return       无
 */
void OLED_Clear(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
    {
        OLED_WR_Byte(0xb0 + i, OLED_CMD);
        OLED_WR_Byte(0x00, OLED_CMD);
        OLED_WR_Byte(0x10, OLED_CMD);
        for (n = 0; n < 128; n++)
            OLED_WR_Byte(0, OLED_DATA);
    }
}

/**
 * @description: OLED 显示单个字符
 * @return       无
 * @param {uint8_t} x 显示字符的x坐标，范围0~127
 * @param {uint8_t} y 显示字符的y坐标，字符大小为16，取值0,2,4,6；字符大小6，取值0,1,2,3,4,5,6,7
 * @param {uint8_t} chr 显示的单个字符，在字库中出现的字符
 * @param {uint8_t} Char_Size 字符大小，取16或者8
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size)
{
    uint8_t c = 0;
    uint8_t i = 0;
    c = chr - ' ';
    if (x > 127)
    {
        x = 0;
        y = y + 2;
    }
    if (Char_Size == 16)
    {
        OLED_Set_Pos(x, y);
        for (i = 0; i < 8; i++)
            OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
        OLED_Set_Pos(x, y + 1);
        for (i = 0; i < 8; i++)
            OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
    }
    else
    {
        OLED_Set_Pos(x, y);
        for (i = 0; i < 6; i++)
            OLED_WR_Byte(F6x8[c][i], OLED_DATA);
    }
}

/**
 * @description: OLED 显示字符串，会自动换行
 * @return       无
 * @param {uint8_t} x 显示字符串第一个字符的x坐标，范围0~127
 * @param {uint8_t} y 显示字符串第一个字符的y坐标，字符大小为16，取值0,2,4,6；字符大小6，取值0,1,2,3,4,5,6,7
 * @param {char} *chr 显示的字符串
 * @param {uint8_t} Char_Size 字符大小，取16或者8
 */
void OLED_ShowString(uint8_t x, uint8_t y, char *chr, uint8_t Char_Size)
{
    unsigned char j = 0;
    while (chr[j] != '\0')
    {
        OLED_ShowChar(x, y, chr[j], Char_Size);
        x += 8;
        if (x > 120)
        {
            x = 0;
            y += 2;
        }
        j++;
    }
}

/**
 * @description: OLED 显示汉字
 * @return       无
 * @param {uint8_t} x 显示汉字的x坐标
 * @param {uint8_t} y 显示汉字的y坐标
 * @param {uint8_t} no 显示汉字在字库中的序号
 */
void OLED_ShowCHinese(uint8_t x, uint8_t y, uint8_t no)
{
    uint8_t t, adder = 0;
    OLED_Set_Pos(x, y);
    for (t = 0; t < 16; t++)
    {
        OLED_WR_Byte(Hzk[2 * no][t], OLED_DATA);
        adder += 1;
    }
    OLED_Set_Pos(x, y + 1);
    for (t = 0; t < 16; t++)
    {
        OLED_WR_Byte(Hzk[2 * no + 1][t], OLED_DATA);
        adder += 1;
    }
}

/**
 * @description: 求m^n的函数
 * @return       m^n的结果
 * @param {uint8_t} m 底数
 * @param {uint8_t} n 指数
 */
uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--)
        result *= m;
    return result;
}

/**
 * @description: OLED 显示数字
 * @return       无
 * @param {uint8_t} x 显示数字的第一个位置的x坐标
 * @param {uint8_t} y 显示数字的第一个位置的y坐标
 * @param {uint32_t} num 欲显示的数字
 * @param {uint8_t} len 显示所占的长度，不建议小于真正要显示的数字的长度
 * @param {uint8_t} size2 显示的数字的大小，16、8可选
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size2)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                OLED_ShowChar(x + (size2 / 2) * t, y, ' ', size2);
                continue;
            }
            else
                enshow = 1;
        }
        OLED_ShowChar(x + (size2 / 2) * t, y, temp + '0', size2);
    }
}
void OLED_Set_Pos(uint8_t x, uint8_t y)
{
    OLED_WR_Byte(0xb0 + y, OLED_CMD);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte((x & 0x0f), OLED_CMD);
}
