#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* MPU6050 components */
#include "mpu6050.h"
#include "mpu6050_hal.h"

// 定义日志标签，用于标识日志输出来源
static const char *TAG = "example_usage";

// 定义 MPU6050 设备结构体，并设置 I2C 端口和 I2C 地址
Mpu6050_Dev_t Dev = {
    (uint8_t)I2C_NUM_0,                // I2C 端口号
    I2C_ADDRESS_MPU5060_AD0_L          // I2C 地址（当 AD0 引脚为低时）
};

// 主程序入口
void app_main(void)
{
    int16_t err = MPU6050_OK;          // 定义错误码变量并初始化为 OK
    uint8_t dev_id = 0;                // 定义设备 ID 变量
    Mpu6050_AccelData_t AccelData;     // 定义加速度数据结构体
    Mpu6050_GyroData_t GyroData;       // 定义陀螺仪数据结构体

    // 初始化 I2C 总线
    err |= mpu6050_hal_init(Dev.pI2cPort);

    // 初始化 MPU6050 设备
    err |= Mpu6050_Init(&Dev, &Mpu6050_DefaultConfig);

    // 获取 MPU6050 设备 ID
    err |= Mpu6050_GetDevideId(&Dev, &dev_id);

    // 检查是否成功获取设备 ID
    if(err == MPU6050_OK){
        ESP_LOGI(TAG, "Revision ID: 0x%02x", dev_id);  // 输出设备 ID
    } 
    else{
        ESP_LOGE(TAG, "Unable to read device ID!");    // 输出错误信息
    }

    // 检查初始化是否成功且设备 ID 是否正确
    if (err == MPU6050_OK && dev_id == WHO_AM_I_VAL)
    {
        ESP_LOGI(TAG, "MPU6050 initialization successful");  // 输出初始化成功信息

        // 无限循环，持续获取 MPU6050 数据
        while(1)
        {
            // 获取加速度数据
            err |= Mpu6050_GetAccelData(&Dev, &AccelData);
            // 获取陀螺仪数据
            err |= Mpu6050_GetGyroData(&Dev, &GyroData);

            // 检查数据读取是否成功
            if(err == MPU6050_OK)
            {
                // 输出加速度数据
                ESP_LOGI(TAG, "Accelerometer:");
                ESP_LOGI(TAG, "Linear Acceleration X = %.02fm/s^2", AccelData.Accel_X);
                ESP_LOGI(TAG, "Linear Acceleration Y = %.02fm/s^2", AccelData.Accel_Y);
                ESP_LOGI(TAG, "Linear Acceleration Z = %.02fm/s^2", AccelData.Accel_Z);

                // 输出陀螺仪数据
                ESP_LOGI(TAG, "Gyroscope:");
                ESP_LOGI(TAG, "Angular Velocity X = %.02f°/s", GyroData.Gyro_X);
                ESP_LOGI(TAG, "Angular Velocity Y = %.02f°/s", GyroData.Gyro_Y);
                ESP_LOGI(TAG, "Angular Velocity Z = %.02f°/s", GyroData.Gyro_Z);
            }
            else{
                ESP_LOGE(TAG, "Error reading data!");  // 输出数据读取错误信息
            }
            
            // 延时 1000 毫秒（1 秒）
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
    else{
        ESP_LOGE(TAG, "MPU6050 initialization failed!");  // 输出初始化失败信息
    }
}
