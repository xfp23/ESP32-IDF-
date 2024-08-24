# ESP32-IDF- 基于5.2.2版本

**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息

© 2024 . 未经许可不得复制、修改或分发。 此文献为 **DXG工作室** 所有。

# 简介

- 非易失性存储 (NVS) 库主要用于在 flash 中存储键值格式的数据。

- NVS 库通过调用 `esp_partition` API 使用主 flash 的部分空间，即类型为 data 且子类型为 nvs 的所有分区。应用程序可调用 `nvs_open()` API 选择使用带有 nvs 标签的分区，也可以通过调用 `nvs_open_from_partition()` API 选择使用指定名称的任意分区

## 键值对

NVS 的操作对象为键值对，其中键是 ASCII 字符串，当前支持的最大键长为 15 个字符。值可以为以下几种类型：

- 整数型：uint8_t、int8_t、uint16_t、int16_t、uint32_t、int32_t、uint64_t 和 int64_t；

- 以 0 结尾的字符串；

- 可变长度的二进制数据 (BLOB)

***字符串值当前上限为 4000 字节，其中包括空终止符。BLOB 值上限为 508,000 字节或分区大小的 97.6% 减去 4000 字节，以较低值为准。***

- 键必须唯一。为现有的键写入新值时，会将旧的值及数据类型更新为写入操作指定的值和数据类型。
- 读取值时会执行数据类型检查。如果读取操作预期的数据类型与对应键的数据类型不匹配，则返回错误

## 命名空间

为了减少不同组件之间键名的潜在冲突，NVS 将每个键值对分配给一个命名空间。命名空间的命名规则遵循键名的命名规则，例如，最多可占 15 个字符。此外，单个 NVS 分区最多只能容纳 254 个不同的命名空间。命名空间的名称在调用 `nvs_open()` 或 `nvs_open_from_partition` 中指定，调用后将返回一个不透明句柄，用于后续调用 `nvs_get_*`、`nvs_set_*` 和 `nvs_commit` 函数。这样，一个句柄关联一个命名空间，键名便不会与其他命名空间中相同键名冲突。请注意，**不同 NVS 分区中具有相同名称的命名空间将被视为不同的命名空间。**

## 数据安全

nvs支持加密,请参考[NVS加密](nvslock.md)


# 环境配置

1. 配置cmake
```c
REQUIRES nvs_flash
```

2. 包含所需头文件

```c

#include "nvs.h"

```

# 编程指南

## 存入读取单个值

1. 初始化nvs分区

宏:
```c
#define ESP_ERR_NVS_BASE                    0x1100                     /*!< 错误代码的起始编号 */
#define ESP_ERR_NVS_NOT_INITIALIZED         (ESP_ERR_NVS_BASE + 0x01)  /*!< 存储驱动程序未初始化 */
#define ESP_ERR_NVS_NOT_FOUND               (ESP_ERR_NVS_BASE + 0x02)  /*!< 找不到请求的条目或命名空间尚不存在，并且模式为 NVS_READONLY */
#define ESP_ERR_NVS_TYPE_MISMATCH           (ESP_ERR_NVS_BASE + 0x03)  /*!< 设置或获取操作的类型与存储在 NVS 中的值的类型不匹配 */
#define ESP_ERR_NVS_READ_ONLY               (ESP_ERR_NVS_BASE + 0x04)  /*!< 存储句柄以只读方式打开 */
#define ESP_ERR_NVS_NOT_ENOUGH_SPACE        (ESP_ERR_NVS_BASE + 0x05)  /*!< 底层存储中没有足够的空间来保存值 */
#define ESP_ERR_NVS_INVALID_NAME            (ESP_ERR_NVS_BASE + 0x06)  /*!< 命名空间名称不符合约束 */
#define ESP_ERR_NVS_INVALID_HANDLE          (ESP_ERR_NVS_BASE + 0x07)  /*!< 句柄已关闭或为空 */
#define ESP_ERR_NVS_REMOVE_FAILED           (ESP_ERR_NVS_BASE + 0x08)  /*!< 值未更新，因为闪存写入操作失败。值已写入，但如果再次初始化 NVS 并且闪存操作不再失败，更新将完成。 */
#define ESP_ERR_NVS_KEY_TOO_LONG            (ESP_ERR_NVS_BASE + 0x09)  /*!< 键名太长 */
#define ESP_ERR_NVS_PAGE_FULL               (ESP_ERR_NVS_BASE + 0x0a)  /*!< 内部错误；nvs API 函数从不返回此错误 */
#define ESP_ERR_NVS_INVALID_STATE           (ESP_ERR_NVS_BASE + 0x0b)  /*!< 由于先前的错误，NVS 处于不一致的状态。请再次调用 nvs_flash_init 和 nvs_open，然后重试。 */
#define ESP_ERR_NVS_INVALID_LENGTH          (ESP_ERR_NVS_BASE + 0x0c)  /*!< 字符串或 blob 长度不足以存储数据 */
#define ESP_ERR_NVS_NO_FREE_PAGES           (ESP_ERR_NVS_BASE + 0x0d)  /*!< NVS 分区不包含任何空页。如果 NVS 分区被截断，可能会发生这种情况。擦除整个分区并再次调用 nvs_flash_init。 */
#define ESP_ERR_NVS_VALUE_TOO_LONG          (ESP_ERR_NVS_BASE + 0x0e)  /*!< 值不适合条目，或字符串或 blob 长度超过实现支持的长度 */
#define ESP_ERR_NVS_PART_NOT_FOUND          (ESP_ERR_NVS_BASE + 0x0f)  /*!< 在分区表中找不到指定名称的分区 */

#define ESP_ERR_NVS_NEW_VERSION_FOUND       (ESP_ERR_NVS_BASE + 0x10)  /*!< NVS 分区包含新格式的数据，并且无法被此版本的代码识别 */
#define ESP_ERR_NVS_XTS_ENCR_FAILED         (ESP_ERR_NVS_BASE + 0x11)  /*!< 写入 NVS 条目时 XTS 加密失败 */
#define ESP_ERR_NVS_XTS_DECR_FAILED         (ESP_ERR_NVS_BASE + 0x12)  /*!< 读取 NVS 条目时 XTS 解密失败 */
#define ESP_ERR_NVS_XTS_CFG_FAILED          (ESP_ERR_NVS_BASE + 0x13)  /*!< XTS 配置设置失败 */
#define ESP_ERR_NVS_XTS_CFG_NOT_FOUND       (ESP_ERR_NVS_BASE + 0x14)  /*!< 找不到 XTS 配置 */
#define ESP_ERR_NVS_ENCR_NOT_SUPPORTED      (ESP_ERR_NVS_BASE + 0x15)  /*!< 此版本不支持 NVS 加密 */
#define ESP_ERR_NVS_KEYS_NOT_INITIALIZED    (ESP_ERR_NVS_BASE + 0x16)  /*!< NVS 密钥分区未初始化 */
#define ESP_ERR_NVS_CORRUPT_KEY_PART        (ESP_ERR_NVS_BASE + 0x17)  /*!< NVS 密钥分区损坏 */
#define ESP_ERR_NVS_WRONG_ENCRYPTION        (ESP_ERR_NVS_BASE + 0x19)  /*!< NVS 分区标记为使用通用闪存加密进行加密。由于 NVS 加密方式不同，这是被禁止的。 */

#define ESP_ERR_NVS_CONTENT_DIFFERS         (ESP_ERR_NVS_BASE + 0x18)  /*!< 内部错误；nvs API 函数从不返回此错误。NVS 键与存储的内容不同 */

#define NVS_DEFAULT_PART_NAME               "nvs"   /*!< NVS 分区表中默认的分区名称 */

#define NVS_PART_NAME_MAX_SIZE              16   /*!< 分区名称的最大长度（不包括空终止符） */
#define NVS_KEY_NAME_MAX_SIZE               16   /*!< NVS 键名的最大长度（包括空终止符） */
#define NVS_NS_NAME_MAX_SIZE                NVS_KEY_NAME_MAX_SIZE /*!< NVS 命名空间名称的最大长度（包括空终止符） */


```

- 初始化示例
```c
    esp_err_t err = nvs_flash_init(); //初始化nvs分区
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) { //检查
        // 如果 NVS 分区已损坏，需要擦除并重新初始化
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
```