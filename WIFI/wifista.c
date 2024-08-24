#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_system.h"
#include "string.h"

esp_event_handler_instance_t event_handle;
esp_event_handler_instance_t IP_event_handle;

void esp_event_handler_cb(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_ETH_GOT_IP) {
        ip_event_got_ip_t *ip_data  = (ip_event_got_ip_t*) event_data;
        esp_ip4_addr_t ip = ip_data->ip_info.ip;
        printf("IPAddress : " IPSTR "\n", IP2STR(&ip));
    }
}

void wifi_init() {
    nvs_flash_init();  // 初始化NVS
    esp_netif_init();  // 初始化网络接口
    esp_event_loop_create_default();  // 创建事件循环
    esp_netif_create_default_wifi_sta();

    // 注册事件回调
    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        esp_event_handler_cb,
                                        NULL,
                                        &event_handle);

    esp_event_handler_instance_register(IP_EVENT,
                                        IP_EVENT_ETH_GOT_IP,  // 监听WiFi连接的IP获取事件
                                        esp_event_handler_cb,
                                        NULL,
                                        &IP_event_handle);

    wifi_init_config_t wifi_conf = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_conf);
    esp_wifi_set_mode(WIFI_MODE_STA);
    wifi_config_t wifi_conf_interface = {
        .sta = {
            .ssid = "Pxf",
            .password = "2544601564"
        }
    };
    esp_wifi_set_config(WIFI_IF_STA, &wifi_conf_interface);   
    esp_wifi_start();                                                             
}

void app_main() {
    wifi_init();
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
