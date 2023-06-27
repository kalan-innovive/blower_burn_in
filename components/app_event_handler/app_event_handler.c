#include <stdio.h>

#include "app_event_handler.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_system.h"

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "cJSON.h"
#include "msg16.h"

extern queue_task_t task;
extern QueueHandle_t task_queue_handle;

// Static variables
static const char *TAG = "APP_EH";

// Prototypes
static void msg16_request_handler(app_request_t *request);
static void settings_request_handler(app_request_t *request);
static void db_request_handler(app_request_t *request);

// Waits for a task to be added to the queue
// Sends the task to the specified handler
// If response is needed the callback is used to handler the response
void event_handler_task(void *pvParams) {
	while (1) {
		// wait for a task in the queue
		queue_task_t task;
		if (xQueueReceive(task_queue_handle, (void*) &task,
		portMAX_DELAY) != pdTRUE) {
			continue;
		}

		// create an app request from the queue task
		app_request_t request = { .msg_id = task.id, .respons_cb =
				(void (*)) task.respons_cb, .request = task.json_msg };

		// process the request based on the message id
		switch (request.msg_id) {
		case MSG16_REQUEST:
			msg16_request_handler(&request);
			break;
		case SETTINGS_REQUEST:
			settings_request_handler(&request);
			break;
		case DB_REQUEST:
			db_request_handler(&request);
			break;
		default:
			break;
		}
	}
}

static void msg16_request_handler(app_request_t *request) {
	// parse the json message into a msg16 struct
	cJSON *json = cJSON_Parse(request->request);
	if (json == NULL) {
		request->respons_cb((void*) request);
		return;
	}
	cJSON *msg16_json = cJSON_GetObjectItem(json, "msg16");
	if (msg16_json == NULL) {
		cJSON_Delete(json);
		request->respons_cb((void*) request);
		return;
	}

	char *msg16_str = msg16_json->valuestring;
	char *token = strtok(msg16_str, "|");
	if (token == NULL || strcmp(token, "write") != 0) {
		cJSON_Delete(json);
		request->respons_cb((void*) request);
		return;
	}

	msg16_t msg;
	msg.type = 20;

	token = strtok(NULL, "|");
	if (token == NULL) {
		cJSON_Delete(json);
		request->respons_cb((void*) request);
		return;
	}
	msg.dev_id = (uint16_t) atoi(token);

	token = strtok(NULL, "|");
	if (token == NULL) {
		cJSON_Delete(json);
		request->respons_cb((void*) request);
		return;
	}
	msg.addr = (uint16_t) atoi(token);

	token = strtok(NULL, "|");
	if (token == NULL) {
		cJSON_Delete(json);
		request->respons_cb((void*) request);
		return;
	}
	msg.len = (uint16_t) atoi(token);

//    msg16.msg_val = malloc(msg16.len * sizeof(uint16_t));
//    if (msg16.msg_val == NULL) {
//        cJSON_Delete(json);
//        request->respons_cb((void*) request);
//        return;
//    }

	for (int i = 0; i < msg.len; i++) {
		token = strtok(NULL, "|");
		if (token == NULL) {
//            free(msg.msg_val);
			cJSON_Delete(json);
			request->respons_cb((void*) request);
			return;
		}
		msg.msg_val[i] = (uint16_t) atoi(token);
	}

	// do something with the msg16 struct
	// ...

	// free memory and callback with success
//    free(msg.msg_val);
	cJSON_Delete(json);
	request->respons_cb(0);
}

// function to create a DB GET request
// need to free the
int create_db_get_req(app_request_t *req, unsigned chipID) {
	// check that the req is not NULL
	if (req == NULL) {
		return 0; // failed
	}

	// create a cJSON object
	cJSON *root = cJSON_CreateObject();
	if (root == NULL) {
		return 0; // failed
	}

	// add items to the object
	cJSON_AddStringToObject(root, "node_name", "esp3");
	cJSON_AddStringToObject(root, "cmd", "calibration/pre_post_burnin");
	cJSON_AddNumberToObject(root, "chip_id", (double) chipID);

	// print the JSON object to a string
	req->request = cJSON_Print(root);
	if (req->request == NULL) {
		cJSON_Delete(root); // clean up memory
		return 0; // failed
	}

//    // copy jsonString to the buffer
//    strcpy(buff, jsonString);
//
//    // clean up memory
//    free(jsonString);
	cJSON_Delete(root);

	return 1; // succeeded
}

static void settings_request_handler(app_request_t *request) {
	ESP_LOGI(TAG, "Request for settings is zero");

}

static void db_request_handler(app_request_t *request) {
	ESP_LOGD(TAG, "Request for db msgID:%d", request->msg_id);
	ESP_LOGD(TAG, "Request for db msgID:%d", request->msg_id);
	ESP_LOGD(TAG, "Request for db msgID:%d", request->msg_id);

}

// function to parse a JSON string and call a callback function with the parsed data
//
int db_get_handler(queue_task_t *task) {
	if (task == NULL || task->json_msg == NULL) {
		return 0; // failed
	}

	// parse the JSON string
	cJSON *root = cJSON_Parse(task->json_msg);
	if (root == NULL) {
		return 0; // failed
	}

	// create a app_request_t struct and populate it with the parsed data
//    app_request_t *app_request = malloc(sizeof(app_request_t));
	eh_event_t *app_request = malloc(sizeof(eh_event_t));
	db_resp_pre_post_burnin_t *db_ppb = malloc(
			sizeof(db_resp_pre_post_burnin_t));

	if (app_request == NULL) {
		cJSON_Delete(root); // clean up memory
		return 0; // failed
	}

	// get and set the chipID
	cJSON *chip_id = cJSON_GetObjectItem(root, "chip_id");
	if (chip_id != NULL && cJSON_IsNumber(chip_id)) {
		db_ppb->chipID = chip_id->valueint;
	}

	// get and set the vas_offset and qc_offset
	cJSON *vas_value = cJSON_GetObjectItem(root, "vas_value");
	cJSON *qc_value = cJSON_GetObjectItem(root, "qc_value");
	if (vas_value != NULL && cJSON_IsNumber(vas_value) && qc_value != NULL
			&& cJSON_IsNumber(qc_value)) {
		db_ppb->vas_cal_val = vas_value->valuedouble;
		db_ppb->qc_cal_val = qc_value->valuedouble;
	}

	// Set the array list to 0
	db_ppb->num_burnin = 0;
	// get and set the offset_array
	cJSON *burnin_value = cJSON_GetObjectItem(root, "burnin_value");
	if (burnin_value != NULL && cJSON_IsArray(burnin_value)) {
		int array_size = cJSON_GetArraySize(burnin_value);
		for (int i = 0; i < array_size && i < 10; i++) {
			cJSON *item = cJSON_GetArrayItem(burnin_value, i);
			if (item != NULL && cJSON_IsNumber(item)) {
				db_ppb->burnin_val[i] = item->valueint;
				db_ppb->num_burnin++;
			}
		}
	}

	// set the id
	app_request->msg_id = task->id;
	app_request->type = DB_GET_PRE_POST_BURNIN;

	// call the callback function with the populated app_request_t struct
//	task->response_cb(app_request);

	// clean up memory
	free(app_request);
	cJSON_Delete(root);

	return 1; // succeeded
}

