#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"

#define STORAGE_NAMESPACE "storage"

#if CONFIG_IDF_TARGET_ESP32C3
#define BOOT_MODE_PIN GPIO_NUM_9 // ESP32-C3 的引脚定义
#else
#define BOOT_MODE_PIN GPIO_NUM_0 // 其他 ESP32 系列的引脚定义
#endif //CONFIG_IDF_TARGET_ESP32C3

/* 保存模块重启次数到 NVS
   首先读取并递增之前保存的值。
   如果过程出错，则返回错误。
 */
esp_err_t save_restart_counter(void)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // 打开 NVS
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // 读取重启计数器
    int32_t restart_counter = 0; // 如果 NVS 中未设置值，默认值为 0
    err = nvs_get_i32(my_handle, "restart_conter", &restart_counter);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

    // 写入新的重启计数器值
    restart_counter++;
    err = nvs_set_i32(my_handle, "restart_conter", restart_counter);
    if (err != ESP_OK) return err;

    // 提交更改
    err = nvs_commit(my_handle);
    if (err != ESP_OK) return err;

    // 关闭 NVS 句柄
    nvs_close(my_handle);
    return ESP_OK;
}

/* 保存新的运行时间到 NVS
   首先读取之前保存的时间表，
   然后将新值添加到表的末尾。
   如果过程出错，则返回错误。
 */
esp_err_t save_run_time(void)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // 打开 NVS
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // 读取 Blob 所需的内存空间大小
    size_t required_size = 0;  // 如果 NVS 中未设置值，默认值为 0
    err = nvs_get_blob(my_handle, "run_time", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;

    // 如果有之前保存的 Blob，则读取它
    uint32_t* run_time = malloc(required_size + sizeof(uint32_t));
    if (required_size > 0) {
        err = nvs_get_blob(my_handle, "run_time", run_time, &required_size);
        if (err != ESP_OK) {
            free(run_time);
            return err;
        }
    }

    // 写入新值，包括之前保存的 Blob（如果有的话）
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

/* 从 NVS 读取并打印重启计数器
   和保存的运行时间表。
   如果过程出错，则返回错误。
 */
esp_err_t print_what_saved(void)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // 打开 NVS
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // 读取重启计数器
    int32_t restart_counter = 0; // 如果 NVS 中未设置值，默认值为 0
    err = nvs_get_i32(my_handle, "restart_conter", &restart_counter);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
    printf("重启计数器 = %" PRIu32 "\n", restart_counter);

    // 读取运行时间 Blob
    size_t required_size = 0;  // 如果 NVS 中未设置值，默认值为 0
    err = nvs_get_blob(my_handle, "run_time", NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) return err;
    printf("运行时间:\n");
    if (required_size == 0) {
        printf("尚未保存任何数据！\n");
    } else {
        uint32_t* run_time = malloc(required_size);
        err = nvs_get_blob(my_handle, "run_time", run_time, &required_size);
        if (err != ESP_OK) {
            free(run_time);
            return err;
        }
        for (int i = 0; i < required_size / sizeof(uint32_t); i++) {
            printf("%d: %" PRIu32 "\n", i + 1, run_time[i]);
        }
        free(run_time);
    }

    // 关闭 NVS 句柄
    nvs_close(my_handle);
    return ESP_OK;
}


void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS 分区被截断，需要擦除
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // 打印 NVS 中保存的数据
    err = print_what_saved();
    if (err != ESP_OK) printf("读取数据时发生错误 (%s)!\n", esp_err_to_name(err));

    // 保存重启计数器
    err = save_restart_counter();
    if (err != ESP_OK) printf("保存重启计数器时发生错误 (%s)!\n", esp_err_to_name(err));

    // 配置 GPIO 引脚
    gpio_reset_pin(BOOT_MODE_PIN);
    gpio_set_direction(BOOT_MODE_PIN, GPIO_MODE_INPUT);

    /* 读取 GPIO0 的状态。如果 GPIO0 持续为低电平超过 1000 毫秒，
       则保存模块的运行时间并重启模块。
     */
    while (1) {
        if (gpio_get_level(BOOT_MODE_PIN) == 0) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            if (gpio_get_level(BOOT_MODE_PIN) == 0) {
                // 保存运行时间
                err = save_run_time();
                if (err != ESP_OK) printf("保存运行时间时发生错误 (%s)!\n", esp_err_to_name(err));
                printf("正在重启...\n");
                fflush(stdout);
                esp_restart(); // 重启系统
            }
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}
