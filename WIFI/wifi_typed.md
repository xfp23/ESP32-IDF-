# 版权信息

© 2024 . 未经许可不得复制、修改或分发。此文献为 **DXG工作室** 所有。

---

## 简介

此文档列举了ESP-IDF WiFi组件中的WiFi STA（Station）模式的相关类型信息，适用于开发基于ESP32的WiFi应用程序。

## WiFi 配置类型

在WiFi配置中，`wifi_config_t` 结构体是一个联合体，用于存储不同模式下的WiFi配置，包括AP、STA和NAN模式。根据你所使用的模式，选择相应的配置。

```c
typedef union {
    wifi_ap_config_t  ap;  /**< AP模式配置 */
    wifi_sta_config_t sta; /**< STA模式配置 */
    wifi_nan_config_t nan; /**< NAN模式配置 */
} wifi_config_t;
```

### 事件句柄类型

```c
typedef void* esp_event_handler_instance_t; /**< 识别已注册事件处理程序实例的上下文指针 */

```
### WiFi 事件

在STA模式下，ESP32通过事件机制来处理WiFi状态的变化。以下是`wifi_event_t`枚举中与STA模式相关的事件：

```c
typedef enum {
    WIFI_EVENT_WIFI_READY = 0,           /**< WiFi 准备就绪 */
    WIFI_EVENT_SCAN_DONE,                /**< 完成扫描AP（接入点） */
    WIFI_EVENT_STA_START,                /**< Station（客户端）启动 */
    WIFI_EVENT_STA_STOP,                 /**< Station（客户端）停止 */
    WIFI_EVENT_STA_CONNECTED,            /**< Station（客户端）已连接到AP */
    WIFI_EVENT_STA_DISCONNECTED,         /**< Station（客户端）已从AP断开连接 */
    WIFI_EVENT_STA_AUTHMODE_CHANGE,      /**< Station（客户端）连接的AP的身份验证模式发生了变化 */

    WIFI_EVENT_STA_WPS_ER_SUCCESS,       /**< Station（客户端）WPS在enrollee模式下成功 */
    WIFI_EVENT_STA_WPS_ER_FAILED,        /**< Station（客户端）WPS在enrollee模式下失败 */
    WIFI_EVENT_STA_WPS_ER_TIMEOUT,       /**< Station（客户端）WPS在enrollee模式下超时 */
    WIFI_EVENT_STA_WPS_ER_PIN,           /**< Station（客户端）WPS在enrollee模式下输入PIN码 */
    WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP,   /**< Station（客户端）WPS在enrollee模式下发生PBC重叠 */

    WIFI_EVENT_AP_START,                 /**< Soft-AP（热点模式）启动 */
    WIFI_EVENT_AP_STOP,                  /**< Soft-AP（热点模式）停止 */
    WIFI_EVENT_AP_STACONNECTED,          /**< 有设备连接到Soft-AP */
    WIFI_EVENT_AP_STADISCONNECTED,       /**< 有设备从Soft-AP断开连接 */
    WIFI_EVENT_AP_PROBEREQRECVED,        /**< 在Soft-AP模式下接收到探测请求包 */

    WIFI_EVENT_FTM_REPORT,               /**< 接收到FTM（Fine Timing Measurement）过程的报告 */

    /* 添加新事件请放在此处 */
    WIFI_EVENT_STA_BSS_RSSI_LOW,         /**< AP的RSSI值低于配置的阈值 */
    WIFI_EVENT_ACTION_TX_STATUS,         /**< Action Tx操作的状态指示 */
    WIFI_EVENT_ROC_DONE,                 /**< Remain-on-Channel操作完成 */

    WIFI_EVENT_STA_BEACON_TIMEOUT,       /**< Station（客户端）信标超时 */

    WIFI_EVENT_CONNECTIONLESS_MODULE_WAKE_INTERVAL_START,   /**< 无连接模块唤醒间隔开始 */

    WIFI_EVENT_AP_WPS_RG_SUCCESS,       /**< Soft-AP（热点模式）WPS在注册器模式下成功 */
    WIFI_EVENT_AP_WPS_RG_FAILED,        /**< Soft-AP（热点模式）WPS在注册器模式下失败 */
    WIFI_EVENT_AP_WPS_RG_TIMEOUT,       /**< Soft-AP（热点模式）WPS在注册器模式下超时 */
    WIFI_EVENT_AP_WPS_RG_PIN,           /**< Soft-AP（热点模式）WPS在注册器模式下输入PIN码 */
    WIFI_EVENT_AP_WPS_RG_PBC_OVERLAP,   /**< Soft-AP（热点模式）WPS在注册器模式下发生PBC重叠 */

    WIFI_EVENT_ITWT_SETUP,              /**< iTWT设置 */
    WIFI_EVENT_ITWT_TEARDOWN,           /**< iTWT拆除 */
    WIFI_EVENT_ITWT_PROBE,              /**< iTWT探测 */
    WIFI_EVENT_ITWT_SUSPEND,            /**< iTWT暂停 */
    WIFI_EVENT_TWT_WAKEUP,              /**< TWT唤醒 */

    WIFI_EVENT_NAN_STARTED,              /**< NAN（邻近感知网络）发现已启动 */
    WIFI_EVENT_NAN_STOPPED,              /**< NAN（邻近感知网络）发现已停止 */
    WIFI_EVENT_NAN_SVC_MATCH,            /**< NAN服务发现匹配 */
    WIFI_EVENT_NAN_REPLIED,              /**< 回复了NAN对等体的服务发现匹配 */
    WIFI_EVENT_NAN_RECEIVE,              /**< 接收到跟进消息 */
    WIFI_EVENT_NDP_INDICATION,           /**< 接收到来自NAN对等体的NDP请求 */
    WIFI_EVENT_NDP_CONFIRM,              /**< NDP确认指示 */
    WIFI_EVENT_NDP_TERMINATED,           /**< NAN数据路径终止指示 */
    WIFI_EVENT_HOME_CHANNEL_CHANGE,      /**< WiFi主信道变更，扫描时不触发 */

    WIFI_EVENT_MAX,                      /**< 无效的WiFi事件ID */
} wifi_event_t;
```

### WiFi初始化配置

在使用ESP-IDF框架进行WiFi初始化时，可以通过`wifi_init_config_t`结构体进行配置。该结构体包含了WiFi模块的初始化参数，例如接收和发送缓冲区的数量、是否启用CSI（信道状态信息）等。对于大多数应用程序，可以使用默认宏`WIFI_INIT_CONFIG_DEFAULT`进行初始化配置。

```c
typedef struct {
    wifi_osi_funcs_t*      osi_funcs;              /**< WiFi OS函数指针 */
    wpa_crypto_funcs_t     wpa_crypto_funcs;       /**< WiFi站点连接时的加密功能 */
    int                    static_rx_buf_num;      /**< WiFi静态接收缓冲区的数量 */
    int                    dynamic_rx_buf_num;     /**< WiFi动态接收缓冲区的数量 */
    int                    tx_buf_type;            /**< WiFi发送缓冲区类型 */
    int                    static_tx_buf_num;      /**< WiFi静态发送缓冲区的数量 */
    int                    dynamic_tx_buf_num;     /**< WiFi动态发送缓冲区的数量 */
    int                    rx_mgmt_buf_type;       /**< WiFi接收管理缓冲区类型 */
    int                    rx_mgmt_buf_num;        /**< WiFi接收管理缓冲区的数量 */
    int                    cache_tx_buf_num;       /**< WiFi发送缓存缓冲区的数量 */
    int                    csi_enable;             /**< WiFi信道状态信息（CSI）启用标志 */
    int                    ampdu_rx_enable;        /**< WiFi AMPDU（聚合MAC协议数据单元）接收功能启用标志 */
    int                    ampdu_tx_enable;        /**< WiFi AMPDU（聚合MAC协议数据单元）发送功能启用标志 */
    int                    amsdu_tx_enable;        /**< WiFi AMSDU（聚合MAC服务数据单元）发送功能启用标志 */
    int                    nvs_enable;             /**< WiFi NVS（非易失性存储器）闪存启用标志 */
    int                    nano_enable;            /**< Nano选项，用于printf/scan系列函数的启用标志 */
    int                    rx_ba_win;              /**< WiFi块确认接收窗口大小 */
    int                    wifi_task_core_id;      /**< WiFi任务核心ID */
    int                    beacon_max_len;         /**< WiFi软AP信标的最大长度 */
    int                    mgmt_sbuf_num;          /**< WiFi管理短缓冲区的数量，最小值为6，最大值为32 */
    uint64_t               feature_caps;           /**< 启用额外的WiFi功能和能力 */
    bool                   sta_disconnected_pm;    /**< WiFi站点断开连接时的电源管理标志 */
    int                    espnow_max_encrypt_num; /**< espnow支持的最大加密节点数量 */
    int                    magic;                  /**< WiFi初始化魔术数字，应为最后一个字段 */
} wifi_init_config_t; //如果不想自定义配置,可直接利用以下宏初始化:

#define WIFI_INIT_CONFIG_DEFAULT() { \
    .osi_funcs = &g_wifi_osi_funcs, \
    .wpa_crypto_funcs = g_wifi_default_wpa_crypto_funcs, \
    .static_rx_buf_num = CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM,\
    .dynamic_rx_buf_num = CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM,\
    .tx_buf_type = CONFIG_ESP_WIFI_TX_BUFFER_TYPE,\
    .static_tx_buf_num = WIFI_STATIC_TX_BUFFER_NUM,\
    .dynamic_tx_buf_num = WIFI_DYNAMIC_TX_BUFFER_NUM,\
    .rx_mgmt_buf_type = CONFIG_ESP_WIFI_DYNAMIC_RX_MGMT_BUF,\
    .rx_mgmt_buf_num = WIFI_RX_MGMT_BUF_NUM_DEF,\
    .cache_tx_buf_num = WIFI_CACHE_TX_BUFFER_NUM,\
    .csi_enable = WIFI_CSI_ENABLED,\
    .ampdu_rx_enable = WIFI_AMPDU_RX_ENABLED,\
    .ampdu_tx_enable = WIFI_AMPDU_TX_ENABLED,\
    .amsdu_tx_enable = WIFI_AMSDU_TX_ENABLED,\
    .nvs_enable = WIFI_NVS_ENABLED,\
    .nano_enable = WIFI_NANO_FORMAT_ENABLED,\
    .rx_ba_win = WIFI_DEFAULT_RX_BA_WIN,\
    .wifi_task_core_id = WIFI_TASK_CORE_ID,\
    .beacon_max_len = WIFI_SOFTAP_BEACON_MAX_LEN, \
    .mgmt_sbuf_num = WIFI_MGMT_SBUF_NUM, \
    .feature_caps = WIFI_FEATURE_CAPS, \
    .sta_disconnected_pm = WIFI_STA_DISCONNECTED_PM_ENABLED,  \
    .espnow_max_encrypt_num = CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM, \
    .magic = WIFI_INIT_CONFIG_MAGIC\
}
```
### STA配置

以下是`wifi_sta_config_t`结构体的定义，用于配置STA模式下的WiFi连接参数。

```c
/** @brief 设备的STA配置设置 */
typedef struct {
    uint8_t ssid[32];                         /**< 目标AP的SSID（服务集标识符）。*/
    uint8_t password[64];                     /**< 目标AP的密码。*/
    wifi_scan_method_t scan_method;           /**< 扫描方式：全通道扫描或快速扫描。*/
    bool bssid_set;                           /**< 是否设置目标AP的MAC地址。通常情况下，station_config.bssid_set需要为0；只有在用户需要检查AP的MAC地址时，才需要将其设置为1。*/
    uint8_t bssid[6];                         /**< 目标AP的MAC地址。*/
    uint8_t channel;                          /**< 目标AP的信道。设置为1~13时，将从指定信道开始扫描，然后再连接AP。如果未知AP的信道，则将其设置为0。*/
    uint16_t listen_interval;                 /**< 当WIFI_PS_MAX_MODEM被设置时，ESP32站点接收信标的监听间隔。单位：AP信标间隔。默认为3，如果设置为0。*/
    wifi_sort_method_t sort_method;           /**< 按信号强度或安全模式对连接AP进行排序。*/
    wifi_scan_threshold_t  threshold;         /**< 当设置了scan_threshold时，只会使用具有比选定的认证模式更安全的认证模式且信号强度高于最小RSSI的AP。*/
    wifi_pmf_config_t pmf_cfg;                /**< 保护管理帧的配置。将在RSN信息元素中的RSN能力中播发。*/
    uint32_t rm_enabled:1;                    /**< 是否启用无线电测量功能。*/
    uint32_t btm_enabled:1;                   /**< 是否启用BSS过渡管理功能。*/
    uint32_t mbo_enabled:1;                   /**< 是否启用MBO（移动性优化）。*/
    uint32_t ft_enabled:1;                    /**< 是否启用FT（快速过渡）。*/
    uint32_t owe_enabled:1;                   /**< 是否启用OWE（开放无线网络增强）。*/
    uint32_t transition_disable:1;            /**< 是否启用过渡禁用功能。*/
    uint32_t reserved:26;                     /**< 保留用于将来功能集的位。*/
    wifi_sae_pwe_method_t sae_pwe_h2e;        /**< SAE PWE推导方法的配置。*/
    wifi_sae_pk_mode_t sae_pk_mode;           /**< SAE-PK（公钥）认证方法的配置。*/
    uint8_t failure_retry_cnt;                /**< 站点在移动到下一个AP之前的连接重试次数。scan_method应设置为WIFI_ALL_CHANNEL_SCAN以使用此配置。 
                                                   注意：启用此功能可能会导致连接时间增加，如果最佳AP表现不正常。*/
    uint32_t he_dcm_set:1;                                        /**< 是否设置DCM最大星座图进行传输和接收。*/
    uint32_t he_dcm_max_constellation_tx:2;                       /**< 表示STA支持的DCM在TB PPDU中的最大星座图。0：不支持。1：BPSK，2：QPSK，3：16-QAM。默认值为3。*/
    uint32_t he_dcm_max_constellation_rx:2;                       /**< 表示STA支持的DCM在数据字段和HE-SIG-B字段中的最大星座图。0：不支持。1：BPSK，2：QPSK，3：16-QAM。默认值为3。*/
    uint32_t he_mcs9_enabled:1;                                   /**< 是否支持HE-MCS 0到9。默认值为0。*/
    uint32_t he_su_beamformee_disabled:1;                         /**< 是否禁用作为SU波束成形器操作的支持。*/
    uint32_t he_trig_su_bmforming_feedback_disabled:1;            /**< 是否禁用在HE TB探测序列中支持SU反馈传输。*/
    uint32_t he_trig_mu_bmforming_partial_feedback_disabled:1;    /**< 是否禁用在HE TB探测序列中支持部分带宽MU反馈传输。*/
    uint32_t he_trig_cqi_feedback_disabled:1;                     /**< 是否禁用在HE TB探测序列中支持CQI反馈传输。*/
    uint32_t he_reserved:22;                                      /**< 保留用于将来功能集的位。*/
    uint8_t sae_h2e_identifier[SAE_H2E_IDENTIFIER_LEN];/**< 用于H2E的密码标识符。这需要是以null终止的字符串。*/
} wifi_sta_config_t;
```

### AP配置

```c
/** @brief 设备的Soft-AP配置设置 */
typedef struct {
    uint8_t ssid[32];           /**< Soft-AP的SSID。如果ssid_len字段为0，则此字段必须为以Null结尾的字符串。否则，长度将根据ssid_len字段设置。 */
    uint8_t password[64];       /**< Soft-AP的密码。 */
    uint8_t ssid_len;           /**< SSID字段的可选长度。 */
    uint8_t channel;            /**< Soft-AP的信道。 */
    wifi_auth_mode_t authmode;  /**< Soft-AP的认证模式。在Soft-AP模式下，不支持AUTH_WEP、AUTH_WAPI_PSK和AUTH_OWE。当认证模式设置为WPA2_PSK、WPA2_WPA3_PSK或WPA3_PSK时，配对密码将被WIFI_CIPHER_TYPE_CCMP覆盖。*/
    uint8_t ssid_hidden;        /**< 是否广播SSID，默认值为0，表示广播SSID。 */
    uint8_t max_connection;     /**< 允许连接的最大站点数量。 */
    uint16_t beacon_interval;   /**< 信标间隔，应该是100的倍数。单位：TU（时间单位，1 TU = 1024微秒）。范围：100 ~ 60000。默认值：100。 */
    wifi_cipher_type_t pairwise_cipher;   /**< SoftAP的配对密码，组密码将使用此密码派生。加密值从WIFI_CIPHER_TYPE_TKIP开始有效，在此之前的枚举值将被视为无效，并将使用默认的加密套件（TKIP+CCMP）。SoftAP模式下有效的加密套件为WIFI_CIPHER_TYPE_TKIP、WIFI_CIPHER_TYPE_CCMP和WIFI_CIPHER_TYPE_TKIP_CCMP。*/
    bool ftm_responder;         /**< 是否启用FTM应答器模式。 */
    wifi_pmf_config_t pmf_cfg;  /**< 保护管理帧的配置。 */
    wifi_sae_pwe_method_t sae_pwe_h2e;  /**< SAE PWE推导方法的配置。 */
} wifi_ap_config_t;
```


# 回调函数示例：

```c
void esp_event_handler_cb(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)//此处处理连接wifi事件
    {
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_ETH_GOT_IP) // 此处处理连接成功获取ip事件
    {
        ip_event_got_ip_t *getInfoStruct = (ip_event_got_ip_t *)event_data;
        esp_ip4_addr_t ip = getInfoStruct->ip_info.ip;
        printf("get ip success!" IPSTR "\n", IP2STR(&ip));
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED)//此处处理AP被连接事件
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        printf("Station " MACSTR " joined, AID=%d\n",
               MAC2STR(event->mac), event->aid);
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) //此处处理AP断开连接事件
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        printf("Station " MACSTR " left, AID=%d\n",
               MAC2STR(event->mac), event->aid);
    }
}
```
