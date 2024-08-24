#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

void app_main(void)
{
    // 初始化 NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // 如果 NVS 分区被截断并需要擦除
        // 则重新尝试初始化 nvs_flash
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    // 检查初始化过程中的错误
    ESP_ERROR_CHECK( err );

    // 打开 NVS 句柄
    printf("\n");
    printf("正在打开非易失性存储 (NVS) 句柄... ");
    nvs_handle_t my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("打开 NVS 句柄时出错 (%s)!\n", esp_err_to_name(err));
    } else {
        printf("完成\n");

        // 读取存储的值
        printf("正在从 NVS 中读取重启计数器 ... ");
        int32_t restart_counter = 0; // 如果 NVS 中没有设置值，默认值为 0
        err = nvs_get_i32(my_handle, "restart_counter", &restart_counter);
        switch (err) {
            case ESP_OK:
                printf("完成\n");
                printf("重启计数器 = %" PRIu32 "\n", restart_counter);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("该值尚未初始化！\n");
                break;
            default :
                printf("读取时出错 (%s)!\n", esp_err_to_name(err));
        }

        // 写入新值
        printf("正在更新 NVS 中的重启计数器 ... ");
        restart_counter++;
        err = nvs_set_i32(my_handle, "restart_counter", restart_counter);
        printf((err != ESP_OK) ? "失败！\n" : "完成\n");

        // 提交写入的值。
        // 设置任何值后，必须调用 nvs_commit() 以确保更改已写入
        // 到闪存存储。实现可能会在其他时间写入存储，
        // 但这不保证一定会发生。
        printf("正在将更新提交到 NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "失败！\n" : "完成\n");

        // 关闭 NVS 句柄
        nvs_close(my_handle);
    }

    printf("\n");

    // 重启模块
    for (int i = 10; i >= 0; i--) {
        printf("将在 %d 秒后重启...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("现在重启。\n");
    fflush(stdout);
    esp_restart();
}
