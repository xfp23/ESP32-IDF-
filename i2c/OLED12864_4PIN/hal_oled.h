#ifndef __HAL_OLED_H
#define __HAL_OLED_H
#include "driver/i2c_master.h"

#define OLED_ADDR 0x3c        //屏幕地址
#define OLED_CMD 0
#define OLED_DATA 1
#define OLED_FREQ 100000
#define OLED_WIDTH 64
#define OLED_LEN 128


typedef enum {
    GOO_UT,                  //熄灭
    LIGHT_UP                //点亮
}Pixel_mode_t;

typedef enum {
    big=16,     //大号字体
    small=8     //小号字体
}font_size_t;

typedef void(*INIT)(void);              //初始化
typedef void (*SHOWCHAR)(uint8_t,uint8_t,uint8_t,font_size_t);
typedef void (*SHOWNUM)(uint8_t,uint8_t,uint32_t,font_size_t);
typedef void (*SHOWSTRING)(uint8_t,uint8_t,char *,font_size_t);
typedef void (*DRAWPOINT)(uint8_t,uint8_t,Pixel_mode_t);
typedef void (*SHOWFLOAT)(uint8_t,uint8_t,float,font_size_t);
typedef void (*REFRESH)(void);
typedef void (*CLEAR)(void);



typedef struct {
    INIT init;
    SHOWCHAR show_char;
    SHOWNUM show_num;
    SHOWSTRING show_string;
    SHOWFLOAT show_float;
    CLEAR clear;
    REFRESH refresh;
    DRAWPOINT point;
}OLED_DEV;




void OLED_Init(void);                                                                 //初始化屏幕
void OLED_Set_Pos(uint8_t x, uint8_t y);                                              //设置屏幕坐标
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, font_size_t Char_Size);             //显示单个字节
void OLED_Clear(void);                                                                //清屏
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, font_size_t size2);
uint32_t oled_pow(uint8_t m, uint8_t n);                                               //坐标辅助函数
void OLED_ShowString(uint8_t x, uint8_t y, char *chr, font_size_t Char_Size);             //显示字符串
void OLED_ShowFloat(uint8_t x, uint8_t y, float num, font_size_t size2);
void OLED_DrawPoint(uint8_t x, uint8_t y, Pixel_mode_t mode);
void OLED_Refresh();                                                                  //刷新
void OLED_Set_Pos(uint8_t x, uint8_t y);                                               //设置坐标
void oled_new_config(OLED_DEV *dev,i2c_master_dev_handle_t * handle);
esp_err_t OLED_WR_Byte(uint8_t byte, uint8_t cmd);
#endif