/**
 * @brief 安装 I2C 驱动程序
 * @note 并非所有 Espressif 芯片都支持从机模式（例如 ESP32C2）
 *
 * @param i2c_num I2C端口号
 * @param mode I2C 模式（主机或从机）。
 * @param slv_rx_buf_len 接收缓冲区大小。只有从机模式才会使用该值，在主机模式下会被忽略。
 * @param slv_tx_buf_len 发送缓冲区大小。只有从机模式才会使用该值，在主机模式下会被忽略。
 * @param intr_alloc_flags 用于分配中断的标志。一个或多个 (ORred) ESP_INTR_FLAG_* 值。
 * 请参阅 esp_intr_alloc.h 了解更多信息。
 *        @笔记
 * 在master模式下，如果cache很可能被禁用（比如写flash）并且slave对时间敏感，
 * 建议使用`ESP_INTR_FLAG_IRAM`。在这种情况下，请使用i2c读写函数中从内部RAM分配的内存，
 * 因为当缓存被禁用时，我们无法在中断处理函数中访问 psram（如果 psram 已启用）。
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 * - ESP_FAIL 驱动安装错误
 */
esp_err_t i2c_driver_install(i2c_port_t i2c_num, i2c_mode_t mode, size_t slv_rx_buf_len, size_t slv_tx_buf_len, int intr_alloc_flags);

/**
 * @brief 删除 I2C 驱动程序
 *
 * @note 该函数不保证线程安全。
 * 调用delete函数之前请确保没有线程会持续持有信号量。
 *
 * @param i2c_num 要删除的I2C端口
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_driver_delete(i2c_port_t i2c_num);

/**
 * @brief 使用给定的配置配置 I2C 总线。
 *
 * @param i2c_num 要配置的I2C端口
 * @param i2c_conf 指向 I2C 配置的指针
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_param_config(i2c_port_t i2c_num, const i2c_config_t *i2c_conf);

/**
 * @brief 重置 I2C tx 硬件 fifo
 *
 * @param i2c_num I2C端口号
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_reset_tx_fifo(i2c_port_t i2c_num);

/**
 * @brief 重置 I2C rx fifo
 *
 * @param i2c_num I2C端口号
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_reset_rx_fifo(i2c_port_t i2c_num);

/**
 * @brief 配置 I2C SCK 和 SDA 信号的 GPIO 引脚。
 *
 * @param i2c_num I2C端口号
 * @param sda_io_num I2C SDA 信号的 GPIO 编号
 * @param scl_io_num I2C SCL 信号的 GPIO 编号
 * @param sda_pullup_en 启用SDA引脚的内部上拉
 * @param scl_pullup_en 启用SCL引脚的内部上拉
 * @param mode I2C模式
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_set_pin(i2c_port_t i2c_num, int sda_io_num, int scl_io_num,
                      bool sda_pullup_en, bool scl_pullup_en, i2c_mode_t mode);

/**
 * @brief 对连接到特定 I2C 端口的设备执行写入操作。
 * 该函数是 `i2c_master_start()`、`i2c_master_write()`、`i2c_master_read()` 等的包装器...
 * 只能在 I2C 主模式下调用。
 *
 * @param i2c_num 用于执行传输的 I2C 端口号
 * @param device_address I2C设备的7位地址
 * @param write_buffer 在总线上发送的字节
 * @param write_size 写入缓冲区的大小（以字节为单位）
 * @paramticks_to_wait 在发出超时之前等待的最大滴答数。
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 * - ESP_FAIL 发送命令错误，从机尚未确认传输。
 * - ESP_ERR_INVALID_STATE I2C 驱动程序未安装或未处于主模式。
 * - ESP_ERR_TIMEOUT 由于总线繁忙而操作超时。
 */
esp_err_t i2c_master_write_to_device(i2c_port_t i2c_num, uint8_t device_address,
                                     const uint8_t* write_buffer, size_t write_size,
                                     TickType_t ticks_to_wait);

/**
 * @brief 对连接到特定 I2C 端口的设备执行读取。
 * 该函数是 `i2c_master_start()`、`i2c_master_write()`、`i2c_master_read()` 等的包装器...
 * 只能在 I2C 主模式下调用。
 *
 * @param i2c_num 用于执行传输的 I2C 端口号
 * @param device_address I2C设备的7位地址
 * @param read_buffer 存储总线上接收到的字节的缓冲区
 * @param read_size 读取缓冲区的大小（以字节为单位）
 * @paramticks_to_wait 在发出超时之前等待的最大滴答数。
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 * - ESP_FAIL 发送命令错误，从机尚未确认传输。
 * - ESP_ERR_INVALID_STATE I2C 驱动程序未安装或未处于主模式。
 * - ESP_ERR_TIMEOUT 由于总线繁忙而操作超时。
 */
esp_err_t i2c_master_read_from_device(i2c_port_t i2c_num, uint8_t device_address,
                                      uint8_t* read_buffer, size_t read_size,
                                      TickType_t ticks_to_wait);

/**
 * @brief 对 I2C 总线上的设备执行写入操作，然后执行读取操作。
 * 在“写”和“读”之间使用重复的启动信号，因此总线
 * 直到两笔交易完成后才释放。
 * 该函数是 `i2c_master_start()`、`i2c_master_write()`、`i2c_master_read()` 等的包装器...
 * 只能在 I2C 主模式下调用。
 *
 * @param i2c_num 用于执行传输的 I2C 端口号
 * @param device_address I2C设备的7位地址
 * @param write_buffer 在总线上发送的字节
 * @param write_size 写入缓冲区的大小（以字节为单位）
 * @param read_buffer 存储总线上接收到的字节的缓冲区
 * @param read_size 读取缓冲区的大小（以字节为单位）
 * @paramticks_to_wait 在发出超时之前等待的最大滴答数。
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 * - ESP_FAIL 发送命令错误，从机尚未确认传输。
 * - ESP_ERR_INVALID_STATE I2C 驱动程序未安装或未处于主模式。
 * - ESP_ERR_TIMEOUT 由于总线繁忙而操作超时。
 */
esp_err_t i2c_master_write_read_device(i2c_port_t i2c_num, uint8_t device_address,
                                       const uint8_t* write_buffer, size_t write_size,
                                       uint8_t* read_buffer, size_t read_size,
                                       TickType_t ticks_to_wait);


/**
 * @brief 使用给定的缓冲区创建并初始化 I2C 命令列表。
 * 数据或信号的所有分配（START、STOP、ACK...）将
 * 在此缓冲区内执行。
 * 该缓冲区必须在整个事务期间有效。
 * 完成 I2C 事务后，需要调用 `i2c_cmd_link_delete_static()`。
 *
 * @note **强烈**建议不要在堆栈上分配此缓冲区。数据大小
 * 下面使用的将来可能会增加，导致可能的堆栈溢出作为宏
 * `I2C_LINK_RECOMMENDED_SIZE` 也会返回更大的值。
 * 更好的选择是使用静态或动态分配的缓冲区（使用“malloc”）。
 *
 * @param buffer 用于命令分配的缓冲区
 * @param size 缓冲区的大小（以字节为单位）
 *
 * @return I2C命令链接的句柄，如果提供的缓冲区太小，请返回NULL
 * 使用“I2C_LINK_RECOMMENDED_SIZE”宏来获取缓冲区的建议大小。
 */
i2c_cmd_handle_t i2c_cmd_link_create_static(uint8_t* buffer, uint32_t size);

/**
 * @brief 使用给定的缓冲区创建并初始化 I2C 命令列表。
 * 完成I2C事务后，需要调用`i2c_cmd_link_delete()`
 * 释放并归还资源。
 * 所需的字节将动态分配。
 *
 * @return I2C 命令链接句柄，如果动态内存不足，则返回 NULL。
 */
i2c_cmd_handle_t i2c_cmd_link_create(void);

/**
 * @brief 释放使用“i2c_cmd_link_create_static”静态分配的 I2C 命令列表。
 *
 * @param cmd_handle 静态分配的 I2C 命令列表。该句柄的创建应归功于
 * `i2c_cmd_link_create_static()` 函数
 */
void i2c_cmd_link_delete_static(i2c_cmd_handle_t cmd_handle);

/**
 * @brief 释放 I2C 命令列表
 *
 * @param cmd_handle I2C 命令列表。该句柄的创建应归功于
 * `i2c_cmd_link_create()` 函数
 */
void i2c_cmd_link_delete(i2c_cmd_handle_t cmd_handle);

/**
 * @brief 将“开始信号”排队到给定的命令列表。
 * 该函数只能在 I2C 主模式下调用。
 * 调用 `i2c_master_cmd_begin()` 发送所有排队的命令。
 *
 * @param cmd_handle I2C命令列表
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 * - ESP_ERR_NO_MEM 用于创建 `cmd_handler` 的静态缓冲区太小
 * - ESP_FAIL 堆上没有剩余内存
 */
esp_err_t i2c_master_start(i2c_cmd_handle_t cmd_handle);

/**
 * @brief 将“写入字节”命令排队到命令列表中。
 * 将在 I2C 端口上发送单个字节。该功能只能是
 * 在 I2C 主模式下调用。
 * 调用`i2c_master_cmd_begin()`发送所有排队的命令
 *
 * @param cmd_handle I2C命令列表
 * @param data 要在端口上发送的字节
 * @param ack_en 启用ACK信号
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 * - ESP_ERR_NO_MEM 用于创建 `cmd_handler` 的静态缓冲区太小
 * - ESP_FAIL 堆上没有剩余内存
 */
esp_err_t i2c_master_write_byte(i2c_cmd_handle_t cmd_handle, uint8_t data, bool ack_en);

/**
 * @brief 将“写入（多个）字节”命令排队到命令列表中。
 * 该函数只能在 I2C 主模式下调用。
 * 调用`i2c_master_cmd_begin()`发送所有排队的命令
 *
 * @param cmd_handle I2C命令列表
 * @param data 要发送的字节。该缓冲区应保持**有效**，直到事务完成。
 * 如果 PSRAM 已启用且“intr_flag”设置为“ESP_INTR_FLAG_IRAM”，
 * `data` 应该从内部 RAM 分配。
 * @param data_len 数据缓冲区的长度（以字节为单位）
 * @param ack_en 启用ACK信号
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 * - ESP_ERR_NO_MEM 用于创建 `cmd_handler` 的静态缓冲区太小
 * - ESP_FAIL 堆上没有剩余内存
 */
esp_err_t i2c_master_write(i2c_cmd_handle_t cmd_handle, const uint8_t *data, size_t data_len, bool ack_en);

/**
 * @brief 将“读取字节”命令排队到命令列表中。
 * 将在 I2C 总线上读取单个字节。该功能只能是
 * 在 I2C 主模式下调用。
 * 调用`i2c_master_cmd_begin()`发送所有排队的命令
 *
 * @param cmd_handle I2C命令列表
 * @param data 接收到的字节将被存储的指针。该缓冲区应保持**有效**
 * 直到交易完成。
 * @param ack ACK信号
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 * - ESP_ERR_NO_MEM 用于创建 `cmd_handler` 的静态缓冲区太小
 * - ESP_FAIL 堆上没有剩余内存
 */
esp_err_t i2c_master_read_byte(i2c_cmd_handle_t cmd_handle, uint8_t *data, i2c_ack_type_t ack);

/**
 * @brief 将“读取（多个）字节”命令排队到命令列表中。
 * 将在 I2C 总线上读取多个字节。该功能只能是
 * 在 I2C 主模式下调用。
 * 调用`i2c_master_cmd_begin()`发送所有排队的命令
 *
 * @param cmd_handle I2C命令列表
 * @param data 接收到的字节将被存储的指针。该缓冲区应保持**有效**
 * 直到交易完成。
 * @param data_len “数据”缓冲区的大小（以字节为单位）
 * @param ack ACK信号
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 * - ESP_ERR_NO_MEM 用于创建 `cmd_handler` 的静态缓冲区太小
 * - ESP_FAIL 堆上没有剩余内存
 */
esp_err_t i2c_master_read(i2c_cmd_handle_t cmd_handle, uint8_t *data, size_t data_len, i2c_ack_type_t ack);

/**
 * @brief 将“停止信号”排队到给定的命令列表。
 * 该函数只能在 I2C 主模式下调用。
 * 调用 `i2c_master_cmd_begin()` 发送所有排队的命令。
 *
 * @param cmd_handle I2C命令列表
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 * - ESP_ERR_NO_MEM 用于创建 `cmd_handler` 的静态缓冲区太小
 * - ESP_FAIL 堆上没有剩余内存
 */
esp_err_t i2c_master_stop(i2c_cmd_handle_t cmd_handle);

/**
 * @brief 在主模式下发送 I2C 总线上的所有排队命令。
 * 任务将被阻塞，直到所有命令都发送出去。
 * I2C端口受互斥锁保护，因此该函数是线程安全的。
 * 该函数只能在 I2C 主模式下调用。
 *
 * @param i2c_num I2C端口号
 * @param cmd_handle I2C命令列表
 * @paramticks_to_wait 在发出超时之前等待的最大滴答数。
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 * - ESP_FAIL 发送命令错误，从机尚未确认传输。
 * - ESP_ERR_INVALID_STATE I2C 驱动程序未安装或未处于主模式。
 * - ESP_ERR_TIMEOUT 由于总线繁忙而操作超时。
 */
esp_err_t i2c_master_cmd_begin(i2c_port_t i2c_num, i2c_cmd_handle_t cmd_handle, TickType_t ticks_to_wait);

#if SOC_I2C_SUPPORT_SLAVE
/**
 * @brief Write bytes to internal ringbuffer of the I2C slave data. When the TX fifo empty, the ISR will
 *        fill the hardware FIFO with the internal ringbuffer's data.
 *        @note This function shall only be called in I2C slave mode.
 *
 * @param i2c_num I2C port number
 * @param data Bytes to write into internal buffer
 * @param size Size, in bytes, of `data` buffer
 * @param ticks_to_wait Maximum ticks to wait.
 *
 * @return
 *     - ESP_FAIL (-1) Parameter error
 *     - Other (>=0) The number of data bytes pushed to the I2C slave buffer.
 */
int i2c_slave_write_buffer(i2c_port_t i2c_num, const uint8_t *data, int size, TickType_t ticks_to_wait);

/**
 * @brief Read bytes from I2C internal buffer. When the I2C bus receives data, the ISR will copy them
 *        from the hardware RX FIFO to the internal ringbuffer.
 *        Calling this function will then copy bytes from the internal ringbuffer to the `data` user buffer.
 *        @note This function shall only be called in I2C slave mode.
 *
 * @param i2c_num I2C port number
 * @param data Buffer to fill with ringbuffer's bytes
 * @param max_size Maximum bytes to read
 * @param ticks_to_wait Maximum waiting ticks
 *
 * @return
 *     - ESP_FAIL(-1) Parameter error
 *     - Others(>=0) The number of data bytes read from I2C slave buffer.
 */
int i2c_slave_read_buffer(i2c_port_t i2c_num, uint8_t *data, size_t max_size, TickType_t ticks_to_wait);
#endif // SOC_I2C_SUPPORT_SLAVE

/**
 * @brief 设置I2C主时钟周期
 *
 * @param i2c_num I2C端口号
 * @param high_period SCL 为高电平时的时钟周期数，high_period 为 14 位值
 * @param low_period SCL为低电平时的时钟周期数，low_period是一个14位值
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_set_period(i2c_port_t i2c_num, int high_period, int low_period);

/**
 * @brief 获取I2C主时钟周期
 *
 * @param i2c_num I2C端口号
 * @param high_period 指针在SCL为高电平时获取时钟周期数，将得到一个14位值
 * @param low_period 指针在SCL为低电平时获取时钟周期数，将得到一个14位值
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_get_period(i2c_port_t i2c_num, int *high_period, int *low_period);

/**
 * @brief 在 I2C 总线上启用硬件过滤器
 * 有时I2C总线会受到高频噪声（约20ns）或上升沿的干扰
 * SCL时钟非常慢，这些可能会导致主状态机中断。
 * 启用硬件滤波器可以滤除高频干扰，使主控更加稳定。
 * @note 启用过滤器会减慢 SCL 时钟速度。
 *
 * @param i2c_num 要过滤的 I2C 端口号
 * @param cyc_num 需要过滤 APB 周期 (0<= cyc_num <=7)。
 * 当脉冲周期小于 cyc_num * APB_cycle 时，I2C 控制器将忽略该脉冲。
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_filter_enable(i2c_port_t i2c_num, uint8_t cyc_num);

/**
 * @brief 禁用 I2C 总线上的过滤器
 *
 * @param i2c_num I2C端口号
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_filter_disable(i2c_port_t i2c_num);

/**
 * @brief 设置I2C主机启动信号时序
 *
 * @param i2c_num I2C端口号
 * @param setup_time SDA下降沿和SCL上升沿之间的时钟数，用于起始标记，它是一个10位值。
 * @paramhold_time SDA下降沿和SCL下降沿之间的时钟编号，用于起始标记，它是一个10位值。
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_set_start_timing(i2c_port_t i2c_num, int setup_time, int hold_time);

/**
 * @brief 获取I2C主机启动信号时序
 *
 * @param i2c_num I2C端口号
 * @param setup_time 获取设置时间的指针
 * @param Hold_time 获取保持时间的指针
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_get_start_timing(i2c_port_t i2c_num, int *setup_time, int *hold_time);

/**
 * @brief 设置I2C主机停止信号时序
 *
 * @param i2c_num I2C端口号
 * @param setup_time SCL上升沿和SDA上升沿之间的时钟编号，它是一个10位值。
 * @paramhold_time STOP 位上升沿后的时钟数，它是一个 14 位值。
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_set_stop_timing(i2c_port_t i2c_num, int setup_time, int hold_time);

/**
 * @brief 获取I2C主机停止信号时序
 *
 * @param i2c_num I2C端口号
 * @param setup_time 获取设置时间的指针。
 * @paramhold_time 获取保持时间的指针。
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_get_stop_timing(i2c_port_t i2c_num, int *setup_time, int *hold_time);

/**
 * @brief 设置I2C数据信号时序
 *
 * @param i2c_num I2C端口号
 * @paramsample_time时钟数I2C用于在SCL上升沿之后在SDA上采样数据，它是一个10位值
 * @paramhold_time时钟数I2C用于在SCL下降沿后保持数据，它是一个10位值
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_set_data_timing(i2c_port_t i2c_num, int sample_time, int hold_time);

/**
 * @brief 获取I2C数据信号时序
 *
 * @param i2c_num I2C端口号
 * @param Sample_time 获取采样时间的指针
 * @param Hold_time 获取保持时间的指针
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_get_data_timing(i2c_port_t i2c_num, int *sample_time, int *hold_time);

/**
 * @brief 设置I2C超时值
 * @param i2c_num I2C端口号
 * @param timeout I2C总线超时值（单位：APB 80Mhz时钟周期）
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_set_timeout(i2c_port_t i2c_num, int timeout);

/**
 * @brief 获取I2C超时值
 * @param i2c_num I2C端口号
 * @param timeout指针获取超时值
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_get_timeout(i2c_port_t i2c_num, int *timeout);

/**
 * @brief 设置I2C数据传输模式
 *
 * @param i2c_num I2C端口号
 * @param tx_trans_mode I2C发送数据模式
 * @param rx_trans_mode I2C接收数据模式
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_set_data_mode(i2c_port_t i2c_num, i2c_trans_mode_t tx_trans_mode, i2c_trans_mode_t rx_trans_mode);

/**
 * @brief 获取I2C数据传输模式
 *
 * @param i2c_num I2C端口号
 * @param tx_trans_mode 获取I2C发送数据模式的指针
 * @param rx_trans_mode 获取I2C接收数据模式的指针
 *
 * @返回
 * - ESP_OK 成功
 * - ESP_ERR_INVALID_ARG 参数错误
 */
esp_err_t i2c_get_data_mode(i2c_port_t i2c_num, i2c_trans_mode_t *tx_trans_mode, i2c_trans_mode_t *rx_trans_mode);