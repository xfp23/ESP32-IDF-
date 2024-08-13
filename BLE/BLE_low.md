**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息

© 2024 . 未经许可不得复制、修改或分发。 此文献为 [小風的藏書閣](https://t.me/xfp2333) 所有。

# 低功耗蓝牙编程指南

- 蓝牙相对复杂,[此处注解了部分知识点](bluetooth_low.md)
# 简介



# 环境配置

1. 包含头文件

```c
#include "esp_bt_device.h"
```

2. CMAKE配置

```c
REQUIRES bt
```

# 资源分配
```c
typedef struct {
    bool                    set_scan_rsp;           /*!< 是否将此广告数据设置为扫描响应数据 */
    bool                    include_name;           /*!< 广告数据中是否包含设备名称 */
    bool                    include_txpower;        /*!< 广告数据中是否包含发射功率 */
    int                     min_interval;           /*!< 广告数据中显示的从设备首选连接最小间隔。
                                                    连接间隔的计算方式为：
                                                    connIntervalmin = Conn_Interval_Min * 1.25 ms
                                                    Conn_Interval_Min 范围：0x0006 到 0x0C80
                                                    0xFFFF 表示没有特定的最小值。
                                                    上述未定义的值保留用于将来使用。*/

    int                     max_interval;           /*!< 广告数据中显示的从设备首选连接最大间隔。
                                                    连接间隔的计算方式为：
                                                    connIntervalmax = Conn_Interval_Max * 1.25 ms
                                                    Conn_Interval_Max 范围：0x0006 到 0x0C80
                                                    Conn_Interval_Max 应等于或大于 Conn_Interval_Min。
                                                    0xFFFF 表示没有特定的最大值。
                                                    上述未定义的值保留用于将来使用。*/

    int                     appearance;             /*!< 设备的外观属性 */
    uint16_t                manufacturer_len;       /*!< 制造商数据长度 */
    uint8_t                 *p_manufacturer_data;   /*!< 制造商数据指针 */
    uint16_t                service_data_len;       /*!< 服务数据长度 */
    uint8_t                 *p_service_data;        /*!< 服务数据指针 */
    uint16_t                service_uuid_len;       /*!< 服务UUID长度 */
    uint8_t                 *p_service_uuid;        /*!< 服务UUID数组指针 */
    uint8_t                 flag;                   /*!< 发现模式的广告标志，详见 BLE_ADV_DATA_FLAG */
} esp_ble_adv_data_t;

```

- [ble工程直接拿来使用](bluetooth.c)