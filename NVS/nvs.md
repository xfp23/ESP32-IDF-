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

#include "nvs_flash.h"
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

2. 打开nvs分区

- 数据类型:

```c
//句柄类型:
typedef uint32_t nvs_handle_t;

typedef enum {
	NVS_READONLY,  /*!< 只读 */
	NVS_READWRITE  /*!< 读写 */
} nvs_open_mode_t;

esp_err_t nvs_open(const char* namespace_name, nvs_open_mode_t open_mode, nvs_handle_t *out_handle)
```

***打开nvs分区后，使用绑定的句柄操作***

- 代码示例:

```c
err = nvs_open("storage", NVS_READWRITE, &my_handle);
```

3. 读:

```c
/**
 * @brief 从 NVS（非易失性存储器）中获取一个 `int32_t` 类型的值
 *
 * 这个函数用于从指定的命名空间中获取与键（key）相关联的 `int32_t` 类型的值，并将其存储到 `out_value` 指针指向的变量中。
 *
 * @param c_handle nvs_handle_t 类型的句柄，表示已经打开的命名空间。
 * @param key      表示要获取的键值的名称（字符串），它必须是一个以空字符结尾的字符串（最多15个字符）。
 * @param out_value 指向 `int32_t` 类型变量的指针，用于存储从 NVS 中获取的值。
 *
 * @return
 *     - ESP_OK: 表示成功获取到值并存储在 `out_value` 中。
 *     - ESP_ERR_NVS_NOT_FOUND: 表示指定的键在 NVS 中不存在。
 *     - ESP_ERR_NVS_INVALID_HANDLE: 表示传入的句柄无效（例如句柄已关闭或为 NULL）。
 *     - ESP_ERR_NVS_INVALID_NAME: 表示键名不符合 NVS 规范（例如长度超过允许的最大值）。
 *     - ESP_ERR_NVS_INVALID_LENGTH: 表示数据长度不匹配（在这里应该不会发生，因为此函数专门获取 `int32_t` 类型的值）。
 *     - 其他错误代码：表示在读取过程中可能发生的其他错误（如 NVS 未初始化、类型不匹配等）。
 *
 * @note 在调用此函数之前，必须确保已通过 `nvs_open` 成功打开了命名空间，并获得了有效的句柄。
 * @note 键值必须事先通过 `nvs_set_i32` 或其他适当的 `nvs_set_*` 函数存储在 NVS 中。
 */

esp_err_t nvs_get_i32 (nvs_handle_t c_handle, const char* key, int32_t* out_value)

```

- 使用示例:

```c
int32_t restart_counter = 0;// 如果尚未在 NVS 中設置，則值將預設為 0
nvs_get_i32(my_handle, "restart_counter", &restart_counter);
```

4. 写:

- 类型：

```c

/**
 * @brief 将一个 32 位整数值存储到 NVS 中
 *
 * 这个函数用于将指定的 32 位整数值 (`int32_t`) 存储到给定的键值对中。
 * 需要使用 `nvs_handle_t` 类型的句柄来指定命名空间，并使用一个键名来
 * 标识所存储的值。如果键已经存在，则会覆盖旧值。
 *
 * @param handle 已打开的 NVS 句柄，用于指定操作的命名空间。
 * @param key 用于标识存储值的键名，必须是一个以空字符结尾的字符串。
 * @param value 要存储的 `int32_t` 类型的整数值。
 *
 * @return
 *      - ESP_OK: 成功将值存储到 NVS 中。
 *      - ESP_ERR_NVS_INVALID_HANDLE: 句柄无效，可能未正确打开。
 *      - ESP_ERR_NVS_READ_ONLY: 命名空间是只读的，无法进行写入操作。
 *      - ESP_ERR_NVS_INVALID_NAME: 提供的键名无效，可能是空指针或太长。
 *      - ESP_ERR_NVS_INVALID_LENGTH: 数据长度不符合要求。
 *      - ESP_ERR_NVS_NOT_ENOUGH_SPACE: NVS 分区空间不足，无法存储新值。
 *      - ESP_ERR_NVS_REMOVE_FAILED: 删除现有值失败（若值已存在且需要覆盖）。
 *      - ESP_ERR_NVS_VALUE_TOO_LONG: 值的长度超出了 NVS 的限制。
 *      - ESP_ERR_NVS_NOT_FOUND: 没有找到所请求的条目（一般不会发生在 `nvs_set` 中）。
 *      - ESP_ERR_NVS_INTERNAL: NVS 内部发生了其他错误。
 */
esp_err_t nvs_set_i32(nvs_handle_t handle, const char* key, int32_t value);

/**
 * @brief 将已修改的 NVS 数据写入闪存
 *
 * 该函数用于将之前通过 `nvs_set_*` 系列函数设置的键值对，真正地写入到闪存中。
 * 在调用 `nvs_commit` 之前，数据只是保存在 RAM 中。调用此函数后，数据才会
 * 永久保存到 NVS 分区的闪存中。
 *
 * 如果在设置完键值对后未调用此函数，所做的更改将不会持久化，
 * 在设备重启后这些更改会丢失。
 *
 * @param c_handle 已打开的 NVS 句柄，用于指定操作的命名空间。
 *
 * @return
 *      - ESP_OK: 成功将数据写入闪存。
 *      - ESP_ERR_NVS_INVALID_HANDLE: 句柄无效，可能未正确打开。
 *      - ESP_ERR_NVS_NOT_INITIALIZED: NVS 库未初始化。
 *      - ESP_ERR_NVS_PART_NOT_FOUND: 未找到对应的 NVS 分区。
 *      - ESP_ERR_NVS_NOT_ENOUGH_SPACE: 闪存空间不足，无法将数据持久化。
 *      - ESP_ERR_NVS_INTERNAL: NVS 内部发生了其他错误。
 */
esp_err_t nvs_commit(nvs_handle_t c_handle);

```

- 代码示例:

```c
    err = nvs_set_i32(my_handle, "restart_counter", restart_counter);
    // 提交写入的值。
    // 设置任何值后，必须调用 nvs_commit() 以确保更改已写入
    // 到闪存存储。实现可能会在其他时间写入存储，
    // 但这不保证一定会发生。
    err = nvs_commit(my_handle);
```

5. 关闭

- 当所有读写完毕后，确保关闭打开的nvs分区

```c
 nvs_close(my_handle);

```
## 存入读取多数据

- 函数原型:
```c

/**
 * @brief 从 NVS 读取 Blob 数据
 *
 * @param c_handle 打开的 NVS 句柄，通过 `nvs_open` 函数获取。
 * @param key 数据项的键名，用于标识要读取的 Blob 数据。
 * @param out_value 指向用于存储读取数据的缓冲区的指针。如果该缓冲区为空，则仅返回 Blob 数据的大小。
 * @param length 指向 `size_t` 类型的指针，用于传递和接收 Blob 数据的大小。在调用时，它应指向存储 Blob 数据的缓冲区的大小；在返回时，它将包含实际读取的 Blob 数据的大小。
 *
 * @return
 * - `ESP_OK`：成功读取数据。
 * - `ESP_ERR_NVS_NOT_FOUND`：未找到指定键的数据。
 * - `ESP_ERR_NVS_INVALID_LENGTH`：提供的缓冲区大小不足以存储 Blob 数据。
 * - 其他错误码：读取过程中发生其他错误。
 *
 * @note
 * - 如果 `out_value` 为 `NULL`，`length` 将返回所需的 Blob 数据大小。
 * - 在读取之前，`length` 应初始化为 0，以便正确返回所需的大小。
 */
esp_err_t nvs_get_blob(nvs_handle_t c_handle, const char* key, void* out_value, size_t* length);

/**
 * @brief 向 NVS 写入 Blob 数据
 *
 * @param c_handle 打开的 NVS 句柄，通过 `nvs_open` 函数获取。
 * @param key 数据项的键名，用于标识要存储的 Blob 数据。
 * @param value 指向要存储的 Blob 数据的指针。
 * @param length Blob 数据的大小（以字节为单位）。
 *
 * @return
 * - `ESP_OK`：成功写入数据。
 * - `ESP_ERR_NVS_NO_FREE_PAGES`：NVS 分区没有足够的空间来存储数据。
 * - `ESP_ERR_NVS_NOT_ENOUGH_SPACE`：提供的 Blob 数据大小超出了可用空间。
 * - 其他错误码：写入过程中发生其他错误。
 *
 * @note
 * - 写入操作不会自动提交更改。要确保数据写入到闪存，必须调用 `nvs_commit`。
 */
esp_err_t nvs_set_blob(nvs_handle_t c_handle, const char* key, const void* value, size_t length);


```
- 使用示例:

```c
esp_err_t save_run_time(void)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // 打开 NVS 句柄
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // 读取 Blob 所需的内存大小
    size_t required_size = 0;  // 如果之前未设置，则默认值为 0
    err = nvs_get_blob(my_handle, "run_time", NULL, &required_size); //此处获取大小存储于required_size中
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

    // 如果有可用的 Blob，则读取之前保存的 Blob
    uint32_t* run_time = malloc(required_size + sizeof(uint32_t));
    if (required_size > 0) {
        err = nvs_get_blob(my_handle, "run_time", run_time, &required_size);
        if (err != ESP_OK) {
            free(run_time);
            return err;
        }
    }

    // 将新值写入 Blob，包括之前保存的 Blob（如果有）
    required_size += sizeof(uint32_t);
    run_time[required_size / sizeof(uint32_t) - 1] = xTaskGetTickCount() * portTICK_PERIOD_MS;
    err = nvs_set_blob(my_handle, "run_time", run_time, required_size);
    free(run_time);

    if (err != ESP_OK) return err;

    // 提交更改
    err = nvs_commit(my_handle);
    if (err != ESP_OK) return err;

    // 关闭 NVS 句柄
    nvs_close(my_handle);
    return ESP_OK;
}
```


# 扩展函数提供:

```c

//写函数:
extern "C" esp_err_t nvs_set_i8  (nvs_handle_t handle, const char* key, int8_t value);
extern "C" esp_err_t nvs_set_u8  (nvs_handle_t handle, const char* key, uint8_t value);
extern "C" esp_err_t nvs_set_i16 (nvs_handle_t handle, const char* key, int16_t value);
extern "C" esp_err_t nvs_set_u16 (nvs_handle_t handle, const char* key, uint16_t value);
extern "C" esp_err_t nvs_set_i32 (nvs_handle_t handle, const char* key, int32_t value);
extern "C" esp_err_t nvs_set_u32 (nvs_handle_t handle, const char* key, uint32_t value);
extern "C" esp_err_t nvs_set_i64 (nvs_handle_t handle, const char* key, int64_t value);
extern "C" esp_err_t nvs_set_u64 (nvs_handle_t handle, const char* key, uint64_t value);

//读函数:
extern "C" esp_err_t nvs_get_i8  (nvs_handle_t c_handle, const char* key, int8_t* out_value);
extern "C" esp_err_t nvs_get_u8  (nvs_handle_t c_handle, const char* key, uint8_t* out_value);
extern "C" esp_err_t nvs_get_i16 (nvs_handle_t c_handle, const char* key, int16_t* out_value);
extern "C" esp_err_t nvs_get_u16 (nvs_handle_t c_handle, const char* key, uint16_t* out_value);
extern "C" esp_err_t nvs_get_i32 (nvs_handle_t c_handle, const char* key, int32_t* out_value);
extern "C" esp_err_t nvs_get_u32 (nvs_handle_t c_handle, const char* key, uint32_t* out_value);
extern "C" esp_err_t nvs_get_i64 (nvs_handle_t c_handle, const char* key, int64_t* out_value);
extern "C" esp_err_t nvs_get_u64 (nvs_handle_t c_handle, const char* key, uint64_t* out_value);

```

# 代码示例

- [单值写入读取示例](nvs_onevalue.c)
- [大空间内容读写示例](nvs_oblovalue.c)
