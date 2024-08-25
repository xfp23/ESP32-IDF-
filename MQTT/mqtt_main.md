# ESP32-IDF- 基于5.2.2版本

**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息

© 2024 . 未经许可不得复制、修改或分发。 此文献为 **DXG工作室** 所有。
---

# 简介

- 本文档为基于 TCP 的 MQTT 默认端口为`1883`
- [此处为基于TLS的MQTT文档](mqtt_tls.md)
- [此处为基于 WebSocket Secure ](mqtt_web.md)

## 概述

-  ESP-MQTT 是 MQTT 协议客户端的实现，MQTT 是一种基于发布/订阅模式的轻量级消息传输协议。ESP-MQTT 当前支持 MQTT v5.0

## 特性

- 支持基于 TCP 的 MQTT、基于 Mbed TLS 的 SSL、基于 WebSocket 的 MQTT 以及基于 WebSocket Secure 的 MQTT

- 通过 URI 简化配置流程

- 多个实例（一个应用程序中有多个客户端）

- 支持订阅、发布、认证、遗嘱消息、保持连接心跳机制以及 3 个服务质量 (QoS) 级别（组成全功能客户端）

# 编程指南

## 环境准备

1. 配置cmake

```c

REQUIRES mqtt

```

2. 包含头文件

```c

#include "mqtt_client.h"

```

## 配置

1. 通过设置 `esp_mqtt_client_config_t` 结构体中的字段来进行配置

```c
这是 `esp_mqtt_client_config_t` 结构体的翻译，包含了 MQTT 客户端的配置信息：

```c

// 句柄类型:

esp_mqtt_client_handle_t;
/**
 * *MQTT* 客户端配置结构体
 *
 *  - 默认值可以通过 menuconfig 设置
 *  - 所有证书和密钥数据可以以 PEM 或 DER 格式传递。PEM 格式必须有一个终止的 NULL 字符，并且相关的 len 字段设置为 0。DER 格式要求相关的 len 字段设置为正确的长度。
 */
typedef struct esp_mqtt_client_config_t {
    /**
     *   代理相关配置
     */
    struct broker_t {
        /**
         * 代理地址
         *
         *  - uri 优先于其他字段
         *  - 如果 uri 未设置，则至少应该设置 hostname、transport 和 port。
         */
        struct address_t {
            const char *uri; /*!< 完整的 *MQTT* 代理 URI */
            const char *hostname; /*!< 主机名，如果设置为 IPv4，传递为字符串 */
            esp_mqtt_transport_t transport; /*!< 选择传输协议 */
            const char *path;               /*!< URI 中的路径 */
            uint32_t port;                  /*!< *MQTT* 服务器端口 */
        } address; /*!< 代理地址配置 */

        /**
         * 代理身份验证
         *
         * 如果字段未设置，则不验证代理的身份。建议为安全原因设置此结构中的选项。
         */
        struct verification_t {
            bool use_global_ca_store; /*!< 使用全局 ca_store，查看 esp-tls 文档了解详细信息。 */
            esp_err_t (*crt_bundle_attach)(void *conf); /*!< 指向 ESP x509 证书捆绑附加函数的指针，用于证书捆绑的使用。客户端只附加捆绑，清理必须由用户完成。 */
            const char *certificate; /*!< 证书数据，默认为 NULL。客户端不复制或释放，用户需要清理。 */
            size_t certificate_len; /*!< 指向证书的缓冲区的长度。 */
            const struct psk_key_hint *psk_hint_key; /*!< 指向 esp_tls.h 中定义的 PSK 结构的指针，以启用 PSK 认证（作为证书验证的替代）。仅在没有其他验证代理的方法时启用 PSK。客户端不复制或释放，用户需要清理。 */
            bool skip_cert_common_name_check; /*!< 跳过对服务器证书 CN 字段的任何验证，这会降低 TLS 的安全性，使 *MQTT* 客户端容易受到 MITM 攻击 */
            const char **alpn_protos;        /*!< 支持的应用程序协议的 NULL 终止列表，用于 ALPN。 */
            const char *common_name;         /*!< 指向包含服务器证书通用名称的字符串的指针。如果非 NULL，服务器证书 CN 必须与此名称匹配。如果 NULL，服务器证书 CN 必须与主机名匹配。如果 skip_cert_common_name_check=true，则忽略此项。客户端不复制或释放，用户需要清理。 */
        } verification; /*!< 代理的安全验证 */
    } broker; /*!< 代理地址和安全验证 */

    /**
     * 客户端相关的凭证用于身份验证。
     */
    struct credentials_t {
        const char *username;    /*!< *MQTT* 用户名 */
        const char *client_id;   /*!< 设置 *MQTT* 客户端标识符。如果 set_null_client_id = true，则忽略。如果 NULL，则设置默认客户端 ID。默认客户端 ID 为 ``ESP32_%CHIPID%``，其中 `%CHIPID%` 是 MAC 地址最后 3 个字节的十六进制格式 */
        bool set_null_client_id; /*!< 选择 NULL 客户端 ID */
        /**
         * 客户端认证
         *
         * 与客户端通过代理进行身份验证相关的字段
         *
         * 对于使用 TLS 的双向认证，用户可以选择证书和密钥、硬件安全模块或数字签名外设（如果可用）。
         *
         */
        struct authentication_t {
            const char *password;    /*!< *MQTT* 密码 */
            const char *certificate; /*!< 用于 SSL 双向认证的证书，如果不需要双向认证则不需要提供。必须提供 `key`。客户端不复制或释放，用户需要清理。 */
            size_t certificate_len;  /*!< 指向证书的缓冲区的长度。 */
            const char *key;       /*!< 用于 SSL 双向认证的私钥，如果不需要双向认证则不需要提供。如果不为 NULL，还必须提供 `certificate`。客户端不复制或释放，用户需要清理。 */
            size_t key_len; /*!< 指向密钥的缓冲区的长度。 */
            const char *key_password; /*!< 客户端密钥解密密码，不是 PEM 也不是 DER，如果提供 `key_password` 必须正确设置 `key_password_len`。 */
            int key_password_len;    /*!< 指向 `key_password` 的密码的长度 */
            bool use_secure_element; /*!< 启用安全元素，可在 ESP32-ROOM-32SE 中使用，用于 SSL 连接 */
            void *ds_data; /*!< 数字签名参数的承载体，数字签名外设在一些 Espressif 设备中可用。客户端不复制或释放，用户需要清理。 */
        } authentication; /*!< 客户端认证 */
    } credentials; /*!< 用户凭证用于代理 */

    /**
     * *MQTT* 会话相关配置
     */
    struct session_t {
        /**
         * 最后遗言消息配置。
         */
        struct last_will_t {
            const char *topic; /*!< LWT（最后遗言）消息主题 */
            const char *msg; /*!< LWT 消息，可以是 NULL 终止的 */
            int msg_len; /*!< LWT 消息长度，如果 msg 不是 NULL 终止的必须具有正确的长度 */
            int qos;     /*!< LWT 消息 QoS */
            int retain;  /*!< LWT 保留消息标志 */
        } last_will; /*!< 最后遗言配置 */
        bool disable_clean_session; /*!< *MQTT* 清理会话，默认 clean_session 为 true */
        int keepalive;              /*!< *MQTT* 保持连接时间，默认 120 秒
                                        配置此值时，请记住客户端尝试以实际设置的时间间隔的一半与代理通信。
                                        这种保守的方法允许在代理超时发生之前更多的尝试 */
        bool disable_keepalive; /*!< 设置 `disable_keepalive=true` 以关闭保持连接机制，默认启用保持连接。注意：设置 `keepalive` 值为 `0` 不会禁用保持连接功能，而是使用默认的保持连接周期 */
        esp_mqtt_protocol_ver_t protocol_ver; /*!< 用于连接的 *MQTT* 协议版本。*/
        int message_retransmit_timeout; /*!< 超时用于重新传输失败的数据包 */
    } session; /*!< *MQTT* 会话配置。 */

    /**
     * 网络相关配置
     */
    struct network_t {
        int reconnect_timeout_ms; /*!< 如果自动重连未禁用，在此值后重新连接到代理（默认为 10 秒） */
        int timeout_ms; /*!< 如果网络操作未在此值之后完成，则中止，单位为毫秒（默认为 10 秒）。 */
        int refresh_connection_after_ms; /*!< 在此值之后刷新连接（以毫秒为单位） */
        bool disable_auto_reconnect;     /*!< 客户端将重新连接到服务器（出现错误/断开）。设置 `disable_auto_reconnect=true` 以禁用 */
        esp_transport_handle_t transport; /*!< 自定义传输句柄。警告：传输在客户端生命周期内应有效，并在 esp_mqtt_client_destroy 调用时销毁。 */
        struct ifreq * if_name; /*!< 数据传输的接口名称。未设置时使用默认接口 */
    } network; /*!< 网络配置 */

    /**
     * 客户端任务配置
     */
    struct task_t {
        int priority;   /*!< *MQTT* 任务优先级 */
        int stack_size; /*!< *MQTT* 任务堆栈大小 */
    } task; /*!< FreeRTOS 任务配置。*/

    /**
     * 客户端缓冲区大小配置
     *
     * 客户端有两个缓冲区，分别用于输入和输出。
     */
    struct buffer_t {
        int size;     /*!< *MQTT* 发送/接收缓冲区的大小 */
        int out_size; /*!< *MQTT* 输出缓冲区的大小。如果未定义，默认为由 ``buffer_size`` 定义的大小 */
    } buffer; /*!< 缓冲区大小配置。*/

    /**
     * 客户端发件箱配置选项。
     */
    struct outbox_config_t {
        uint64_t limit; /*!< 发件箱的大小限制（以字节为单位）。*/
    } outbox; /*!< 发件箱配置。 */
} esp_mqtt_client_config_t;


/**
 * @brief 初始化 MQTT 客户端
 *
 * 该函数根据提供的配置结构体初始化一个 MQTT 客户端实例。初始化成功后，返回的客户端句柄可用于后续的 MQTT 操作，如连接、订阅和发布消息等。
 *
 * @param[in] config 指向 `esp_mqtt_client_config_t` 结构体的指针，包含 MQTT 客户端的配置参数。这些参数包括代理的地址、身份验证信息、会话配置、网络配置等。
 *
 * @return 如果初始化成功，返回一个 `esp_mqtt_client_handle_t` 类型的句柄，用于表示创建的 MQTT 客户端实例；如果初始化失败，返回 `NULL`。
 *
 * @note
 * - 请确保 `config` 指向的配置结构体中的所有字段均已正确设置。
 * - 在使用完客户端句柄后，应该调用 `esp_mqtt_client_destroy` 函数来释放相关资源。
 * - `esp_mqtt_client_init` 不会启动 MQTT 客户端，启动操作需要调用 `esp_mqtt_client_start`。
 * - 错误处理：在实际应用中，应该检查函数返回值并处理可能的错误情况。
 */
esp_mqtt_client_handle_t esp_mqtt_client_init(const esp_mqtt_client_config_t *config);

/**
 * @brief 注册处理程序的通配符，用于匹配任何事件基。
 *
 * 此宏定义为 `NULL`，用于表示可以为任意事件基注册事件处理程序。当使用此宏作为 `event_base` 参数时，处理程序将处理所有事件基中的事件。
 */
#define ESP_EVENT_ANY_BASE     NULL

/**
 * @brief 注册处理程序的通配符，用于匹配任何事件 ID。
 *
 * 此宏定义为 `-1`，用于表示可以为任意事件 ID 注册事件处理程序。当使用此宏作为 `event_id` 参数时，处理程序将处理所有事件 ID 的事件。
 */
#define ESP_EVENT_ANY_ID       -1

/**
 * @brief 注册 MQTT 客户端事件处理函数
 *
 * 该函数用于注册一个事件处理函数，该函数将在指定的 MQTT 事件发生时被调用。可以通过该机制来处理各种 MQTT 事件，例如连接成功、消息到达、订阅成功等。
 *
 * @param[in] client 指向 MQTT 客户端句柄的指针，表示需要注册事件处理函数的 MQTT 客户端实例。
 * @param[in] event 需要处理的 MQTT 事件类型，使用 `esp_mqtt_event_id_t` 枚举值表示。
 * @param[in] event_handler 指向事件处理函数的指针。该函数将在指定的事件发生时被调用。事件处理函数的原型为 `void (*event_handler)(void *handler_arg, esp_mqtt_event_handle_t event_data)`。
 * @param[in] event_handler_arg 传递给事件处理函数的参数。事件处理函数可以使用该参数传递额外的数据或上下文。
 *
 * @return
 * - `ESP_OK`：事件处理函数注册成功。
 * - `ESP_ERR_INVALID_ARG`：提供的参数无效，例如 `client` 为 `NULL` 或 `event_handler` 为 `NULL`。
 * - `ESP_ERR_NO_MEM`：内存分配失败。
 * - 其他错误码：表示函数执行过程中发生了其他错误。
 *
 * @note
 * - 同一事件类型可以注册多个事件处理函数。事件处理函数会按照注册顺序被调用。
 * - 注册的事件处理函数将在事件发生时被调用，因此在函数内应避免长时间的阻塞操作。
 * - 如果需要注销事件处理函数，可以使用 `esp_event_handler_unregister` 函数。
 */
esp_err_t esp_mqtt_client_register_event(esp_mqtt_client_handle_t client, esp_mqtt_event_id_t event, esp_event_handler_t event_handler, void *event_handler_arg);


/**
 * @brief 启动 MQTT 客户端
 *
 * 该函数用于启动 MQTT 客户端的连接过程。调用此函数后，客户端将连接到配置中指定的 MQTT 代理服务器，并开始进行 MQTT 协议的通信。
 *
 * @param[in] client 指向 MQTT 客户端句柄的指针。此句柄是在调用 `esp_mqtt_client_init` 函数时创建的 MQTT 客户端实例。
 *
 * @return
 * - `ESP_OK`：MQTT 客户端成功启动并开始连接过程。
 * - `ESP_ERR_INVALID_ARG`：提供的 `client` 参数无效，例如 `client` 为 `NULL`。
 * - `ESP_ERR_INVALID_STATE`：客户端当前状态不允许启动，例如客户端已经在连接过程中或已连接。
 * - `ESP_ERR_NO_MEM`：内存分配失败。
 * - 其他错误码：表示函数执行过程中发生了其他错误。
 *
 * @note
 * - 启动客户端后，可以使用 `esp_mqtt_client_register_event` 注册事件处理函数，以处理连接成功、消息到达等事件。
 * - 启动客户端时，如果客户端已处于连接状态或连接过程中的状态，将会返回 `ESP_ERR_INVALID_STATE` 错误。
 * - 启动客户端可能需要一些时间来完成连接过程。可以使用事件处理函数来监控连接状态。
 */
esp_err_t esp_mqtt_client_start(esp_mqtt_client_handle_t client);

```
- 地址:
1. 通过 address 结构体的 uri 字段或者 hostname、transport 以及 port 的组合，可以设置服务器地址。也可以选择设置 path，该字段对 WebSocket 连接而言非常有用。

2. 基于 TCP 的 MQTT 示例：

`mqtt://mqtt.eclipseprojects.io`：基于 TCP 的 MQTT，默认端口 1883

`mqtt://mqtt.eclipseprojects.io:1884`：基于 TCP 的 MQTT，端口 1884

`mqtt://username:password@mqtt.eclipseprojects.io:1884`：基于 TCP 的 MQTT， 端口 1884，

- 代码示例：

```c
const esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = "mqtt://mqtt.eclipseprojects.io",
};
esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
esp_mqtt_client_start(client);
```


## 发布与订阅

1. 函数原型

```c

typedef enum esp_mqtt_event_id_t {
    MQTT_EVENT_ANY = -1, /*!< 任意事件，用于注册事件处理程序时捕获任何事件 */
    MQTT_EVENT_ERROR = 0, /*!< 发生错误事件，附加上下文信息：
                           - 连接返回代码
                           - esp_tls 错误句柄（如果支持） */
    MQTT_EVENT_CONNECTED, /*!< 成功连接事件，附加上下文信息：session_present 标志 */
    MQTT_EVENT_DISCONNECTED, /*!< 断开连接事件 */
    MQTT_EVENT_SUBSCRIBED, /*!< 订阅成功事件，附加上下文信息：
                            - msg_id 消息 ID
                            - error_handle 订阅失败时的 `error_type`
                            - data 指向 broker 响应的指针，用于检查错误
                            - data_len 此事件数据的长度 */
    MQTT_EVENT_UNSUBSCRIBED, /*!< 取消订阅事件，附加上下文信息：msg_id */
    MQTT_EVENT_PUBLISHED, /*!< 发布成功事件，附加上下文信息：msg_id */
    MQTT_EVENT_DATA, /*!< 收到数据事件，附加上下文信息：
                      - msg_id 消息 ID
                      - topic 收到的主题指针
                      - topic_len 主题长度
                      - data 收到的数据指针
                      - data_len 此事件的数据长度
                      - current_data_offset 当前数据偏移量
                      - total_data_len 接收的总数据长度
                      - retain 消息的 retain 标志
                      - qos 消息的 QoS 级别
                      - dup 消息的 dup 标志
                      注：对于较长的消息，可能会触发多个 MQTT_EVENT_DATA 事件。
                      在这种情况下，只有第一个事件包含主题指针和长度，其余事件仅包含当前数据长度和偏移量更新。 */
    MQTT_EVENT_BEFORE_CONNECT, /*!< 连接前事件 */
    MQTT_EVENT_DELETED, /*!< 删除消息事件，当消息未能在到期前发送并确认时触发，附加上下文信息：msg_id（已删除消息的 ID） */
    MQTT_USER_EVENT, /*!< 自定义事件，允许将任务排队到 MQTT 事件处理程序中。可以使用 `esp_mqtt_event_t` 类型中的所有字段来传递附加的上下文数据给处理程序 */
} esp_mqtt_event_id_t;

/**
 * @brief 发布MQTT消息到指定的主题。
 *
 * 该函数用于向指定的MQTT主题发布消息。消息可以根据指定的QoS（服务质量）级别和是否保留进行控制。
 *
 * @param client 指向MQTT客户端的句柄。该句柄是在调用 `esp_mqtt_client_init()` 函数时返回的。
 * @param topic  要发布消息的MQTT主题。主题是一个以斜杠分隔的字符串，表示消息的分类。
 * @param data   要发布的消息数据。可以是任何字符串类型的数据。
 * @param len    消息数据的长度。如果传递0，函数将自动计算消息数据的长度。
 * @param qos    服务质量级别（QoS）。值可以是0、1或2：
 *               - 0：最多一次传递（即不要求确认交付）。
 *               - 1：至少一次传递（即需要确认交付）。
 *               - 2：仅一次传递（即消息确保不会重复且交付成功）。
 * @param retain 是否保留消息。值可以是0或1：
 *               - 0：不保留消息。
 *               - 1：保留消息（即在新客户端订阅该主题时会接收到此消息）。
 *
 * @return 成功时返回消息的标识符（msg_id），失败时返回负值。
 *
 * @note 
 * - 如果QoS为0，则返回的msg_id为0，因为不需要确认交付。
 * - 保留消息可以确保当新的客户端订阅该主题时，仍然能够接收到最新的消息。
 */
int esp_mqtt_client_publish(esp_mqtt_client_handle_t client, const char *topic, const char *data, int len, int qos, int retain)

/**
 * @brief 订阅多个MQTT主题。
 *
 * 该函数用于订阅多个MQTT主题列表。订阅成功后，客户端将接收发布到这些主题的消息。
 *
 * @param client     指向MQTT客户端的句柄。该句柄是在调用 `esp_mqtt_client_init()` 函数时返回的。
 * @param topic_list 指向主题列表的指针。每个主题都包含一个主题名和对应的服务质量（QoS）级别。
 * @param size       要订阅的主题数量，即 `topic_list` 数组中的主题数量。
 *
 * @return 成功时返回消息的标识符（msg_id），失败时返回负值。
 *
 * @note
 * - 订阅的主题和对应的QoS级别由 `esp_mqtt_topic_t` 结构体表示。该结构体包含两个成员：`topic`（主题名）和 `qos`（服务质量级别）。
 * - QoS级别可以是0、1或2，表示服务质量的不同级别：
 *   - 0：最多一次传递。
 *   - 1：至少一次传递。
 *   - 2：仅一次传递。
 * - 如果 `size` 为0，则函数不会执行任何订阅操作。
 */
int esp_mqtt_client_subscribe_multiple(esp_mqtt_client_handle_t client,
                                       const esp_mqtt_topic_t *topic_list, int size);


/**
 * @brief 选择适当的订阅函数进行订阅的便利宏。
 *
 * 该宏根据传入的 `topic_type` 参数的类型来自动选择适当的订阅函数。 
 * 如果 `topic_type` 是 `char*` 类型（单个主题名），将调用 `esp_mqtt_client_subscribe_single` 函数。 
 * 如果 `topic_type` 是 `esp_mqtt_topic_t*` 类型（多个主题的数组），将调用 `esp_mqtt_client_subscribe_multiple` 函数。
 *
 * @note
 * - `esp_mqtt_client_subscribe_single` 的用法与之前的 `esp_mqtt_client_subscribe` 相同，详细信息请参见相关文档。
 *
 * @param client_handle    MQTT 客户端句柄，该句柄用于标识当前的 MQTT 客户端连接。
 * @param topic_type       订阅的主题类型。如果是单个主题，需为 `char*` 或 `const char*` 类型；
 *                         如果是多个主题，需为 `esp_mqtt_topic_t*` 类型。
 * @param qos_or_size      如果订阅的是单个主题，表示服务质量（QoS）级别；
 *                         如果订阅的是多个主题，表示订阅数组的大小。
 *
 * @return 成功时返回订阅消息的 `message_id`；
 *         失败时返回 -1；
 *         如果消息发送队列已满，返回 -2。
 *
 * 用法示例：
 * 
 * ```c
 * // 订阅单个主题
 * int msg_id = esp_mqtt_client_subscribe(client_handle, "/topic/qos1", 1);
 *
 * // 订阅多个主题
 * esp_mqtt_topic_t topics[] = {
 *     { "/topic/qos0", 0 },
 *     { "/topic/qos1", 1 },
 * };
 * msg_id = esp_mqtt_client_subscribe(client_handle, topics, 2);
 * ```
 *
 * @note
 * - 如果 `topic_type` 是 `char*` 或 `const char*`，则 `qos_or_size` 应表示单个主题的 QoS 级别。
 * - 如果 `topic_type` 是 `esp_mqtt_topic_t*`，则 `qos_or_size` 应表示 `esp_mqtt_topic_t` 数组的大小。
 */
#define esp_mqtt_client_subscribe(client_handle, topic_type, qos_or_size) _Generic((topic_type), \
      char *: esp_mqtt_client_subscribe_single, \
      const char *: esp_mqtt_client_subscribe_single, \
      esp_mqtt_topic_t*: esp_mqtt_client_subscribe_multiple \
    )(client_handle, topic_type, qos_or_size)


```

2. 一般事件都在回调中处理

- 回调函数示例:

```c
/**
 * @brief 处理MQTT事件的回调函数。
 *
 * 该函数是MQTT事件的事件处理程序，根据不同的事件ID执行相应的操作。
 *
 * @param handler_args 用户自定义参数，在注册事件处理程序时传递。
 * @param base 事件基，表示事件所属的事件循环。
 * @param event_id 事件ID，指示发生的具体MQTT事件类型。
 * @param event_data 指向事件数据的指针，根据不同的事件ID，`event_data` 可能包含不同的信息。
 *
 * @note 函数内部使用了`switch`语句来处理各种不同的MQTT事件，例如连接、订阅、取消订阅、发布、接收数据和错误处理等。
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    // 日志输出，显示事件来自的事件循环基和事件ID
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);

    // 将事件数据转换为MQTT事件句柄
    esp_mqtt_event_handle_t event = event_data;

    // 获取MQTT客户端句柄
    esp_mqtt_client_handle_t client = event->client;

    int msg_id;  // 用于存储发布和订阅消息的ID

    // 根据事件ID处理不同的MQTT事件
    switch ((esp_mqtt_event_id_t)event_id) {

    // 处理MQTT连接成功事件
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

        // 发布一条QoS 1的消息到主题"/topic/qos1"
        msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        // 订阅主题"/topic/qos0"和"/topic/qos1"
        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        // 取消订阅主题"/topic/qos1"
        msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
        ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
        break;

    // 处理MQTT断开连接事件
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    // 处理MQTT订阅成功事件
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);

        // 订阅成功后，发布一条消息到"/topic/qos0"
        msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;

    // 处理MQTT取消订阅事件
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    // 处理MQTT发布成功事件
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    // 处理MQTT接收到消息数据事件
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");

        // 打印接收到的主题和数据
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;

    // 处理MQTT错误事件
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");

        // 检查是否是TCP传输相关的错误，并打印相应的错误信息
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;

    // 处理其他未明确列出的事件
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}
```

3. 不依靠事件回调阻塞式操作:

- 调用 `esp_mqtt_client_publish` 函数来发布消息：

```c
int msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "hello world", 0, 1, 0);
if (msg_id != -1) {
    ESP_LOGI(TAG, "Message published successfully, msg_id=%d", msg_id);
} else {
    ESP_LOGE(TAG, "Failed to publish message");
}

```

- 调用 esp_mqtt_client_subscribe 来订阅主题:

```c
int msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
if (msg_id != -1) {
    ESP_LOGI(TAG, "Subscribed successfully, msg_id=%d", msg_id);
} else {
    ESP_LOGE(TAG, "Failed to subscribe");
}

```

- [MQTT示例](mqtt_example.c)