#include <stdio.h>
#include "hal/spi_types.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h" 
#include "freertos/task.h"
#include "string.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define PIN_NUM_CLK 5
#define PIN_NUM_MOSI   4
#define PIN_NUM_MISO   16
#define PIN_NUM_CS     7

#define PIN_BK_LIGHT  17
#define PIN_NUM_RST    15
#define PIN_NUM_DC     6

spi_device_handle_t dev_handle;
uint8_t PARALLEL_LINES = 30;

typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;

DRAM_ATTR static const lcd_init_cmd_t lcd_init_cmds[] = {
    /* Power contorl B, power control = 0, DC_ENA = 1 */
    {0xCF, {0x00, 0x83, 0X30}, 3},
    /* Power on sequence control,
     * cp1 keeps 1 frame, 1st frame enable
     * vcl = 0, ddvdh=3, vgh=1, vgl=2
     * DDVDH_ENH=1
     */
    {0xED, {0x64, 0x03, 0X12, 0X81}, 4},
    /* Driver timing control A,
     * non-overlap=default +1
     * EQ=default - 1, CR=default
     * pre-charge=default - 1
     */
    {0xE8, {0x85, 0x01, 0x79}, 3},
    /* Power control A, Vcore=1.6V, DDVDH=5.6V */
    {0xCB, {0x39, 0x2C, 0x00, 0x34, 0x02}, 5},
    /* Pump ratio control, DDVDH=2xVCl */
    {0xF7, {0x20}, 1},
    /* Driver timing control, all=0 unit */
    {0xEA, {0x00, 0x00}, 2},
    /* Power control 1, GVDD=4.75V */
    {0xC0, {0x26}, 1},
    /* Power control 2, DDVDH=VCl*2, VGH=VCl*7, VGL=-VCl*3 */
    {0xC1, {0x11}, 1},
    /* VCOM control 1, VCOMH=4.025V, VCOML=-0.950V */
    {0xC5, {0x35, 0x3E}, 2},
    /* VCOM control 2, VCOMH=VMH-2, VCOML=VML-2 */
    {0xC7, {0xBE}, 1},
    /* Memory access contorl, MX=MY=0, MV=1, ML=0, BGR=1, MH=0 */
    {0x36, {0x48}, 1},
    /* Pixel format, 16bits/pixel for RGB/MCU interface */
    {0x3A, {0x55}, 1},
    /* Frame rate control, f=fosc, 70Hz fps */
    {0xB1, {0x00, 0x1B}, 2},
    /* Enable 3G, disabled */
    {0xF2, {0x08}, 1},
    /* Gamma set, curve 1 */
    {0x26, {0x01}, 1},
    /* Positive gamma correction */
    {0xE0, {0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0X87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00}, 15},
    /* Negative gamma correction */
    {0XE1, {0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x1F}, 15},
    /* Column address set, SC=0, EC=0xEF */
    {0x2B, {0x00, 0x00, 0x01, 0x3f}, 4},
    /* Page address set, SP=0, EP=0x013F */
    {0x2A, {0x00, 0x00, 0x00, 0xEF}, 4},
    /* Memory write */
    {0x2C, {0}, 0},
    /* Entry mode set, Low vol detect disabled, normal display */
    {0xB7, {0x07}, 1},
    /* Display function control */
    {0xB6, {0x0A, 0x82, 0x27, 0x00}, 4},
    /* Sleep out */
    {0x11, {0}, 0x80},
    /* Display on */
    {0x29, {0}, 0x80},
    {0, {0}, 0xff},
};

void lcd_data(spi_device_handle_t spi, const uint8_t *data, int len)
{
    esp_err_t ret;
    spi_transaction_t t;
    if (len == 0) {
        return;    //no need to send anything
    }
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length = len * 8;             //Len is in bytes, transaction length is in bits.
    t.tx_buffer = data;             //Data
    t.user = (void*)1;              //D/C needs to be set to 1
    ret = spi_device_polling_transmit(spi, &t); //Transmit!
    assert(ret == ESP_OK);          //Should have had no issues.
}

//发送命令
void lcd_cmd(spi_device_handle_t handle, uint8_t cmd, bool keep_cs_active)
{
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    t.rx_buffer = NULL;
    t.user = (void*)0;
    if (keep_cs_active) {
        t.flags = SPI_TRANS_CS_KEEP_ACTIVE;   //Keep CS active after data transfer
    }
    esp_err_t ret = spi_device_polling_transmit(handle, &t);
}


void spi_pre_cb(spi_transaction_t *trans)
{
    int dc = (int)trans->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

int post_num = 0;
void spi_post_cb(spi_transaction_t *trans)
{
    //使用esp log打印spi传输的数据
    // ets_printf("ISR Triggered\n");;
    post_num++;
    if (post_num %2 == 0)
    {
        gpio_set_level(PIN_BK_LIGHT, 0);
    }
    else
    {
        gpio_set_level(PIN_BK_LIGHT, 1);
    }
}

void spi_init()
{
    //esp32 idf 初始化spi3
    spi_bus_config_t bus_config = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .flags = 0,
        .max_transfer_sz = 240 * PARALLEL_LINES * sizeof(uint16_t)
    };
    spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO);

    spi_device_interface_config_t dev_config = {
        .clock_source = SPI_CLK_SRC_DEFAULT,
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .clock_speed_hz = 10000,
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .pre_cb = spi_pre_cb,
        .post_cb = spi_post_cb,
        .queue_size = 7,  
    };
    spi_bus_add_device(SPI2_HOST, &dev_config, &dev_handle);

    //初始化PIN_NUM_RST，PIN_NUM_RST，PIN_NUM_DC管脚为推挽输出
    gpio_config_t pGPIOConfig = {
        .pin_bit_mask = (1ULL << PIN_NUM_RST) | (1ULL << PIN_NUM_DC) | (1ULL << PIN_BK_LIGHT),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&pGPIOConfig);

    gpio_set_level(PIN_NUM_RST,0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_set_level(PIN_NUM_RST, 1);

    gpio_set_level(PIN_BK_LIGHT,1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
}

uint16_t get_pixel_format()
{
    // When using SPI_TRANS_CS_KEEP_ACTIVE, bus must be locked/acquired
    spi_device_acquire_bus(dev_handle, portMAX_DELAY);

    lcd_cmd(dev_handle, 0x0C, true);

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8*2;
    t.flags = SPI_TRANS_USE_RXDATA;
    t.user = (void*)1;

    esp_err_t ret = spi_device_polling_transmit(dev_handle, &t);

    // Release bus
    spi_device_release_bus(dev_handle);
    return *((uint16_t*)t.rx_data);
}


void clear_lcd()
{
    static spi_transaction_t trans[6];
    uint8_t ypos = 0;

    uint16_t* linedata = heap_caps_malloc(240 * PARALLEL_LINES * sizeof(uint16_t), MALLOC_CAP_DMA);
    for (int i = 0; i < 240 * PARALLEL_LINES * sizeof(uint16_t) / 2; i++) {
        linedata[i] = SPI_SWAP_DATA_TX(0xFEA0, 16);
    }
    //memset(linedata, 0x0080, 240 * PARALLEL_LINES * sizeof(uint16_t));
    //In theory, it's better to initialize trans and data only once and hang on to the initialized
    //variables. We allocate them on the stack, so we need to re-init them each call.
    for (int x = 0; x < 6; x++) {
        memset(&trans[x], 0, sizeof(spi_transaction_t));
        if ((x & 1) == 0) {
            //Even transfers are commands
            trans[x].length = 8;
            trans[x].user = (void*)0;
        } else {
            //Odd transfers are data
            trans[x].length = 8 * 4;
            trans[x].user = (void*)1;
        }
        trans[x].flags = SPI_TRANS_USE_TXDATA;
    }
    
    trans[0].tx_data[0] = 0x2A;         //Column Address Set
    trans[1].tx_data[0] = 0;            //Start Col High
    trans[1].tx_data[1] = 0;            //Start Col Low
    trans[1].tx_data[2] = (240 - 1) >> 8;   //End Col High
    trans[1].tx_data[3] = (240 - 1) & 0xff; //End Col Low
    trans[2].tx_data[0] = 0x2B;         //Page address set
    trans[3].tx_data[0] = ypos >> 8;    //Start page high
    trans[3].tx_data[1] = ypos & 0xff;  //start page low
    trans[3].tx_data[2] = (ypos + PARALLEL_LINES - 1) >> 8; //end page high
    trans[3].tx_data[3] = (ypos + PARALLEL_LINES - 1) & 0xff; //end page low
    trans[4].tx_data[0] = 0x2C;         //memory write
    trans[5].tx_buffer = linedata;      //finally send the line data
    trans[5].length = 240 * 2 * 8 * PARALLEL_LINES;  //Data length, in bits
    trans[5].flags = 0; //undo SPI_TRANS_USE_TXDATA flag

    //Queue all transactions.
    for (int x = 0; x < 6; x++) {
        spi_device_queue_trans(dev_handle, &trans[x], portMAX_DELAY);
        
    }
}

void app_main(void)
{
    spi_init();
    uint16_t pixel_format = get_pixel_format();
    printf("pixel_format = %04x\n", pixel_format);

    int cmd = 0;
    while (lcd_init_cmds[cmd].databytes != 0xff) {
        lcd_cmd(dev_handle, lcd_init_cmds[cmd].cmd, false);
        lcd_data(dev_handle, lcd_init_cmds[cmd].data, lcd_init_cmds[cmd].databytes & 0x1F);
        if (lcd_init_cmds[cmd].databytes & 0x80) {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        cmd++;
    }

    clear_lcd();
    while (1)
    {
        //send_cmd(dev_handle, 0x11);
        //send_cmd(dev_handle, 0xB6);
        
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
}
