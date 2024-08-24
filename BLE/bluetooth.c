#include <stdio.h>
#include <stdlib.h>
#include "init_System.h"
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "driver/gpio.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "sdkconfig.h"
#define GATTS_TAG "GATTS_DEMO" // 日志标签，用于在调试信息中标记GATT服务端的相关日志

/// 声明静态函数，用于处理GATT事件
static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

// 定义服务、特征和描述符的UUID
#define GATTS_SERVICE_UUID_TEST_A   0x00FF  // 服务UUID
#define GATTS_CHAR_UUID_TEST_A      0xFF01  // 特征UUID
#define GATTS_DESCR_UUID_TEST_A     0x3333  // 描述符UUID
#define GATTS_NUM_HANDLE_TEST_A     4       // 服务的句柄数量

#define TEST_DEVICE_NAME            "pxf-ESP32" // 设备名称
#define TEST_MANUFACTURER_DATA_LEN  17      // 厂商数据的长度

#define GATTS_DEMO_CHAR_VAL_LEN_MAX 0x40    // 特征值的最大长度

#define PREPARE_BUF_MAX_SIZE 1024 // 准备缓冲区的最大大小

static uint8_t char1_str[] = {0x11,0x22,0x33}; // 特征值
static esp_gatt_char_prop_t a_property = 0; // 特征属性

// 特征值的属性定义
static esp_attr_value_t gatts_demo_char1_val =
{
    .attr_max_len = GATTS_DEMO_CHAR_VAL_LEN_MAX, // 特征值的最大长度
    .attr_len     = sizeof(char1_str),           // 特征值的实际长度
    .attr_value   = char1_str,                   // 特征值数据
};

static uint8_t adv_config_done = 0; // 广播配置完成标志
#define adv_config_flag      (1 << 0) // 广播数据配置标志
#define scan_rsp_config_flag (1 << 1) // 扫描响应数据配置标志

// 广播服务的UUID
static uint8_t adv_service_uuid128[32] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    // 第一个UUID，16位，[12]，[13] 是值
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xEE, 0x00, 0x00, 0x00,
    // 第二个UUID，32位，[12]，[13]，[14]，[15] 是值
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};

// 广播数据定义
static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false, // 设置为假，表示这是广播数据而非扫描响应数据
    .include_name = true,  // 广播中包含设备名称
    .include_txpower = false, // 广播中不包含发射功率
    .min_interval = 0x0006, // 从设备连接的最小间隔，单位为 0.625ms
    .max_interval = 0x0010, // 从设备连接的最大间隔，单位为 0.625ms
    .appearance = 0x00,    // 设备外观
    .manufacturer_len = 0, // 厂商数据长度
    .p_manufacturer_data = NULL, // 厂商数据指针
    .service_data_len = 0, // 服务数据长度
    .p_service_data = NULL, // 服务数据指针
    .service_uuid_len = sizeof(adv_service_uuid128), // 服务UUID长度
    .p_service_uuid = adv_service_uuid128, // 服务UUID
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT), // 广播标志
};

// 扫描响应数据定义
static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,  // 设置为真，表示这是扫描响应数据
    .include_name = true,  // 扫描响应中包含设备名称
    .include_txpower = true, // 扫描响应中包含发射功率
    //.min_interval = 0x0006,
    //.max_interval = 0x0010,
    .appearance = 0x00,    // 设备外观
    .manufacturer_len = 0, // 厂商数据长度
    .p_manufacturer_data = NULL, // 厂商数据指针
    .service_data_len = 0, // 服务数据长度
    .p_service_data = NULL, // 服务数据指针
    .service_uuid_len = sizeof(adv_service_uuid128), // 服务UUID长度
    .p_service_uuid = adv_service_uuid128, // 服务UUID
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT), // 扫描响应数据标志
};

// 广播参数设置
static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20, // 广播最小间隔，单位为 0.625ms
    .adv_int_max        = 0x40, // 广播最大间隔，单位为 0.625ms
    .adv_type           = ADV_TYPE_IND, // 广播类型，指示广播数据类型
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC, // 自有地址类型
    //.peer_addr            =
    //.peer_addr_type       =
    .channel_map        = ADV_CHNL_ALL, // 广播频道映射，所有频道
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY, // 广播过滤策略，允许任何扫描和连接
};

#define PROFILE_NUM 2 // GATT配置文件数量
#define PROFILE_A_APP_ID 0 // 配置文件A的应用ID
#define PROFILE_B_APP_ID 1 // 配置文件B的应用ID

// GATT配置文件实例
struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb; // GATT事件处理回调函数
    uint16_t gatts_if; // GATT接口
    uint16_t app_id; // 应用ID
    uint16_t conn_id; // 连接ID
    uint16_t service_handle; // 服务句柄
    esp_gatt_srvc_id_t service_id; // 服务ID
    uint16_t char_handle; // 特征句柄
    esp_bt_uuid_t char_uuid; // 特征UUID
    esp_gatt_perm_t perm; // 特征权限
    esp_gatt_char_prop_t property; // 特征属性
    uint16_t descr_handle; // 描述符句柄
    esp_bt_uuid_t descr_uuid; // 描述符UUID
};

// 存储GATT接口的配置文件数组
static struct gatts_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gatts_cb = gatts_profile_a_event_handler, // GATT事件处理函数
        .gatts_if = ESP_GATT_IF_NONE, // 初始时未获取GATT接口，设置为ESP_GATT_IF_NONE
    }
};

// 准备缓冲区的环境结构
typedef struct {
    uint8_t                 *prepare_buf; // 准备缓冲区指针
    int                     prepare_len; // 准备缓冲区长度
} prepare_type_env_t;
void example_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
prepare_type_env_t prepare_env;
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {

    // 广告数据设置完成事件
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~adv_config_flag);  // 更新广告配置标志
        if (adv_config_done == 0) {
            // 如果所有广告配置完成，开始广播
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;

    // 扫描响应数据设置完成事件
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~scan_rsp_config_flag);  // 更新扫描响应配置标志
        if (adv_config_done == 0) {
            // 如果所有扫描响应配置完成，开始广播
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;

    // 广播启动完成事件
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        // 广播启动完成事件，指示广播启动成功或失败
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTS_TAG, "Advertising start failed");
        }
        break;

    // 广播停止完成事件
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            // 广播停止失败
            ESP_LOGE(GATTS_TAG, "Advertising stop failed");
        } else {
            // 广播停止成功
            ESP_LOGI(GATTS_TAG, "Stop adv successfully");
        }
        break;

    // 更新连接参数事件
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
        ESP_LOGI(GATTS_TAG, "update connection params status = %d, min_int = %d, max_int = %d, conn_int = %d, latency = %d, timeout = %d",
                  param->update_conn_params.status,      // 更新状态
                  param->update_conn_params.min_int,     // 最小连接间隔
                  param->update_conn_params.max_int,     // 最大连接间隔
                  param->update_conn_params.conn_int,    // 当前连接间隔
                  param->update_conn_params.latency,     // 延迟
                  param->update_conn_params.timeout);    // 超时
        break;

    default:
        // 处理所有未处理的事件
        break;
    }
}




static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    switch (event) {
    // 应用注册事件
    case ESP_GATTS_REG_EVT:
        ESP_LOGI(GATTS_TAG, "REGISTER_APP_EVT, status %d, app_id %d", param->reg.status, param->reg.app_id);
        // 设置服务的 UUID 和 ID
        gl_profile_tab[PROFILE_A_APP_ID].service_id.is_primary = true;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.inst_id = 0x00;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_A_APP_ID].service_id.id.uuid.uuid.uuid16 = GATTS_SERVICE_UUID_TEST_A;

        // 设置设备名称
        esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(TEST_DEVICE_NAME);
        if (set_dev_name_ret){
            ESP_LOGE(GATTS_TAG, "set device name failed, error code = %x", set_dev_name_ret);
        }

        // 配置广告数据
        esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
        if (ret){
            ESP_LOGE(GATTS_TAG, "config adv data failed, error code = %x", ret);
        }
        adv_config_done |= adv_config_flag;
        
        // 配置扫描响应数据
        ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
        if (ret){
            ESP_LOGE(GATTS_TAG, "config scan response data failed, error code = %x", ret);
        }
        adv_config_done |= scan_rsp_config_flag;

        // 创建服务
        esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[PROFILE_A_APP_ID].service_id, GATTS_NUM_HANDLE_TEST_A);
        break;
        
    // 处理读请求事件
    case ESP_GATTS_READ_EVT: {
        ESP_LOGI(GATTS_TAG, "GATT_READ_EVT, conn_id %d, trans_id %" PRIu32 ", handle %d", param->read.conn_id, param->read.trans_id, param->read.handle);
        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = 4;
        rsp.attr_value.value[0] = 0xde;
        rsp.attr_value.value[1] = 0xed;
        rsp.attr_value.value[2] = 0xbe;
        rsp.attr_value.value[3] = 0xef;
        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
        break;
    }
    
    // 处理写请求事件
case ESP_GATTS_WRITE_EVT: {
        ESP_LOGI(GATTS_TAG, "GATT_WRITE_EVT, conn_id %d, trans_id %" PRIu32 ", handle %d", param->write.conn_id, param->write.trans_id, param->write.handle);
        if (!param->write.is_prep){
            ESP_LOGI(GATTS_TAG, "GATT_WRITE_EVT, value len %d, value :", param->write.len);
            esp_log_buffer_hex(GATTS_TAG, param->write.value, param->write.len);
            //uart_write_bytes(UART_PORT, param->write.value,param->write.len);
			//uart_flush(UART_PORT);
            if (gl_profile_tab[PROFILE_A_APP_ID].descr_handle == param->write.handle && param->write.len == 2){
                uint16_t descr_value = param->write.value[1]<<8 | param->write.value[0];
                if (descr_value == 0x0001){
                    if (a_property & ESP_GATT_CHAR_PROP_BIT_NOTIFY){
                        ESP_LOGI(GATTS_TAG, "notify enable");
                        uint8_t notify_data[15];
                        for (int i = 0; i < sizeof(notify_data); ++i)
                        {
                            notify_data[i] = i%0xff;
                        }
                        //the size of notify_data[] need less than MTU size
                        esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, gl_profile_tab[PROFILE_A_APP_ID].char_handle,
                                                sizeof(notify_data), notify_data, false);
                    }
                }else if (descr_value == 0x0002){
                    if (a_property & ESP_GATT_CHAR_PROP_BIT_INDICATE){
                        ESP_LOGI(GATTS_TAG, "indicate enable");
                        uint8_t indicate_data[15];
                        for (int i = 0; i < sizeof(indicate_data); ++i)
                        {
                            indicate_data[i] = i%0xff;
                        }
                        //the size of indicate_data[] need less than MTU size
                        esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, gl_profile_tab[PROFILE_A_APP_ID].char_handle,
                                                sizeof(indicate_data), indicate_data, true);
                    }
                }
                else if (descr_value == 0x0000){
                    ESP_LOGI(GATTS_TAG, "notify/indicate disable ");
                }else{
                    ESP_LOGE(GATTS_TAG, "unknown descr value");
                    esp_log_buffer_hex(GATTS_TAG, param->write.value, param->write.len);
                }

            }
        }
        example_write_event_env(gatts_if, &prepare_env, param);    
        break;
    }
    
    // 处理执行写事件
    case ESP_GATTS_EXEC_WRITE_EVT:
        ESP_LOGI(GATTS_TAG,"ESP_GATTS_EXEC_WRITE_EVT");
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
        break;

    // 处理 MTU 更新事件
    case ESP_GATTS_MTU_EVT:
        ESP_LOGI(GATTS_TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        break;

    // 处理注销事件
    case ESP_GATTS_UNREG_EVT:
        break;

    // 处理创建服务事件
    case ESP_GATTS_CREATE_EVT:
        ESP_LOGI(GATTS_TAG, "CREATE_SERVICE_EVT, status %d,  service_handle %d", param->create.status, param->create.service_handle);
        gl_profile_tab[PROFILE_A_APP_ID].service_handle = param->create.service_handle;
        gl_profile_tab[PROFILE_A_APP_ID].char_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_A_APP_ID].char_uuid.uuid.uuid16 = GATTS_CHAR_UUID_TEST_A;

        esp_ble_gatts_start_service(gl_profile_tab[PROFILE_A_APP_ID].service_handle);
        a_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
        esp_err_t add_char_ret = esp_ble_gatts_add_char(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].char_uuid,
                                                        ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                                        a_property,
                                                        &gatts_demo_char1_val, NULL);
        if (add_char_ret){
            ESP_LOGE(GATTS_TAG, "add char failed, error code =%x", add_char_ret);
        }
        break;

    // 处理包含服务事件
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        break;

    // 处理添加特征事件
    case ESP_GATTS_ADD_CHAR_EVT: {
        uint16_t length = 0;
        const uint8_t *prf_char;

        ESP_LOGI(GATTS_TAG, "ADD_CHAR_EVT, status %d,  attr_handle %d, service_handle %d",
                param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);
        gl_profile_tab[PROFILE_A_APP_ID].char_handle = param->add_char.attr_handle;
        gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_A_APP_ID].descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
        esp_err_t get_attr_ret = esp_ble_gatts_get_attr_value(param->add_char.attr_handle,  &length, &prf_char);
        if (get_attr_ret == ESP_FAIL){
            ESP_LOGE(GATTS_TAG, "ILLEGAL HANDLE");
        }

        ESP_LOGI(GATTS_TAG, "the gatts demo char length = %x", length);
        for(int i = 0; i < length; i++){
            ESP_LOGI(GATTS_TAG, "prf_char[%x] =%x", i, prf_char[i]);
        }
        esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(gl_profile_tab[PROFILE_A_APP_ID].service_handle, &gl_profile_tab[PROFILE_A_APP_ID].descr_uuid,
                                                                ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, NULL, NULL);
        if (add_descr_ret){
            ESP_LOGE(GATTS_TAG, "add char descr failed, error code =%x", add_descr_ret);
        }
        break;
    }
    
    // 处理添加特征描述符事件
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        gl_profile_tab[PROFILE_A_APP_ID].descr_handle = param->add_char_descr.attr_handle;
        ESP_LOGI(GATTS_TAG, "ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d",
                 param->add_char_descr.status, param->add_char_descr.attr_handle, param->add_char_descr.service_handle);
        break;

    // 处理删除服务事件
    case ESP_GATTS_DELETE_EVT:
        break;

    // 处理启动服务事件
    case ESP_GATTS_START_EVT:
        ESP_LOGI(GATTS_TAG, "SERVICE_START_EVT, status %d, service_handle %d",
                 param->start.status, param->start.service_handle);
        break;

    // 处理停止服务事件
    case ESP_GATTS_STOP_EVT:
        break;

    // 处理设备连接事件
    case ESP_GATTS_CONNECT_EVT: {
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        /* 对于 iOS 系统，请参考 Apple 官方文档了解 BLE 连接参数的限制。*/
        conn_params.latency = 0;
        conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
        conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
        conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
        ESP_LOGI(GATTS_TAG, "ESP_GATTS_CONNECT_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x:",
                 param->connect.conn_id,
                 param->connect.remote_bda[0], param->connect.remote_bda[1], param->connect.remote_bda[2],
                 param->connect.remote_bda[3], param->connect.remote_bda[4], param->connect.remote_bda[5]);
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = param->connect.conn_id;
        // 开始向对端设备发送更新连接参数请求
        esp_ble_gap_update_conn_params(&conn_params);
        break;
    }
    
    // 处理设备断开事件
    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(GATTS_TAG, "ESP_GATTS_DISCONNECT_EVT, disconnect reason 0x%x", param->disconnect.reason);
        esp_ble_gap_start_advertising(&adv_params);
        break;
    
    // 处理确认事件
    case ESP_GATTS_CONF_EVT:
        ESP_LOGI(GATTS_TAG, "ESP_GATTS_CONF_EVT, status %d attr_handle %d", param->conf.status, param->conf.handle);
        if (param->conf.status != ESP_GATT_OK){
            esp_log_buffer_hex(GATTS_TAG, param->conf.value, param->conf.len);
        }
        break;
        
    // 其他事件
    case ESP_GATTS_OPEN_EVT:
    case ESP_GATTS_CANCEL_OPEN_EVT:
    case ESP_GATTS_CLOSE_EVT:
    case ESP_GATTS_LISTEN_EVT:
    case ESP_GATTS_CONGEST_EVT:
    default:
        break;
    }
}



static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    /* 如果事件是注册事件，则为每个配置文件存储 gatts_if */
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            // 存储与应用程序 ID 相关的 gatts_if
            gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;
        } else {
            // 注册失败时输出日志
            ESP_LOGI(GATTS_TAG, "注册应用程序失败, app_id %04x, 状态 %d",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }

    /* 如果 gatts_if 等于配置文件 A，则调用配置文件 A 的回调处理函数，
     * 这里调用每个配置文件的回调函数 */
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE 表示不指定特定的 gatt_if，需要调用每个配置文件的回调函数 */
                    gatts_if == gl_profile_tab[idx].gatts_if) {
                if (gl_profile_tab[idx].gatts_cb) {
                    // 调用相应配置文件的回调函数处理事件
                    gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}


void init_bluetooth()
{
    esp_err_t ret;
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
     esp_bt_controller_init(&bt_cfg);

     esp_bt_controller_enable(ESP_BT_MODE_BLE);

     esp_bluedroid_init();
     esp_bluedroid_enable();

     esp_ble_gatts_register_callback(gatts_event_handler);
     esp_ble_gap_register_callback(gap_event_handler);
     esp_ble_gatts_app_register(PROFILE_A_APP_ID);
     esp_ble_gatts_app_register(PROFILE_B_APP_ID);
    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret){
        ESP_LOGE(GATTS_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }

    return;
}

void example_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param) {
    esp_gatt_status_t status = ESP_GATT_OK;
    if (param->write.need_rsp) {
        if (param->write.is_prep) {
            if (prepare_write_env->prepare_buf == NULL) {
                prepare_write_env->prepare_buf = (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
                prepare_write_env->prepare_len = 0;
                if (prepare_write_env->prepare_buf == NULL) {
                    ESP_LOGE(GATTS_TAG, "No memory for prepare buffer");
                    status = ESP_GATT_NO_RESOURCES;
                }
            } else {
                if (param->write.offset > PREPARE_BUF_MAX_SIZE) {
                    status = ESP_GATT_INVALID_OFFSET;
                } else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE) {
                    status = ESP_GATT_INVALID_ATTR_LEN;
                }
            }

            esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
            if (gatt_rsp != NULL) {
                gatt_rsp->attr_value.len = param->write.len;
                gatt_rsp->attr_value.handle = param->write.handle;
                gatt_rsp->attr_value.offset = param->write.offset;
                gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
                memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
                esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, gatt_rsp);
                free(gatt_rsp);
            } else {
                ESP_LOGE(GATTS_TAG, "malloc failed for gatt_rsp");
            }

            if (status == ESP_GATT_OK) {
                memcpy(prepare_write_env->prepare_buf + param->write.offset, param->write.value, param->write.len);
                prepare_write_env->prepare_len += param->write.len;
            }
        } else {
            esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, NULL);
        }
    }
}

void app_main()
{
    init_bluetooth();
    while (1)
    {
        DELAY_MS(1000);
    }
    
}