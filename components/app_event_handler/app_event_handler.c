#include <stdio.h>

#include "app_event_handler.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "cJSON.h"
#include "msg16.h"

extern queue_task_t task;
extern QueueHandle_t task_queue_handle;

// Static variables
static const char* TAG = "APP_EH";

// Prototypes
static void msg16_request_handler(app_request_t* request);
static void settings_request_handler(app_request_t* request);
static void db_request_handler(app_request_t* request);


void event_handler_task(void* pvParams) {
    while (1) {
        // wait for a task in the queue
        queue_task_t task;
        if (xQueueReceive(task_queue_handle,(void*) &task, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        // create an app request from the queue task
        app_request_t request = {
            .msg_id = task.id,
            .respons_cb = (void (*)) task.respons_cb,
            .request = task.json_msg
        };

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


static void msg16_request_handler(app_request_t* request) {
    // parse the json message into a msg16 struct
    cJSON* json = cJSON_Parse(request->request);
    if (json == NULL) {
        request->respons_cb((void*) request);
        return;
    }
    cJSON* msg16_json = cJSON_GetObjectItem(json, "msg16");
    if (msg16_json == NULL) {
        cJSON_Delete(json);
        request->respons_cb((void*) request);
        return;
    }

    char* msg16_str = msg16_json->valuestring;
    char* token = strtok(msg16_str, "|");
    if (token == NULL || strcmp(token, "write") != 0) {
        cJSON_Delete(json);
        request->respons_cb((void*) request);
        return;
    }

    msg16_t msg16;
    msg16.type = 20;

    token = strtok(NULL, "|");
    if (token == NULL) {
        cJSON_Delete(json);
        request->respons_cb((void*) request);
        return;
    }
    msg16.dev_id = (uint16_t) atoi(token);

    token = strtok(NULL, "|");
    if (token == NULL) {
        cJSON_Delete(json);
        request->respons_cb((void*) request);
        return;
    }
    msg16.addr = (uint16_t) atoi(token);

    token = strtok(NULL, "|");
    if (token == NULL) {
        cJSON_Delete(json);
        request->respons_cb((void*) request);
        return;
    }
    msg16.len = (uint16_t) atoi(token);

//    msg16.msg_val = malloc(msg16.len * sizeof(uint16_t));
//    if (msg16.msg_val == NULL) {
//        cJSON_Delete(json);
//        request->respons_cb((void*) request);
//        return;
//    }

    for (int i = 0; i < msg16.len; i++) {
        token = strtok(NULL, "|");
        if (token == NULL) {
            free(msg16.msg_val);
            cJSON_Delete(json);
            request->respons_cb((void*) request);
            return;
        }
        msg16.msg_val[i] = (uint16_t) atoi(token);
    }

    // do something with the msg16 struct
    // ...

    // free memory and callback with success
    free(msg16.msg_val);
    cJSON_Delete(json);
    request->respons_cb(0);
}


static void db_request_handler(app_request_t* request) {
	ESP_LOGI(TAG, "Request for db is zero");

}


static void settings_request_handler(app_request_t* request) {
	ESP_LOGI(TAG, "Request for settings is zero");

}

