#include <stdio.h>
#include "cJSON.h"

void app_main(void) {
    // JSON 字符串
    const char *json_string = "{\"name\":\"John\", \"age\":30, \"height\":5.9, \"is_student\":false, "
                              "\"hobbies\":[\"reading\", \"gaming\", \"cycling\"], "
                              "\"scores\":[85, 90, 78]}";

    // 先判断是否是有效的 JSON 字符串
    cJSON *root = cJSON_Parse(json_string);
    if (root == NULL) {
        printf("不是有效的 JSON 格式\n");
        return;
    }

    // 获取并打印字符串型值
    if (cJSON_HasObjectItem(root, "name")) {
        const char *name_value = cJSON_GetObjectItem(root, "name")->valuestring;
        printf("Name: %s\n", name_value);
    } else {
        printf("Key 'name' not found in JSON\n");
    }

    // 获取并打印整型值
    if (cJSON_HasObjectItem(root, "age")) {
        int age_value = cJSON_GetObjectItem(root, "age")->valueint;
        printf("Age: %d\n", age_value);
    } else {
        printf("Key 'age' not found in JSON\n");
    }

    // 获取并打印浮点型值
    if (cJSON_HasObjectItem(root, "height")) {
        double height_value = cJSON_GetObjectItem(root, "height")->valuedouble;
        printf("Height: %.1f\n", height_value);
    } else {
        printf("Key 'height' not found in JSON\n");
    }

    // 获取并打印布尔型值
    if (cJSON_HasObjectItem(root, "is_student")) {
        int is_student_value = cJSON_IsTrue(cJSON_GetObjectItem(root, "is_student"));
         
    } else {
        printf("Key 'is_student' not found in JSON\n");
    }

    // 获取并打印字符串数组
    if (cJSON_HasObjectItem(root, "hobbies")) {
        cJSON *hobbies = cJSON_GetObjectItem(root, "hobbies");
        if (cJSON_IsArray(hobbies)) {
            int hobbies_size = cJSON_GetArraySize(hobbies);
            for (int i = 0; i < hobbies_size; i++) {
                const char *hobby_value = cJSON_GetArrayItem(hobbies, i)->valuestring;
                printf("Hobby %d: %s\n", i + 1, hobby_value);
            }
        }
    } else {
        printf("Key 'hobbies' not found in JSON\n");
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
    } else {
        printf("Key 'scores' not found in JSON\n");
    }

    // 释放 JSON 对象占用的内存
    cJSON_Delete(root);
}
