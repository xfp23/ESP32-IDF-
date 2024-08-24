**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息

 ©  2024 . 未经许可不得复制、修改或分发。 此文献为 **DXG工作室**  所有。

 # 简介

- [wifi模块类型文档](wifi_typed.md)

ESP32 的 Wi-Fi AP 模式允许它作为一个无线接入点，能够让其他设备（如智能手机、笔记本电脑等）连接到它，就像连接到家庭或办公室的 Wi-Fi 路由器一样。这种模式非常适用于需要创建本地网络的应用场景，比如在物联网设备中，ESP32 可以通过 AP 模式为其他设备提供一个连接点，从而实现数据交换或远程控制。

### 主要特点

1. **SSID（服务集标识符）**: 在 AP 模式下，ESP32 会广播一个网络名称，即 SSID。用户设备可以扫描并找到这个网络名称，然后连接到 ESP32。

2. **信道选择**: AP 模式下，ESP32 可以在指定的 Wi-Fi 信道上工作。选择合适的信道可以避免干扰，优化网络性能，尤其是在有多个 Wi-Fi 网络的环境中。

3. **安全性**: ESP32 支持多种加密和认证模式，例如开放模式（无密码）或 WPA2 加密模式，确保连接的设备和数据传输的安全。

4. **最大连接数**: ESP32 允许多个设备同时连接到它。用户可以设置最大连接数，通常为 4 个设备。这对于小型本地网络非常实用。

5. **本地网络创建**: 在 AP 模式下，ESP32 可以独立工作，无需外部路由器，适合局部区域的网络通信，例如智能家居控制系统或临时的现场设备调试。

# 环境配置

1. 配置cmake

```c

REQUIRES esp_wifi nvs_flash

```

2. 头文件包含:

```c
#include "esp_wifi.h"
#include "esp_mac.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_system.h"
```

# 程序编写

### ESP32 WiFi AP模式教程

在这篇教程中，我们将学习如何使用ESP32的WiFi AP（Access Point）模式，使ESP32能够作为一个无线接入点，允许其他设备连接到它。

### 步骤 1: 初始化NVS和网络接口

首先，我们需要初始化NVS（Non-Volatile Storage）和网络接口。这是因为WiFi设置保存在NVS中，并且我们需要初始化网络接口以便管理WiFi连接。

```c
void wifi_init() {
    nvs_flash_init();  // 初始化NVS
    esp_netif_init();  // 初始化网络接口
}
```

### 步骤 2: 创建事件循环

为了处理WiFi事件，我们需要创建一个事件循环。

```c
void wifi_init() {
    // ... 之前的代码 ...
    esp_event_loop_create_default();  // 创建事件循环
}
```

### 步骤 3: 创建WiFi AP接口

创建一个WiFi AP接口来管理WiFi连接。这个接口负责广播网络名称（SSID）并允许其他设备连接。

```c
void wifi_init() {
    // ... 之前的代码 ...
    esp_netif_create_default_wifi_ap();  // 创建WiFi AP接口
}
```

### 步骤 4: 注册事件回调函数

我们需要注册事件回调函数，以便处理各种WiFi事件，例如设备连接和断开连接。

```c
void wifi_init() {
    // ... 之前的代码 ...
    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        esp_event_handler_cb,
                                        NULL,
                                        &event_handle);
}
```

### 步骤 5: 配置并启动WiFi

配置WiFi AP模式，包括SSID、密码、信道等信息，然后启动WiFi。

```c
void wifi_init() {
    // ... 之前的代码 ...

    wifi_init_config_t wifi_conf = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_conf);
    esp_wifi_set_mode(WIFI_MODE_AP);  // 设置为AP模式

    wifi_config_t wifi_conf_interface = {
        .ap = {
            .ssid = "PXF-ESP32",
            .ssid_len = strlen("PXF-ESP32"),
            .channel = 6,
            .password = "12345678",
            .max_connection = 12,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .required = false,
            },
        }
    };
    esp_wifi_set_config(WIFI_IF_AP, &wifi_conf_interface);  // 配置AP模式
    esp_wifi_start();  // 启动WiFi
}
```

### 步骤 6: 处理WiFi事件

在事件回调函数中，我们处理各种WiFi事件，例如设备连接和断开连接。

```c
void esp_event_handler_cb(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) //此处处理AP被连接事件
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        printf("Station " MACSTR " joined, AID=%d\n",
               MAC2STR(event->mac), event->aid);
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) // 此处处理AP断开事件
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        printf("Station " MACSTR " left, AID=%d\n",
               MAC2STR(event->mac), event->aid);
    }
}
```

### 步骤 7: 主程序

在`app_main`中，调用`wifi_init()`来启动WiFi AP模式，并使用FreeRTOS的`vTaskDelay`函数让程序持续运行。

```c
void app_main() {
    wifi_init();
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));  // 让程序持续运行
    }
}
```