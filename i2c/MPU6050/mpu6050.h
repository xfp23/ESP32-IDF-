#ifndef MPU5060_H
#define MPU5060_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mpu6050_hal.h"

typedef enum{
    DLPF_CFG_260_256 = 0x00U,
    DLPF_CFG_184_188 = 0x01U,
    DLPF_CFG_94_98 = 0x02U,
    DLPF_CFG_44_42 = 0x03U,
    DLPF_CFG_21_20 = 0x04U,
    DLPF_CFG_10_10 = 0x05U,
    DLPF_CFG_5_5 = 0x06U
} Mpu6050_DlpfCfg_t;

typedef enum{
    FSR_250 = 0x00U,
    FSR_500 = 0x01U,
    FSR_1000 = 0x02U,
    FSR_2000 = 0x03U
} Mpu6050_FsSel_t;

typedef enum{
    AFSR_2 = 0x00U,
    AFSR_4 = 0x01U,
    AFSR_8 = 0x02U,
    AFSR_16 = 0x03U
} Mpu6050_AfsSel_t;

typedef enum{
    PWR_MODE_NORMAL = 0x00U,
    PWR_MODE_SLEEP = 0x01U,
    PWR_MODE_CYCLE = 0x02U,
} Mpu6050_PwrMode_t;

typedef enum{
    CLK_SRC_8MHZ = 0x00U,
    CLK_SRC_PLL_X_GYRO_REF = 0x01U,
    CLK_SRC_PLL_Y_GYRO_REF = 0x02U,
    CLK_SRC_PLL_Z_GYRO_REF = 0x03U,
    CLK_SRC_PLL_32MHZ = 0x04U,
    CLK_SRC_PLL_19MHZ = 0x05U,
    CLK_SRC_RESERVE = 0x06U,
    CLK_SRC_STOP = 0x07U
} Mpu6050_ClkSrc_t;

typedef enum{
    WAKE_CTRL_1P25HZ = 0x00U,
    WAKE_CTRL_5HZ = 0x01U,
    WAKE_CTRL_20HZ = 0x02U,
    WAKE_CTRL_40HZ = 0x03U
} Mpu6050_LpWakeCtrl_t;

/**
 * @brief Accelerometer raw data
 */
typedef struct{
    int16_t Accel_X_Raw;     /** @brief Accel X raw data */
    int16_t Accel_Y_Raw;     /** @brief Accel Y raw data */
    int16_t Accel_Z_Raw;     /** @brief Accel Z raw data */
} Mpu6050_AccelRawData_t;

/**
 * @brief Accelerometer data
 */
typedef struct{
    double Accel_X;     /** @brief Accel X data */
    double Accel_Y;     /** @brief Accel Y data */
    double Accel_Z;     /** @brief Accel Z data */
} Mpu6050_AccelData_t;

/**
 * @brief Gyroscope raw data
 */
typedef struct{
    int16_t Gyro_X_Raw;     /** @brief Gyro X raw data */
    int16_t Gyro_Y_Raw;     /** @brief Gyro Y raw data */
    int16_t Gyro_Z_Raw;     /** @brief Gyro Z raw data */
} Mpu6050_GyroRawData_t;

/**
 * @brief Gyroscope data
 */
typedef struct{
    double Gyro_X;     /** @brief Gyro X data */
    double Gyro_Y;     /** @brief Gyro Y data */
    double Gyro_Z;     /** @brief Gyro Z data */
} Mpu6050_GyroData_t;

typedef struct{
    uint8_t u8TempDisable;
    Mpu6050_LpWakeCtrl_t eLpWakeCtrl;
    Mpu6050_ClkSrc_t eClkSrc;
    Mpu6050_AfsSel_t eAfsSel;
    Mpu6050_FsSel_t eFsSel;
    Mpu6050_DlpfCfg_t eDlpfCfg;
} Mpu6050_Config_t;

/**
 * @brief Driver handler type definitions.
 */
typedef struct{
    uint8_t pI2cPort;           /** @brief I2C port/instance */
    uint8_t u8I2cAddress;       /** @brief I2C address */
} Mpu6050_Dev_t;

extern Mpu6050_Config_t Mpu6050_DefaultConfig;

/**
 * @brief MPU6050 I2C slave address when pin AD0 is low
 */
#define I2C_ADDRESS_MPU5060_AD0_L       ((uint8_t)0x68U)

/**
 * @brief MPU6050 I2C slave address when pin AD0 is high
 */
#define I2C_ADDRESS_MPU5060_AD0_H       ((uint8_t)0x69U)

/**
 * @brief MPU6050 command code registers
 */
#define REG_CONFIG                      ((uint8_t)0x1AU)
#define REG_GYRO_CONFIG                 ((uint8_t)0x1BU)
#define REG_ACCEL_CONFIG                ((uint8_t)0x1CU)
#define REG_MOT_THR                     ((uint8_t)0x1FU)
#define REG_FIFO_EN                     ((uint8_t)0x23U)
#define REG_READ_ACCEL_DATA             ((uint8_t)0x3BU)
#define REG_READ_TEMP_DATA              ((uint8_t)0x41U)
#define REG_READ_GYRO_DATA              ((uint8_t)0x43U)
#define REG_PWR_MGMT_1                  ((uint8_t)0x6BU)
#define REG_PWR_MGMT_2                  ((uint8_t)0x6CU)
#define REG_WHO_AM_I                    ((uint8_t)0x75U)

/**
 * @brief Mask and shift macros
 */
#define FIFO_EN_MASK                    ((uint8_t)0x40U)
#define FIFO_RESET_MASK                 ((uint8_t)0x04U)
#define FIFO_TEMP_EN_MASK               ((uint8_t)0x80U)
#define FIFO_XG_EN_MASK                 ((uint8_t)0x40U)
#define FIFO_YG_EN_MASK                 ((uint8_t)0x20U)
#define FIFO_ZG_EN_MASK                 ((uint8_t)0x10U)
#define FIFO_ACCEL_EN_MASK              ((uint8_t)0x08U)
#define FIFO_SLV2_EN_MASK               ((uint8_t)0x04U)
#define FIFO_SLV1_EN_MASK               ((uint8_t)0x02U)
#define FIFO_SLV0_EN_MASK               ((uint8_t)0x01U)

#define GYRO_FS_SEL_MASK                ((uint8_t)0x18U)
#define GYRO_FS_SEL_SHIFT               ((uint8_t)0x03U)

#define ACCEL_FS_SEL_MASK               ((uint8_t)0x18U)
#define ACCEL_FS_SEL_SHIFT              ((uint8_t)0x03U)

#define DLPF_CFG_MASK                   ((uint8_t)0x07U)

/* PWR_MGMT_1 related macros */
#define DEV_RESET_MASK                  ((uint8_t)0x80U)
#define PWR_MODE_SLEEP_MASK             ((uint8_t)0x40U)
#define PWR_MODE_CYCLE_MASK             ((uint8_t)0x20U)
#define TEMP_DIS_MASK                   ((uint8_t)0x08)
#define CLKSEL_MASK                     ((uint8_t)0x07)

/* PWR_MGMT_2 related macros */
#define LP_WAKE_CTRL_MASK               ((uint8_t)0xC0U)
#define STBY_XA                         ((uint8_t)0x20U)
#define STBY_YA                         ((uint8_t)0x10U)
#define STBY_ZA                         ((uint8_t)0x08U)
#define STBY_XG                         ((uint8_t)0x04U)
#define STBY_YG                         ((uint8_t)0x02U)
#define STBY_ZG                         ((uint8_t)0x01U)

#define WHO_AM_I_MASK                   ((uint8_t)0x7EU)

#define MSB_8BIT_SHIFT                  ((uint8_t)0x08U)

/**
 * @brief Other macros
 */
#define WHO_AM_I_VAL                    ((uint8_t)0x68U)
#define TRUE                            ((uint8_t)0x01U)
#define FALSE                           ((uint8_t)0x00U)
#define ACCEL_GRAVITY                   ((double)9.81)

/*==================================================================================================
*                                       GLOBAL FUNCTIONS
==================================================================================================*/

/**
* @brief 初始化设备。
* @details 将用户配置加载到设备。
*
* @param[in] pDev 指向设备处理程序的指针。
* @param[in] 初始/默认设备配置的配置结构。
*
* @return int16_t 返回代码。
*
*/
int16_t Mpu6050_Init(Mpu6050_Dev_t *pDev, const Mpu6050_Config_t *pConfig);

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
int16_t Mpu6050_SetPowerMode(Mpu6050_Dev_t *pDev, Mpu6050_PwrMode_t ePwrMode);

/**
* @brief        Reset device.
* @details      Perform device reset.
*
* @param[in]    pDev        Pointer to device handler.
*
* @return       int16_t     Return code.
*
*/
int16_t Mpu6050_Reset(Mpu6050_Dev_t *pDev);

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
int16_t Mpu6050_ClockSelect(Mpu6050_Dev_t *pDev, Mpu6050_ClkSrc_t eClkSrc);

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
int16_t Mpu6050_AccelFsSel(Mpu6050_Dev_t *pDev, Mpu6050_AfsSel_t eAfsSel);

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
int16_t Mpu6050_GyroFsSel(Mpu6050_Dev_t *pDev, Mpu6050_FsSel_t eFsSel);

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
int16_t Mpu6050_DlpfConfig(Mpu6050_Dev_t *pDev, Mpu6050_DlpfCfg_t eDlpfCfg);

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
int16_t Mpu6050_LpWakeCtrl(Mpu6050_Dev_t *pDev, Mpu6050_LpWakeCtrl_t eLpWakeCtrl);

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
int16_t Mpu6050_GetAccelData(Mpu6050_Dev_t *pDev, Mpu6050_AccelData_t *AccelData);
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
int16_t Mpu6050_GetGyroData(Mpu6050_Dev_t *pDev, Mpu6050_GyroData_t *GyroData);

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
int16_t Mpu6050_GetDevideId(Mpu6050_Dev_t *pDev, uint8_t *pId);

#ifdef __cplusplus
}
#endif

#endif /* MPU6050_H */