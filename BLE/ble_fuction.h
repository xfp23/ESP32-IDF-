typedef unsigned short int uint16_t;
typedef unsigned char uint8_t;
/***************蓝牙API类型，注释出其功能 */

typedef enum {
    // BLE_4.2 特性支持
    ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT = 0,        /*!< 广告数据设置完成事件 */
    ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT,       /*!< 扫描响应数据设置完成事件 */
    ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT,          /*!< 扫描参数设置完成事件 */
    ESP_GAP_BLE_SCAN_RESULT_EVT,                      /*!< 扫描结果事件，每次有一个扫描结果准备好时触发 */
    ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT,        /*!< 原始广告数据设置完成事件 */
    ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT,   /*!< 原始扫描响应数据设置完成事件 */
    ESP_GAP_BLE_ADV_START_COMPLETE_EVT,               /*!< 开始广告完成事件 */
    ESP_GAP_BLE_SCAN_START_COMPLETE_EVT,              /*!< 开始扫描完成事件 */
    // BLE_包含
    ESP_GAP_BLE_AUTH_CMPL_EVT = 8,                    /*!< 认证完成事件 */
    ESP_GAP_BLE_KEY_EVT,                              /*!< 对等设备密钥事件 */
    ESP_GAP_BLE_SEC_REQ_EVT,                          /*!< 安全请求事件 */
    ESP_GAP_BLE_PASSKEY_NOTIF_EVT,                    /*!< 密码通知事件 */
    ESP_GAP_BLE_PASSKEY_REQ_EVT,                      /*!< 密码请求事件 */
    ESP_GAP_BLE_OOB_REQ_EVT,                          /*!< OOB（Out-of-Band）请求事件 */
    ESP_GAP_BLE_LOCAL_IR_EVT,                         /*!< 本地IR（Identity Root 128位随机静态值用于生成长期密钥）事件 */
    ESP_GAP_BLE_LOCAL_ER_EVT,                         /*!< 本地ER（加密根值用于生成身份解析密钥）事件 */
    ESP_GAP_BLE_NC_REQ_EVT,                           /*!< 数字比较请求事件 */
    // BLE_4.2 特性支持
    ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT,                /*!< 停止广告完成事件 */
    ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT,               /*!< 停止扫描完成事件 */
    // BLE_包含
    ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT = 19,        /*!< 设置静态随机地址完成事件 */
    ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT,               /*!< 更新连接参数完成事件 */
    ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT,          /*!< 设置数据包长度完成事件 */
    ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT,       /*!< 启用/禁用本地设备隐私完成事件 */
    ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT,         /*!< 移除配对设备完成事件 */
    ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT,          /*!< 清除配对设备完成事件 */
    ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT,            /*!< 获取配对设备列表完成事件 */
    ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT,               /*!< 读取RSSI完成事件 */
    ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT,        /*!< 更新白名单完成事件 */
    // BLE_4.2 特性支持
    ESP_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT, /*!< 更新重复例外列表完成事件 */
    // BLE_包含
    ESP_GAP_BLE_SET_CHANNELS_EVT = 29,                /*!< 设置BLE通道完成事件 */
    // BLE_5.0 特性支持
    ESP_GAP_BLE_READ_PHY_COMPLETE_EVT,                /*!< 读取PHY完成事件 */
    ESP_GAP_BLE_SET_PREFERRED_DEFAULT_PHY_COMPLETE_EVT, /*!< 设置默认PHY首选完成事件 */
    ESP_GAP_BLE_SET_PREFERRED_PHY_COMPLETE_EVT,       /*!< 设置PHY首选完成事件 */
    ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT,   /*!< 扩展设置随机地址完成事件 */
    ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT,      /*!< 扩展广告参数设置完成事件 */
    ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT,        /*!< 扩展广告数据设置完成事件 */
    ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT,   /*!< 扩展扫描响应数据设置完成事件 */
    ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT,           /*!< 扩展广告开始完成事件 */
    ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT,            /*!< 扩展广告停止完成事件 */
    ESP_GAP_BLE_EXT_ADV_SET_REMOVE_COMPLETE_EVT,      /*!< 扩展广告设置移除完成事件 */
    ESP_GAP_BLE_EXT_ADV_SET_CLEAR_COMPLETE_EVT,       /*!< 扩展广告设置清除完成事件 */
    ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT, /*!< 周期性广告参数设置完成事件 */
    ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT,   /*!< 周期性广告数据设置完成事件 */
    ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT,      /*!< 周期性广告开始完成事件 */
    ESP_GAP_BLE_PERIODIC_ADV_STOP_COMPLETE_EVT,       /*!< 周期性广告停止完成事件 */
    ESP_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT,/*!< 周期性广告同步创建完成事件 */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT,/*!< 扩展广告同步取消完成事件 */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT, /*!< 扩展广告同步终止完成事件 */
    ESP_GAP_BLE_PERIODIC_ADV_ADD_DEV_COMPLETE_EVT,    /*!< 周期性广告添加设备完成事件 */
    ESP_GAP_BLE_PERIODIC_ADV_REMOVE_DEV_COMPLETE_EVT, /*!< 周期性广告移除设备完成事件 */
    ESP_GAP_BLE_PERIODIC_ADV_CLEAR_DEV_COMPLETE_EVT,  /*!< 周期性广告清除设备完成事件 */
    ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT,     /*!< 扩展扫描参数设置完成事件 */
    ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT,          /*!< 扩展扫描开始完成事件 */
    ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT,           /*!< 扩展扫描停止完成事件 */
    ESP_GAP_BLE_PREFER_EXT_CONN_PARAMS_SET_COMPLETE_EVT, /*!< 扩展连接参数设置完成事件 */
    ESP_GAP_BLE_PHY_UPDATE_COMPLETE_EVT,              /*!< BLE PHY 更新完成事件 */
    ESP_GAP_BLE_EXT_ADV_REPORT_EVT,                   /*!< 扩展广告报告完成事件 */
    ESP_GAP_BLE_SCAN_TIMEOUT_EVT,                     /*!< 扫描超时完成事件 */
    ESP_GAP_BLE_ADV_TERMINATED_EVT,                   /*!< 广告终止数据完成事件 */
    ESP_GAP_BLE_SCAN_REQ_RECEIVED_EVT,                /*!< 接收到扫描请求完成事件 */
    ESP_GAP_BLE_CHANNEL_SELECT_ALGORITHM_EVT,         /*!< 通道选择算法完成事件 */
    ESP_GAP_BLE_PERIODIC_ADV_REPORT_EVT,              /*!< 周期性广告报告完成事件 */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT,           /*!< 周期性广告同步丢失完成事件 */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT,          /*!< 周期性广告同步建立完成事件 */
    // BLE_包含
    ESP_GAP_BLE_SC_OOB_REQ_EVT,                       /*!< 安全连接OOB请求事件 */
    ESP_GAP_BLE_SC_CR_LOC_OOB_EVT,                    /*!< 安全连接创建OOB数据完成事件 */
    ESP_GAP_BLE_GET_DEV_NAME_COMPLETE_EVT,            /*!< 获取BT设备名称完成事件 */
    // BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER
    ESP_GAP_BLE_PERIODIC_ADV_RECV_ENABLE_COMPLETE_EVT,/*!< 周期性广告接收使能完成事件 */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_TRANS_COMPLETE_EVT, /*!< 周期性广告同步传输完成事件 */
    ESP_GAP_BLE_PERIODIC_ADV_SET_INFO_TRANS_COMPLETE_EVT, /*!< 周期性广告设置信息传输完成事件 */
    ESP_GAP_BLE_SET_PAST_PARAMS_COMPLETE_EVT,         /*!< 设置周期性广告同步传输参数完成事件 */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_TRANS_RECV_EVT,     /*!< 周期性广告同步传输接收事件 */
    // DTM
    ESP_GAP_BLE_DTM_TEST_UPDATE_EVT,                  /*!< 直接测试模式状态改变事件 */
    // BLE_包含
    ESP_GAP_BLE_ADV_CLEAR_COMPLETE_EVT,               /*!< 清除广告完成事件 */
    ESP_GAP_BLE_VENDOR_CMD_COMPLETE_EVT,              /*!< 厂商HCI命令完成事件 */
    ESP_GAP_BLE_EVT_MAX,                              /*!< 最大广告事件完成 */
} esp_gap_ble_cb_event_t;

typedef struct {
    bool                    set_scan_rsp;           /*!< 是否将此广告数据设置为扫描响应 */
    bool                    include_name;           /*!< 广告数据是否包含设备名称 */
    bool                    include_txpower;        /*!< 广告数据是否包含发射功率 */
    int                     min_interval;           /*!< 广告数据显示从设备首选的最小连接间隔。
                                                    连接间隔如下所示：
                                                    connIntervalmin = Conn_Interval_Min * 1.25 毫秒
                                                    连接间隔最小值范围：0x0006 到 0x0C80
                                                    0xFFFF 表示无特定最小值。
                                                    上述未定义的值保留供将来使用。*/

    int                     max_interval;           /*!< 广告数据显示从设备首选的最大连接间隔。
                                                    连接间隔如下所示：
                                                    connIntervalmax = Conn_Interval_Max * 1.25 毫秒
                                                    连接间隔最大值范围：0x0006 到 0x0C80
                                                    连接间隔最大值应等于或大于连接间隔最小值。
                                                    0xFFFF 表示无特定最大值。
                                                    上述未定义的值保留供将来使用。*/

    int                     appearance;             /*!< 设备的外观 */
    uint16_t                manufacturer_len;       /*!< 厂商数据长度 */
    uint8_t                 *p_manufacturer_data;   /*!< 厂商数据指针 */
    uint16_t                service_data_len;       /*!< 服务数据长度 */
    uint8_t                 *p_service_data;        /*!< 服务数据指针 */
    uint16_t                service_uuid_len;       /*!< 服务UUID长度 */
    uint8_t                 *p_service_uuid;        /*!< 服务UUID数组指针 */
    uint8_t                 flag;                   /*!< 发现模式的广告标志，详见BLE_ADV_DATA_FLAG */
} esp_ble_adv_data_t;

/********************************************************************************************** */
typedef struct {
    esp_ble_scan_type_t     scan_type;              /*!< 扫描类型 */
    esp_ble_addr_type_t     own_addr_type;          /*!< 自身地址类型 */
    esp_ble_scan_filter_t   scan_filter_policy;     /*!< 扫描过滤策略 */
    uint16_t                scan_interval;          /*!< 扫描间隔。定义为控制器启动上一次LE扫描到开始下一次LE扫描的时间间隔。
                                                      范围：0x0004 到 0x4000 默认：0x0010（10 毫秒）
                                                      时间 = N * 0.625 毫秒
                                                      时间范围：2.5 毫秒到10.24 秒 */
    uint16_t                scan_window;            /*!< 扫描窗口。LE扫描的持续时间。LE_Scan_Window
                                                      应小于或等于LE_Scan_Interval
                                                      范围：0x0004 到 0x4000 默认：0x0010（10 毫秒）
                                                      时间 = N * 0.625 毫秒
                                                      时间范围：2.5 毫秒到10240 毫秒 */
    esp_ble_scan_duplicate_t  scan_duplicate;       /*!< Scan_Duplicates参数控制链路层是否应过滤掉
                                                        主机的重复广告报告（BLE_SCAN_DUPLICATE_ENABLE），或者链路层是否应为每个接收到的数据包生成广告报告 */
} esp_ble_scan_params_t;

//设置扫描方式:

typedef enum {
    BLE_SCAN_TYPE_PASSIVE   =   0x0,            /*!< 被动扫描 */
    BLE_SCAN_TYPE_ACTIVE    =   0x1,            /*!< 主动扫描 */
} esp_ble_scan_type_t;

//设置自身地址类型

typedef enum {
    BLE_ADDR_TYPE_PUBLIC        = 0x00,     /*!< 公共设备地址 */
    BLE_ADDR_TYPE_RANDOM        = 0x01,     /*!< 随机设备地址。要设置此地址，请使用函数 esp_ble_gap_set_rand_addr(esp_bd_addr_t rand_addr) */
    BLE_ADDR_TYPE_RPA_PUBLIC    = 0x02,     /*!< 使用公共标识地址的可解析私有地址 (RPA) */
    BLE_ADDR_TYPE_RPA_RANDOM    = 0x03,     /*!< 使用随机标识地址的可解析私有地址 (RPA)。要设置此地址，请使用函数 esp_ble_gap_set_rand_addr(esp_bd_addr_t rand_addr) */
} esp_ble_addr_type_t;

//设置扫描过滤策略:
typedef enum {
    BLE_SCAN_FILTER_ALLOW_ALL           = 0x0,  /*!< 接受所有：
                                                  1. 广告数据包，除了未定向到该设备的定向广告数据包（默认）。 */
    BLE_SCAN_FILTER_ALLOW_ONLY_WLST     = 0x1,  /*!< 仅接受：
                                                  1. 广告主地址在白名单中的设备发送的广告数据包。
                                                  2. 忽略未定向到该设备的定向广告数据包。 */
    BLE_SCAN_FILTER_ALLOW_UND_RPA_DIR   = 0x2,  /*!< 接受所有：
                                                  1. 非定向广告数据包，和
                                                  2. 发起者地址是可解析私有地址的定向广告数据包，和
                                                  3. 定向到该设备的定向广告数据包。 */
    BLE_SCAN_FILTER_ALLOW_WLIST_RPA_DIR = 0x3,  /*!< 接受所有：
                                                  1. 广告主地址在白名单中的设备发送的广告数据包，和
                                                  2. 发起者地址是可解析私有地址的定向广告数据包，和
                                                  3. 定向到该设备的定向广告数据包。 */
} esp_ble_scan_filter_t;

//设置Scan_Duplicates参数控制链路层是否应过滤掉
typedef enum {
    BLE_SCAN_DUPLICATE_DISABLE           = 0x0,  /*!< 链路层应为每个接收到的数据包生成广告报告 */
    BLE_SCAN_DUPLICATE_ENABLE            = 0x1,  /*!< 链路层应过滤掉主机的重复广告报告 */
    #if (BLE_50_FEATURE_SUPPORT == TRUE)
    BLE_SCAN_DUPLICATE_ENABLE_RESET,             /*!< 启用重复过滤，每个扫描周期重置，仅支持BLE 5.0。 */
    #endif
    BLE_SCAN_DUPLICATE_MAX                       /*!< 保留供将来使用。 */
} esp_ble_scan_duplicate_t;

/************************************************************************************************************************************ */

typedef struct {
    uint16_t                adv_int_min;        /*!< 未定向和低占空比定向广告的最小广告间隔。
                                                  范围：0x0020 到 0x4000 默认：N = 0x0800（1.28 秒）
                                                  时间 = N * 0.625 毫秒 时间范围：20 毫秒到 10.24 秒 */
    uint16_t                adv_int_max;        /*!< 未定向和低占空比定向广告的最大广告间隔。
                                                  范围：0x0020 到 0x4000 默认：N = 0x0800（1.28 秒）
                                                  时间 = N * 0.625 毫秒 时间范围：20 毫秒到 10.24 秒 广告最大间隔 */
    esp_ble_adv_type_t      adv_type;           /*!< 广告类型 */
    esp_ble_addr_type_t     own_addr_type;      /*!< 本设备蓝牙地址类型 */
    esp_bd_addr_t           peer_addr;          /*!< 对端设备蓝牙地址 */
    esp_ble_addr_type_t     peer_addr_type;     /*!< 对端设备蓝牙地址类型，仅支持公共地址类型和随机地址类型 */
    esp_ble_adv_channel_t   channel_map;        /*!< 广告信道图 */
    esp_ble_adv_filter_t    adv_filter_policy;  /*!< 广告过滤策略 */
} esp_ble_adv_params_t;

typedef enum {
    ADV_TYPE_IND                = 0x00,  /*!< 普通非定向广告 */
    ADV_TYPE_DIRECT_IND_HIGH    = 0x01,  /*!< 高占空比定向广告 */
    ADV_TYPE_SCAN_IND           = 0x02,  /*!< 扫描响应广告 */
    ADV_TYPE_NONCONN_IND        = 0x03,  /*!< 不可连接的非定向广告 */
    ADV_TYPE_DIRECT_IND_LOW     = 0x04,  /*!< 低占空比定向广告 */
} esp_ble_adv_type_t;

//对端蓝牙设备地址类型 就是一个uint8_t的6个数组
typedef uint8_t esp_bd_addr_t[6];


typedef enum {
    ADV_CHNL_37     = 0x01, /*!< 广告信道 37 */
    ADV_CHNL_38     = 0x02, /*!< 广告信道 38 */
    ADV_CHNL_39     = 0x04, /*!< 广告信道 39 */
    ADV_CHNL_ALL    = 0x07, /*!< 所有广告信道 (37, 38, 39) */
} esp_ble_adv_channel_t;


//设置广告过滤策略
typedef enum {
    ///允许任何设备的扫描和连接请求
    ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY  = 0x00,
    ///仅允许白名单设备的扫描请求，但允许任何设备的连接请求
    ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY,
    ///允许任何设备的扫描请求，但仅允许白名单设备的连接请求
    ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST,
    ///仅允许白名单设备的扫描和连接请求
    ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST,
    ///广告过滤策略值检查的枚举结束值
} esp_ble_adv_filter_t;


/*************************************************************************************************** */

typedef struct {
    esp_bd_addr_t bda;                              /*!< 蓝牙设备地址 */
    uint16_t min_int;                               /*!< 最小连接间隔 */
    uint16_t max_int;                               /*!< 最大连接间隔 */
    uint16_t latency;                               /*!< 连接的从属延迟，以连接事件数表示。范围：0x0000 到 0x01F3 */
    uint16_t timeout;                               /*!< LE链路的监督超时时间。范围：0x000A 到 0x0C80。
                                                      强制范围：0x000A 到 0x0C80 时间 = N * 10 毫秒
                                                      时间范围：100 毫秒到 32 秒 */
} esp_ble_conn_update_params_t;



/***********************************************************************************************************/
//设置频道: 
#define ESP_GAP_BLE_CHANNELS_LEN 5
typedef uint8_t esp_gap_ble_channels[ESP_GAP_BLE_CHANNELS_LEN];

typedef struct {
    esp_ble_gap_sync_t filter_policy;       /*!< 配置周期性广告同步的过滤策略：
                                                 0：使用广告SID、广告商地址类型和广告商地址参数来确定要监听的广告商。
                                                 1：使用周期性广告商列表来确定要监听的广告商。 */
    #if (CONFIG_BT_BLE_FEAT_CREATE_SYNC_ENH)
    esp_ble_gap_sync_t reports_disabled;    /*!< 仅支持esp32c2、esp32c6和esp32h2；可通过menuconfig设置：
                                                 0：初始启用报告。
                                                 1：初始禁用报告。 */
    esp_ble_gap_sync_t filter_duplicates;   /*!< 仅支持esp32c2、esp32c6和esp32h2；可通过menuconfig设置：
                                                 0：初始禁用重复过滤。
                                                 1：初始启用重复过滤。 */
    #endif
    uint8_t sid;                            /*!< 周期性广告的SID */
    esp_ble_addr_type_t addr_type;          /*!< 周期性广告的地址类型 */
    esp_bd_addr_t addr;                     /*!< 周期性广告的地址 */
    uint16_t skip;                          /*!< 可跳过的最大周期性广告事件数量 */
    uint16_t sync_timeout;                  /*!< 同步超时时间 */
} esp_ble_gap_periodic_adv_sync_params_t;

//设置周期性广告同步过滤策略
typedef uint8_t esp_ble_gap_sync_t;


    //GATT回调函数类型：
    typedef void (* esp_gatts_cb_t)(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);


    typedef enum {
    ESP_GATTS_REG_EVT                 = 0,       /*!< 当注册应用程序 ID 时，发生此事件 */
    ESP_GATTS_READ_EVT                = 1,       /*!< 当 GATT 客户端请求读取操作时，发生此事件 */
    ESP_GATTS_WRITE_EVT               = 2,       /*!< 当 GATT 客户端请求写入操作时，发生此事件 */
    ESP_GATTS_EXEC_WRITE_EVT          = 3,       /*!< 当 GATT 客户端请求执行写入时，发生此事件 */
    ESP_GATTS_MTU_EVT                 = 4,       /*!< 当设置 MTU 完成时，发生此事件 */
    ESP_GATTS_CONF_EVT                = 5,       /*!< 当接收到确认时，发生此事件 */
    ESP_GATTS_UNREG_EVT               = 6,       /*!< 当注销应用程序 ID 时，发生此事件 */
    ESP_GATTS_CREATE_EVT              = 7,       /*!< 当创建服务完成时，发生此事件 */
    ESP_GATTS_ADD_INCL_SRVC_EVT       = 8,       /*!< 当添加包含服务完成时，发生此事件 */
    ESP_GATTS_ADD_CHAR_EVT            = 9,       /*!< 当添加特征完成时，发生此事件 */
    ESP_GATTS_ADD_CHAR_DESCR_EVT      = 10,      /*!< 当添加描述符完成时，发生此事件 */
    ESP_GATTS_DELETE_EVT              = 11,      /*!< 当删除服务完成时，发生此事件 */
    ESP_GATTS_START_EVT               = 12,      /*!< 当启动服务完成时，发生此事件 */
    ESP_GATTS_STOP_EVT                = 13,      /*!< 当停止服务完成时，发生此事件 */
    ESP_GATTS_CONNECT_EVT             = 14,      /*!< 当 GATT 客户端连接时，发生此事件 */
    ESP_GATTS_DISCONNECT_EVT          = 15,      /*!< 当 GATT 客户端断开连接时，发生此事件 */
    ESP_GATTS_OPEN_EVT                = 16,      /*!< 当连接到对等设备时，发生此事件 */
    ESP_GATTS_CANCEL_OPEN_EVT         = 17,      /*!< 当从对等设备断开连接时，发生此事件 */
    ESP_GATTS_CLOSE_EVT               = 18,      /*!< 当 GATT 服务器关闭时，发生此事件 */
    ESP_GATTS_LISTEN_EVT              = 19,      /*!< 当 GATT 监听待连接时，发生此事件 */
    ESP_GATTS_CONGEST_EVT             = 20,      /*!< 当发生拥塞时，发生此事件 */
    /* 以下是额外事件 */
    ESP_GATTS_RESPONSE_EVT            = 21,      /*!< 当 GATT 发送响应完成时，发生此事件 */
    ESP_GATTS_CREAT_ATTR_TAB_EVT      = 22,      /*!< 当 GATT 创建属性表完成时，发生此事件 */
    ESP_GATTS_SET_ATTR_VAL_EVT        = 23,      /*!< 当 GATT 设置属性值完成时，发生此事件 */
    ESP_GATTS_SEND_SERVICE_CHANGE_EVT = 24,      /*!< 当 GATT 发送服务变更指示完成时，发生此事件 */
} esp_gatts_cb_event_t;


typedef uint8_t    esp_gatt_if_t;                           /*!< GATT 接口类型，不同的 GATT 客户端应用程序使用不同的 gatt_if */
/************************************************************************************************* */
//设置GATid
typedef struct {
    esp_bt_uuid_t   uuid;                   /*!< UUID，唯一标识符 */
    uint8_t         inst_id;                /*!< 实例 ID */
} __attribute__((packed)) esp_gatt_id_t;

typedef struct {
    esp_gatt_id_t   id;                     /*!< GATT ID，包括 UUID 和实例 */
    bool            is_primary;             /*!< 表示此服务是否为主服务 */
} __attribute__((packed)) esp_gatt_srvc_id_t;


/******************************************************************************************* */

typedef struct
{
    esp_attr_control_t      attr_control;                   /*!< The attribute control type */
    esp_attr_desc_t         att_desc;                       /*!< The attribute type */
} esp_gatts_attr_db_t;

typedef struct
{
#define ESP_GATT_RSP_BY_APP             0   /*!< 响应由应用程序处理 */
#define ESP_GATT_AUTO_RSP               1   /*!< 响应由 GATT 堆栈自动处理 */

    /**
     * @brief 如果 auto_rsp 设置为 ESP_GATT_RSP_BY_APP，表示读写操作的响应将由应用程序处理。
     *        如果 auto_rsp 设置为 ESP_GATT_AUTO_RSP，表示读写操作的响应将由 GATT 堆栈自动处理。
     */
    uint8_t auto_rsp;
} esp_attr_control_t;

typedef struct
{
    uint16_t uuid_length;              /*!< UUID 长度 */
    uint8_t  *uuid_p;                  /*!< UUID 值 */
    uint16_t perm;                     /*!< 属性权限 */
    uint16_t max_length;               /*!< 元素的最大长度 */
    uint16_t length;                   /*!< 元素的当前长度 */
    uint8_t  *value;                   /*!< 元素值数组 */
} esp_attr_desc_t;



typedef struct
{
    uint16_t attr_max_len;                                  /*!< 属性最大值长度 */
    uint16_t attr_len;                                      /*!< 属性当前值长度 */
    uint8_t  *attr_value;                                   /*!< 指向属性值的指针 */
} esp_attr_value_t;
