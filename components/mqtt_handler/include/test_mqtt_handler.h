/*
 * test_mqtt_handler.h
 *
 *  Created on: Jun 13, 2023
 *      Author: ekalan
 */

#ifndef COMPONENTS_MQTT_HANDLER_INCLUDE_TEST_MQTT_HANDLER_H_
#define COMPONENTS_MQTT_HANDLER_INCLUDE_TEST_MQTT_HANDLER_H_
#include "esp_err.h"
#include "esp_log.h"
#include "string.h"

#ifndef T_LOG
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RESET "\x1b[0m"
#define T_LOG(tag, message, ...) esp_log_write(ESP_LOG_INFO, tag, COLOR_BLUE "[%s]" message COLOR_RESET "\n", tag, ##__VA_ARGS__)

#endif
#define MQTT_TAG "test_mqtt"
//#define LOG_T(tag, message, ...) T_LOG(MQTT_TAG, "[%s] [%d] [%s] " message, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define _INNO_TEST_ASSERT(condition, line, message) \
    if (!(condition)) { \
        ESP_LOGE("unit test", "%d : msg:%s", line, message); \
    }

#define _INNO_ASSERT_EQUAL_STRING_MESSAGE(expected, actual, message) \
    do { \
        if (strcmp((expected), (actual)) != 0) { \
            ESP_LOGE("unit test", "Assertion failed at line %d: %s. Expected: %s, Actual: %s", __LINE__, (message), (expected), (actual)); \
            return ESP_FAIL; \
        } \
    } while (0)

//int test_mqtt_handler(void);
//void test_setup_mqtt(void);
void run_mqtt_unit_tests(void);
void run_mqtt_system_tests(void);

//void test_setup_mqtt_system(void);

#endif /* COMPONENTS_MQTT_HANDLER_INCLUDE_TEST_MQTT_HANDLER_H_ */
