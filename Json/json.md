**本频道只提供idf编程方法，不提供知识点注解**

# 版权信息

© 2024 . 未经许可不得复制、修改或分发。 此文献为 [小風的藏書閣](https://t.me/xfp2333)  所有。

# 简介

- ESP-IDF 并没有直接提供处理JSON格式的方法,本文档使用[CJSONK库](https://github.com/DaveGamble/cJSON.git)处理JSON数据

- 以下JSON数据是本教程的示例模板，同时也展示了JSON格式

```json
{
    "name": "John",
    "age": 30,
    "height": 5.9,
    "is_student": false,
    "hobbies": ["reading", "gaming", "cycling"],
    "scores": [85, 90, 78]
}

{
    "name": "John",
    "age": 30,
    "is_student": false,
    "scores": [85, 90, 78]
}

```

# 环境准备

1. 下载CJSON库，查看简介处

2. 将`cJson.c` 与 `cJSON.h` 复制于工程的`main`目录下

3. 将`cJson.c` 添加于`cmakelist.txt`的`SRCS`中

4. 包含所需头文件：

```c

#include "cJSON.h"

```

# 使用CJSON库

## 创建并序列化json对象
1. 将字符串序列化为CJSON对象

```c

//json对象数据类型，这是一个使用链表的struct
typedef struct cJSON
{
    /* 指向链表中的下一个 cJSON 元素的指针，用于遍历数组或对象链。 */
    struct cJSON *next;
    
    /* 指向链表中的上一个 cJSON 元素的指针，用于遍历数组或对象链。 */
    struct cJSON *prev;
    
    /* 指向该对象的子项的链表的指针。对于数组或对象，子项会连接成链表。 */
    struct cJSON *child;

    /* 当前 cJSON 元素的类型。 */
    int type;

    /* 当类型为 cJSON_String 或 cJSON_Raw 时，存储字符串值的指针。 */
    char *valuestring;

    /* 存储整数值的字段，已废弃。推荐使用 cJSON_SetNumberValue 代替。 */
    int valueint;

    /* 当类型为 cJSON_Number 时，存储浮点数值的字段。 */
    double valuedouble;

    /* 如果该项是对象的子项或在对象的子项列表中，则存储项的名称字符串。 */
    char *string;
} cJSON;

/**
 * @brief 解析 JSON 格式的字符串为 cJSON 对象。
 * 
 * @param value 指向 JSON 格式字符串的指针。该字符串必须是有效的 JSON 格式，并且以空字符（\0）结尾。
 * @return 返回指向 cJSON 结构体的指针，该指针表示解析后的 JSON 对象。如果解析失败，返回 NULL。
 */
cJSON *cJSON_Parse(const char *value);


```

- 使用示例:

```c
    const char *json_string = "{\"name\":\"John\", \"age\":30, \"is_student\":false, \"scores\":[85, 90, 78]}";

    // 先判断是否是有效的 JSON 字符串
    cJSON *root = cJSON_Parse(json_string);
    if (root == NULL) {
        printf("不是有效的 JSON 格式\n");
        return;
    }
```

## 检查键名是否存在

```c
/**
 * @brief 检查 cJSON 对象中是否存在指定的键。
 * 
 * @param object 指向要检查的 cJSON 对象的指针。该对象应为 cJSON 类型。
 * @param string 要检查的键名（字符串）。这是你希望在 cJSON 对象中查找的键名。
 * 
 * @return 如果指定的键存在于 cJSON 对象中，返回 true (cJSON_True)；否则，返回 false (cJSON_False)。
 */
CJSON_PUBLIC(cJSON_bool) cJSON_HasObjectItem(const cJSON *object, const char *string);

```

- 使用示例:

```c

    // 检查是否包含指定的键名并打印对应的键值
    if (cJSON_HasObjectItem(root, "name")) {
        const char *name_value = cJSON_GetObjectItem(root, "name")->valuestring;
        printf("Name: %s\n", name_value);
    } else {
        printf("Key 'name' not found in JSON\n");
    }

```

## 获取单个键值

```c

/**
 * @brief 获取 cJSON 对象中指定键的值。
 * 
 * @param object 指向要获取项的 cJSON 对象的指针。该对象应为 cJSON 类型。
 * @param string 要获取值的键名（字符串）。这是你希望从 cJSON 对象中获取其对应值的键名。
 * 
 * @return 如果指定的键存在，返回一个指向该键对应 cJSON 对象的指针；否则，返回 NULL。
 */
CJSON_PUBLIC(cJSON *) cJSON_GetObjectItem(const cJSON * const object, const char * const string);

/**
 * @brief 检查给定的 cJSON 对象是否表示布尔值 true。
 *
 * 这个函数用于判断传入的 `cJSON` 对象是否代表布尔值 `true`。在 `cJSON` 中，布尔值 `true` 被表示为 `1`，而 `false` 被表示为 `0`。
 *
 * @param item 指向要检查的 `cJSON` 对象的指针。
 *
 * @return 如果 `item` 表示布尔值 `true`，则返回 `cJSON_True`。否则，返回 `cJSON_False`。
 */
CJSON_PUBLIC(cJSON_bool) cJSON_IsTrue(const cJSON *item);

```

- 使用示例:

```c

const char *name_value = cJSON_GetObjectItem(root, "name")->valuestring; //获取字符串

int age_value = cJSON_GetObjectItem(root, "age")->valueint;              //获取整数

int is_student_value = cJSON_IsTrue(cJSON_GetObjectItem(root, "is_student"));     //处理bool

double height_value = cJSON_GetObjectItem(root, "height")->valuedouble;  //处理浮点数

```


## 获取数组元素

```c

/**
 * @brief 检查给定的 cJSON 对象是否表示 JSON 数组。
 *
 * 这个函数用于判断传入的 `cJSON` 对象是否代表一个 JSON 数组。JSON 数组是一个有序的值列表，用方括号 `[]` 包裹起来。
 *
 * @param item 指向要检查的 `cJSON` 对象的指针。
 *
 * @return 如果 `item` 表示一个 JSON 数组，返回 `cJSON_True`；否则，返回 `cJSON_False`。
 */
CJSON_PUBLIC(cJSON_bool) cJSON_IsArray(const cJSON * const item);

/**
 * @brief 获取 JSON 数组中指定索引的元素。
 *
 * 这个函数从一个 JSON 数组中获取指定位置的元素。JSON 数组是一个有序的值列表，索引从 0 开始。如果索引超出了数组的范围，函数返回 `NULL`。
 *
 * @param array 指向 JSON 数组的 `cJSON` 对象的指针。
 * @param index 要获取的元素的索引（从 0 开始）。
 *
 * @return 指向指定索引位置的 `cJSON` 元素的指针。如果索引超出了数组的范围，则返回 `NULL`。
 */
CJSON_PUBLIC(cJSON *) cJSON_GetArrayItem(const cJSON *array, int index);


/**
 * @brief 获取 JSON 数组中元素的数量。
 *
 * 这个函数用于获取 JSON 数组中包含的元素的数量。JSON 数组是一个有序的值列表，用方括号 `[]` 包裹起来。
 *
 * @param array 指向 JSON 数组的 `cJSON` 对象的指针。
 *
 * @return 数组中元素的数量。如果 `array` 不是一个 JSON 数组，则返回 0。
 */
CJSON_PUBLIC(int) cJSON_GetArraySize(const cJSON *array);

 
```

- 使用示例:

```c
//处理字符数组:
cJSON *hobbies = cJSON_GetObjectItem(root, "hobbies");
       if (cJSON_IsArray(hobbies)) {
            int hobbies_size = cJSON_GetArraySize(hobbies); //获取数组大小
            for (int i = 0; i < hobbies_size; i++) {
                const char *hobby_value = cJSON_GetArrayItem(hobbies, i)->valuestring; //根据索引取值
                printf("Hobby %d: %s\n", i + 1, hobby_value);
            }
        }


    // 获取并打印整数型数组
    if (cJSON_HasObjectItem(root, "scores")) {
        cJSON *scores = cJSON_GetObjectItem(root, "scores");
        if (cJSON_IsArray(scores)) {
            int scores_size = cJSON_GetArraySize(scores);
            for (int i = 0; i < scores_size; i++) {
                int score_value = cJSON_GetArrayItem(scores, i)->valueint;
                printf("Score %d: %d\n", i + 1, score_value);
            }
        }
    }

```

## 释放JSON对象占用的资源

```c
/**
 * @brief 释放并删除一个 `cJSON` 对象及其所有子对象。
 *
 * 这个函数用于递归地释放 `cJSON` 对象及其包含的所有子对象所占用的内存。调用此函数后，传入的 `cJSON` 对象指针将不再有效，
 * 不能继续使用，否则会导致未定义行为。
 *
 * @param item 指向需要删除的 `cJSON` 对象的指针。该对象可能是一个单独的 JSON 值，也可能是一个包含多个子项的 JSON 对象或数组。
 */
CJSON_PUBLIC(void) cJSON_Delete(cJSON *item);


```

- 使用示例:

```c
// 释放 JSON 对象占用的内存
cJSON_Delete(root);

```

# 代码示例

- [CJSON程序示例](cjson.c)