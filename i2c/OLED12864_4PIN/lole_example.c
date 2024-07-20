#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "oled.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SDA GPIO_NUM_17
#define SCL GPIO_NUM_18
#define I2C_NUM 0
#define GLITCH_IGNORE 7

#define DEVICE_ADDRESS 0x3c
//注意，句柄一定要声明为全局
i2c_master_bus_handle_t i2c_master_bus_handle;
i2c_master_dev_handle_t OLED_handle;
//声明一个OLED对象
OLED_DEV oled;
void init_i2c_bus();

void app_main(void)
{
	init_i2c_bus();
	oled_new_config(&oled,&OLED_handle);
	oled.init();
	while(1)
	{
		//oled.show_string(0,2,"ESP32",small);
		//vTaskDelay(6000);
		oled.clear();
		for(int x = 10;x<110;x++)
		{
			for(int y = 30;y < 50; y++)
			{
				oled.point(x,y,LIGHT_UP);
				//vTaskDelay(1);
			}
		}
	}
	

}

void init_i2c_bus()
{
	i2c_master_bus_config_t i2c_mast_bus_config = {
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.i2c_port = I2C_NUM,
		.scl_io_num = SCL,
		.sda_io_num = SDA,
		.glitch_ignore_cnt = GLITCH_IGNORE,
		.flags.enable_internal_pullup = true,
	};
	ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mast_bus_config, &i2c_master_bus_handle));
	i2c_device_config_t dev_cfg = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = DEVICE_ADDRESS,
		.scl_speed_hz = 400000,
	};
	ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_master_bus_handle, &dev_cfg, &OLED_handle));
}
