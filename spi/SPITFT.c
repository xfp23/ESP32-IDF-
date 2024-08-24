#include <stdio.h>
#include "hal/spi_types.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h" 
#include "freertos/task.h"
#include "string.h"
#include "driver/gpio.h"
#include "esp_log.h"

// 引脚定义
#define PIN_NUM_CLK 5
#define PIN_NUM_MOSI 4
#define PIN_NUM_MISO 16
#define PIN_NUM_CS 7

#define PIN_BK_LIGHT 17
#define PIN_NUM_RST 15
#define PIN_NUM_DC 6

spi_device_handle_t dev_handle;
uint8_t PARALLEL_LINES = 30;  // 并行传输的行数

// 初始化LCD的命令结构体
typedef struct {
    uint8_t cmd;       // 命令
    uint8_t data[16];  // 数据
    uint8_t databytes; // 数据长度; bit 7 = 发送后延时; 0xFF = 命令结束.
} lcd_init_cmd_t;

// DRAM_ATTR指定变量放在DRAM中
DRAM_ATTR static const lcd_init_cmd_t lcd_init_cmds[] = {
    // LCD初始化命令序列
    {0xCF, {0x00, 0x83, 0X30}, 3},
    {0xED, {0x64, 0x03, 0X12, 0X81}, 4},
    {0xE8, {0x85, 0x01, 0x79}, 3},
    {0xCB, {0x39, 0x2C, 0x00, 0x34, 0x02}, 5},
    {0xF7, {0x20}, 1},
    {0xEA, {0x00, 0x00}, 2},
    {0xC0, {0x26}, 1},
    {0xC1, {0x11}, 1},
    {0xC5, {0x35, 0x3E}, 2},
    {0xC7, {0xBE}, 1},
    {0x36, {0x48}, 1},
    {0x3A, {0x55}, 1},
    {0xB1, {0x00, 0x1B}, 2},
    {0xF2, {0x08}, 1},
    {0x26, {0x01}, 1},
    {0xE0, {0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0X87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00}, 15},
    {0XE1, {0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x1F}, 15},
    {0x2B, {0x00, 0x00, 0x01, 0x3f}, 4},
    {0x2A, {0x00, 0x00, 0x00, 0xEF}, 4},
    {0x2C, {0}, 0},
    {0xB7, {0x07}, 1},
    {0xB6, {0x0A, 0x82, 0x27, 0x00}, 4},
    {0x11, {0}, 0x80},
    {0x29, {0}, 0x80},
    {0, {0}, 0xff},
};

// 发送数据到LCD
void lcd_data(spi_device_handle_t spi, const uint8_t *data, int len)
{
    esp_err_t ret;
    spi_transaction_t t;
    if (len == 0) {
        return;    // 无需发送
    }
    memset(&t, 0, sizeof(t));       // 清空事务结构体
    t.length = len * 8;             // 数据长度，以位为单位
    t.tx_buffer = data;             // 发送的数据
    t.user = (void*)1;              // D/C 置为 1
    ret = spi_device_polling_transmit(spi, &t); // 发送数据
    assert(ret == ESP_OK);          // 确保无错误
}

// 发送命令到LCD
void lcd_cmd(spi_device_handle_t handle, uint8_t cmd, bool keep_cs_active)
{
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       // 清空事务结构体
    t.length = 8;                   // 命令长度为 8 位
    t.tx_buffer = &cmd;             // 发送的命令
    t.rx_buffer = NULL;             // 无接收缓冲区
    t.user = (void*)0;              // D/C 置为 0
    if (keep_cs_active) {
        t.flags = SPI_TRANS_CS_KEEP_ACTIVE;   // 保持CS激活
    }
    esp_err_t ret = spi_device_polling_transmit(handle, &t); // 发送命令
}

// SPI传输前的回调函数
void spi_pre_cb(spi_transaction_t *trans)
{
    int dc = (int)trans->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

// SPI传输后的回调函数
int post_num = 0;
void spi_post_cb(spi_transaction_t *trans)
{
    // 使用esp log打印SPI传输的数据
    post_num++;
    if (post_num % 2 == 0)
    {
        gpio_set_level(PIN_BK_LIGHT, 0); // 关闭背光
    }
    else
    {
        gpio_set_level(PIN_BK_LIGHT, 1); // 打开背光
    }
}

// 初始化SPI
void spi_init()
{
    // ESP32 IDF 初始化 SPI3
    spi_bus_config_t bus_config = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .flags = 0,
        .max_transfer_sz = 240 * PARALLEL_LINES * sizeof(uint16_t) // 最大传输大小
    };
    spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO);

    spi_device_interface_config_t dev_config = {
        .clock_source = SPI_CLK_SRC_DEFAULT,
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .clock_speed_hz = 10000, // 时钟速度
        .mode = 0,               // SPI模式
        .spics_io_num = PIN_NUM_CS, // 片选引脚
        .pre_cb = spi_pre_cb, // 传输前的回调函数
        .post_cb = spi_post_cb, // 传输后的回调函数
        .queue_size = 7,  
    };
    spi_bus_add_device(SPI2_HOST, &dev_config, &dev_handle);

    // 初始化 RST、DC 和 背光 引脚为推挽输出
    gpio_config_t pGPIOConfig = {
        .pin_bit_mask = (1ULL << PIN_NUM_RST) | (1ULL << PIN_NUM_DC) | (1ULL << PIN_BK_LIGHT),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&pGPIOConfig);

    gpio_set_level(PIN_NUM_RST, 0);  // 复位LCD
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_set_level(PIN_NUM_RST, 1);

    gpio_set_level(PIN_BK_LIGHT, 1); // 打开背光
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

// 获取像素格式
uint16_t get_pixel_format()
{
    // 使用 SPI_TRANS_CS_KEEP_ACTIVE 时，需要锁定总线
    spi_device_acquire_bus(dev_handle, portMAX_DELAY);

    lcd_cmd(dev_handle, 0x0C, true); // 发送读取像素格式命令

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8 * 2; // 接收 16 位数据
    t.flags = SPI_TRANS_USE_RXDATA;
    t.user = (void*)1;

    esp_err_t ret = spi_device_polling_transmit(dev_handle, &t);

    // 释放总线
    spi_device_release_bus(dev_handle);
    return *((uint16_t*)t.rx_data); // 返回像素格式
}

// 清除LCD显示
void clear_lcd()
{
    static spi_transaction_t trans[6]; // 存储SPI事务
    uint8_t ypos = 0;
    uint16_t* linedata = heap_caps_malloc(240 * PARALLEL_LINES * sizeof(uint16_t), MALLOC_CAP_DMA);

    // 填充颜色数据
    for (int i = 0; i < 240 * PARALLEL_LINES; i++) {
        linedata[i] = SPI_SWAP_DATA_TX(0xFEA0, 16); // 填充颜色
    }

    // 配置SPI事务
    for (int x = 0; x < 6; x++) {
        memset(&trans[x], 0, sizeof(spi_transaction_t)); // 清空事务
        if ((x & 1) == 0) {
            trans[x].length = 8; // 设置命令长度为8位
            trans[x].user = (void*)0; // 表示命令
        } else {
            trans[x].length = 8 * 4; // 设置数据长度
            trans[x].user = (void*)1; // 表示数据
        }
        trans[x].flags = SPI_TRANS_USE_TXDATA;
    }

    trans[0].tx_data[0] = 0x2A;         // 列地址设置命令
    trans[1].tx_data[0] = 0;            // 起始列高位
    trans[1].tx_data[1] = 0;            // 起始列低位
    trans[1].tx_data[2] = 0;            // 结束列高位
    trans[1].tx_data[3] = 239;          // 结束列低位

    trans[2].tx_data[0] = 0x2B;         // 行地址设置命令
    trans[3].tx_data[0] = ypos >> 8;    // 起始行高位
    trans[3].tx_data[1] = ypos & 0xff;  // 起始行低位
    trans[3].tx_data[2] = (ypos + PARALLEL_LINES - 1) >> 8; // 结束行高位
    trans[3].tx_data[3] = (ypos + PARALLEL_LINES - 1) & 0xff; // 结束行低位

    trans[4].tx_data[0] = 0x2C;         // 内存写入命令

    trans[5].tx_buffer = linedata;      // 设置数据缓冲区
    trans[5].length = 240 * PARALLEL_LINES * 16; // 数据长度
    trans[5].flags = 0;                 // 无额外标志

    // 执行 SPI 事务
    for (int i = 0; i < 6; i++) {
        esp_err_t ret = spi_device_polling_transmit(dev_handle, &trans[i]);
        assert(ret == ESP_OK);
    }

    heap_caps_free(linedata); // 释放分配的内存
}
