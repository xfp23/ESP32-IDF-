#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include "esp_private/adc_private.h"
#include "hal/adc_types.h"
#include "hal/uart_types.h"
#include "soc/clk_tree_defs.h"
#include <unistd.h>
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "math.h"

#define UART_NUM 0
#define TX_PIN 19
#define RX_PIN 20

//句柄；
adc_oneshot_unit_handle_t adc_handle;
QueueHandle_t uart_handle;
void init_one_adc();
int adc_read();
void init_uart();
void print_adc_result(int);
void app_main(void)
{
	init_uart();
	init_one_adc();
	while(1)
	{
		print_adc_result(adc_read());
		vTaskDelay(100);
	}
}
void init_one_adc()
{
	adc_oneshot_unit_init_cfg_t adc_cfg = {
		.unit_id = ADC_UNIT_1,
		.clk_src = ADC_RTC_CLK_SRC_DEFAULT,
		.ulp_mode = ADC_ULP_MODE_DISABLE,
	};
	ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_cfg, &adc_handle));
	adc_oneshot_chan_cfg_t adc_chan_cfg = {
		.atten = ADC_ATTEN_DB_0,
		.bitwidth = ADC_BITWIDTH_12,
	};
	ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle,ADC_CHANNEL_9, &adc_chan_cfg));
	
}
void init_uart()
{
	uart_config_t uart_cfg = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_APB,
	};
	ESP_ERROR_CHECK(uart_param_config(UART_NUM,&uart_cfg));
	ESP_ERROR_CHECK(uart_set_pin(UART_NUM,TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
	ESP_ERROR_CHECK(uart_driver_install(UART_NUM,1024, 1024,10, &uart_handle,0));
}
int adc_read()
{
	int result;
	ESP_ERROR_CHECK(adc_oneshot_read(adc_handle,ADC_CHANNEL_9,&result));
	result = (result * 3.3)/pow(2,12);
	return result;
}

void print_adc_result(int adc_value)
{
	char adc_print[20];
	sprintf(adc_print,"adc_value: %d",adc_value);
	uart_tx_chars(UART_NUM, adc_print, strlen(adc_print));
	uart_flush(UART_NUM);
}