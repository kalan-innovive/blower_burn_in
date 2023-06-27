/*
 * test_event_handler.h
 *
 *  Created on: Jun 20, 2023
 *      Author: ekalan
 */

#ifndef COMPONENTS_APP_EVENT_HANDLER_INCLUDE_TEST_EVENT_HANDLER_H_
#define COMPONENTS_APP_EVENT_HANDLER_INCLUDE_TEST_EVENT_HANDLER_H_

#include "esp_types.h"
#include "esp_event.h"
#include "esp_err.h"
#include "esp_log.h"

#ifndef T_LOG
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RESET "\x1b[0m"
#define T_LOG(tag, message, ...) esp_log_write(ESP_LOG_INFO, tag, COLOR_BLUE "[%s] " message COLOR_RESET "\n", tag, ##__VA_ARGS__)
#endif

esp_err_t test_event_handler_system(void);
esp_err_t test_event_handler_unit(void);
void test_eh_response(void *handler_arg, esp_event_base_t base,
		int32_t id, void *event_data);
#endif /* COMPONENTS_APP_EVENT_HANDLER_INCLUDE_TEST_EVENT_HANDLER_H_ */
