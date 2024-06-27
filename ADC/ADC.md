# ADC Continuous Mode API 参考

## 结构体

### `adc_digi_pattern_config_t`

- ADC数字控制器模式配置

#### 公共成员

- `uint8_t atten` - ADC通道的衰减
- `uint8_t channel` - ADC通道
- `uint8_t unit` - ADC单元
- `uint8_t bit_width` - ADC输出位宽

### `adc_digi_output_data_t`

- ADC数字控制器（DMA模式）输出数据格式

#### 公共成员

- `uint16_t data` - ADC实际输出数据信息。分辨率为12位或11位。
- `uint16_t channel` - ADC通道索引信息

### `adc_ulp_mode_t`

- ADC ULP模式

#### 枚举值

- `ADC_ULP_MODE_DISABLE` - 禁用ADC ULP模式
- `ADC_ULP_MODE_FSM` - ADC由ULP FSM控制
- `ADC_ULP_MODE_RISCV` - ADC由ULP RISCV控制

### `adc_digi_convert_mode_t`

- ADC数字控制器（DMA模式）工作模式

#### 枚举值

- `ADC_CONV_SINGLE_UNIT_1` - 仅使用ADC1进行转换
- `ADC_CONV_SINGLE_UNIT_2` - 仅使用ADC2进行转换
- `ADC_CONV_BOTH_UNIT` - 同时使用ADC1和ADC2进行转换
- `ADC_CONV_ALTER_UNIT` - 轮流使用ADC1和ADC2进行转换

### `adc_digi_output_format_t`

- ADC数字控制器（DMA模式）输出数据格式选项

#### 枚举值

- `ADC_DIGI_OUTPUT_FORMAT_TYPE1` - 参见`adc_digi_output_data_t.type1`
- `ADC_DIGI_OUTPUT_FORMAT_TYPE2` - 参见`adc_digi_output_data_t.type2`

## 函数

### `esp_err_t adc_continuous_io_to_channel(int io_num, adc_unit_t *const unit_id, adc_channel_t *const channel)`

- 从给定的GPIO编号获取ADC通道

#### 参数

- `io_num` - [输入] GPIO编号
- `unit_id` - [输出] ADC单元
- `channel` - [输出] ADC通道

#### 返回值

- `ESP_OK` - 成功
- `ESP_ERR_INVALID_ARG` - 参数无效
- `ESP_ERR_NOT_FOUND` - IO不是有效的ADC引脚

