#include "mpu6050_hal.h" 

/* I2C User Defines */
#define I2C_MASTER_SCL_IO           GPIO_NUM_8      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           GPIO_NUM_9      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          100000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/**
* @brief        I2c initialization.
* @details      I2c initialization.
*
* @return       int16_t     Return code.
*
*/
int16_t mpu6050_hal_init(uint8_t pI2cPort)
{
    int16_t err = MPU6050_OK;

    /* User implementation here */

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,    //Disable this if I2C lines have pull up resistor in place
        .scl_pullup_en = GPIO_PULLUP_ENABLE,    //Disable this if I2C lines have pull up resistor in place
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config((i2c_port_t)pI2cPort, &conf);

    if(i2c_driver_install((i2c_port_t)pI2cPort, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0) == ESP_FAIL)
    {
        err = MPU6050_ERR;
    }

    return err;
}

/*================================================================================================*/
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
int16_t mpu6050_i2c_hal_read(const uint8_t address, uint8_t pI2cPort, uint8_t *reg, uint8_t *pRxBuffer, const uint16_t count)
{
    int16_t err = MPU6050_OK;

    /* User implementation here */

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, address << 1 | I2C_MASTER_WRITE, 1);
	i2c_master_write(cmd, reg, 1, true);
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, address << 1 | I2C_MASTER_READ, 1);
	i2c_master_read(cmd, pRxBuffer, count, I2C_MASTER_LAST_NACK);
	i2c_master_stop(cmd);
	if(i2c_master_cmd_begin((i2c_port_t)pI2cPort, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS) == ESP_FAIL)
    {
        err = MPU6050_ERR;
    }
	i2c_cmd_link_delete(cmd);

    return err;
}

/*================================================================================================*/
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
int16_t mpu6050_i2c_hal_write(const uint8_t address, uint8_t pI2cPort, uint8_t *pTxBuffer, const uint16_t count)
{
    int16_t err = MPU6050_OK;

    /* User implementation here */

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, 1);
    i2c_master_write(cmd, pTxBuffer, count, 1);
    i2c_master_stop(cmd);
    if(i2c_master_cmd_begin((i2c_port_t)pI2cPort, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS) == ESP_FAIL)
    {
        err = MPU6050_ERR;
    }
    i2c_cmd_link_delete(cmd);


    return err;
}

/*================================================================================================*/
/**
* @brief        Execute ms delay.
* @details      Execute ms delay for hal usage.
*
* @param[in]    ms      Time in milliseconds.
*
* @return       void
*
*/
void mpu6050_i2c_hal_ms_delay(uint32_t ms) {

    /* User implementation here */
    
    vTaskDelay(pdMS_TO_TICKS(ms));
    
}