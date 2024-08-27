# 版权

© 2024 . 未经许可不得复制、修改或分发。 此文献为 [小風的藏書閣](https://t.me/xfp2333)  所有。

# 前言

- [蓝牙说明文档](bluetooth_low.md)

## ESP32 GATT 服务端开发教程

### 1. 环境准备

1. cmake配置

```c
REQUIRES bt nvs_flash
```

2. 蓝牙组件只配置cmake系统并不会去编译

- VScode用户:

1. 点击底部的SDK配置编辑器
2. 在界面搜索栏搜索`blue`关键字
3. 点击选中`bluetooth`组件

- Eclipse用户:

1. 很遗憾，本人没找到把组件添加进去的方法,但是你仍然可以按照第二步提供的方法继续
2. 在项目创建工程的时候就提前选中官方示例模板创建，建议按照`gatt_client`示例创建

### 2. 创建 GATT 服务端

GATT（Generic Attribute Profile）是蓝牙低功耗（BLE）的一种协议，用于定义如何在设备之间交换数据。以下是创建 GATT 服务端的基本步骤。

#### 2.1 引入必要的头文件

在您的主代码文件中，引入 ESP-IDF 和 BLE 相关的头文件：

```c
#include <stdio.h>                   // 标准输入输出函数
#include <stdlib.h>                  // 标准库函数
#include <string.h>                  // 字符串处理函数
#include "freertos/FreeRTOS.h"       // FreeRTOS 支持
#include "freertos/task.h"           // FreeRTOS 任务管理
#include "esp_system.h"              // ESP32 系统相关函数
#include "esp_log.h"                 // 日志记录功能
#include "nvs_flash.h"               // 非易失性存储功能
#include "driver/gpio.h"             // GPIO 驱动
#include "esp_gap_ble_api.h"         // BLE GAP API
#include "esp_gatts_api.h"           // GATT API
#include "esp_bt_defs.h"             // 蓝牙定义
#include "esp_bt_main.h"             // 蓝牙主程序
#include "esp_gatt_common_api.h"     // GATT 公共 API
```

#### 2.2 定义 UUID 和属性

定义服务、特征和描述符的 UUID 以及特征值的最大长度等：

```c
#define GATTS_SERVICE_UUID_TEST_A   0x00FF       // 服务 UUID
#define GATTS_CHAR_UUID_TEST_A      0xFF01       // 特征 UUID
define GATTS_DESCR_UUID_TEST_A     0x3333       // 描述符 UUID
#define GATTS_NUM_HANDLE_TEST_A     4            // 句柄数量
#define GATTS_DEMO_CHAR_VAL_LEN_MAX 0x40         // 特征值最大长度
#define TEST_DEVICE_NAME            "pxf-ESP32"  // 设备名称
#define PREPARE_BUF_MAX_SIZE 1024                // 准备缓冲区最大大小

// 定义特征值数据
static uint8_t char1_str[] = {0x11, 0x22, 0x33};
static esp_attr_value_t gatts_demo_char1_val = {
    .attr_max_len = GATTS_DEMO_CHAR_VAL_LEN_MAX, // 特征值最大长度
    .attr_len = sizeof(char1_str),               // 特征值实际长度
    .attr_value = char1_str,                     // 特征值数据
};
```

#### 2.3 配置广播数据

配置 BLE 广播数据和扫描响应数据：

```c
// 广播的服务 UUID
static uint8_t adv_service_uuid128[32] = {
    // 示例 UUID
};

// 广播数据
static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,                      // 设置扫描响应数据标志
    .include_name = true,                      // 包括设备名称
    .include_txpower = false,                  // 不包括发射功率
    .min_interval = 0x0006,                    // 最小间隔
    .max_interval = 0x0010,                    // 最大间隔
    .appearance = 0x00,                        // 外观
    .service_uuid_len = sizeof(adv_service_uuid128), // 服务 UUID 长度
    .p_service_uuid = adv_service_uuid128,     // 服务 UUID
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT), // 广播标志
};

// 扫描响应数据
static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,                      // 设置扫描响应数据标志
    .include_name = true,                      // 包括设备名称
    .include_txpower = true,                   // 包括发射功率
    .appearance = 0x00,                        // 外观
    .service_uuid_len = sizeof(adv_service_uuid128), // 服务 UUID 长度
    .p_service_uuid = adv_service_uuid128,     // 服务 UUID
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT), // 广播标志
};
```

#### 2.4 配置广播参数

设置广播参数：

```c
static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,                      // 最小广播间隔
    .adv_int_max = 0x40,                      // 最大广播间隔
    .adv_type = ADV_TYPE_IND,                 // 广播类型
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,   // 设备地址类型
    .channel_map = ADV_CHNL_ALL,              // 广播频道
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY, // 广播过滤策略
};
```

#### 2.5 创建 GATT 服务端配置文件

定义 GATT 服务端配置文件结构体，并创建一个配置文件实例：

```c
#define PROFILE_NUM 1                        // 配置文件数量
#define PROFILE_A_APP_ID 0                   // 配置文件应用 ID

// GATT 配置文件结构体
struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;                 // GATT 回调函数
    uint16_t gatts_if;                       // GATT 句柄
    uint16_t app_id;                         // 应用 ID
    uint16_t conn_id;                        // 连接 ID
    uint16_t service_handle;                 // 服务句柄
    esp_gatt_srvc_id_t service_id;           // 服务 ID
    uint16_t char_handle;                    // 特征句柄
    esp_bt_uuid_t char_uuid;                 // 特征 UUID
    esp_gatt_perm_t perm;                   // 权限
    esp_gatt_char_prop_t property;           // 特征属性
    uint16_t descr_handle;                   // 描述符句柄
    esp_bt_uuid_t descr_uuid;                // 描述符 UUID
};

// 配置文件实例
static struct gatts_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gatts_cb = gatts_profile_a_event_handler, // 设置回调函数
        .gatts_if = ESP_GATT_IF_NONE,              // 未分配的 GATT 句柄
    }
};
```

#### 2.6 处理 GATT 事件

实现 GATT 事件处理函数：

```c
static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    switch (event) {
    case ESP_GATTS_REG_EVT:                    // 注册事件
        ESP_LOGI(GATTS_TAG, "REGISTER_APP_EVT, status %d, app_id %d", param->reg.status, param->reg.app_id);
        // 配置服务、特征、描述符等
        break;

    case ESP_GATTS_READ_EVT:                   // 读取事件
        // 处理读取请求
        break;

    case ESP_GATTS_WRITE_EVT:                  // 写入事件
        // 处理写入请求
        break;

    case ESP_GATTS_CREATE_EVT:                 // 创建事件
        // 服务创建事件
        break;

    case ESP_GATTS_START_EVT:                  // 启动事件
        // 服务启动事件
        break;

    default:
        break;
    }
}
```

#### 2.7 初始化和启动

在 `app_main()` 中初始化 BLE：

```c
void app_main() {
    // 初始化 NVS（非易失性存储）
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());   // 处理 NVS 错误
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 初始化 Bluetooth
    esp_bt_controller_init();               // 初始化 Bluetooth 控制器
    esp_bt_controller_enable(ESP_BT_MODE_BTDM); // 启用 Bluetooth 模式
    esp_bluedroid_init();                  // 初始化 Bluedroid
    esp_bluedroid_enable();                // 启用 Bluedroid
    esp_ble_gatts_register_callback(gatts_profile_a_event_handler); // 注册 GATT 回调
    esp_ble_gap_register_callback(gap_event_handler);               // 注册 GAP 回调
    esp_ble_gatts_app_register(PROFILE_A_APP_ID);                   // 注册 GATT 应用
}
```

- [蓝牙代码示例](bluetooth.c)