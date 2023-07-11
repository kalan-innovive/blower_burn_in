/*
 * test_event_handler.c
 *
 *  Created on: Jun 20, 2023
 *      Author: ekalan
 */

#include <stdio.h>
#include <stdlib.h>
#include <cJSON.h>
#include "test_event_handler.h"
#include "esp_types.h"
#include "esp_event.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_log.h"
#include "app_event_handler.h"
//#include <assert.h>
#include <string.h>
#include "garbage_collector.h"

const static char *TAG = "test_eh";

//#ifndef T_LOG
//#define COLOR_BLUE "\x1b[34m"
//#define COLOR_RESET "\x1b[0m"
//#define T_LOG(tag, message, ...) esp_log_write(ESP_LOG_INFO, tag, COLOR_BLUE "[%s] " message COLOR_RESET "\n", tag, ##__VA_ARGS__)
//#endif
/**
 * Prototypes
 */
static esp_err_t test_server_eh_resp(void);

static char* create_json_from_struct(const db_resp_pre_post_burnin_t *ppb);
static esp_err_t test_get_eh_event_id_string(void);
static esp_err_t test_db_pre_post_parser(void);
static esp_err_t test_server_eh_process_resp(void);
static esp_err_t test_to_valuint();

/**
 * Static test members
 */
static const char *node_name = "esp40";
#define NUM_EVENTS 3

static db_resp_pre_post_burnin_t ppb_test = {
		.chipID = 98765,
		.msg = NULL,
		.burnin_len = 3,
		.vas_cal_val = 12,
		.qc_cal_val = 17,
		.num_burnin = 3,
		.burnin_val = { 10, 11, 13, 0 },
};

static eh_event_t e_good =
		{
				.type = DB_RESP,
				.msg_id = 30,
				.msg_struct =
						(void*)
						"{\"node_name\": \"esp3\", \"cmd\": \"calibration/pre_post_burnin\", \"chip_id\": 482184725, \"vas_val\": 34, \"qc_val\": 35, \"burnin_value\" :[30, 28]}",
				.valid = 1,
		};

static eh_event_t e_bad1 =
		{
				.type = DB_RESP,
				.msg_id = 101,
				.msg_struct =
						(void*)
						"\"node_name\": \"esp3\", \"cmd\": \"calibration/pre_post_burnin\", \"chip_id\": 482184725, \"vas_val\": 34, \"qc_val\": 35, \"burnin_value\" :[30, 28]}",
				.valid = 1,
		};

static eh_event_t e_bad2 =
		{
				.type = DB_RESP,
				.msg_id = 102,
				.msg_struct =
						(void*)
						"{\"node_name\": \"esp3\", \"cmd\": \"calibration/pre_post_burnin\",\"qc_val\": 35, \"burnin_value\" :[30, 28]}",
				.valid = 1,
		};

static eh_event_t event_list[NUM_EVENTS];

/**
 * system test the eh_handler
 */
esp_err_t test_event_handler_system(void) {
	// Setup the events to test the loop
	esp_err_t err = ESP_OK;
	event_list[0] = e_good;
	event_list[1] = e_bad1;
	event_list[2] = e_bad2;

	// Initialize the event handler loop
	init_event_handler();

	T_LOG(TAG, "%s, Testing test_server_eh_resp", __FUNCTION__);
	ESP_ERROR_CHECK(test_server_eh_resp());
	vTaskDelay(500 / portTICK_PERIOD_MS);

	deinit_event_handler();
	vTaskDelay(500 / portTICK_PERIOD_MS);

	return err;
}
/*
 Can you create a python script theat acts as a mqtt server. The server has a file that it savess data to called board_cal.json. board_cal.json loads a list of json objects into a list called board_list.
 The list object is defined as:
 {"chip_id":123456,
 vas_val: -23.0,
 qc_val: -22.0,
 burnin_value":[-24.0,-21.0]
 }
 The script should create a mqtt client that subscribes to the folleowing topcs: esp/set and esp/get.
 The esp/set topic gets a json string in the format :"{"node_name": "esp140", "cmd": "calibration", "chip_id": 1234567, "value":-25.0}"
 the callback handles the message by looking up the chip id in board_list and adding the "value" to the burnin_value list in the corresponding chip_id json object and saving the updated list back to board_cal.json file
 if the chip_id is not found it adds a new object with the chipid and sets the vas_value and qc_val to 0xffff.
 The esp/set topic gets a json string in the format :"{"node_name": "esp140", "cmd": "calibration/pre_post_burnin", "chip_id": 1234567}"
 it looks up the chip_id in the board_list and appends the vas_value, qc_val, and burnin_value to the original json message in the format:
 "{"node_name": "esp140",
 "cmd": "calibration/pre_post_burnin",
 "chip_id": 1234567
 " 	 vas_val: -23.0,
 qc_val: -22.0,
 burnin_value":[-24.0,-21.0, -25.0]}"
 It then posts the json string to the topic contained in the node_name ex: esp140
 */
/**
 * system test the eh_handler
 */
esp_err_t test_event_handler_unit(void) {

	esp_err_t err = ESP_OK;

	// Helper funciton testst
	T_LOG(TAG, "%s, Testing test_garbage_collector", __FUNCTION__);
	ESP_ERROR_CHECK(test_to_valuint());

//	ESP_ERROR_CHECK(test_garbage_collector());
//
//	// Initialize the event handler loop
//
//	T_LOG(TAG, "%s, Testing test_get_eh_event_id_string", __FUNCTION__);
//	ESP_ERROR_CHECK(test_get_eh_event_id_string());
//
//	T_LOG(TAG, "%s, Testing test_db_pre_post_parser", __FUNCTION__);
//	ESP_ERROR_CHECK(test_db_pre_post_parser());
//
//	T_LOG(TAG, "%s, Testing test_server_eh_process_resp", __FUNCTION__);
//	ESP_ERROR_CHECK(test_server_eh_process_resp());
//
//	vTaskDelay(500 / portTICK_PERIOD_MS);

	return err;
}

/**
 * @brief event handler function to test eh response
 *  - Called by event handler when a DB_GET_PRE_POST_BURNIN is posted
 */
void test_eh_response(void *handler_arg, esp_event_base_t base,
		int32_t id, void *event_data) {

	T_LOG(TAG, "%s, Received from loop id: %ld", __FUNCTION__, id);

	// Cast handler_arg to eh_event_t
	eh_event_t *event = (eh_event_t*) event_data;

	if (event == NULL) {
		ESP_LOGE(TAG, "%s, Error: event_data==NULL", __FUNCTION__);
		return;
	}

	// Check that the event is valid, the msg_id is 30, and the type DB_RESP_UNPACKED
	if (event->valid && event->msg_id < 100
			&& event->type == DB_RESP_UNPACKED) {

		ESP_LOGD(TAG, "%s, Event valid=%d, msg_id=%d, type=%d",
				__FUNCTION__, event->valid, event->msg_id, event->type);

		// Cast msg_struct into a db_resp_pre_post_burnin_t
		db_resp_pre_post_burnin_t *data =
				(db_resp_pre_post_burnin_t*) event->msg_struct;
		assert(data != NULL); // Assuming msg should not be NULL

		// Now you can access the members of the db_resp_pre_post_burnin_t struct
		ESP_LOGD(TAG, "chipID: %u", data->chipID);
		ESP_LOGD(TAG, "burnin_len: %u", data->burnin_len);
		ESP_LOGD(TAG, "vas_cal_val: %d", data->vas_cal_val);
		ESP_LOGD(TAG, "qc_cal_val: %d", data->qc_cal_val);
		ESP_LOGD(TAG, "num_burnin: %d", data->num_burnin);
		for (int i = 0; i < data->num_burnin; i++) {
			ESP_LOGD(TAG, "burnin_val[%d]: %d", i, data->burnin_val[i]);
		}
	} else if (event->msg_id >= 100 && event->valid == 0) {
		// to test different failures
		ESP_LOGD(TAG, "%s, Event valid=%d, msg_id=%d, type=%d",
				__FUNCTION__, event->valid, event->msg_id, event->type);

	} else {
		ESP_LOGE(TAG, "%s, Error: Event valid=%d, msg_id=%d, type=%d",
				__FUNCTION__, event->valid, event->msg_id, event->type);
	}
}

static esp_err_t test_server_eh_resp(void) {
	// Register event with DB_GET_PRE_POST_BURNIN

	esp_event_base_t base = get_event_handler_base();
	esp_event_loop_handle_t loop = get_event_handler_loop();
	esp_event_loop_run(loop, 100);
	vTaskDelay(10);

	T_LOG(TAG, "%s, Testing server event handler posting event.\n",
			__FUNCTION__);

	ESP_ERROR_CHECK(
			esp_event_handler_instance_register_with(loop, base, DB_GET_PRE_POST_BURNIN, test_eh_response, &loop, NULL));

	eh_event_t *e = NULL;
	for (int i = 0; i < NUM_EVENTS; i++) {
		e = &(event_list[i]);

		esp_event_loop_run(loop, 100);
		vTaskDelay(10);

		T_LOG(TAG,
				"%s, Created event:%p, type:DB_RESP, msg_id:%d msg_struct:%s valid:%d",
				__FUNCTION__, (void* ) e, e->msg_id,
				(char* ) e->msg_struct,
				e->valid);

		T_LOG(TAG,
				"%s, Posting to loop:%p, base:%s, id:%d %s calling test_eh_response.",
				__FUNCTION__, (void* ) loop, base, SERVER_EH_RESPONSE,
				"DB_GET_PRE_POST_BURNIN");
		ESP_ERROR_CHECK(
				esp_event_post_to(loop, base, SERVER_EH_RESPONSE, e, sizeof(*e), portMAX_DELAY));

		esp_event_loop_run(loop, 100);
		vTaskDelay(10);

	}

	return ESP_OK;
}

static char* create_json_from_struct(const db_resp_pre_post_burnin_t *ppb) {
	cJSON *root = cJSON_CreateObject();

	cJSON_AddStringToObject(root, "node_name", node_name);
	cJSON_AddStringToObject(root, "cmd", cal_pre_post_burn_str);
	cJSON_AddNumberToObject(root, "chip_id", ppb->chipID);
	cJSON_AddNumberToObject(root, "vas_val", ppb->vas_cal_val);
	cJSON_AddNumberToObject(root, "qc_val", ppb->qc_cal_val);

	cJSON *burnin_values = cJSON_CreateIntArray(ppb->burnin_val,
			ppb->num_burnin);
	cJSON_AddItemToObject(root, "burnin_value", burnin_values);

	char *json_output = cJSON_Print(root);

	cJSON_Delete(root);
	return json_output;
}

/**
 * @brief Unit test for get_eh_event_id_string
 */
static esp_err_t test_get_eh_event_id_string(void) {
	assert(strcmp(get_eh_event_id_string(MSG16_REQUEST), "MSG16_REQUEST") == 0);
	assert(
			strcmp(get_eh_event_id_string(SETTINGS_REQUEST), "SETTINGS_REQUEST")
					== 0);
	assert(
			strcmp(get_eh_event_id_string(SERVER_EH_RESPONSE),
					"SERVER_EH_RESPONSE") == 0);
	assert(
			strcmp(get_eh_event_id_string(SERVER_EH_REQUEST),
					"SERVER_EH_REQUEST") == 0);
	assert(strcmp(get_eh_event_id_string(DB_REQUEST), "DB_REQUEST") == 0);
	assert(
			strcmp(get_eh_event_id_string(DB_GET_PRE_POST_BURNIN),
					"DB_GET_PRE_POST_BURNIN") == 0);
	assert(
			strcmp(get_eh_event_id_string(DB_SET_CALIBRATION),
					"DB_SET_CALIBRATION") == 0);
	assert(
			strcmp(get_eh_event_id_string(DB_LIST_BUCKETS), "DB_LIST_BUCKETS")
					== 0);

	// Testing an unknown event id
	assert(strcmp(get_eh_event_id_string(12345), "Unknown event id") == 0);

	T_LOG(TAG, "%s,All assertions passed", __FUNCTION__);

	return ESP_OK;
}

static esp_err_t test_server_eh_process_resp(void) {

	eh_event_t *result = server_eh_process_resp(&e_good);

	assert(result->msg_id == e_good.msg_id);

	ESP_RETURN_ON_FALSE(result->type == DB_RESP_UNPACKED, ESP_FAIL, TAG,
			"result->type=%d", result->type);

	assert(result->msg_struct != NULL);
	assert(result->valid == 1);

	if (result == NULL) {
		// Failed to process the event.
		return ESP_FAIL;
	}

	// Successfully processed the event. Do something with result if necessary.

	// Free the result when you're done with it.
	free(result);

	T_LOG(TAG, "%s, All assertions passed", __FUNCTION__);

	return ESP_OK;
}

static esp_err_t test_db_pre_post_parser(void) {
	char *buff =
			"{\"node_name\": \"esp3\", \"cmd\": \"calibration/pre_post_burnin\", \"chip_id\": 482184725, \"vas_val\": 34, \"qc_val\": 35, \"burnin_value\" :[30, 28]}";

	db_resp_pre_post_burnin_t *ppb = NULL;
	T_LOG(TAG, "%s,     db_ppb:%p , &db_ppb:%p , size of (*db_ppb):%d",
			__FUNCTION__,
			(void* ) ppb, (void* ) &ppb, sizeof(*ppb));

	cJSON *root = cJSON_Parse(buff);

	// Send the json object and the pointer to event
	esp_err_t err = db_pre_post_parser(root, &ppb);
	T_LOG(TAG, "%s,     db_ppb:%p , &db_ppb:%p , size of (*db_ppb):%d",
			__FUNCTION__,
			(void* ) ppb, (void* ) &ppb, sizeof(*ppb));

	assert(ppb != NULL);
	assert(ppb->chipID = 482184725);
	assert(ppb->msg == NULL);
	assert(ppb->burnin_len == 0);
	assert(ppb->vas_cal_val == 34);
	assert(ppb->qc_cal_val == 35);
	assert(ppb->num_burnin == 2);
	assert(ppb->burnin_val[0] == 30);
	assert(ppb->burnin_val[1] == 28);

	print_ppb(ppb);
	free(ppb);

	T_LOG(TAG, "%s, All assertions passed", __FUNCTION__);

	return err;
}

#include <limits.h>

static esp_err_t test_to_valuint()
{
	esp_err_t err = ESP_OK;

	unsigned max_val = 0xffffffff;
//	printf(
//			"%s Max double: %lld, Max unsigned%u;  size of double: %zu num bits:%d",
//			__FUNCTION__,
//			LONG_LONG_MAX, max_val, sizeof(double), (int) sizeof(double) * 8);
	double max_d = 0xffffffffffff;
	T_LOG(TAG,
			"%s Max double: %lf;  size of double: %d num bits:%d",
			__FUNCTION__, max_d
			, sizeof(double), sizeof(double) * 8);

	T_LOG(TAG, "%s Max unsigned: %u;  size of unsigned: %d num bits:%d",
			__FUNCTION__,
			UINT_MAX, sizeof(unsigned), sizeof(unsigned) * 8);

	// Test case 1: Valid unsigned 32-bit integer
	unsigned val = max_val - 1;
	cJSON *json1 = cJSON_CreateNumber((double) val);
	unsigned int result1 = to_valuint(json1);

	err &= (result1 == val) ? ESP_OK : ESP_FAIL;
	T_LOG(TAG, "%s, Test 1 - Expected: %u, Actual: %u", __FUNCTION__, val,
			result1);
	cJSON_Delete(json1);

	// Test case 2: Valid unsigned 32-bit integer exceeding UINT_MAX
	cJSON *json2 = cJSON_CreateNumber(4294967296);
	unsigned int result2 = to_valuint(json2);

	T_LOG(TAG, "%s, Test 2 - Expected(4294967296):4294967295, Actual: %u",
			__FUNCTION__,
			result2);
	err &= (result2 == 4294967295) ? ESP_OK : ESP_FAIL;
	cJSON_Delete(json2);

	// Test case 3: Invalid input (not a number)
	cJSON *json3 = cJSON_CreateString("invalid");
	unsigned int result3 = to_valuint(json3);

	T_LOG(TAG,
			"%s,Test 3 - Invalid entry value should return 0 (invalid), Actual: %u\n",
			__FUNCTION__, result3);
	err &= (result3 == 0) ? ESP_OK : ESP_FAIL;
	cJSON_Delete(json3);

	return err;
}

