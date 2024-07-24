#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "hal/uart_types.h"
#include "soc/clk_tree_defs.h"
#include "esp_log.h"
#include <string.h>

#define RX_PIN GPIO_NUM_4
#define TX_PIN GPIO_NUM_5
#define BAUD   115200
#define UART_PORT 0
#define RX_BUFFER_SIZE (1024)
#define TX_BUFFER_SIZE (1024)

QueueHandle_t uart_queue;
void init_uart()
{
	uart_config_t uart_conf = {
		.baud_rate = BAUD,
		.data_bits = UART_DATA_8_BITS,
		.stop_bits = UART_STOP_BITS_1,
		.parity = UART_PARITY_DISABLE,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_DEFAULT,
	};
	ESP_ERROR_CHECK(uart_param_config(UART_PORT,&uart_conf));
	ESP_ERROR_CHECK(uart_set_pin(UART_PORT, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
	ESP_ERROR_CHECK(uart_driver_install(UART_PORT, RX_BUFFER_SIZE, TX_BUFFER_SIZE, 10, &uart_queue, 0));
}


void app_main(void)
{
	init_uart();
	char data[] = {"hello pxf23"};
	char * str = "hello esp32";
	char rx_data[128];
	int length = 0;
    while (true) {
		uart_write_bytes(UART_PORT, data, sizeof(data));
		vTaskDelay(pdMS_TO_TICKS(1000));
		uart_tx_chars(UART_PORT, str,strlen(str));
		vTaskDelay(pdMS_TO_TICKS(1000));
		length = uart_read_bytes(UART_PORT,rx_data,sizeof(rx_data),100);
		if(length > 0)
		{
			uart_write_bytes(UART_PORT, rx_data,length);
			uart_flush(UART_PORT);
		}
    }
}
