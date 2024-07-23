#include "hal_oled.h"
#include "oled.h"
#include "oled_font.h"

static i2c_master_dev_handle_t *OLED_handle;

static uint8_t OLED_GRAM[128][8];

void oled_new_config(OLED_DEV *dev, i2c_master_dev_handle_t *handle) {
    dev->init = OLED_Init;
    dev->point = OLED_DrawPoint;
    dev->refresh = OLED_Refresh;
    dev->show_char = OLED_ShowChar;
    dev->show_num = OLED_ShowNum;
    dev->show_string = OLED_ShowString;
    dev->show_float = OLED_ShowFloat;
    dev->clear = OLED_Clear;
    dev->handle = handle;
}

void OLED_Init() {
    const uint8_t init_cmds[] = {
        0xAE,      
        0x00,       
        0x10,       
        0x40,       
        0xB0,     
        0x81, 0xFF,
        0xA1,     
        0xA6,       
        0xA8, 0x3F, 
        0xC8,      
        0xD3, 0x00,
        0xD5, 0x80,
        0xD9, 0xF1, 
        0xDA, 0x12, 
        0xDB, 0x40, 
        0x8D, 0x14, 
        0xAF        
    };

    for (uint8_t i = 0; i < sizeof(init_cmds); i++) {
        OLED_WR_Byte(init_cmds[i], OLED_CMD);
    }

    OLED_Clear();
}

esp_err_t OLED_WR_Byte(uint8_t byte, uint8_t cmd) {
    uint8_t write_buf[2] = { (cmd == OLED_DATA) ? 0x40 : 0x00, byte };
    esp_err_t ret = i2c_master_transmit(*OLED_handle, write_buf, sizeof(write_buf), -1);
    if (ret != ESP_OK) {
        ESP_LOGE("OLED", "I2C transmission failed: %s", esp_err_to_name(ret));
    }
    return ret;
}

void OLED_Clear(void) {
    for (uint8_t page = 0; page < 8; page++) {
        OLED_Set_Pos(0, page);
        for (uint8_t col = 0; col < 128; col++) {
            OLED_WR_Byte(0x00, OLED_DATA);
        }
    }
}

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, font_size_t Char_Size) {
    uint8_t c = chr - ' ';
    if (x > 127) {
        x = 0;
        y = y + 2;
    }
    if (Char_Size == 16) {
        OLED_Set_Pos(x, y);
        for (uint8_t i = 0; i < 8; i++)
            OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
        OLED_Set_Pos(x, y + 1);
        for (uint8_t i = 0; i < 8; i++)
            OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
    } else {
        OLED_Set_Pos(x, y);
        for (uint8_t i = 0; i < 6; i++)
            OLED_WR_Byte(F6x8[c][i], OLED_DATA);
    }
}

void OLED_ShowString(uint8_t x, uint8_t y, char *chr, font_size_t Char_Size) {
    while (*chr != '\0') {
        OLED_ShowChar(x, y, *chr, Char_Size);
        x += (Char_Size == 16) ? 8 : 6;
        if (x > 127) {
            x = 0;
            y += (Char_Size == 16) ? 2 : 1;
        }
        chr++;
    }
}

uint32_t oled_pow(uint8_t m, uint8_t n) {
    uint32_t result = 1;
    while (n--)
        result *= m;
    return result;
}

void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, font_size_t size2) {
    uint8_t len = 0;
    uint32_t temp = num;
    do {
        len++;
        temp /= 10;
    } while (temp > 0);

    for (uint8_t t = 0; t < len; t++) {
        uint8_t digit = (num / oled_pow(10, len - t - 1)) % 10;

        if (t > 0 || digit > 0 || len == 1) {
            OLED_ShowChar(x + (size2 / 2) * t, y, digit + '0', size2);
        } else {
            OLED_ShowChar(x + (size2 / 2) * t, y, ' ', size2);
        }
    }
}

void OLED_ShowFloat(uint8_t x, uint8_t y, float num, font_size_t size2) {
  
    char str[16];
    snprintf(str, sizeof(str), "%.*f", 2, num); 
    OLED_ShowString(x, y, str, size2);
}

void OLED_Set_Pos(uint8_t x, uint8_t y) {
    if (y > 7) y = 7; 

    OLED_WR_Byte(0xB0 + y, OLED_CMD);                   
    OLED_WR_Byte(((x & 0xF0) >> 4) | 0x10, OLED_CMD); 
    OLED_WR_Byte((x & 0x0F), OLED_CMD);    
}

void OLED_WriteData(uint8_t x, uint8_t y, uint8_t data) {
    OLED_Set_Pos(x, y);
    OLED_WR_Byte(data, OLED_DATA);
}

void OLED_DrawPoint(uint8_t x, uint8_t y, Pixel_mode_t mode) {
    uint8_t page = y / 8; 
    uint8_t bit = 1 << (y % 8); 

    if (mode) {
        OLED_GRAM[x][page] |= bit;  
    } else {
        OLED_GRAM[x][page] &= ~bit;
    }

    OLED_WriteData(x, page, OLED_GRAM[x][page]);
}

void OLED_Refresh() {
    for (uint8_t page = 0; page < 8; page++) {
        OLED_Set_Pos(0, page);
        for (uint8_t x = 0; x < 128; x++) {
            OLED_WR_Byte(OLED_GRAM[x][page], OLED_DATA);
        }
    }
}
