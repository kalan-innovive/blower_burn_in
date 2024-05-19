/*
 * burn_in.c
 *
 *  Created on: May 1, 2023
 *      Author: ekalan
 */

#include <stdio.h>
//#include "esp_heap_caps.h"

#include "string.h"
#include <time.h>
#include "stdlib.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_check.h"
#include "esp_err.h"
#include "bsp_board.h"
#include "bsp/esp-bsp.h"

#include "ui_blower_burn_in.h"
#include "burn_in.h"
#include "serial_inno.h"
#include "offset_data.h"
#include "app_event_handler.h"
#include "gui/ui_main.h"
#include "mqtt_handler.h"
#include "burnin_types.h"
#include "burnin_valve.h"

static const char *TAG = "burn-in";

extern TaskHandle_t burn_in_handle;
extern TaskHandle_t valve_handle;

#define NUM_TESTS 2

/**
 *  Static variables ______
 */

static int test_cycle = 1;
//static int num_points = 3;
//static blower_test_state_t test_blower[4];
static bool rack_on = false;
static bool rack_power_changed;

static unsigned chipid_list[4];
static int testing[4];
static int cur_offset[4];
static unsigned blower_type_list[4];
static int testing_valve;
static int vas_v[4];
static int QC_v[4];
static const dev_id devIDs[4] = { DEV_SUPA, DEV_EXHA, DEV_SUPB, DEV_EXB };

/**
 *  ___Proto Type definitions ______
 */
static void init_burn_in(void);
static void update_ui_blower_vals(burn_in_ui_value_t *brn_val);

static int update_rack_blower_list(void);
static void setup_blower_(burn_in_ui_value_t *brn_val);
//static void update_blower_ui_val(burn_in_test_value_t *brn_val);
static int check_rack_power_changed(void);
static int set_rack_status(bool status);
static void init_blower_test(burn_in_ui_value_t *brn_val);
static int log_calibration_values();
//static int request_ppb_values();
static esp_err_t register_burnin_eh_calback(void);
static void on_ppb_response(void *handler_arg, esp_event_base_t base,
		int32_t id, void *event_data);
static int get_ppb_values();
static int check_power_on();
//static void on_settings_update(void *handler_arg, esp_event_base_t base,
//		int32_t id, void *event_data);

/**
 *  ___Static Function  definitions ______
 */

static void init_burn_in() {
	ESP_LOGI(TAG,
			"%s, Initializing burn in reseting cur_offset cas_v and qc_val  setting power, rack on to default",
			__FUNCTION__);
	testing_valve = 0;
	for (int i = 0; i < 4; i++) {
		testing[i] = 0;
		cur_offset[i] = DEF_OFFSET_VAL;
		vas_v[i] = DEF_OFFSET_VAL;
		QC_v[i] = DEF_OFFSET_VAL;
		blower_type_list[i] = 0;
	}
	check_rack_power_changed();
	rack_on = false;

}
/*
 * sets the rack_on and rack_power_changed flags
 * returns if the rack_power_changed status
 */
static int set_rack_status(bool status) {
	if (status != rack_on) {
		ESP_LOGD(TAG,
				"%s, set new rack status:%d , rack_power_changed:%d, rack_on:%d",
				__FUNCTION__, status, rack_power_changed, rack_on);
		rack_on = status;
		rack_power_changed = true;
	}
	return rack_power_changed;

}

/**
 * Checks if the rack_on status has changed
 * returns if the rack_power_changed status
 * Resets the rack_power_changed status to false
 */
static int check_rack_power_changed() {
	if (rack_power_changed) {
		ESP_LOGD(TAG, "%s, Rack Power changed checked returning 1",
				__FUNCTION__);
		rack_power_changed = false;
		return 1;
	}
	ESP_LOGD(TAG, "%s, Rack Power changed checked returning 0", __FUNCTION__);

	return 0;
}

/**
 * Checks for connected blowers
 * Updates chipid_list, and
 * @return: number of updated devices.
 * Note: only use after rack
 *
 */
static int update_rack_blower_list() {
	int suc;
//	rack_on = false;

	unsigned chipid = 0;
	unsigned valve_type = 0;
	int offset = 0;
	int num_avail = 0;

	for (int i = 0; i < 4; i++) {
		ESP_LOGD(TAG, "%s, Checking for Device:%d", __FUNCTION__, devIDs[i]);
		chipid = 0;
		valve_type= 0;
		offset = DEF_OFFSET_VAL;

		// Send a message and check for response
		vTaskDelay(1);
		suc = get_raw_pressure(devIDs[i], &offset);
		vTaskDelay(1);


		ESP_LOGD(TAG, "%s, Checking for Device:%d, Available: %d, Offset:%d",
				__FUNCTION__, devIDs[i], suc, offset);
		// If we can get the offset check if the chip id is readable
		if (suc == 1) {
			vTaskDelay(1);
			suc = get_chipid(devIDs[i], &chipid);
			vTaskDelay(1);

			if (suc == 1) {

				// Set the current offset value for the chip id
				cur_offset[i] = offset;
				ESP_LOGD(TAG, "%s, Updating Device:%d, ChipID: %d, Offset:%d",
						__FUNCTION__, devIDs[i], chipid, offset);
				chipid_list[i] = chipid;

				// Update the testing list
				testing[i] = 1;
				num_avail += 1;

				vTaskDelay(1);
				suc = get_blower_type(devIDs[i],  &blower_type_list[i]);
				if (suc == 1) {

					// Set the current offset value for the chip id
					cur_offset[i] = offset;
					ESP_LOGD(TAG, "%s, Updating blower type for dev:%d type: %d",
							__FUNCTION__, devIDs[i], blower_type_list[i]);
					testing_valve += (blower_type_list[i] == VALVE) ? 1:0;
				}
				vTaskDelay(1);
			}
			else {
				ESP_LOGW(TAG, "%s, Blower %d; Could not update ChipID",
						__FUNCTION__,
						devIDs[i]);

			}
		}
	}
	// When using valve blowers if the device connected is blower need to turn testing off
	if (testing_valve){
		// Iterate through the 2 options
		for(int i=0; i<2;i++){

			if (blower_type_list[i] == VALVE && (blower_type_list[i+2] == FAN || blower_type_list[i+2] == UKNOWN_TYPE)){
				blower_type_list[i+2] = CONTROL;
				testing[i+2] = 0;
			}
			ESP_LOGI(TAG, "%s, Blower type %d; Pared with blower type %d, valve testing=%x",
											__FUNCTION__,
											blower_type_list[i],blower_type_list[i+2], testing_valve);
		}

		// TODO: Check if need to change the status on the blowers testing
	}

	return num_avail;
}

/**
 * Checks for connected blowers in the system
 * Updates the rack on off status
 * @return: number of available devices.
 */
static int check_power_on() {
	int suc;
	int devIDs[4] = { DEV_SUPA, DEV_EXHA, DEV_SUPB, DEV_EXB };
	int num_avail = 0;

	for (int i = 0; i < 4; i++) {
		ESP_LOGV(TAG, "Checking for Device:%d", devIDs[i]);

		// Send a message and check for response
		suc = check_dev_id(devIDs[i]);
		vTaskDelay(0);

		// If we receive a response we update the power on and return
		if (suc == 1) {
			num_avail++;
		}
		if (num_avail > 0) {
			int r = set_rack_status(true);
			if (r)
				ESP_LOGI(TAG, "Rack Turned ON");
		} else {
			int r = set_rack_status(false);
			if (r)
				ESP_LOGI(TAG, "Rack Turned OFF");
		}
	}
	return num_avail;
}

/**
 * Sets up the blower in the blower list chipData object array
 * Initializes the current testing list ui components name and chipid
 * @param burn_in_test_value_t *brn_val
 */
static void setup_blower_(burn_in_ui_value_t *brn_val) {
	brn_val->values_changed = true;

// Get values from blowers and update values on ui
	for (int i = 0; i < 4; i++) {
		blower_test_value_t *blower = &brn_val->blowers[i];

		blower->is_testing = testing[i];
		blower->values_changed = true;
		burn_in_testing_state_t state = get_burn_in_state();

		if (blower_type_list[i] == CONTROL) {
			strcpy(blower->name, control_blower_device_names[i]);
			char chipid_str[16];
			unsigned c_id = chipid_list[i];
			sprintf(chipid_str, "%u", c_id);
			strcpy(blower->chip_id, chipid_str);
			blower->offset = cur_offset[i];
			blower->state = RUNNING_BLOWER_TEST;
			blower->is_testing = 1;
			blower->range = -1;
			blower->vas_offset = 0;
			blower->qc_offset = 0;
			blower->min_val = 5;
			blower->max_val = -5;
			blower->num_point = 0;


		}else if (testing[i] &&blower_type_list[i] == VALVE  ){
			blower->state = RUNNING_BLOWER_TEST;
		}
		// If the blower is in the list update the values
		else if (testing[i] && blower_type_list[i] == FAN) {

			ESP_LOGD(TAG, "[%s] Setting Up Blower:%s, ChipID:%s, offset:%d",
					__FUNCTION__,
					blower->name, blower->chip_id, blower->offset);

			strcpy(blower->name, test_blower_device_names[i]);
			char chipid_str[16];
			unsigned c_id = chipid_list[i];
			sprintf(chipid_str, "%u", c_id);
			strcpy(blower->chip_id, chipid_str);
			blower->offset = cur_offset[i];

			// Set the state
			// TODO: change to a set state function


		} else {
			ESP_LOGD(TAG,
					"[%s] Initializing Empty Blower:%s, ChipID:%s, offset:%d",
					__FUNCTION__,
					blower->name, brn_val->blowers[i].chip_id,
					brn_val->blowers[i].offset);

			strcpy(brn_val->blowers[i].chip_id, "");
			blower->offset = DEF_OFFSET_VAL;

			blower->state = UNINIT_BLOWER_TEST;
		}
		// Clear out previous values
		blower->range = DEF_OFFSET_VAL;
		blower->vas_offset = DEF_OFFSET_VAL;
		blower->qc_offset = DEF_OFFSET_VAL;

		// Set the array of burn in values to default
		memset(blower->burn_in_offset, DEF_OFFSET_VAL,
		NUM_OF_TEST * sizeof(int));

		blower->min_val = DEF_OFFSET_VAL;
		blower->max_val = DEF_OFFSET_VAL;
		blower->num_point = 0;

	}
}

/**
 * @brief - Updates the blower offset value for all blowers in testing array
 * on the ui.
 * @param burn_in_test_value_t *brn_val - object that contains the ui update list
 *
 * Requires that the blower values have already been initialized and burn in array is updated with
 * latest offset
 */
static void update_ui_blower_vals(burn_in_ui_value_t *brn_val) {
	brn_val->values_changed = true;

// Get values from blowers and update values on ui
	for (int i = 0; i < 4; i++) {
// If the blower is in the list update the values
		blower_test_value_t *blower = &brn_val->blowers[i];
		ESP_LOGD(TAG, "[%s,%d] Blower:%s, testing:%d",
				__FUNCTION__, __LINE__,
				blower->name, blower->chip_id, blower->offset);

		if (blower->is_testing) {
			blower_test_value_t *blower = &brn_val->blowers[i];

			ESP_LOGD(TAG,
					"[%s,%d]	Updating blower Blower:%s, ChipID:%s, offset:%d",
					__FUNCTION__, __LINE__,
					blower->name, blower->chip_id, blower->offset);

			blower->values_changed = true;

			// Get the calibration data pointer for the current blower using the chipid
			ChipData *cd;
			cd = getChipData(chipid_list[i]);

			// If in the list update from array otherwise use values from vas_v
			if (cd != NULL) {
				// Add the new offset to the list
				// Set the current, vas and qc offsets
				blower->offset = cur_offset[i];
				blower->vas_offset = cd->vas_offset;
				blower->qc_offset = cd->qc_offset;

				// If the database returns -2222 do not calculate the range
				if (blower->vas_offset < -250 || blower->vas_offset > 250 ||blower->qc_offset < -250 ||blower->qc_offset > 250 ){
					// Send a request for the values
					blower->min_val = -41;
					blower->max_val = 41;
					blower->range = 0;
				}

				else if (blower->vas_offset == DEF_OFFSET_VAL || blower->qc_offset == DEF_OFFSET_VAL){
					// Send a request for the values
//					get_ppb_values();
					blower->min_val = 10;
					blower->max_val = -10;
					blower->range = 0;
				} else {


					// Set the min max and range values
					blower->min_val = get_min_last_n(cd, 4);
					blower->max_val = get_max_last_n(cd, 4);
					blower->range = offset_range_last_n(cd, 4);
					// Range should be greater than 0 to get rid of false posatives
					blower->range = (blower->range==0) ? 1 : blower->range;
				}
				// set the burn in array
				int num_burn_val = copy_array(&cd->offset_array,
						blower->burn_in_offset, NUM_OF_TEST);
				blower->num_point = num_burn_val;
				// Check if there are any values in burn in

				ESP_LOGD(TAG,
						"[%s,%d]    Copied %d items to the array\t Current test array count %d________________",
						__FUNCTION__, __LINE__, num_burn_val,
						cd->offset_array.count);

				ESP_LOGD(TAG,
						"[%s,%d]	Updated Blower:%s, ChipID:%s, offset:%d, num_points: %d",
						__FUNCTION__, __LINE__,
						blower->name, blower->chip_id, blower->offset,
						blower->num_point);

			} else {
				ESP_LOGE(TAG,
						"[%s,%d]	Failed update on Blower:%s, ChipID:%s, offset:%d chip data returned NULL",
						__FUNCTION__, __LINE__,
						blower->name, blower->chip_id, blower->offset);
				return;
			}
			int passing = 0;
			passing = (blower->min_val > -40) ? 1 : 0;
			passing &= (blower->max_val < 40) ? 1 : 0;
			passing &= (blower->range <= 15) ? 1 : 0;

			if (passing) {
				blower->state =
						(blower->num_point >= REQ_POINTS_PASS) ?
								SUCCESS_BLOWER_TEST :
								RUNNING_BLOWER_TEST;
			} else {
				blower->state = FAILED_BLOWER_TEST;
			}

		}
	}
}

static void init_blower_test(burn_in_ui_value_t *brn_val) {
	brn_val->values_changed = true;

// Get values from blowers and update values on ui
	for (int i = 0; i < 4; i++) {
		blower_test_value_t *blower = &brn_val->blowers[i];
// If the blower is in the list update the values
		testing[i] = 0;
		blower_type_list[i] = UKNOWN_TYPE;

		blower->is_testing = 0;
		blower->values_changed = true;

		strcpy(blower->name, test_blower_device_names[i]);
		strcpy(blower->chip_id, " ");
		blower->offset = DEF_OFFSET_VAL;
		blower->range = DEF_OFFSET_RANGE;
		blower->vas_offset = DEF_OFFSET_VAL;
		blower->qc_offset = DEF_OFFSET_VAL;
		for (int ii = 0; ii < NUM_OF_TEST; ii++) {
			blower->burn_in_offset[0] = DEF_OFFSET_VAL;
		}
		blower->state = UNINIT_BLOWER_TEST;

		blower->min_val = DEF_OFFSET_VAL;
		blower->max_val = DEF_OFFSET_VAL;
		blower->num_point = 0;
	}
}

static int log_calibration_values() {
	int ret = 0;
	for (int i = 0; i < 4; i++) {
		if (testing[i]) {
			// send the request to log the calibration value
			esp_err_t err = set_calibration_val(chipid_list[i],
					cur_offset[i]);
			ESP_LOGD(TAG,
					"%s, %dLogging calibration value to database. chip_id=%u, value=%d",
					__FUNCTION__, i, chipid_list[i], cur_offset[i]);

			if (err != ESP_OK) {
				ESP_LOGE(TAG,
						"[%s, %d]Error logging calibration value to database",
						__FUNCTION__, __LINE__);
				ret--;
			}
		}
	}
	return ret;
}

/**
 * @brief Sends  async request for the chip id pre post burnin values from the server
 *  @return
 *   - 0 if all succesful
 *   - -[num of failed transmisions or incorrectly formated inputs]
 */
static int get_ppb_values() {
	int ret = 0;
	for (int i = 0; i < 4; i++) {
		if (testing[i]) {
			// send the request to log the calibration value
			esp_err_t err = request_ppb_vals(chipid_list[i]);
			if (err != ESP_OK) {
				ESP_LOGE(TAG,
						"[%s, %d]Error sending request for ppb values to database",
						__FUNCTION__, __LINE__);
				ret--;
				vTaskDelay(300 / portTICK_PERIOD_MS);
			}
		}
	}
	return ret;
}

/**
 * Initializes the needed server event handler callbacks
 */
static esp_err_t register_burnin_eh_calback(void) {
// Register event with DB_GET_PRE_POST_BURNIN

	esp_event_base_t base = get_event_handler_base();
	esp_event_loop_handle_t loop = get_event_handler_loop();
	ESP_LOGD(TAG, "[%s, %d] Setting up burnin callback",
			__FUNCTION__, __LINE__);

	ESP_ERROR_CHECK(
			esp_event_handler_instance_register_with(loop, base, DB_GET_PRE_POST_BURNIN, on_ppb_response, &loop, NULL));
	// Used when a settings value is updated
//	ESP_ERROR_CHECK(
//			esp_event_handler_instance_register_with(loop, base, SETTINGS_REQUEST, on_settings_update, &loop, NULL));
	return ESP_OK;
}

/**
 * @brief event handler function for Burn in ppb event available
 *  - Called by event handler when a DB_GET_PRE_POST_BURNIN is posted
 */
static void on_ppb_response(void *handler_arg, esp_event_base_t base,
		int32_t id, void *event_data) {

	ESP_LOGI(TAG,
			"[%s,%d]/t Received from loop id: %ld event pointer:%p",
			__FUNCTION__, __LINE__, id, (void* )event_data);

// Cast handler_arg to eh_event_t
	eh_event_t *event = (eh_event_t*) event_data;

	if (event == NULL) {
		ESP_LOGE(TAG, "%s, Error: event_data==NULL at:%p,",
				__FUNCTION__,
				(void* ) event);
//		return;
	}

// Check that the event is valid
	/* TODO: Check that this is thread sage*/
	else if (event->valid && event->type == DB_RESP_UNPACKED) {

		ESP_LOGD(TAG, "[%s,%d]\t Event valid=%d, msg_id=%d, type=%d",
				__FUNCTION__, __LINE__, event->valid, event->msg_id,
				event->type);

// Cast msg_struct into a db_resp_pre_post_burnin_t
		db_resp_pre_post_burnin_t *data =
				(db_resp_pre_post_burnin_t*) event->msg_struct;

//Error Checking that we have the correct struct
		if (data == NULL) {
			ESP_LOGE(TAG, "%s, msg_struct == null at:%p",
					__FUNCTION__, (void* )data);
			return;
		}
		ESP_LOGD(TAG, "[%s,%d]/t, msg_struct == :%p",
				__FUNCTION__, __LINE__, (void* )data);

// Now you can access the members of the db_resp_pre_post_burnin_t struct
// TODO: use the print function
		ESP_LOGD(TAG, "chipID: %u", data->chipID);
		ESP_LOGD(TAG, "burnin_len: %u", data->burnin_len);
		ESP_LOGD(TAG, "vas_cal_val: %d", data->vas_cal_val);
		ESP_LOGD(TAG, "qc_cal_val: %d", data->qc_cal_val);
		ESP_LOGD(TAG, "num_burnin: %d", data->num_burnin);
		for (int i = 0; i < data->num_burnin; i++) {
			ESP_LOGD(TAG, "    burnin_val[%d]: %d", i,
					data->burnin_val[i]);
		}

// Check for the internal chip data
// Not sure if this is neccessary
		unsigned c_id = data->chipID;
		ChipData c_data;
		int r = chip_data_index(c_id);
		ESP_LOGD(TAG, "Updating index: %d", r);
		c_data.chipID = c_id;
		c_data.qc_offset = data->qc_cal_val;
		c_data.vas_offset = data->vas_cal_val;

// Update the data array
		r = insert_array(&c_data.offset_array, data->burnin_val,
				data->num_burnin);
		if (r < 0) {
			ESP_LOGE(TAG,
					"[%s,%d]/t Error copying the burn in array at: %p",
					__FUNCTION__, __LINE__, (void* ) data->burnin_val);
		}
		r = addChipData(&c_data);
		if (r < 0) {
			ESP_LOGE(TAG,
					"[%s,%d]/t Error adding chip data for %u",
					__FUNCTION__, __LINE__, c_id);
		}

	} else {
		ESP_LOGE(TAG,
				"[%s,%d]/t Error: Event valid=%d, msg_id=%d, type=%d",
				__FUNCTION__, __LINE__, event->valid, event->msg_id,
				event->type);
	}
}

/**
 * @brief event handler function for Burn in ppb event available
 *  - Called by event handler when a DB_GET_PRE_POST_BURNIN is posted
 */
static void on_settings_update(void *handler_arg, esp_event_base_t base,
		int32_t id, void *event_data) {

	ESP_LOGI(TAG,
			"[%s,%d]/t Received from loop id: %ld event pointer:%p",
			__FUNCTION__, __LINE__, id, (void* )event_data);

// Cast handler_arg to eh_event_t
	eh_event_t *event = (eh_event_t*) event_data;

	if (event == NULL) {
		ESP_LOGE(TAG, "%s, Error: event_data==NULL at:%p,",
				__FUNCTION__,
				(void* ) event);
//		return;
	}

// Check that the event is valid
	/* TODO: Check that this is thread sage*/
	else if (event->valid && event->type == SETTINGS_REQUEST) {

		ESP_LOGD(TAG, "[%s,%d]\t Event valid=%d, msg_id=%d, type=%d",
				__FUNCTION__, __LINE__, event->valid, event->msg_id,
				event->type);

// Cast msg_struct into a db_resp_pre_post_burnin_t
		settings_req_t *setting =
				(settings_req_t*) event->msg_struct;

//Error Checking that we have the correct struct
		if (setting == NULL) {
			ESP_LOGE(TAG, "%s, msg_struct == null at:%p",
					__FUNCTION__, (void* )setting);
			return;
		}
		ESP_LOGD(TAG, "[%s,%d]/t, Settings request type == :%d",
				__FUNCTION__, __LINE__, setting->type);

		switch (setting->type) {
			case IP_ADDR:
				handle_ip_addr((char*) setting->data);
				break;
			case SSID_STR:
				//				handle_ssid_str((char*) setting->data);
				break;
			case WIFI_PASSWD:
				//				handle_wifi_passwd((char*) setting->data);
				break;
			case MQTT_CONF:
				//				handle_mqtt_conf((mqtt_conf_str*) setting->data);
				break;
			case MQTT_STATUS:
				handle_mqtt_status(*(bool*) setting->data);
				break;
			case MODBUS_POWER:
				handle_modbus_power(*(bool*) setting->data);
				break;
			case NODE_NAME:
				handle_node_name((char*) setting->data);
				break;
			case SERIAL_STATUS:
				handle_serial_status(*(bool*) setting->data);
				break;
			case BLT_STATUS:
				handle_blt_status(*(bool*) setting->data);
				break;
			default:
				// unknown type should break here and hang durring debug
				break;
		}
	}
}
static unsigned valve_pos = 0;
static void move_valve_blower() {
	int devIDs[4] = { DEV_SUPA, DEV_EXHA, DEV_SUPB, DEV_EXB };
	valve_pos = (valve_pos == 0) ? 99 : 0;
	for (int i = 0; i < 4; i++) {
		if (testing[i] && blower_type_list[i]==VALVE) {
			// send the request to log the calibration value
			int err = set_pwm(devIDs[i], valve_pos);
			if (err != 1) {
				ESP_LOGE(TAG,
						"[%s, %d]Error sending request for ppb values to database",
						__FUNCTION__, __LINE__);
				vTaskDelay(200 / portTICK_PERIOD_MS);
			}
		}
	}
}

/**
 *  ___ Public Function  definitions ______
 */

void burn_in_task(void *pvParameter) {
//	vTaskDelay(APP_START_DELAY_MS / portTICK_PERIOD_MS);
	init_burn_in();
	int count = 0;
	int num_available = 0;
	long unsigned mem_original = 0;
	long unsigned mem_prev = 0;
	long unsigned mem_cur = 0;
	blower_burnin_handle_t b;
	b.num_iterations= 99;


	ESP_LOGI(TAG, "[APP] Starting burn in task ");
	ESP_LOGI(TAG, "[APP] Checking for Rack ON event  ");
	mem_original = esp_get_free_heap_size();
	mem_cur = mem_original;
	ESP_LOGI(TAG, "[APP] Original heap:%lu, ", mem_original);

	ESP_LOGI(TAG, "[APP] Rack initiation completed  ");

	init_chipArrray();

	vTaskDelay(APP_START_DELAY_MS / portTICK_PERIOD_MS);
	register_burnin_eh_calback();

	ESP_LOGI(TAG, "[APP] Starting loop ");

	while (true) {
// Check for blowers and keep a local copy of values for when the ui is changed
		//TODO: check power on in correct loop
		burn_in_testing_state_t state = get_burn_in_state();
		if (state == -1) {
			ESP_LOGW(TAG,
					"[APP] Current state is in error needs to be handled ");
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			continue;
		}

//Periodically print out state
		if (count % 10 == 0 || rack_power_changed) {
			ESP_LOGD(TAG,
					"[APP] Num Blowers: %d, Current State: %s Rack Powered On:%d Value changed:%d",
					num_available, burnin_state_to_str(state), rack_on,
					rack_power_changed);
			mem_prev = mem_cur;
			mem_cur = esp_get_free_heap_size();
			int mem_change = (mem_cur - mem_prev);
			ESP_LOGD(TAG,
					"[APP] Original heap:%lu, Free memory: %lu bytes, Heap change:%d",
					mem_original, mem_cur, mem_change);
		}
		// Loop Delay and value update
		vTaskDelay(APP_LOOP_RATE_MS / portTICK_PERIOD_MS);
		count++;
		ESP_ERROR_CHECK(update_test_values());

		// Clear modbus messages
//		int num_cleared_msg = clear_uart_rx_queue();
//		if (num_cleared_msg) {
//			ESP_LOGW(TAG, "[APP] Cleared MODBus messages:%d", num_cleared_msg);
//		}

		/**
		 * Loop Runs on the current state
		 */

		if (state == CANCEL_BURNIN_TEST) {
			// only need to update when the test_cycle is positive

			if (test_cycle != -1) {

				// Default state and test restart state
				// Reset all test values
				if (test_vals_acquire(10)) {
					// TODO: Reset the test
					init_burn_in();
					burn_in_ui_value_t *b_val;
					b_val = get_test_vals();
					//				setup_blower_(b_val);
					/* TODO: Remove the coments */
					init_blower_test(b_val);
					test_vals_release();
					//				update_test_state(STARTING_BURNIN_TEST);

					// Set test cycles to neg so we are not consistently checking this
					test_cycle = -1;

				}
				ESP_LOGI(TAG, "[APP] Cancel Event reseting test: cycle %d",
						test_cycle);
			}
			// Continue Loop until start is pressed
			continue;
		}

		num_available = check_power_on();

		if (state == STARTING_BURNIN_TEST && rack_on) {
			// Starting Burn in state requires the rack to be powered on and the start button to be pressed
			// The default state is the cancel state and all test will continue to run until the cancel test is set
			// Reset the test cycles if cancel event was initiated
			test_cycle = (test_cycle >= 0) ? test_cycle : 1;
			ESP_LOGD(TAG, "[%d][APP] STARTING_BURNIN_TEST cycles: %d",
					__LINE__,
					test_cycle);
			vTaskDelay(10 / portTICK_PERIOD_MS);

			// This state is changed when the user presses start or a a cycle is finished and the rack is turned on
			ESP_LOGI(TAG, "[APP] STARTING_BURNIN_TEST conditions checking");

			// Why is this being called and not checked?????
//			check_rack_power_changed();
			num_available = update_rack_blower_list();
			ESP_LOGI(TAG, "[APP] STARTING_BURNIN_TEST found %d, devices ",
					num_available);

			/** FIXME: use the event handler to update the values */
			/* FIXME: not updating the ui*/
			testing_valve = 0;

			if (test_vals_acquire(10)) {
				ESP_LOGI(TAG, "[APP] STARTING_BURNIN_TEST Acquiring UI");

				//Acquired ui update the values
				burn_in_ui_value_t *b_val;

				b_val = get_test_vals();
				// Initializes Offset range and chipID values to UI
				setup_blower_(b_val);

				// Update with current values
//				update_ui_blower_vals(b_val);
				test_vals_release();

				// Update the ui values
				update_test_values();

				// Need to delay for 1000 msec to gaurauntee that all samples have been collected
				vTaskDelay(1000 / portTICK_PERIOD_MS);
				num_available = update_rack_blower_list();

				// Get the current values if available from the server allow a .5 sec delay
				int ppb_sec = get_ppb_values();
				ESP_LOGD(TAG,
						"[%d][APP] STARTING_BURNIN_TEST in ppb_success: %d",
						__LINE__, ppb_sec);
				vTaskDelay(500 / portTICK_PERIOD_MS);

				ESP_LOGI(TAG, "[APP] STARTING_BURNIN_TEST Updated test values");

				if (check_rack_power_changed()) {
					// Should be called once during burn in test
					if (testing_valve>0){
						update_test_state(STARTING_VALVE_TEST);
					}

				}
			} else {
				// Reset the flag since we did not aquire ui
				ESP_LOGW(TAG, "[%d][APP] Could not acquire ui to start burn in",
						__LINE__);
				rack_power_changed = true;

			}

		}
		else if (state == RUNNING_BURNIN_TEST) {
			update_rack_blower_list();
//			if (count % UPDATE_UI_COUNT == 0)
			if (count % 45 == 5) {
				get_ppb_values();
			}
			if (count % 3 == 1){

				// Update every
				if (test_vals_acquire(10)) {
					ESP_LOGI(TAG,
							"[APP] RUNNING_BURNIN_TEST Burn in test Acquiring UI");

					//Acquired ui update the values
					burn_in_ui_value_t *b_val;
					b_val = get_test_vals();
					log_calibration_values();
					// Update with current values
					update_ui_blower_vals(b_val);

					test_vals_release();
					update_test_values();

					ESP_LOGI(TAG,
							"[APP] RUNNING_BURNIN_TEST updating test values");

				}
			}

		}
		else if (state == FINISHED_BURNIN_TEST && !rack_on) {
			// State transition if rack is turned off
			check_rack_power_changed();
			ESP_LOGI(TAG,
					"[APP] FINISHED_BURNIN_TEST- submitting value for cycle");
			if (testing_valve>0){
				move_valve_blower();
			} else {
			esp_err_t ret = start_cooldown();

				if (ret == ESP_OK) {
					ESP_LOGD(TAG, "%s, Rack turned off transition state",
							__FUNCTION__);
				}
			}
		}

		else if (state == STARTING_VALVE_TEST ) {
			// Check if power is on
			if (!rack_on) {
				// If powered off go back to statting Burnin
				esp_err_t ret = update_test_state(STARTING_BURNIN_TEST);

				if (ret == ESP_OK) {
					test_cycle++;
					ESP_LOGI(TAG, "[APP] Changing State to Start power interrupted: Running cycle %d", test_cycle);
				}

			} else {
				// Update the state to Running when valve test has started

				//Setup the default handler
				if (default_valve_burnin_config(&b)==-1) {
					ESP_LOGI(TAG, "[APP] Could not create default Valve configuration ");
				}
				for (int i=0;i<4;i++){
					// Add the id to burn in handler
					device_valve_blower_t *dev_hand = b.blower_handle[i];
					dev_hand->blower_type = blower_type_list[i];
					dev_hand->type = devIDs[i];
					dev_hand->chip_id = chipid_list[i];
					dev_hand->chip_id_valid = true;


				}
				// Delete the handle if it exists
				if( valve_handle != NULL ) {
				     vTaskDelete( valve_handle );

				}
				set_pwm(devIDs[0], 1);
				set_pwm(devIDs[1], 85);
				set_pwm(devIDs[2], 1);
				set_pwm(devIDs[3], 85);

				// Create the valve task
				xTaskCreate(&valve_burnin_task, "valve_burnin", 1024 * 4,
					(void*) &b, 6, &valve_handle);
				vTaskDelay(5000 / portTICK_PERIOD_MS);
				ESP_LOGI(TAG, "[APP] Created valve task: Running cycle %d", test_cycle);

				esp_err_t ret = update_test_state(RUNNING_VALVE_TEST);

				if (ret == ESP_OK) {
					test_cycle++;
					ESP_LOGI(TAG, "[APP] Starting Valve BurnIn Procedure Test: Running cycle %d", test_cycle);
					ESP_LOGI(TAG, "[APP]     -Blower Type SA %d", blower_type_list[0]);
					ESP_LOGI(TAG, "[APP]     -Blower Type EA %d", blower_type_list[1]);
					ESP_LOGI(TAG, "[APP]     -Blower Type SB %d", blower_type_list[2]);
					ESP_LOGI(TAG, "[APP]     -Blower Type EB %d", blower_type_list[3]);



					// Set the state to running for HVAC and VALVE

				}
			}
			// update the ui
		}
		else if (state == RUNNING_VALVE_TEST) {
			// Move the valve around to exercise it
			// Check if power is on
			if (test_cycle++ > (18*60)){
				b.runner_handle[0]->state = burnin_passed;
			}
			if (test_cycle > (20* 60)){
				b.runner_handle[1]->state = burnin_passed;
				b.progress = prog_stop;
			}
			ESP_LOGI(TAG, "[APP] RUNNING_VALVE_TEST %d",test_cycle);
			if (!rack_on) {
				if (test_vals_acquire(10)) {
					ESP_LOGI(TAG, "[APP] RUNNING_VALVE_TEST power turned off resetting to Starting");

					//Acquired ui update the values
					burn_in_ui_value_t *b_val;

					b_val = get_test_vals();

					// Update with current values
	//				update_ui_blower_vals(b_val);
					test_vals_release();

					// Update the ui values
					update_test_values();


				} else {
					// Reset the flag since we did not aquire ui
					ESP_LOGW(TAG, "[%d][APP] Could not acquire ui to start burn in",
							__LINE__);
					rack_power_changed = true;

				}

				// Update the state to starting
				esp_err_t ret = update_test_state(STARTING_BURNIN_TEST);
				if (ret == ESP_OK) {
					test_cycle++;
					ESP_LOGI(TAG, "[APP] Finished: Running cycle %d", test_cycle);
				}
				// TODO kill valve tasks
				if( valve_handle != NULL ) {
				     vTaskDelete( valve_handle );

				}

			}
			else {
				int passed_num = 0;
				// Check on valve tasks
				if (b.progress == prog_running) {
					//Acquired ui update if state changed
					// Power On check blower task progress
					if (test_vals_acquire(10)) {
						burn_in_ui_value_t *b_val;
						b_val = get_test_vals();


						//Iterate through runner_handle states
						for (int i=0; i<2;i++) {
							burnin_state_t state = b.runner_handle[i]->state;

							if (state == burnin_init){
								b_val->blowers[i].state = STARTING_BLOWER_TEST;
								b_val->blowers[i+2].state = STARTING_BLOWER_TEST;
							}
							else if (state <=  burnin_record_test) {
								b_val->blowers[i].state = RUNNING_BLOWER_TEST;
								b_val->blowers[i+2].state = RUNNING_BLOWER_TEST;
							}
							else if (state ==  burnin_passed) {
								b_val->blowers[i].state = SUCCESS_BLOWER_TEST;
								b_val->blowers[i+2].state = RUNNING_BLOWER_TEST;
								passed_num++;
							}
							else if (state <=  burnin_warning) {
								b_val->blowers[i].state = RUNNING_BLOWER_TEST;
								b_val->blowers[i+2].state = STARTING_BLOWER_TEST;

							}
							else if (state ==  burnin_finished) {
								b_val->blowers[i].state = SUCCESS_BLOWER_TEST;
								b_val->blowers[i+2].state = WARNING_BLOWER_TEST;

							}
							else if (state ==  burnin_failed) {
								b_val->blowers[i].state = FAILED_BLOWER_TEST;
								b_val->blowers[i+2].state = RUNNING_BLOWER_TEST;

							}
						}
					}
					test_vals_release();
				}
				else if (b.progress == prog_stop || passed_num>=2) {
					// Burn in tests finished turn timer off
					update_test_state(FINISHED_VALVE_TEST);

				}
				// Update the ui values
				update_test_values();
			}

		}
		else if (state == FINISHED_VALVE_TEST) {
			// Move state to
			if (!rack_on) {
				esp_err_t ret = update_test_state(STARTING_BURNIN_TEST);
				set_pwm(devIDs[0], 0);
				set_pwm(devIDs[1], 0);
				set_pwm(devIDs[2], 0);
				set_pwm(devIDs[3], 0);

				if (ret == ESP_OK) {
					test_cycle++;
					ESP_LOGI(TAG, "[APP] Finished: Valve Burn in complete cycle %d", test_cycle);
				}
			}
		}

		else if (state == RUNNING_COOLDOWN_TEST) {
			// Do something
			// Check that the rack is still off to continue the timer
			if (rack_on) {
				// Update the state to startting
				esp_err_t ret = update_test_state(STARTING_BURNIN_TEST);
				if (ret == ESP_OK) {
					test_cycle++;
					ESP_LOGI(TAG, "[APP] Finished: Running cycle %d", test_cycle);
				}
			} else {


				// We do not want to log the value until the timer has run down
				esp_err_t err = update_test_state(CANCEL_BURNIN_TEST);
				if (err != ESP_OK) {
					ESP_LOGE(TAG,
										"Error Updating test state, %d", err);

				}


			}

		} else if (state == FINISHED_BURNIN_CYCLE && rack_on) {
			// Submit the Cycle Burn in value so that we know cooldown has been complete
			ESP_LOGI(TAG,
					"[APP] ____________FINISHED_BURNIN_CYCLE- submitting value for cycle");

			for (int i = 0; i < 4; i++) {

				if (testing[i]) {
					set_cal_burnin_val(chipid_list[i], cur_offset[i]);
					ESP_LOGD(TAG, "[APP] Logging successful test, %u, val=%d",
							chipid_list[i], cur_offset[i]);
				}
			}

			esp_err_t ret = update_test_state(STARTING_BURNIN_TEST);
			if (ret == ESP_OK) {
				test_cycle++;
				ESP_LOGI(TAG, "[APP] Finished: Running cycle %d", test_cycle);

			}

		}

	}
	ESP_LOGE(TAG, "[APP] Burn In task canceled");

	vTaskDelete(burn_in_handle);
}

