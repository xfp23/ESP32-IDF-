/**
 * @brief I2C端口号，可以是I2C_NUM_0 ~ (I2C_NUM_MAX-1)。
 */
typedef enum {
    I2C_NUM_0 = 0,              /*!< I2C端口0 */
#if SOC_I2C_NUM >= 2
    I2C_NUM_1,                  /*!< I2C端口1 */
#endif /* SOC_I2C_NUM >= 2 */
#if SOC_LP_I2C_NUM >= 1
    LP_I2C_NUM_0,               /*!< LP_I2C端口0 */
#endif /* SOC_LP_I2C_NUM >= 1 */
    I2C_NUM_MAX,                /*!< I2C端口最大值 */
} i2c_port_t;

/**
 * @brief I2C设备地址位长度的枚举
 */
typedef enum {
    I2C_ADDR_BIT_LEN_7 = 0,       /*!< I2C地址位长度7 */
#if SOC_I2C_SUPPORT_10BIT_ADDR
    I2C_ADDR_BIT_LEN_10 = 1,      /*!< I2C地址位长度10 */
#endif
} i2c_addr_bit_len_t;

/**
 * @brief 用于计算I2C总线定时的数据结构。
 */
typedef struct {
    uint16_t clkm_div;          /*!< I2C核心时钟分频器 */
    uint16_t scl_low;           /*!< I2C SCL低电平周期 */
    uint16_t scl_high;          /*!< I2C SCL高电平周期 */
    uint16_t scl_wait_high;     /*!< I2C SCL等待高电平周期 */
    uint16_t sda_hold;          /*!< I2C SDA保持时间 */
    uint16_t sda_sample;        /*!< I2C SDA采样时间 */
    uint16_t setup;             /*!< I2C启动和停止条件的设置周期 */
    uint16_t hold;              /*!< I2C启动和停止条件的保持周期 */
    uint16_t tout;              /*!< I2C总线超时时间 */
} i2c_hal_clk_config_t;

typedef enum {
#if SOC_I2C_SUPPORT_SLAVE
    I2C_MODE_SLAVE = 0,   /*!< I2C从模式 */
#endif
    I2C_MODE_MASTER,      /*!< I2C主模式 */
    I2C_MODE_MAX,
} i2c_mode_t;

typedef enum {
    I2C_MASTER_WRITE = 0,   /*!< I2C写数据 */
    I2C_MASTER_READ,        /*!< I2C读数据 */
} i2c_rw_t;

typedef enum {
    I2C_DATA_MODE_MSB_FIRST = 0,  /*!< I2C数据MSB先 */
    I2C_DATA_MODE_LSB_FIRST = 1,  /*!< I2C数据LSB先 */
    I2C_DATA_MODE_MAX
} i2c_trans_mode_t;

__attribute__((deprecated("请使用 'i2c_addr_bit_len_t' 代替")))
typedef enum {
    I2C_ADDR_BIT_7 = 0,    /*!< I2C 7位地址用于从模式 */
    I2C_ADDR_BIT_10,       /*!< I2C 10位地址用于从模式 */
    I2C_ADDR_BIT_MAX,
} i2c_addr_mode_t;

typedef enum {
    I2C_MASTER_ACK = 0x0,        /*!< 每个字节读后发送ACK */
    I2C_MASTER_NACK = 0x1,       /*!< 每个字节读后发送NACK */
    I2C_MASTER_LAST_NACK = 0x2,  /*!< 最后一个字节读后发送NACK */
    I2C_MASTER_ACK_MAX,
} i2c_ack_type_t;

/**
 * @brief I2C从设备延长SCL低电平的原因枚举
 */
typedef enum {
    I2C_SLAVE_STRETCH_CAUSE_ADDRESS_MATCH = 0,   /*!< 当从设备被主设备读取且地址匹配时延长SCL低电平 */
    I2C_SLAVE_STRETCH_CAUSE_TX_EMPTY = 1,        /*!< 从模式下TX FIFO为空时延长SCL低电平 */
    I2C_SLAVE_STRETCH_CAUSE_RX_FULL = 2,         /*!< 从模式下RX FIFO已满时延长SCL低电平 */
    I2C_SLAVE_STRETCH_CAUSE_SENDING_ACK = 3,     /*!< 从设备发送ACK时延长SCL低电平 */
} i2c_slave_stretch_cause_t;

/*
 * @brief 时序配置结构体。用于内部 I2C 复位。
 */
typedef struct {
    int high_period; /*!< 高电平时间 */
    int low_period; /*!< 低电平时间 */
    int wait_high_period; /*!< 等待高电平时间 */
    int rstart_setup; /*!< 重启设置时间 */
    int start_hold; /*!< 启动保持时间 */
    int stop_setup; /*!< 停止设置时间 */
    int stop_hold; /*!< 停止保持时间 */
    int sda_sample; /*!< SDA 采样时间 */
    int sda_hold; /*!< SDA 保持时间 */
    int timeout; /*!< 超时值 */
} i2c_hal_timing_config_t;