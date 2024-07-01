**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息

© 2024 . 未经许可不得复制、修改或分发。 此文献为 [小風的藏書閣](https://t.me/xfp2333) 所有。

# 通用定时器编程指南

## 环境配置
1. 包含头文件:
```c
#include "driver/gptimer.h"
```

2. cmake配置
```c
REQUIRES driver || PRIV_REQUIRES driver
```

## 配置定时器句柄

```c
gptimer_handle_t gptimer = NULL; //创建定时器句柄
gptimer_config_t timer_config = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
};
//应用配置 将配置与句柄绑定
ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
```

**官方是这么解释的:**
1. 