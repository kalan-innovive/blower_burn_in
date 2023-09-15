/*
 * server_eh.c
 *
 *  Created on: April 9, 2023
 *      Author: ekalan
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "app_event_handler.h"
#include "esp_event.h"
//#include "sdkconfig.h"

#include "cJSON.h"
#include "garbage_collector.h"

/**
 * @brief Event Handle config
 *
 */
#define CONFIG_EH_PARSER_RING_BUFFER_SIZE 1024
#define CONFIG_EH_PARSER_TASK_PRIORITY 2
#define CONFIG_EH_PARSER_TASK_STACK_SIZE 2048

#define EH_PARSER_RUNTIME_BUFFER_SIZE (CONFIG_EH_PARSER_RING_BUFFER_SIZE / 2)
#define EH_MAX_STATEMENT_ITEM_LENGTH (16)
#define EH_EVENT_LOOP_QUEUE_SIZE (16)

/**
 * Globel event string definitions
 */
const char *cal_pre_post_burn_str = "calibration/pre_post_burnin";
const char *cal_30_min_str = "calibration/30minlast";
const char *set_cal = "set/calibration";
const char *qc_val_str = "qc_value";
const char *burnin_val_str = "burnin_value";
const char *chip_id_str = "chip_id";
const char *cmd_str = "cmd";
const char *node_name_str = "node_name";

static const char *TAG = "user_event_loop";

/**
 * @brief Define of EH Parser Event base
 *
 */

ESP_EVENT_DEFINE_BASE(APP_EH_EVENT);
ESP_EVENT_DEFINE_BASE(SERIAL_EVENT);
ESP_EVENT_DEFINE_BASE(SERVER_EH_EVENT);
ESP_EVENT_DEFINE_BASE(SETTINGS_EVENT);


esp_event_loop_handle_t eh_loop;

/**
 * Static Prototypes
 */
static void resp_cmd_event_handler(void *handler_arg, esp_event_base_t base,
		int32_t id, void *event_data);

static server_event_t srv_cmd_str_to_enum(const char *cmd_str);

/*
 * @brief- Function to initialize the event loop and register your events
 *
 */
esp_err_t init_event_handler() {
	esp_event_loop_args_t loop_args = {
			.queue_size = 10,
			.task_name = "app_eh_event",
			.task_priority = 6,
			.task_stack_size = 2048 * 4,
			.task_core_id = tskNO_AFFINITY };
	ESP_LOGI(TAG, "%s, APP Event Loop initiated", __FUNCTION__);
	ESP_ERROR_CHECK(esp_event_loop_create(&loop_args, &eh_loop));
	ESP_ERROR_CHECK(
			esp_event_handler_register_with(eh_loop, APP_EH_EVENT, ESP_EVENT_ANY_ID, resp_cmd_event_handler, &eh_loop));
	ESP_LOGI(TAG, "%s, APP Event Loop Garbage Collector Initiated",
			__FUNCTION__);

	garbage_collector_init();
	return ESP_OK;
}

/**
 * @brief Deinit app_event handler Parser
 *
 * @param nmea_hdl handle of NMEA parser
 * @return esp_err_t ESP_OK on success,ESP_FAIL on error
 */
esp_err_t deinit_event_handler(void)
{
	if (eh_loop == NULL) {
		return ESP_OK;
	}
	esp_err_t err = esp_event_loop_delete(eh_loop);
	eh_loop = NULL;
	garbage_collector_deinit();
	return err;
}

esp_event_base_t get_event_handler_base() {
	ESP_LOGI(TAG, "%s, Event Base = %s", __FUNCTION__, APP_EH_EVENT);
	return APP_EH_EVENT;
}

esp_event_loop_handle_t get_event_handler_loop() {
	ESP_LOGI(TAG, "%s, Event Loop = %p", __FUNCTION__, (void* ) eh_loop);
	return eh_loop;
}

/**
 * @Brief
 *  - Converts the event_cmd string into an enum
 *  @ret returns the enum if found
 *   - returns NAN_CMD 0 if not found
 */
static server_event_t srv_cmd_str_to_enum(const char *cmd_str) {

	if (strcmp(cmd_str, cal_pre_post_burn_str) == 0) {
		return PRE_POST_BURN;
	} else if (strcmp(cmd_str, cal_30_min_str) == 0) {
		return CAL_30_MIN;
	} else if (strcmp(cmd_str, set_cal) == 0) {
		return SET_CAL;
	} else {
		// Handle unknown input string here
		return NAN_CMD;/* some default value */
		ESP_LOGW(TAG, "%s, Error parsing command and string: [%s] ",
				__FUNCTION__,
				cmd_str);
	}
}


// The event handler
// Receives incoming requests.
/*
 * Parses Requests and post to correct event topic
 * Ex. Receives mqtt data on topic "calibration/pre_post_burnin"
 *  - packs the response into the proper struct
 */
static void resp_cmd_event_handler(void *handler_arg, esp_event_base_t base,
		int32_t id, void *event_data) {

	eh_event_t *e = (eh_event_t*) event_data;

	esp_event_loop_handle_t *app_loop = (esp_event_loop_handle_t) handler_arg;

	ESP_LOGD(TAG, "%s, loop address: %p",
			__FUNCTION__, (void* )app_loop);

//	eh_event_id_t event_id = (eh_event_id_t) e->type;

	ESP_LOGI(TAG, "EH_resp received, %ld", id);

	eh_event_t *new_event = NULL;
	switch (id) {
//		case MSG16_REQUEST:
//			// Handle MSG16_REQUEST
//			ESP_LOGI(TAG, "%s, MSG16_REQUEST type:%s, msg_id=%d, valid=%s",
//					__FUNCTION__, get_eh_event_id_string(e->type), e->msg_id,
//					(e->valid) ? "True" : "False");
//			break;
//		case SETTINGS_REQUEST:
//			// Handle SETTINGS_REQUEST
//			ESP_LOGI(TAG, "%s, SETTINGS_REQUEST type:%s, msg_id=%d, valid=%s",
//					__FUNCTION__, get_eh_event_id_string(e->type), e->msg_id,
//					(e->valid) ? "True" : "False");
//			break;
		case SERVER_EH_RESPONSE:
			ESP_LOGD(TAG, "%s, SERVER_EH_RESPONSE type:%s, msg_id=%d, valid=%s",
					__FUNCTION__, get_eh_event_id_string(e->type), e->msg_id,
					(e->valid) ? "True" : "False");

			new_event = server_eh_process_resp(e);

			ESP_ERROR_CHECK(
					esp_event_post_to(*app_loop, base, DB_GET_PRE_POST_BURNIN, new_event, sizeof(*new_event), portMAX_DELAY));
			esp_event_loop_run(*app_loop, 100);
			vTaskDelay(5 / portTICK_PERIOD_MS);

			delete_event(new_event); // Free the memory allocated in server_eh_process_resp

			break;
		case DB_REQUEST:
			// Handle DB_REQUEST: once the message is parsed it is posted on the proper handle
			ESP_LOGD(TAG, "%s, DB_REQUEST type:%s, msg_id=%d, valid=%s",
					__FUNCTION__, get_eh_event_id_string(e->type), e->msg_id,
					(e->valid) ? "True" : "False");
			break;
		case DB_GET_PRE_POST_BURNIN:
			// Handle DB_GET_PRE_POST_BURNIN
			ESP_LOGD(TAG,
					"%s, DB_GET_PRE_POST_BURNIN type:%s, msg_id=%d, valid=%s, msg_struct=%p NULL=%d",
					__FUNCTION__, get_eh_event_id_string(e->type), e->msg_id,
					(e->valid) ? "True" : "False", (void* )e->msg_struct,
					e->msg_struct == NULL);

			break;
		case DB_SET_CALIBRATION:
			// Handle DB_SET_CALIBRATION
			ESP_LOGD(TAG,
					"%s, DB_SET_CALIBRATION type:%s, msg_id=%d, valid=%s",
					__FUNCTION__, get_eh_event_id_string(e->type), e->msg_id,
					(e->valid) ? "True" : "False");
			break;
		case DB_LIST_BUCKETS:
			// Handle DB_LIST_BUCKETS
			// Handle DB_SET_CALIBRATION
			ESP_LOGD(TAG,
					"%s, DB_LIST_BUCKETS type:%s, msg_id=%d, valid=%s",
					__FUNCTION__, get_eh_event_id_string(e->type), e->msg_id,
					(e->valid) ? "True" : "False");
			break;
		default:
			// Handle unknown id
			ESP_LOGD(TAG,
					"%s, Handle unknown id type:%s, msg_id=%d, valid=%s",
					__FUNCTION__, get_eh_event_id_string(e->type), e->msg_id,
					(e->valid) ? "True" : "False");
			break;
	}
}















/**
 * @brief Parses incoming json messages from the event handler server
 *
 * @param eh_event_t *event
 *  - Event ->msg_struct contains the buffer to be parsed
 *   will be created and posted to the eventhandler on successful parsing
 *  @return
 *   - eh_event_t *event on Success
 *   - NULL failed initialization
 *   Reminder call must free event handle memory on success
 */
eh_event_t* server_eh_process_resp(eh_event_t *event) {

	/*Todo: move to event checker function*/
	eh_event_t *e = malloc(sizeof(eh_event_t)); // freed int resp_cmd_event_handler
	e->msg_id = 0;
	e->msg_struct = NULL;
	e->type = NAN_CMD;
	e->valid = 0;

	// Check NULL
	if (event == NULL) {
		ESP_LOGE(TAG, "%s, Received NULL event", __FUNCTION__);
		return e;
	}

	// Expecting the msg_struct to be a char* that is json readable
	const char *const msg = event->msg_struct;
	e->msg_id = event->msg_id;

	ESP_LOGI(TAG,
			"%s, Handle server event: %p, msg:%s, type:%d, msg_id=%d, valid=%s",
			__FUNCTION__, (void* ) event, msg, event->type, event->msg_id,
			(event->valid) ? "True" : "False");

	if (msg == NULL || (strlen(msg) == sizeof(msg) - 1)) {
		ESP_LOGE(TAG,
				"%s, Error msg is wither null or is not null terminated: [%s]",
				__FUNCTION__, msg);
	}

	ESP_LOGD(TAG, "%s, JSON parsing [%s]", __FUNCTION__, msg);

	cJSON *root = cJSON_Parse(msg);

	ESP_LOGI(TAG, "%s, Json parsed message. root:%p",
			__FUNCTION__, root);

	if (root == NULL) {
		ESP_LOGE(TAG, "%s, JSON Error error_ptr==NULL: [%s]",
				__FUNCTION__, msg);

		goto end;
	}
	ESP_LOGD(TAG, "%s, JSON getting cmd ", __FUNCTION__);

	// Re-route to the correct parser
	cJSON *cmd = cJSON_GetObjectItem(root, "cmd");
	if (cmd == NULL) {
		ESP_LOGE(TAG, "%s, JSON[cmd] Error error_ptr==NULL: [%s]",
				__FUNCTION__, msg);

		goto end;
	}

	server_event_t cmd_t = srv_cmd_str_to_enum(cmd->valuestring);

	ESP_LOGI(TAG, "%s, Server Event Command:%d,  %s", __FUNCTION__,
			cmd_t, cmd->valuestring);

	switch (cmd_t) {
		case NAN_CMD:
			// Error Reading Command
			break;
		case PRE_POST_BURN:
			ESP_LOGI(TAG, "%s, PRE_POST_BURN: ", __FUNCTION__);

			// Send to parser
			db_resp_pre_post_burnin_t *ppb = NULL;

			ESP_LOGD(TAG, "%s, db_ppb:%p , &db_ppb:%p , size of (*db_ppb):%d",
					__FUNCTION__, (void* ) ppb, (void* )&ppb, sizeof(*ppb));

			esp_err_t ret = db_pre_post_parser(root, &ppb);

			ESP_LOGD(TAG, "%s, db_ppb:%p , &db_ppb:%p , size of (*db_ppb):%d",
					__FUNCTION__, (void* ) ppb, (void* )&ppb, sizeof(*ppb));

			ESP_LOGI(TAG, "%s, parsed json result: %s , %d", __FUNCTION__,
					esp_err_to_name(ret), ret);

			// Create event and post to handler
			e->type = DB_RESP_UNPACKED;
			e->msg_struct = ppb;
			e->valid = (ret == ESP_OK) ? 1 : 0;

			break;
		case SET_CAL:
			break;
		case CAL_30_MIN:
			break;
		default:
			// Should not reach
			ESP_LOGE(TAG, "%s, Reached default error; cmd_t:%d , cmd:%s ",
					__FUNCTION__, cmd_t, cmd->string);
	}
	end:
	cJSON_Delete(root);
	return e;
}

esp_err_t db_pre_post_parser(cJSON *root, db_resp_pre_post_burnin_t **db_ppb) {
//	if (root == NULL) {
//		ESP_LOGW(TAG, "%s, Error before: [%s]\n", __FUNCTION__,
//				cJSON_GetErrorPtr());
//		return ESP_FAIL;
//	}
	*db_ppb = (db_resp_pre_post_burnin_t*) malloc(
			sizeof(db_resp_pre_post_burnin_t)); // Freed in resp_cmd_event_handler

	ESP_LOGD(TAG, "%s, db_ppb:%p , &db_ppb:%p , size of (*db_ppb):%d",
			__FUNCTION__,
			(void* ) *db_ppb, (void* )*&db_ppb, sizeof(**db_ppb));

	(*db_ppb)->msg = NULL;
	(*db_ppb)->burnin_len = 0; // Seems redundant

	cJSON *chip_id = cJSON_GetObjectItem(root, "chip_id");
	cJSON *vas_value = cJSON_GetObjectItem(root, "vas_val");
	cJSON *qc_value = cJSON_GetObjectItem(root, "qc_val");
	cJSON *burnin_value = cJSON_GetObjectItem(root, "burnin_value");

	if (!cJSON_IsNumber(chip_id) || !cJSON_IsNumber(vas_value)
			|| !cJSON_IsNumber(qc_value) || !cJSON_IsArray(burnin_value)) {
		// Log that we didn't find a mandatory field
		return ESP_ERR_INVALID_ARG;
	}
	unsigned c_id = (unsigned) chip_id->valueint;
	ESP_LOGD(TAG, "JSON max value:%d ", INT_MAX);
	ESP_LOGD(TAG, "%s, chipID:%d , %u ", __FUNCTION__,
			chip_id->valueint, c_id);
	ESP_LOGD(TAG, "%s, vas_value:%f ", __FUNCTION__,
			vas_value->valuedouble);
	ESP_LOGD(TAG, "%s, qc_value:%f ", __FUNCTION__,
			qc_value->valuedouble);

	(*db_ppb)->chipID = (unsigned) chip_id->valuedouble;
	(*db_ppb)->vas_cal_val = vas_value->valuedouble;
	(*db_ppb)->qc_cal_val = qc_value->valuedouble;

// Loop over burnin_value array
	int burnin_len = cJSON_GetArraySize(burnin_value);
	if (burnin_len > MAX_CALIBRATION_VALUE_LEN)
		burnin_len = MAX_CALIBRATION_VALUE_LEN;
//	db_ppb->burnin_len = burnin_len;

	for (int i = 0; i < burnin_len; i++) {
		cJSON *item = cJSON_GetArrayItem(burnin_value, i);
		if (cJSON_IsNumber(item)) {
			(*db_ppb)->burnin_val[i] = item->valueint;
		}

	}
	(*db_ppb)->num_burnin = burnin_len;

	return ESP_OK;
}

const char* get_eh_event_id_string(eh_event_id_t event_id) {
	ESP_LOGI(TAG, "%s, event_id: %d,", __FUNCTION__, event_id);

	switch (event_id) {
		case MSG16_REQUEST:
			return "MSG16_REQUEST";
		case MSG16_RESPONSE:
			return "MSG16_RESPONSE";
		case SETTINGS_REQUEST:
			return "SETTINGS_REQUEST";
		case SERVER_EH_RESPONSE:
			return "SERVER_EH_RESPONSE";
		case SERVER_EH_REQUEST:
			return "SERVER_EH_REQUEST";
		case DB_REQUEST:
			return "DB_REQUEST";
		case DB_GET_PRE_POST_BURNIN:
			return "DB_GET_PRE_POST_BURNIN";
		case DB_SET_CALIBRATION:
			return "DB_SET_CALIBRATION";
		case DB_LIST_BUCKETS:
			return "DB_LIST_BUCKETS";
		default:
			return "Unknown event id";
	}
}

void print_ppb(db_resp_pre_post_burnin_t *ppb) {
	if (ppb == NULL)
		return;			// Assuming msg should not be NULL

// Now you can access the members of the db_resp_pre_post_burnin_t ruct
	ESP_LOGI(TAG, "chipID: %u", ppb->chipID);
//        printf("msg: %s\n", data->msg);
	ESP_LOGI(TAG, "burnin_len: %u", ppb->burnin_len);
	ESP_LOGI(TAG, "vas_cal_val: %d", ppb->vas_cal_val);
	ESP_LOGI(TAG, "qc_cal_val: %d", ppb->qc_cal_val);
	ESP_LOGI(TAG, "num_burnin: %d", ppb->num_burnin);
	for (int i = 0; i < ppb->num_burnin; i++) {
		ESP_LOGI(TAG, "burnin_val[%d]: %d", i, ppb->burnin_val[i]);
	}
}

void delete_event(eh_event_t *event) {
	if (event == NULL) {
		return; // Nothing to delete
	}

	// add the dynamically allocated memory
	if (event->msg_struct != NULL) {
		delete_event_later((void**) &event->msg_struct);
	}

	// Finally, add the event itself
	delete_event_later((void**) &event->msg_struct);
}

///**
// * @brief Init NMEA Parser
// *
// * @param config Configuration of NMEA Parser
// * @return nmea_parser_handle_t handle of nmea_parser
// */
//nmea_parser_handle_t nmea_parser_init(const nmea_parser_config_t *config)
//{
//    esp_gps_t *esp_gps = calloc(1, sizeof(esp_gps_t));
//    if (!esp_gps) {
//        ESP_LOGE(GPS_TAG, "calloc memory for esp_fps failed");
//        goto err_gps;
//    }
//    esp_gps->buffer = calloc(1, NMEA_PARSER_RUNTIME_BUFFER_SIZE);
//    if (!esp_gps->buffer) {
//        ESP_LOGE(GPS_TAG, "calloc memory for runtime buffer failed");
//        goto err_buffer;
//    }
//
//    /* Set attributes */
//    esp_gps->all_statements &= 0xFE;
//    esp_event_loop_args_t loop_args = {
//        .queue_size = NMEA_EVENT_LOOP_QUEUE_SIZE,
//        .task_name = NULL
//    };
//    if (esp_event_loop_create(&loop_args, &esp_gps->event_loop_hdl) != ESP_OK) {
//        ESP_LOGE(GPS_TAG, "create event loop faild");
//        goto err_eloop;
//    }
//    /* Create NMEA Parser task */
//    BaseType_t err = xTaskCreate(
//                         eh_parser_task_entry,
//                         "eh_parser",
//                         CONFIG_NMEA_PARSER_TASK_STACK_SIZE,
//                         esp_gps,
//                         CONFIG_NMEA_PARSER_TASK_PRIORITY,
//                         &esp_gps->tsk_hdl);
//    if (err != pdTRUE) {
//        ESP_LOGE(GPS_TAG, "create NMEA Parser task failed");
//        goto err_task_create;
//    }
//    ESP_LOGI(GPS_TAG, "NMEA Parser init OK");
//    return esp_gps;
//    /*Error Handling*/
//err_task_create:
//    esp_event_loop_delete(esp_gps->event_loop_hdl);
//err_eloop:
//err_uart_install:
//    uart_driver_delete(esp_gps->uart_port);
//err_uart_config:
//err_buffer:
//    free(esp_gps->buffer);
//err_gps:
//    free(esp_gps);
//    return NULL;
//}
//
//typedef void *mqtt_handler_t;

////Make sure no other tasks are still using UART before you call this function
//esp_err_t mqtt_driver_delete(mqtt_handler_t *config)
//{
//	if (config ==NULL){
//		return ESP_FAIL;
//	}
//
//    return ESP_OK;
//}

/**
 * @brief Deinit NMEA Parser
 *
 * @param nmea_hdl handle of NMEA parser
 * @return esp_err_t ESP_OK on success,ESP_FAIL on error
 */
//esp_err_t nmea_parser_deinit(nmea_parser_handle_t nmea_hdl)
//{
//    esp_gps_t *esp_gps = (esp_gps_t *)nmea_hdl;
//    vTaskDelete(esp_gps->tsk_hdl);
//    free(esp_gps->buffer);
//    free(esp_gps);
//    return ESP_OK;
//}
/**
 * @brief Add user defined handler for NMEA parser
 *
 * @param nmea_hdl handle of NMEA parser
 * @param event_handler user defined event handler
 * @param handler_args handler specific arguments
 * @return esp_err_t
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for the handler
 *  - ESP_ERR_INVALIG_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 */
//esp_err_t eh_parser_add_handler(nmea_parser_handle_t nmea_hdl,
//		esp_event_handler_t event_handler, void *handler_args) {
//	esp_gps_t *esp_gps = (esp_gps_t*) nmea_hdl;
//	return esp_event_handler_register_with(esp_gps->event_loop_hdl,
//			ESP_EH_EVENT,
//			ESP_EVENT_ANY_ID, event_handler, handler_args);
//}
/**
 * @brief Remove user defined handler for NMEA parser
 *
 * @param nmea_hdl handle of NMEA parser
 * @param event_handler user defined event handler
 * @return esp_err_t
 *  - ESP_OK: Success
 *  - ESP_ERR_INVALIG_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 */
//esp_err_t eh_parser_remove_handler(nmea_parser_handle_t nmea_hdl,
//		esp_event_handler_t event_handler) {
//	esp_gps_t *esp_gps = (esp_gps_t*) nmea_hdl;
//	return esp_event_handler_unregister_with(esp_gps->event_loop_hdl,
//			ESP_EH_EVENT,
//			ESP_EVENT_ANY_ID, event_handler);
//}
/**
 * @brief NMEA Parser Task Entry
 *
 * @param arg argument
 */
//static void eh_parser_task_entry(void *arg) {
//	esp_gps_t *esp_gps = (esp_gps_t*) arg;
//	eh_event_t event;
//
//	while (1) {
//		if (xQueueReceive(esp_gps->event_queue, &event, pdMS_TO_TICKS(200))) {
//			switch (event.type) {
//			case MSG16_REQUEST:
//				// Handle MSG16_REQUEST
//				break;
//			case SETTINGS_REQUEST:
//				// Handle SETTINGS_REQUEST
//				break;
//			case DB_REQUEST:
//				// Handle DB_REQUEST
//				break;
//			case DB_GET_PRE_POST_BURNIN:
//				// Handle DB_GET_PRE_POST_BURNIN
//				break;
//			case DB_SET_CALIBRATION:
//				// Handle DB_SET_CALIBRATION
//				break;
//			case DB_LIST_BUCKETS:
//				// Handle DB_LIST_BUCKETS
//				break;
//
//			default:
//				ESP_LOGW(GPS_TAG, "unknown uart event type: %d", event.type);
//				break;
//			}
//		}
//		/* Drive the event loop */
//		esp_event_loop_run(esp_gps->event_loop_hdl, pdMS_TO_TICKS(50));
//	}
//	vTaskDelete(NULL);
//}
