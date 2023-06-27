/*
 * garbage_collector.h
 *
 *  Created on: Jun 21, 2023
 *      Author: ekalan
 */

#ifndef COMPONENTS_APP_EVENT_HANDLER_INCLUDE_GARBAGE_COLLECTOR_H_
#define COMPONENTS_APP_EVENT_HANDLER_INCLUDE_GARBAGE_COLLECTOR_H_

#include "esp_err.h"

#define GARBAGE_TASK_SIZE 2048
#define GARBAGE_TASK_PRIORITY 2
#define GARBAGE_TASK_QUEUE_SIZE 50

void delete_event_later(void **ptr_ref);
void garbage_collector_init();
void garbage_collector_task(void *param);
void garbage_collector_deinit();

esp_err_t test_garbage_collector();
#endif /* COMPONENTS_APP_EVENT_HANDLER_INCLUDE_GARBAGE_COLLECTOR_H_ */
