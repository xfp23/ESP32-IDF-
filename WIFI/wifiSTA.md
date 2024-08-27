### **WiFi STA编程指南**

---

# 版权信息

© 2024 . 未经许可不得复制、修改或分发。此文献为 [小風的藏書閣](https://t.me/xfp2333) 所有。

---

## 简介

- [wifi模块类型文档](wifi_typed.md)

WiFi STA模式（Station模式）是ESP32最常用的工作模式之一。在这个模式下，ESP32设备作为一个WiFi客户端，连接到一个已有的WiFi网络（如家里的路由器网络）。这是类似于手机或笔记本电脑连接到WiFi网络的方式。

### **主要特点**

1. **客户端模式**：
   - 在STA模式下，ESP32设备会像普通的WiFi设备一样，搜索并连接到指定的WiFi网络。这意味着它可以通过路由器访问局域网或互联网。

2. **动态IP地址获取**：
   - 连接成功后，ESP32通常会通过路由器的DHCP服务自动获取一个IP地址，并使用该地址进行网络通信。

3. **网络安全**：
   - ESP32支持多种WiFi加密协议（如WPA2、WPA3），可以确保连接的安全性。

4. **低功耗运行**：
   - STA模式下的ESP32可以配置为低功耗模式，这对于需要长时间运行的电池供电设备非常有用。

5. **事件驱动架构**：
   - WiFi连接过程中的事件（如连接成功、断开连接、获取IP等）会触发相应的回调函数，方便开发者处理网络状态的变化。

### **应用场景**

WiFi STA模式常用于需要将ESP32设备连接到局域网或互联网的应用场景。例如，智能家居设备通常使用STA模式连接到家庭WiFi网络，从而与其他设备或云服务器进行通信。

---

## 环境配置

### **1.CMake配置**

```c

REQUIRES esp_wifi nvs_flash

```

### **2. 需要包含的头文件**

确保在代码中包含以下头文件，这些文件提供了WiFi初始化、事件处理、网络功能等相关的API接口：

```c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_system.h"
#include "string.h"
```

---

## 初始化

在进行WiFi STA模式的开发之前，您需要初始化一些必要的资源和配置。

### **1. 初始化资源**

以下API接口用于初始化NVS、网络接口和事件循环：

```c
nvs_flash_init();  // 初始化NVS
esp_netif_init();  // 初始化网络接口
esp_event_loop_create_default();  // 创建事件循环
esp_netif_create_default_wifi_sta();
```

### **2. 初始化结构体**

您需要创建并初始化用于配置WiFi的结构体，例如`wifi_init_config_t`和`wifi_config_t`。

#### **WiFi初始化配置**

`wifi_init_config_t`是用于WiFi初始化的配置结构体，通常使用默认配置即可：

```c
wifi_init_config_t wifi_conf = WIFI_INIT_CONFIG_DEFAULT();
esp_wifi_init(&wifi_conf);  // 初始化WiFi驱动
```

#### **WiFi接口配置**

`wifi_config_t`结构体用于配置WiFi连接的参数，包括SSID和密码：

```c
wifi_config_t wifi_conf_interface = {
    .sta = {
        .ssid = "你的SSID",
        .password = "你的密码"
    }
};
esp_wifi_set_config(WIFI_IF_STA, &wifi_conf_interface);  // 设置WiFi接口配置
```

---

## WiFi事件处理

ESP32使用事件驱动架构来处理WiFi相关事件。通过注册事件处理程序，可以在不同的WiFi事件发生时执行相应的操作。

### **1. 定义事件处理回调函数**

事件处理回调函数`esp_event_handler_cb`用于处理WiFi连接和IP地址获取等事件：

```c
void esp_event_handler_cb(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();  // 尝试连接到指定WiFi
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *ip_data  = (ip_event_got_ip_t*) event_data;
        esp_ip4_addr_t ip = ip_data->ip_info.ip;
        printf("IPAddress : " IPSTR "\n", IP2STR(&ip));  // 打印获取的IP地址
    }
}
```

### **2. 注册事件处理程序**

使用`esp_event_handler_instance_register`函数注册WiFi和IP事件处理程序：

```c

esp_event_handler_instance_t event_handle;
esp_event_handler_instance_t IP_event_handle; //声明事件句柄

esp_event_handler_instance_register(WIFI_EVENT,
                                    ESP_EVENT_ANY_ID,
                                    esp_event_handler_cb,
                                    NULL,
                                    &event_handle);

esp_event_handler_instance_register(IP_EVENT,
                                    IP_EVENT_STA_GOT_IP,  // 监听WiFi连接的IP获取事件
                                    esp_event_handler_cb,
                                    NULL,
                                    &IP_event_handle);
```

---

## 启动WiFi

在配置完成之后，您需要启动WiFi连接，并等待获取IP地址。

```c
esp_wifi_set_mode(WIFI_MODE_STA);  // 设置WiFi工作模式为STA
esp_wifi_start();  // 启动WiFi
```

---


