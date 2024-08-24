**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息
© 2024 . 未经许可不得复制、修改或分发。 此文献为 **DXG工作室**  所有。

# OLED12864 编程指南 (新版API) iic设备

- [技术规格书](/PDF/WH-096-4pin-I2C-SSD1306.pdf)
- 设备地址： **0x3c

# 初始化oled

```c

#define OLED_CMD 0
#define OLED_DATA 1

void OLED_Init(void)
{
OLED_WR_Byte(0xAE, OLED_CMD); // 关闭 OLED 面板
OLED_WR_Byte(0x00, OLED_CMD); // 设置低列地址
OLED_WR_Byte(0x10, OLED_CMD); // 设置高列地址
OLED_WR_Byte(0x40, OLED_CMD); // 设置起始行地址
OLED_WR_Byte(0x81, OLED_CMD); // 设置对比度控制寄存器
OLED_WR_Byte(0xCF, OLED_CMD); // 设置 SEG 输出电流亮度
OLED_WR_Byte(0xA1, OLED_CMD); // 设置 SEG/列映射 0xa0 左右反置 0xa1 正常
OLED_WR_Byte(0xC8, OLED_CMD); // 设置 COM/行扫描方向 0xc0 上下反置 0xc8 正常
OLED_WR_Byte(0xA6, OLED_CMD); // 设置正常显示
OLED_WR_Byte(0xA8, OLED_CMD); // 设置多路复用比率(1 to 64)
OLED_WR_Byte(0x3f, OLED_CMD); // 1/64 占空比
OLED_WR_Byte(0xD3, OLED_CMD); // 设置显示偏移 (0x00~0x3F)
OLED_WR_Byte(0x00, OLED_CMD); // 无偏移
OLED_WR_Byte(0xd5, OLED_CMD); // 设置显示时钟分频比/振荡频率
OLED_WR_Byte(0x80, OLED_CMD); // 设置分频比，时钟设为每秒 100 帧
OLED_WR_Byte(0xD9, OLED_CMD); // 设置预充电周期
OLED_WR_Byte(0xF1, OLED_CMD); // 预充电设为 15 个时钟，放电设为 1 个时钟
OLED_WR_Byte(0xDA, OLED_CMD); // 设置 COM 引脚硬件配置
OLED_WR_Byte(0x12, OLED_CMD);
OLED_WR_Byte(0xDB, OLED_CMD); // 设置 vcomh
OLED_WR_Byte(0x30, OLED_CMD); // 设置 VCOM 取消选择电平
OLED_WR_Byte(0x20, OLED_CMD); // 设置页面寻址模式 (0x00/0x01/0x02)
OLED_WR_Byte(0x02, OLED_CMD);
OLED_WR_Byte(0x8D, OLED_CMD); // 设置充电泵使能/禁用
OLED_WR_Byte(0x14, OLED_CMD); // 设置 (0x10) 禁用
OLED_Clear();
OLED_WR_Byte(0xAF, OLED_CMD); // 打开 OLED 面板
}

esp_err_t OLED_WR_Byte(int byte, int cmd)
{
    int ret;
     uint8_t write_buf[2] = {((cmd == 1) ? (0x40) : (0x00)), data};
    ret = i2c_master_transmit(oled_handle,write_buf,sizeof(write_buf),-1);
    return ret;
}

```

- 更多函数定义请查看源码，对写入字节函数做了替换[...](\OLED12864_4PIN\oled.h)