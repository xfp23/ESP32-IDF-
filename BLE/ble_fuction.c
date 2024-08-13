/*************列出蓝牙API的函数功能，之列出函数原型 */

/**REQUIRES bt 配置cmake */
#include "ble_fuction.h"

#include "esp_gap_ble_api.h"
#include "esp_gatt_defs.h"
//调用此函数以发生间隙事件，例如扫描结果。
esp_err_t esp_ble_gap_register_callback(esp_gap_ble_cb_t callback);

//调用此函数以获取当前差距回调。
/*返回
esp_gap_ble_cb_t : callback function
esp_gap_ble_cb_t 回调函数*/
esp_gap_ble_cb_t esp_ble_gap_get_callback(void);


/**
 * @brief 调用此函数以覆盖 BTA 默认 ADV 参数。
 * @param adv_data -- [in] 指向用户定义的 ADV 数据结构的指针。在收到 config_adv_data 的回调之前，无法释放此内存空间。
 * @return esp
 */
esp_err_t esp_ble_gap_config_adv_data(esp_ble_adv_data_t *adv_data);

/**
 * @brief 调用此函数以设置扫描参数。
 * @param 指向用户定义的scan_params数据结构的指针。在回调 set_scan_params 之前，无法释放此内存空间
 */
esp_err_t esp_ble_gap_set_scan_params(esp_ble_scan_params_t *scan_params);

/**
 * @brief 开始扫描
 * @param 保持扫描时间单位为秒
 */
esp_err_t esp_ble_gap_start_scanning(uint32_t duration);


/**
 * @brief 停止扫描
 * @param void
 */
esp_err_t esp_ble_gap_stop_scanning(void);

/**
 * @brief 开始广播
 * @param adv_params -- [in] 指向用户定义的adv_params数据结构的指针。
 */
esp_err_t esp_ble_gap_start_advertising(esp_ble_adv_params_t *adv_params);

/**
 * @brief 停止广播
 * @param 
*/
esp_err_t esp_ble_gap_stop_advertising(void);

/**
 * @brief 更新连接参数，只能在连接通时使用。
 * @param  连接更新参数
 */
esp_err_t esp_ble_gap_update_conn_params(esp_ble_conn_update_params_t *params)

/**
 * @brief 此功能用于设置最大 LE 数据包大小。
 * @param 设备地址
 * @param 数据长度
 */
esp_err_t esp_ble_gap_set_pkt_data_len(esp_bd_addr_t remote_device, uint16_t tx_data_length);

/**
 * @brief 此功能允许配置不可解析的私有地址或静态随机地址。
 * @param 要配置的地址。请参阅下表，了解可能的地址子类型：
 *           | address [47:46] | Address Type             |
          |-----------------|--------------------------|
          |      0b00       | Non-Resolvable Private   |
          |                 | Address                  |
          |-----------------|--------------------------|
          |      0b11       | Static Random Address    |
          |-----------------|--------------------------|
 */
esp_err_t esp_ble_gap_set_rand_addr(esp_bd_addr_t rand_addr);

/**
 * @brief 此函数清除应用程序的随机地址
 */
esp_err_t esp_ble_gap_clear_rand_addr(void);


/**
 * @brief  BLE设置频道。
 * @param 第 n 个此类字段（在 0 到 36 范围内）包含链路层通道索引 n 的值。0 表示通道 n 是坏的。1 表示通道 n 未知。最高有效位是保留的，
 * 应设置为 0。至少应将一个频道标记为未知。
 */
esp_err_t esp_gap_ble_set_channels(esp_gap_ble_channels channels);

/**
 * @brief 此功能用于启用扫描。
 * @param  扫描持续时间，其中 Time = N * 10 ms。范围：0x0001 至 0xFFFF。
 * @param 从控制器开始其最后一次扫描持续时间到开始后续扫描持续时间的时间间隔。时间 = N * 1.28 秒。范围：0x0001 至 0xFFFF。
 */
esp_err_t esp_ble_gap_start_ext_scan(uint32_t duration, uint16_t period);

/**
 * @brief 禁用扫描
 */
esp_err_t esp_ble_gap_stop_ext_scan(void);

/**
 * @brief 此功能用于与广告商的定期广告同步，并开始接收定期广告数据包。
 * @param 同步参数
 */
esp_err_t esp_ble_gap_periodic_adv_create_sync(const esp_ble_gap_periodic_adv_sync_params_t *params)

/************************************GATT******************************************************** */

/**
 * @brief 调用此函数以向 BTA GATTS 模块注册应用程序回调。
 */
esp_err_t esp_ble_gatts_register_callback(esp_gatts_cb_t callback);

/**
 * @brief 调用此函数以获取带有 BTA GATTS 模块的当前应用程序回调。
 */
esp_gatts_cb_t esp_ble_gatts_get_callback(void);

/**
 * @brief /**
 * @brief 调用此函数以获取带有 BTA GATTS 模块的当前应用程序回调。
 */
esp_err_t esp_ble_gatts_app_register(uint16_t app_id)

/**
 * @brief 在GATT服务器上注销
 * @param GATT服务器访问接口
 */
esp_err_t esp_ble_gatts_app_unregister(esp_gatt_if_t gatts_if);

/**
 * @brief 创建服务。创建服务后，将调用回调事件ESP_GATTS_CREATE_EVT，
 * 以向配置文件报告状态和服务 ID。在将包含的服务以及特征/描述符添加到服务中时，需要使用在回调函数中获取的服务 ID。
 * @param  GATT服务器访问接口
 * @param 服务id
 * @param 此服务请求的句柄数
 */
esp_err_t esp_ble_gatts_create_service(esp_gatt_if_t gatts_if, esp_gatt_srvc_id_t *service_id, uint16_t num_handle)

/**
 * @brief 创建服务属性选项卡。
 * @param  指向服务属性选项卡的指针中
 * @param  GATT 服务器访问接口
 * @param 要添加到服务数据库的属性数量。
 * @param  服务的实例 ID
 */
esp_err_t esp_ble_gatts_create_attr_tab(const esp_gatts_attr_db_t *gatts_attr_db, esp_gatt_if_t gatts_if, uint16_t max_nb_attr, uint8_t srvc_inst_id);

/**
 * @brief 调用此函数以将特征添加到服务中。
 * @param  要向其添加此包含服务的 [in] 服务句柄。
 * @param 特征 UUID。
 * @param  特征值声明属性权限。
 * @param  特性属性
 * @param  特性值
 * @param 属性响应控制字节
 */
esp_err_t esp_ble_gatts_add_char(uint16_t service_handle, esp_bt_uuid_t *char_uuid, esp_gatt_perm_t perm, esp_gatt_char_prop_t property, esp_attr_value_t *char_val, esp_attr_control_t *control);


/**
 * @brief 调用此函数以添加特征描述符。完成后，将调用回调事件ESP_GATTS_ADD_DESCR_EVT以报告此描述符的状态和 ID 号
 * @param 要添加此特征描述符的 [in] 服务句柄。
 * @param  描述符访问权限。
 * @param  特征描述符值
 * @param 属性相应控制字节
 */
esp_err_t esp_ble_gatts_add_char_descr(uint16_t service_handle, esp_bt_uuid_t *descr_uuid, esp_gatt_perm_t perm, esp_attr_value_t *char_descr_val, esp_attr_control_t *control);

/**
 * @brief  将设备名称设置为本地设备 注意：此 API 不会影响广告数据。
 */
esp_err_t esp_ble_gap_set_device_name(const char *name)