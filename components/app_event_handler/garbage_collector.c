/*
 * garbage_collector.c
 *
 *  Created on: Jun 21, 2023
 *      Author: ekalan
 */

#include <stdlib.h>
#include "garbage_collector.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_err.h"
#include "string.h"

#ifdef INCLUDE_eTaskGetState
#endif
typedef struct
{
	void *ptr;
	void *ptr_to;
	TickType_t time;
} garbage_item_t;

static QueueHandle_t garbage_queue;
static TaskHandle_t gc_task_handle;
static BaseType_t gc_run = pdTRUE;
static BaseType_t gc_deleted = pdFALSE;

void delete_event_later(void **ptr_ref)
{
	if (ptr_ref == NULL || *ptr_ref == NULL || gc_run != pdTRUE)
		return;

	garbage_item_t item;
	item.ptr = *ptr_ref;
//	item.ptr_to = ptr_ref;
	item.time = xTaskGetTickCount();
	*ptr_ref = NULL;  // Set the original pointer to NULL to avoid further use.
	ESP_LOGD("GC_task", "%s, \t\tAdded pointer= %p, ",
			__FUNCTION__,
			(void* ) item.ptr);
	xQueueSend(garbage_queue, &item, portMAX_DELAY);
}

void garbage_collector_init()
{
	gc_run = pdTRUE;

	garbage_queue = xQueueCreate(GARBAGE_TASK_QUEUE_SIZE,
			sizeof(garbage_item_t));

	xTaskCreate(garbage_collector_task, "garbage_collector_task",
	GARBAGE_TASK_SIZE, NULL, GARBAGE_TASK_PRIORITY,
	NULL);
}

void garbage_collector_deinit()
{
	// Wait a second for the rest of the tasks to be finished
	vTaskDelay(pdMS_TO_TICKS(1000));
	gc_run = pdFALSE;
	// Add a null pointer to the queue to force the task to loop
	void *ptr = NULL;
	xQueueSend(garbage_queue, &ptr, portMAX_DELAY);
	// Waiting for the garbage collector task to delete itself.
	int i = 0;
//	while (eTaskGetState(gc_task_handle) != eDeleted)
	while (gc_deleted == pdFALSE) {
		vTaskDelay(pdMS_TO_TICKS(10));
		if (++i > 10) {
			ESP_LOGE("GC_task",
					"%s, %d. Garbage Collector task has not closed= %d ",
					__FUNCTION__,
					i, gc_run);
			break;
		}
	}
	gc_deleted = pdFALSE;
	gc_task_handle = NULL;
}

void garbage_collector_task(void *param)
{
	garbage_item_t item;

	while (gc_run == pdTRUE)
	{
		if (xQueueReceive(garbage_queue, &item, portMAX_DELAY)) {
			TickType_t now = xTaskGetTickCount();
			if (item.ptr == NULL) {
				continue;
			}
			if (now - item.time >= pdMS_TO_TICKS(1000)) // If the item is 1 second or more old
			{
				ESP_LOGD("GC_task", "%s, \tFreeing pointer at = %p",
						__FUNCTION__,
						(void* ) item.ptr);
//				item.ptr_to = NULL;  // Set the original pointer to NULL to avoid further use.

				free(item.ptr); // Free the memory
			}
			else
			{
				// Put the item back in the queue and delay the task
				xQueueSendToFront(garbage_queue, &item, portMAX_DELAY);
				vTaskDelay(pdMS_TO_TICKS(1000) - (now - item.time));
			}
		}
	}
	ESP_LOGD("GC_task", "%s, \tReceived end task flag", __FUNCTION__);
	gc_deleted = pdTRUE; // Indicate that this task is about to delete itself
	vQueueDelete(garbage_queue); // Delete the queue before task ends
	vTaskDelete(NULL); // Delete itself
}

/**
 * Testing for garbage collector
 *
 */
#define TEST_PTR_COUNT 3

esp_err_t test_garbage_collector()
{
	garbage_collector_init();
	esp_err_t err = ESP_OK;

	void *test_ptrs[TEST_PTR_COUNT];
	void *cpy_ptrs[TEST_PTR_COUNT];

	// Allocate TEST_PTR_COUNT integers and add them to the garbage collector
	for (int ii = 0; ii < 2; ii++) {
		for (int i = 0; i < TEST_PTR_COUNT; i++) {
			char *example = malloc(sizeof(int) * 200);
			sprintf(example, "Test char number%d", i);
			test_ptrs[i] = (void*) example;
			cpy_ptrs[i] = (void*) example;

			if (test_ptrs[i] == NULL)
			{
				ESP_LOGD("GB_TEST",
						"Failed to allocate memory for test_ptrs[%d at:%p]",
						i, (void* ) test_ptrs[i]);
				err = ESP_FAIL;

				continue;
			}

//		*((int*) test_ptrs[i]) = i + 100;

			ESP_LOGD("GB_TEST", "%s, \tCreated test_ptrs[%i] = %p msg:%s",
					__FUNCTION__, i, (void* ) test_ptrs[i],
					(char* ) test_ptrs[i]);

			delete_event_later(&test_ptrs[i]);

		}
		// Wait less than a second and pointers should still be valid
		vTaskDelay(pdMS_TO_TICKS(200));

		for (int i = 0; i < TEST_PTR_COUNT; i++) {

			if (cpy_ptrs[i] == NULL || strlen(cpy_ptrs[i]) < 1)
					{
				ESP_LOGE("GB_TEST",
						"%s, \tFailed: memory was deleted early[%d] , at=%p, len:%d",
						__FUNCTION__, i, (void* ) cpy_ptrs[i],
						strlen((char* ) cpy_ptrs[i]));

				err = ESP_FAIL;

				continue;
			}
			ESP_LOGD("GB_TEST",
					"%s, \tcpy_ptrs[%d] :%p strlen:%d, sizeof:%d",
					__FUNCTION__, i, (void* )cpy_ptrs[i],
					strlen((char* )cpy_ptrs[i]),
					sizeof((char* )cpy_ptrs[i]));

		}

		// Wait for 2 seconds to give the garbage collector time to delete the pointers
		vTaskDelay(pdMS_TO_TICKS(1000));

		// Check if the pointers have been deleted
		for (int i = 0; i < TEST_PTR_COUNT; i++)
				{
			if (test_ptrs[i] != NULL)
			{
				ESP_LOGE("GB_TEST",
						"%s, \test_ptrs[%d] has not been deleted at:%p msg%s",
						__FUNCTION__, i, (void* )test_ptrs[i],
						(char* )test_ptrs[i]);
				err = ESP_FAIL;

			}
			else
			{
				ESP_LOGI("GB_TEST", "test_ptrs[%d] has been deleted", i);
			}
			if (strlen(cpy_ptrs[i]) < 1)
					{
				ESP_LOGE("GB_TEST",
						"%s, \tcpy_ptrs[%d] has not been deleted at:%p strlen:%d, sizeof:%d",
						__FUNCTION__, i, (void* )cpy_ptrs[i],
						strlen((char* )cpy_ptrs[i]),
						sizeof(((char* )cpy_ptrs[i])));
				err = ESP_FAIL;

			}
			else
			{
				ESP_LOGI("GB_TEST",
						"%s, \tcpy_ptrs[%d] has been deleted at:%p strlen:%d, sizeof:%d",
						__FUNCTION__, i, (void* )cpy_ptrs[i],
						strlen((char* )cpy_ptrs[i]),
						sizeof(((char* )cpy_ptrs[i])));
			}
		}
	}
	garbage_collector_deinit();
	return err;
}

