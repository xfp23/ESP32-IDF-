#ifndef MPU6050_HAL
#define MPU6050_HAL

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

/* Hardware Specific Components */
#include "driver/i2c.h"
#include "esp_err.h"

#define MPU6050_ERR     ((int16_t)-1)
#define MPU6050_OK      ((int16_t)0x00)

/**
* @brief        I2c initialization.
* @details      I2c initialization.
*
* @return       int16_t     Return code.
*
*/
int16_t mpu6050_hal_init(uint8_t pI2cPort);

/**
* @brief        Execute I2C read.
* @details      Execute I2C read sequence.
*
* @param[in]    address     Device i2c address.
* @param[in]    reg         Register address.
* @param[in]    count       Number of bytes to read.
* @param[out]   pRxBuffer   Pointer to which data will be stored.
*
* @return       int16_t     Return code.
*
*/
int16_t mpu6050_i2c_hal_read(const uint8_t address, uint8_t pI2cPort, uint8_t *reg, uint8_t *pRxBuffer, const uint16_t count);

/**
* @brief        Execute I2C write.
* @details      Execute I2C write sequence.
*
* @param[in]    address     Device i2c address.
* @param[in]    count       Number of bytes to read.
* @param[out]   pTxBuffer   Pointer to data that will be written.
*
* @return       int16_t     Return code.
*
*/
int16_t mpu6050_i2c_hal_write(const uint8_t address, uint8_t pI2cPort, uint8_t *pTxBuffer, const uint16_t count);

/**
* @brief        Execute ms delay.
* @details      Execute ms delay for hal usage.
*
* @param[in]    ms      Time in milliseconds.
*
* @return       void
*
*/
void mpu6050_i2c_hal_ms_delay(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* MPU6050_HAL */