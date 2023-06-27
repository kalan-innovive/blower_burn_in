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

static const char *TAG = "burn-in";

extern TaskHandle_t burn_in_handle;

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
static int vas_v[4];
static int QC_v[4];

/**
 *  ___Proto Type definitions ______
 */
static void init_burn_in(void);
static void update_ui_blower_vals(burn_in_test_value_t *brn_val);

static int update_rack_blower_list(void);
static void setup_blower_(burn_in_test_value_t *brn_val);
static void update_blower_ui_val(burn_in_test_value_t *brn_val);
static int check_rack_power_changed(void);
static int set_rack_status(bool status);
static void init_blower_test(burn_in_test_value_t *brn_val);
static int log_calibration_values();
//static int request_ppb_values();
static esp_err_t register_burnin_eh_calback(void);
static void on_ppb_response(void *handler_arg, esp_event_base_t base,
		int32_t id, void *event_data);
static int get_ppb_values();

/**
 *  ___Static Function  definitions ______
 */

static void init_burn_in() {
	for (int i = 0; i < 4; i++) {
		testing[i] = 0;
		cur_offset[i] = DEF_OFFSET_VAL;
		vas_v[i] = DEF_OFFSET_VAL;
		QC_v[i] = DEF_OFFSET_VAL;
	}
	rack_power_changed = false;
	rack_on = false;

}
/*
 * sets the rack_on and rack_power_changed flags
 * returns if the rack_power_changed status
 */
static int set_rack_status(bool status) {
	rack_power_changed = (status != rack_on) ? true : false;
	rack_on = status;
	if (rack_power_changed) {
		ESP_LOGI(TAG, "Rack power changed:%d, rack_on:%d", rack_power_changed,
				rack_on);
	}
	return (int) rack_power_changed;

}

/**
 * Checks if the rack_on status has changed
 * returns if the rack_power_changed status
 * Resets the rack_power_changed status to false
 */
static int check_rack_power_changed() {
	if (rack_power_changed) {
		rack_power_changed = false;
		return 1;
	}
	return 0;
}
/**
 * Checks for connected blowers
 * Logs the callibration value to the server if available
 * Updates the rack on off status
 */
static int update_rack_blower_list() {
	int suc;
//	rack_on = false;
	int devIDs[4] = { DEV_SUPA, DEV_EXHA, DEV_SUPB, DEV_EXB };
	unsigned chipid = 0;
	int offset = 0;
	int num_avail = 0;

	for (int i = 0; i < 4; i++) {
		ESP_LOGD(TAG, "Checking for Device:%d", devIDs[i]);
		chipid = 0;
		offset = DEF_OFFSET_VAL;

		// Send a message and check for response
		suc = get_raw_pressure(devIDs[i], &offset);

		ESP_LOGD(TAG, "Checking for Device:%d, Available: %d, Offset:%d",
				devIDs[i], suc, offset);
		// If we can get the offset check if the chip id is readable
		if (suc == 1) {
			suc = get_chipid(devIDs[i], &chipid);

			if (suc == 1) {
				ESP_LOGD(TAG, "%d. Blower %d; Chip ID: %u Offset: %d", i,
						devIDs[i], chipid, offset);
				chipid_list[i] = chipid;

				testing[i] = 1;
				num_avail += 1;
			}
			cur_offset[i] = offset;
			// Log the calibration value
			set_calibration_val(chipid_list[i], cur_offset[i]);

		} else {
			testing[i] = 0;
			chipid_list[i] = 0;
		}

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
	return num_avail;
}

/**
 * Sets up the blower in the blower list chipData object array
 * Initializes the current testing list ui components name and chipid
 * @param burn_in_test_value_t *brn_val
 */
static void setup_blower_(burn_in_test_value_t *brn_val) {
	brn_val->values_changed = true;

	// Get values from blowers and update values on ui
	for (int i = 0; i < 4; i++) {
		blower_test_value_t *blower = &brn_val->blowers[i];

		blower->is_testing = testing[i];
		blower->values_changed = true;

		// If the blower is in the list update the values
		if (testing[i]) {

			ESP_LOGD(TAG, "Setting Up Blower:%s, ChipID:%s, offset:%d",
					blower->name, blower->chip_id, blower->offset);

			strcpy(blower->name, test_blower_device_names[i]);
			char chipid_str[16];
			unsigned c_id = chipid_list[i];
			sprintf(chipid_str, "%u", c_id);
			strcpy(blower->chip_id, chipid_str);

			// Set the state
			blower->state = STARTING_BLOWER_TEST;

		} else {
			ESP_LOGD(TAG, "Initializing Empty Blower:%s, ChipID:%s, offset:%d",
					blower->name, brn_val->blowers[i].chip_id,
					brn_val->blowers[i].offset);

			strcpy(brn_val->blowers[i].chip_id, "");

			blower->state = UNINIT_BLOWER_TEST;
		}
		// Clear out previous values
		blower->offset = DEF_OFFSET_VAL;
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
static void update_ui_blower_vals(burn_in_test_value_t *brn_val) {
	brn_val->values_changed = true;

	// Get values from blowers and update values on ui
	for (int i = 0; i < 4; i++) {
		// If the blower is in the list update the values
		blower_test_value_t *blower = &brn_val->blowers[i];

		if (blower->is_testing) {
			blower_test_value_t *blower = &brn_val->blowers[i];

			ESP_LOGD(TAG, "Updating blower Blower:%s, ChipID:%s, offset:%d",
					blower->name, blower->chip_id, blower->offset);

			blower->values_changed = true;

			// Get the calibration data pointer for the current blower using the chipid
			ChipData *cd;
			cd = getChipData(chipid_list[i]);

			// If in the list update from array otherwise use values from vas_v
			if (cd != NULL) {
				// Add the new offset to the list
				// Set the current, vas and qc offsets
				blower->offset = getCurrOffset(cd);
				blower->vas_offset = cd->vas_offset;
				blower->qc_offset = cd->qc_offset;

				// Set the min max and range values
				blower->min_val = offset_min(cd);
				blower->max_val = offset_max(cd);
				blower->range = blower->max_val - blower->min_val;

				// set the burn in array
				int num_burn_val = copy_array(&cd->offset_array,
						blower->burn_in_offset, NUM_OF_TEST);
				blower->num_point = num_burn_val;

				ESP_LOGI(TAG,
						"Copied %d items to the array\n Current test array count %d________________",
						num_burn_val, cd->offset_array.count);

				ESP_LOGD(TAG,
						"Updated Blower:%s, ChipID:%s, offset:%d, num_points: %d",
						blower->name, blower->chip_id, blower->offset,
						blower->num_point);

			} else {
				ESP_LOGE(TAG,
						"Failed update on Blower:%s, ChipID:%s, offset:%d",
						blower->name, blower->chip_id, blower->offset);
				return;
			}
			int passing = 0;
			passing = (blower->min_val > -80) ? 1 : 0;
			passing &= (blower->max_val < 80) ? 1 : 0;
			passing &= (blower->range <= 11) ? 1 : 0;

			if (passing) {
				blower->state =
						(blower->num_point >= REQ_POINTS_PASS) ?
								SUCCESS_BLOWER_TEST : RUNNING_BLOWER_TEST;
			} else {
				blower->state = FAILED_BLOWER_TEST;
			}

		}
	}
}

static void init_blower_test(burn_in_test_value_t *brn_val) {
	brn_val->values_changed = true;

	// Get values from blowers and update values on ui
	for (int i = 0; i < 4; i++) {
		blower_test_value_t *blower = &brn_val->blowers[i];
		// If the blower is in the list update the values
		testing[i] = 0;

		blower->is_testing = 0;
		blower->values_changed = true;

		strcpy(blower->name, test_blower_device_names[i]);
		strcpy(blower->chip_id, " ");
		blower->offset = DEF_OFFSET_VAL;
		blower->range = DEF_OFFSET_VAL;
		blower->vas_offset = DEF_OFFSET_VAL;
		blower->qc_offset = DEF_OFFSET_VAL;
		for (int ii = 0; ii < NUM_OF_TEST; ii++) {
			blower->burn_in_offset[0] = DEF_OFFSET_VAL;
		}
		blower->state = UNINIT_BLOWER_TEST;

		blower->min_val = DEF_OFFSET_VAL;
		blower->max_val = DEF_OFFSET_VAL;
		blower->num_point = DEF_OFFSET_VAL;
	}
}

static int log_calibration_values() {
	int ret = 0;
	for (int i = 0; i < 4; i++) {
		if (testing[i]) {
			// send the request to log the calibration value
			esp_err_t err = set_calibration_val(chipid_list[i], cur_offset[i]);
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

	ESP_ERROR_CHECK(
			esp_event_handler_instance_register_with(loop, base, DB_GET_PRE_POST_BURNIN, on_ppb_response, &loop, NULL));

	return ESP_OK;
}

/**
 * @brief event handler function for Burn in ppb event available
 *  - Called by event handler when a DB_GET_PRE_POST_BURNIN is posted
 */
static void on_ppb_response(void *handler_arg, esp_event_base_t base,
		int32_t id, void *event_data) {

	ESP_LOGI(TAG, "%s, Received from loop id: %ld", __FUNCTION__, id);

	// Cast handler_arg to eh_event_t
	eh_event_t *event = (eh_event_t*) event_data;

	if (event == NULL) {
		ESP_LOGE(TAG, "%s, Error: event_data==NULL", __FUNCTION__);
		return;
	}

	// Check that the event is valid
	/* TODO: Check that this is thread sage*/
	if (event->valid && event->type == DB_RESP_UNPACKED) {

		ESP_LOGD(TAG, "%s, Event valid=%d, msg_id=%d, type=%d",
				__FUNCTION__, event->valid, event->msg_id, event->type);

		// Cast msg_struct into a db_resp_pre_post_burnin_t
		db_resp_pre_post_burnin_t *data =
				(db_resp_pre_post_burnin_t*) event->msg_struct;

		//Error Checking that we have the correct struct
		if (data != NULL) {
			ESP_LOGE(TAG, "%s, data is null",
					__FUNCTION__);
			return;
		}

		// Now you can access the members of the db_resp_pre_post_burnin_t struct
		// TODO: use the print function
		ESP_LOGD(TAG, "chipID: %u", data->chipID);
		ESP_LOGD(TAG, "burnin_len: %u", data->burnin_len);
		ESP_LOGD(TAG, "vas_cal_val: %d", data->vas_cal_val);
		ESP_LOGD(TAG, "qc_cal_val: %d", data->qc_cal_val);
		ESP_LOGD(TAG, "num_burnin: %d", data->num_burnin);
		for (int i = 0; i < data->num_burnin; i++) {
			ESP_LOGD(TAG, "burnin_val[%d]: %d", i, data->burnin_val[i]);
		}

		// Check for the internal chip data
		// Not sure if this is neccessary
		unsigned c_id = data->chipID;
		ChipData c_data;
		int r = chip_data_index(c_id);

		// If in the list update from array otherwise use values from vas_v
		if (r == 0) {
			c_data.chipID = c_id;
			c_data.qc_offset = data->qc_cal_val;
			c_data.vas_offset = data->vas_cal_val;
			r = addChipData(data->burnin_val);
			if (r < 0) {
				ESP_LOGE(TAG, "%s, Error copying the burn in array for %u",
						__FUNCTION__, c_id);
			}

		} else {
			ESP_LOGE(TAG, "%s, Error: Event valid=%d, msg_id=%d, type=%d",
					__FUNCTION__, event->valid, event->msg_id, event->type);
		}
	}
}

/**
 *  ___ Public Function  definitions ______
 */

void burn_in_task(void *pvParameter) {
	init_burn_in();
	int count = 0;
	int num_available = 0;
	long unsigned mem_original = 0;
	long unsigned mem_prev = 0;
	long unsigned mem_cur = 0;

	ESP_LOGI(TAG, "Starting burn in task ");
	ESP_LOGI(TAG, "Checking for Rack ON event  ");
	mem_original = esp_get_free_heap_size();
	mem_cur = mem_original;
	ESP_LOGI(TAG, "[APP] Original heap:%lu, ", mem_original);

	ESP_LOGI(TAG, "Rack initiation completed  ");

	init_chipArrray();

	vTaskDelay(APP_START_DELAY_MS / portTICK_PERIOD_MS);

	ESP_LOGI(TAG, "Starting loop ");

	while (true) {
		// Check for blowers and keep a local copy of values for when the ui is changed
		num_available = update_rack_blower_list();
		burn_in_test_state_t state = get_test_state();
		if (state == -1) {
			ESP_LOGW(TAG, "Current state is in error needs to be handled ");
			vTaskDelay(30000 / portTICK_PERIOD_MS);
			continue;
		}

		//Periodically print out state
		if (count % 10 == 0) {
			ESP_LOGI(TAG,
					"Num Blowers: %d, Current State: %d Rack Powered On:%d Value changed:%d",
					num_available, state, rack_on, rack_power_changed);
			mem_prev = mem_cur;
			mem_cur = esp_get_free_heap_size();
			int mem_change = (mem_cur - mem_prev);
			ESP_LOGI(TAG,
					"[APP] Original heap:%lu, Free memory: %lu bytes, Heap change:%d",
					mem_original, mem_cur, mem_change);
		}

		// Starting Burn in state requires the rack to be powered on and the start button to be pressed
		// The default state is the cancel state and all test will continue to run until the cancel test is set
		if (state == STARTING_BURNIN_TEST && rack_on) {
			// Reset the test cycles if cancel event was initiated
			test_cycle = (test_cycle >= 0) ? test_cycle : 1;
			// Get the current values if available from the server
			get_ppb_values();

			// Need to delay for 1000 msec to gaurauntee that all samples have been collected
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			num_available = update_rack_blower_list();

			// This state is changed when the user presses start or a a cycle is finished and the rack is turned on
			ESP_LOGI(TAG, "Starting Burn in test conditions checking");
			check_rack_power_changed();
			/** FIXME: use the event handler to update the values */
			if (test_vals_acquire(10)) {
				ESP_LOGI(TAG, "Starting Burn in test Acquiring UI");

				//Acquired ui update the values
				burn_in_test_value_t *b_val;
				b_val = get_test_vals();

				// Update with current values
				setup_blower_(b_val);
				update_ui_blower_vals(b_val);
				test_vals_release();
				update_test_values();

				ESP_LOGI(TAG, "Rack Powered On: Updated test values");

				if (start_burnin() == ESP_OK) {
					ESP_LOGI(TAG, "Started burn in test");
					// Set the count to -1 so that the ui will update on the next loop
					count = -1;
				}
			} else {
				// Reset the flag since we did not update
				ESP_LOGW(TAG, "Could not acquire ui to start burn in");
				rack_power_changed = true;

			}

		} else if (state == RUNNING_BURNIN_TEST) {
			if (count % UPDATE_UI_COUNT == 0)
				// Update every
				if (test_vals_acquire(10)) {
					ESP_LOGI(TAG, "Starting Burn in test Acquiring UI");

					//Acquired ui update the values
					burn_in_test_value_t *b_val;
					b_val = get_test_vals();

					// Update with current values
					update_ui_blower_vals(b_val);
					test_vals_release();
					update_test_values();

					ESP_LOGI(TAG, "Rack Powered On: Updated test values");

					if (start_burnin() == ESP_OK) {
						ESP_LOGI(TAG, "Started burn in test");
					}
				}

		} else if (state == FINISHED_BURNIN_TEST && !rack_on) {
			// State transition if rack is turned off
			if (check_rack_power_changed()) {
				esp_err_t ret = start_cooldown();
				if (ret == ESP_OK) {
					ESP_LOGI(TAG, "Rack turned off transition state");
				}
			}
		} else if (state == RUNNING_COOLDOWN_TEST) {
			// Do something
			// Check that the rack is still off to continue the timer

		} else if (state == FINISHED_BURNIN_CYCLE && rack_on) {

			// Check rack on event
//			if (check_rack_power_changed()){
			// Only update if we successfully updated the state
			esp_err_t ret = update_test_state(STARTING_BURNIN_TEST);
			if (ret == ESP_OK) {
				test_cycle++;
				ESP_LOGI(TAG, "Finished: Running cycle %d", test_cycle);

//				}
			}

		}
		// Default state and test restart state
		// Resset all test values
		else if (state == CANCEL_BURNIN_TEST && test_cycle != -1) {

			if (test_vals_acquire(10)) {
				// TODO: Reset the test
				burn_in_test_value_t *b_val;
				b_val = get_test_vals();
				//				setup_blower_(b_val);
				init_blower_test(b_val);
				test_vals_release();
				//				update_test_state(STARTING_BURNIN_TEST);

				// Set test cycles to neg so we are not consistantly checking this
				test_cycle = -1;

			}
			ESP_LOGI(TAG, "Cancel Event reseting test: cycle %d",
					test_cycle);
		}

		vTaskDelay(APP_LOOP_RATE_MS / portTICK_PERIOD_MS);
		count++;
		update_test_values();

	}
	ESP_LOGE(TAG, "Burn In task canceled");

	vTaskDelete(burn_in_handle);
}

