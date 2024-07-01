/**
 * @brief 初始化 ADC 连续模式驱动并获取其句柄。
 *
 * @param hdl_config ADC 初始化配置结构体指针，参考 adc_continuous_handle_cfg_t。
 * @param ret_handle 输出参数，用于返回初始化后的 ADC 连续模式驱动句柄。
 * @return
 *     - ESP_ERR_INVALID_ARG 参数组合无效。
 *     - ESP_ERR_NOT_FOUND 找不到具有指定标志的空闲中断。
 *     - ESP_ERR_NO_MEM 内存不足。
 *     - ESP_OK 成功。
 */
esp_err_t adc_continuous_new_handle(const adc_continuous_handle_cfg_t *hdl_config, adc_continuous_handle_t *ret_handle);

/**
 * @brief 设置 ADC 连续模式所需的配置。
 *
 * @param handle ADC 连续模式驱动句柄。
 * @param config 指向 adc_continuous_config_t 结构体的指针，包含配置 ADC 的详细信息。
 * @return
 *     - ESP_ERR_INVALID_STATE 驱动状态无效，此时不应调用此 API。
 *     - ESP_ERR_INVALID_ARG 参数组合无效。
 *     - ESP_OK 成功。
 */
esp_err_t adc_continuous_config(adc_continuous_handle_t handle, const adc_continuous_config_t *config);

/**
 * @brief 注册 ADC 连续模式事件的回调函数。
 *
 * @param handle ADC 连续模式驱动句柄。
 * @param cbs 指向 adc_continuous_evt_cbs_t 结构体的指针，包含要注册的回调函数。
 * @param user_data 用户数据指针，将传递给注册的回调函数。
 * @return
 *     - ESP_OK 成功。
 *     - ESP_ERR_INVALID_ARG 参数无效。
 *     - ESP_ERR_INVALID_STATE 驱动状态无效，此时不应调用此 API。
 */
esp_err_t adc_continuous_register_event_callbacks(adc_continuous_handle_t handle, const adc_continuous_evt_cbs_t *cbs, void *user_data);

/**
 * @brief 启动 ADC 连续模式。
 *
 * @param handle ADC 连续模式驱动句柄。
 * @return
 *     - ESP_ERR_INVALID_STATE 驱动状态无效。
 *     - ESP_OK 成功。
 */
esp_err_t adc_continuous_start(adc_continuous_handle_t handle);

/**
 * @brief 从 ADC 连续模式下读取数据。
 *
 * @param handle ADC 连续模式驱动句柄。
 * @param buf 用于存储从 ADC 读取的转换结果的缓冲区。
 * @param length_max 期望从 ADC 读取的最大数据长度（字节）。
 * @param out_length 实际从 ADC 读取的数据长度（输出参数）。
 * @param timeout_ms 等待数据的超时时间（毫秒）。
 * @return
 *     - ESP_ERR_INVALID_STATE 驱动状态无效，通常意味着 ADC 采样速率快于任务处理速率。
 *     - ESP_ERR_TIMEOUT 操作超时。
 *     - ESP_OK 成功。
 */
esp_err_t adc_continuous_read(adc_continuous_handle_t handle, uint8_t *buf, uint32_t length_max, uint32_t *out_length, uint32_t timeout_ms);
