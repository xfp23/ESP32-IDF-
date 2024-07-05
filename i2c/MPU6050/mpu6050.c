#include "mpu6050.h" 
#include "mpu6050_hal.h"

#include "stddef.h"

/*==================================================================================================
*                                       GLOBAL INIT
==================================================================================================*/

Mpu6050_Config_t Mpu6050_DefaultConfig = {
    .u8TempDisable = FALSE,
    .eLpWakeCtrl = WAKE_CTRL_1P25HZ,
    .eClkSrc = CLK_SRC_8MHZ,
    .eAfsSel = AFSR_16,
    .eFsSel = FSR_2000,
    .eDlpfCfg = DLPF_CFG_260_256
};

static uint8_t u8FsSelCurrentVal = (uint8_t)FSR_250;
static uint8_t u8AfsSelCurrentVal = (uint8_t)AFSR_2;
static const double aFsSelVal[4U] = {131, 65.5, 32.8, 16.4};
static const double aAfsSelVal[4U] = {16384, 8192, 4096, 2048};

/*==================================================================================================
*                                       LOCAL FUNCTIONS
==================================================================================================*/

/* Read accelerometer's raw data */
static int16_t Mpu6050_GetAccelRawData(Mpu6050_Dev_t *pDev, Mpu6050_AccelRawData_t *AccelRawData)
{
    int16_t err = MPU6050_OK;
    uint8_t reg = REG_READ_ACCEL_DATA;
    uint8_t data[6U] = {0U};

    /* NULL parameter checking */
    if((NULL == pDev) || (NULL == AccelRawData))
    {
        err |= MPU6050_ERR;
    }
    else
    {
        err |= mpu6050_i2c_hal_read(pDev->u8I2cAddress, pDev->pI2cPort, &reg, data, 6U);

        if (MPU6050_OK == err)
        {
            AccelRawData->Accel_X_Raw = (int16_t)((data[0U] << MSB_8BIT_SHIFT) | data[1U]);
            AccelRawData->Accel_Y_Raw = (int16_t)((data[2U] << MSB_8BIT_SHIFT) | data[3U]);
            AccelRawData->Accel_Z_Raw = (int16_t)((data[4U] << MSB_8BIT_SHIFT) | data[5U]);
        }
    }

    return err;
}

/* Read gyroscope's raw data */
static int16_t Mpu6050_GetGyroRawData(Mpu6050_Dev_t *pDev, Mpu6050_GyroRawData_t *GyroRawData)
{
    int16_t err = MPU6050_OK;
    uint8_t reg = REG_READ_GYRO_DATA;
    uint8_t data[6U] = {0U};

    /* NULL parameter checking */
    if((NULL == pDev) || (NULL == GyroRawData))
    {
        err |= MPU6050_ERR;
    }
    else
    {
        err |= mpu6050_i2c_hal_read(pDev->u8I2cAddress, pDev->pI2cPort, &reg, data, 6U);

        if (MPU6050_OK == err)
        {
            GyroRawData->Gyro_X_Raw = (int16_t)((data[0U] << MSB_8BIT_SHIFT) | data[1U]);
            GyroRawData->Gyro_Y_Raw = (int16_t)((data[2U] << MSB_8BIT_SHIFT) | data[3U]);
            GyroRawData->Gyro_Z_Raw = (int16_t)((data[4U] << MSB_8BIT_SHIFT) | data[5U]);
        }
    }

    return err;
}

/* Read configuration from REG_PWR_MGMT_1 register */
static int16_t Mpu6050_ReadPwrMgmt1(Mpu6050_Dev_t *pDev, uint8_t *cfg)
{
    int16_t err = MPU6050_OK;
    uint8_t reg = REG_PWR_MGMT_1;
    uint8_t data = 0U;

    /* NULL parameter checking */
    if((NULL == pDev) || (NULL == cfg))
    {
        err |= MPU6050_ERR;
    }
    else
    {
        err |= mpu6050_i2c_hal_read(pDev->u8I2cAddress, pDev->pI2cPort, &reg, &data, 1U);

        if (MPU6050_OK == err)
        {
            *cfg = data;
        }
    }

    return err;
}

/* Read configuration from REG_PWR_MGMT_2 register */
static int16_t Mpu6050_ReadPwrMgmt2(Mpu6050_Dev_t *pDev, uint8_t *cfg)
{
    int16_t err = MPU6050_OK;
    uint8_t reg = REG_PWR_MGMT_2;
    uint8_t data = 0U;

    /* NULL parameter checking */
    if((NULL == pDev) || (NULL == cfg))
    {
        err |= MPU6050_ERR;
    }
    else
    {
        err |= mpu6050_i2c_hal_read(pDev->u8I2cAddress, pDev->pI2cPort, &reg, &data, 1U);

        if (MPU6050_OK == err)
        {
            *cfg = data;
        }
    }

    return err;
}

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/**
* @brief        Initialize device.
* @details      Load user config to device.
*
* @param[in]    pDev        Pointer to device handler.
* @param[in]    Config      Structure for initial/default device configuration.
*
* @return       int16_t     Return code.
*
*/
int16_t Mpu6050_Init(Mpu6050_Dev_t *pDev, const Mpu6050_Config_t *pConfig)
{
    int16_t err = MPU6050_OK;

    /* NULL parameter checking */
    if((NULL == pDev) || (NULL == pConfig))
    {
        err |= MPU6050_ERR;
        return err;
    }

    /* Startup time */
    mpu6050_i2c_hal_ms_delay(50);

    /* Perform device reset */
    err |= Mpu6050_Reset(pDev);

    mpu6050_i2c_hal_ms_delay(50);

    /* Set device to normal mode */
    err |= Mpu6050_SetPowerMode(pDev, PWR_MODE_NORMAL);

    mpu6050_i2c_hal_ms_delay(50);

    Mpu6050_AccelFsSel(pDev, pConfig->eAfsSel);
    
    Mpu6050_GyroFsSel(pDev, pConfig->eFsSel);

    mpu6050_i2c_hal_ms_delay(50);

    return err;
}

/*================================================================================================*/
/**
* @brief        Set power mode of the device.
* @details      Set power mode of the device (sleep/cycle/normal).
*
* @param[in]    pDev        Pointer to device handler.
* @param[in]    ePwrMode    Power mode.
*
* @return       int16_t     Return code.
*
*/
int16_t Mpu6050_SetPowerMode(Mpu6050_Dev_t *pDev, Mpu6050_PwrMode_t ePwrMode)
{
    int16_t err = MPU6050_OK;
    uint8_t reg = REG_PWR_MGMT_1;
    uint8_t cfg = 0U;
    uint8_t data[2U] = {0U};

    /* NULL parameter checking */
    if(NULL == pDev)
    {
        err |= MPU6050_ERR;
    }
    /* Parameter check */
    else if(ePwrMode > PWR_MODE_CYCLE)
    {
        err |= MPU6050_ERR;
    }
    else
    {
        err |= Mpu6050_ReadPwrMgmt1(pDev, &cfg);

        if (MPU6050_OK == err)
        {
            data[0] = reg;
            data[1] = cfg;

            switch(ePwrMode)
            {
                case PWR_MODE_NORMAL:
                    data[1] &= (~PWR_MODE_SLEEP_MASK & ~PWR_MODE_CYCLE_MASK);
                    break;
                case PWR_MODE_SLEEP:
                    data[1] |= PWR_MODE_SLEEP_MASK;
                    break;
                case PWR_MODE_CYCLE:
                    data[1] &= ~(PWR_MODE_SLEEP_MASK);
                    data[1] |= PWR_MODE_CYCLE_MASK;
                    break;
                default:
                    /* Invalid parameter */
                    return MPU6050_ERR;
            }

            err |= mpu6050_i2c_hal_write(pDev->u8I2cAddress, pDev->pI2cPort, data, 2U);
        }
    }

    return err;
}

/*================================================================================================*/
/**
* @brief        Reset device.
* @details      Perform device reset.
*
* @param[in]    pDev        Pointer to device handler.
*
* @return       int16_t     Return code.
*
*/
int16_t Mpu6050_Reset(Mpu6050_Dev_t *pDev)
{
    int16_t err = MPU6050_OK;
    uint8_t reg = REG_PWR_MGMT_1;
    uint8_t cfg = 0U;
    uint8_t data[2U] = {0U};

    /* NULL parameter checking */
    if(NULL == pDev)
    {
        err |= MPU6050_ERR;
    }
    else
    {
        err |= Mpu6050_ReadPwrMgmt1(pDev, &cfg);

        if (MPU6050_OK == err)
        {
            data[0] = reg;
            data[1] = cfg | DEV_RESET_MASK;

            err |= mpu6050_i2c_hal_write(pDev->u8I2cAddress, pDev->pI2cPort, data, 2U);
        }
    }

    return err;
}

/*================================================================================================*/
/**
* @brief        Clock select.
* @details      Select clock source.
*
* @param[in]    pDev        Pointer to device handler.
* @param[in]    eClkSrc     PClock source.
*
* @return       int16_t     Return code.
*
*/
int16_t Mpu6050_ClockSelect(Mpu6050_Dev_t *pDev, Mpu6050_ClkSrc_t eClkSrc)
{
    int16_t err = MPU6050_OK;
    uint8_t reg = REG_PWR_MGMT_1;
    uint8_t cfg = 0U;
    uint8_t data[2U] = {0U};

    /* NULL parameter checking */
    if(NULL == pDev)
    {
        err |= MPU6050_ERR;
    }
    /* Parameter check */
    else if((eClkSrc > CLK_SRC_STOP) || (eClkSrc == CLK_SRC_RESERVE))
    {
        err |= MPU6050_ERR;
    }
    else
    {
        err |= Mpu6050_ReadPwrMgmt1(pDev, &cfg);

        if (MPU6050_OK == err)
        {
            data[0] = reg;
            data[1] = cfg & ~CLKSEL_MASK;
            data[1] |= (uint8_t)eClkSrc;

            err |= mpu6050_i2c_hal_write(pDev->u8I2cAddress, pDev->pI2cPort, data, 2U);
        }
    }

    return err;
}

/*================================================================================================*/
/**
* @brief        Full scale range setting.
* @details      Configure the accelerometer's full scale range.
*
* @param[in]    pDev        Pointer to device handler.
* @param[in]    eAfsSel     Accelerometer full scale range value.
*
* @return       int16_t     Return code.
*
*/
int16_t Mpu6050_AccelFsSel(Mpu6050_Dev_t *pDev, Mpu6050_AfsSel_t eAfsSel)
{
    int16_t err = MPU6050_OK;
    uint8_t reg = REG_ACCEL_CONFIG;
    uint8_t cfg = 0U;
    uint8_t data[2U] = {0U};

    /* NULL parameter checking */
    if(NULL == pDev)
    {
        err |= MPU6050_ERR;
    }
    /* Parameter check */
    else if(eAfsSel > AFSR_16)
    {
        err |= MPU6050_ERR;
    }
    else
    {
        err |= mpu6050_i2c_hal_read(pDev->u8I2cAddress, pDev->pI2cPort, &reg, &cfg, 1U);

        if (MPU6050_OK == err)
        {
            data[0] = reg;
            data[1] = cfg & ~ACCEL_FS_SEL_MASK;
            data[1] |= ((uint8_t)eAfsSel << ACCEL_FS_SEL_SHIFT);

            err |= mpu6050_i2c_hal_write(pDev->u8I2cAddress, pDev->pI2cPort, data, 2U);

            /* Confirm acceleration FsSel value and store it in the variable */
            cfg = 0U;
            err |= mpu6050_i2c_hal_read(pDev->u8I2cAddress, pDev->pI2cPort, &reg, &cfg, 1U);

            if (MPU6050_OK == err)
            {
                u8AfsSelCurrentVal = (cfg & ACCEL_FS_SEL_MASK) >> ACCEL_FS_SEL_SHIFT;
            }
        }
    }

    return err;
}

/*================================================================================================*/
/**
* @brief        Full scale range setting.
* @details      Configure the gyroscope's full scale range.
*
* @param[in]    pDev        Pointer to device handler.
* @param[in]    eFsSel      Gyroscope full scale range value.
*
* @return       int16_t     Return code.
*
*/
int16_t Mpu6050_GyroFsSel(Mpu6050_Dev_t *pDev, Mpu6050_FsSel_t eFsSel)
{
    int16_t err = MPU6050_OK;
    uint8_t reg = REG_GYRO_CONFIG;
    uint8_t cfg = 0U;
    uint8_t data[2U] = {0U};

    /* NULL parameter checking */
    if(NULL == pDev)
    {
        err |= MPU6050_ERR;
    }
    /* Parameter check */
    else if(eFsSel > FSR_2000)
    {
        err |= MPU6050_ERR;
    }
    else
    {
        err |= mpu6050_i2c_hal_read(pDev->u8I2cAddress, pDev->pI2cPort, &reg, &cfg, 1U);

        if (MPU6050_OK == err)
        {
            data[0] = reg;
            data[1] = cfg & ~GYRO_FS_SEL_MASK;
            data[1] |= ((uint8_t)eFsSel << GYRO_FS_SEL_SHIFT);

            err |= mpu6050_i2c_hal_write(pDev->u8I2cAddress, pDev->pI2cPort, data, 2U);

            /* Confirm gyroscope FsSel value and store it in the variable */
            cfg = 0U;
            err |= mpu6050_i2c_hal_read(pDev->u8I2cAddress, pDev->pI2cPort, &reg, &cfg, 1U);

            if (MPU6050_OK == err)
            {
                u8FsSelCurrentVal = (cfg & GYRO_FS_SEL_MASK) >> GYRO_FS_SEL_SHIFT;
            }
        }
    }

    return err;
}

/*================================================================================================*/
/**
* @brief        Configures the DLPF setting.
* @details      Configures the digital low pass filter setting.
*
* @param[in]    pDev        Pointer to device handler.
* @param[in]    eDlpfCfg    Digital low pass filter setting value.
*
* @return       int16_t     Return code.
*
*/
int16_t Mpu6050_DlpfConfig(Mpu6050_Dev_t *pDev, Mpu6050_DlpfCfg_t eDlpfCfg)
{
    int16_t err = MPU6050_OK;
    uint8_t reg = REG_CONFIG;
    uint8_t cfg = 0U;
    uint8_t data[2] = {0U};

    /* Parameter check */
    if(eDlpfCfg > DLPF_CFG_5_5)
    {
        err |= MPU6050_ERR;
    }
    else
    {
        err |= mpu6050_i2c_hal_read(pDev->u8I2cAddress, pDev->pI2cPort, &reg, &cfg, 1U);

        if (MPU6050_OK == err)
        {
            data[0] = reg;
            data[1] = cfg & ~DLPF_CFG_MASK;
            data[1] |= (uint8_t)eDlpfCfg;

            err |= mpu6050_i2c_hal_write(pDev->u8I2cAddress, pDev->pI2cPort, data, 2U);
        }
    }

    return err;
}

/*================================================================================================*/
/**
* @brief        Low power wake control.
* @details      Specifies the frequency of wake-ups during Accelerometer Only Low PowerMode.
*
* @param[in]    pDev        Pointer to device handler.
* @param[in]    eLpWakeCtrl Wake control frequency (Hz).
*
* @return       int16_t     Return code.
*
*/
int16_t Mpu6050_LpWakeCtrl(Mpu6050_Dev_t *pDev, Mpu6050_LpWakeCtrl_t eLpWakeCtrl)
{
    int16_t err = MPU6050_OK;
    uint8_t reg = REG_PWR_MGMT_1;
    uint8_t cfg = 0U;
    uint8_t data[2U] = {0U};

    /* NULL parameter checking */
    if(NULL == pDev)
    {
        err |= MPU6050_ERR;
    }
    /* Parameter check */
    else if(eLpWakeCtrl > WAKE_CTRL_40HZ)
    {
        err |= MPU6050_ERR;
    }
    else
    {
        err |= Mpu6050_ReadPwrMgmt2(pDev, &cfg);

        if (MPU6050_OK == err)
        {
            data[0] = reg;
            data[1] = cfg | ((uint8_t)eLpWakeCtrl | LP_WAKE_CTRL_MASK);

            err |= mpu6050_i2c_hal_write(pDev->u8I2cAddress, pDev->pI2cPort, data, 2U);
        }
    }

    return err;
}

/*================================================================================================*/
/**
* @brief        Get accelerometer data.
* @details      Get 3 axis accelerometer data.
*
* @param[in]    pDev        Pointer to device handler.
* @param[in]    AccelData   Pointer where the data will be stored.
*
* @return       int16_t     Return code.
*
*/
int16_t Mpu6050_GetAccelData(Mpu6050_Dev_t *pDev, Mpu6050_AccelData_t *AccelData)
{
    int16_t err = MPU6050_OK;
    Mpu6050_AccelRawData_t AccelRawData;

    /* NULL parameter checking */
    if(NULL == pDev || (NULL == AccelData))
    {
        err |= MPU6050_ERR;
    }
    else
    {
        err |= Mpu6050_GetAccelRawData(pDev, &AccelRawData);

        if (MPU6050_OK == err)
        {
            AccelData->Accel_X = (double)((double)AccelRawData.Accel_X_Raw * ACCEL_GRAVITY) / aAfsSelVal[u8AfsSelCurrentVal];
            AccelData->Accel_Y = (double)((double)AccelRawData.Accel_Y_Raw * ACCEL_GRAVITY) / aAfsSelVal[u8AfsSelCurrentVal];
            AccelData->Accel_Z = (double)((double)AccelRawData.Accel_Z_Raw * ACCEL_GRAVITY) / aAfsSelVal[u8AfsSelCurrentVal];
        }
    }

    return err;
}

/*================================================================================================*/
/**
* @brief        Get gyroscope data.
* @details      Get 3 axis gyroscope data.
*
* @param[in]    pDev        Pointer to device handler.
* @param[in]    GyroData    Pointer where the data will be stored.
*
* @return       int16_t     Return code.
*
*/
int16_t Mpu6050_GetGyroData(Mpu6050_Dev_t *pDev, Mpu6050_GyroData_t *GyroData)
{
    int16_t err = MPU6050_OK;
    Mpu6050_GyroRawData_t GyroRawData;

    /* NULL parameter checking */
    if(NULL == pDev || (NULL == GyroData))
    {
        err |= MPU6050_ERR;
    }
    else
    {
        err |= Mpu6050_GetGyroRawData(pDev, &GyroRawData);

        if (MPU6050_OK == err)
        {
            GyroData->Gyro_X = (double)GyroRawData.Gyro_X_Raw / aFsSelVal[u8FsSelCurrentVal];
            GyroData->Gyro_Y = (double)GyroRawData.Gyro_Y_Raw / aFsSelVal[u8FsSelCurrentVal];
            GyroData->Gyro_Z = (double)GyroRawData.Gyro_Z_Raw / aFsSelVal[u8FsSelCurrentVal];
        }
    }

    return err;
}

/*================================================================================================*/
/**
* @brief        Read device ID.
* @details      Read 8bit device id.
*
* @param[in]    pDev        Pointer to device handler.
* @param[in]    pId         Pointer to which the data will be stored.
*
* @return       int16_t     Return code.
*
*/
int16_t Mpu6050_GetDevideId(Mpu6050_Dev_t *pDev, uint8_t *pId)
{
    int16_t err = MPU6050_OK;
    uint8_t reg = REG_WHO_AM_I;
    uint8_t data = 0U;

    /* NULL parameter checking */
    if((NULL == pDev) || (NULL == pId))
    {
        err |= MPU6050_ERR;
    }
    else
    {
        err |= mpu6050_i2c_hal_read(pDev->u8I2cAddress, pDev->pI2cPort, &reg, &data, 1U);

        if (MPU6050_OK == err)
        {
            *pId = (data & WHO_AM_I_MASK);
        }
    }

    return err;
}