/*
 * ui_blower_burn_in.c
 *
 *  Created on: Apr 23, 2023
 *      Author: ekalan
 */
#include <stddef.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "lvgl.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"

#include "ui.h"
#include "ui_blower_burn_in.h"
#include "ui_main.h"

// Used for debug and dev
#include "esp_random.h"
//#include "stdlib.h"

/**
 * ______ Forward Declarations _____
 */
extern lv_timer_t *ui_timer;
char test_blower_device_names[4][16] = {
		"Supply A",
		"Exhaust A",
		"Supply B",
		"Exhaust B" };

/**
 * ______ Static Functions _____
 */
static void init_colors(void);
static void set_array_to_default(int *arr, int length);
static void update_blower_test_val_ui(blower_test_label_map_t *b_lv,
		blower_test_value_t *b_vals);
static void setup_burnin_ui_structs(void);
static void setup_burnin_test_struct(void);
static void update_label_with_int(lv_obj_t *label, int value);
//void create_chart_with_data(lv_obj_t *chart, int *data_points, size_t data_points_count);
static void print_blower_vals(blower_test_value_t *b_val);
static void default_blower_test_val_ui(blower_test_label_map_t *b_lv,
		blower_test_value_t *b_vals);
static void detail_view_chart_init(lv_obj_t *chart);

/**
 * ______ Static Declartaions _____
 */
static lv_color_t lv_green;
static lv_color_t lv_red;
static lv_color_t lv_default;
static lv_color_t lv_light;
static lv_color_t lv_dark;
static lv_color_t lv_blue;

static lv_chart_series_t *series;

static burn_in_lv_obj_map_t brn_ui_map;
static burn_in_ui_value_t brn_val;

static lv_chart_series_t *ser1;
static lv_chart_series_t *ser2;

static SemaphoreHandle_t semaphore_burnin_display_values = NULL;
static char ip_v4[20];
static char node_name[20];

static int chart_data_burnin[NUM_OF_TEST];
static int chart_data_prev_val[NUM_OF_TEST];

static const char default_chip_id[16] = { "000" };
static const char *tag = "UI_blower-BI";

/**
 * ______ Public Functions _____
 */

/*
 * Call after the display has been initialized
 *
 */
esp_err_t init_test_vals(void) {
	// Initialise the ui structs

	ESP_LOGD(tag, "Initializing Blower burn in UI structure");
	setup_burnin_ui_structs();
	init_colors();
	detail_view_chart_init(ui_Blower_Val_Chart);

	esp_err_t res = ESP_OK;
	semaphore_burnin_display_values = xSemaphoreCreateBinary();

	if (semaphore_burnin_display_values == NULL) {
		ESP_LOGE(tag, "Could not create Semaphore.");
		res = ESP_FAIL;
	} else {
		ESP_LOGI(tag, "Created Semaphore.");
		xSemaphoreGive(semaphore_burnin_display_values);
	}

	setup_burnin_test_struct();
	update_test_values();
	// Initialize the series
	series = lv_chart_add_series(brn_ui_map.detail_lv.chart,
			lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);

	ESP_LOGD(tag, "Blower burn in UI structure initialized %s",
			((res==ESP_OK) ? "Success" : "Failed"));
	return res;

}

/*
 * Call function when updating the ui
 */
esp_err_t update_test_values(void) {
//	ESP_LOGI(tag, "Updating testing values: %d, values changed %d",brn_val.brn_state, brn_val.values_changed);

	if (test_vals_acquire(1)) {
// We do not need to update values if the test is cancelled and we are not restarting or no values have changed
//		if(brn_val.brn_state < CANCEL_BURNIN_TEST && brn_val.values_changed){
		if (brn_val.values_changed) {

			ESP_LOGD(tag, "Burn in state: %s, values changed %d",
					burnin_state_to_str(brn_val.brn_state),
					brn_val.values_changed);

			for (int i = 0; i < 4; i++) {
				blower_test_label_map_t *b_map = &brn_ui_map.blower_lv[i];
				blower_test_value_t *b_vals = &brn_val.blowers[i];

				// Check if any blower values changed
//		    	if (b_vals->values_changed && b_vals->is_testing) {
				if (b_vals->values_changed) {
					ESP_LOGI(tag, "Updating Blower %s",
							test_blower_device_names[i]);
//		    		print_blower_vals(b_vals);
					if (b_vals->is_testing) {
						update_blower_test_val_ui(b_map, b_vals);
						update_blower_id_ui(b_map, b_vals);

					} else {
						default_blower_test_val_ui(b_map, b_vals);
					}
					update_blower_status_ui(b_map->status_label, b_vals->state);

					b_vals->values_changed = false;

				}
				update_connect_values();
			}
		}
		brn_val.values_changed = false;
		test_vals_release();
		return ESP_OK;

	} else {
		ESP_LOGW(tag, "[%s, %d] Failed to acquire test values", __FUNCTION__,
				__LINE__);
	}
	return ESP_FAIL;
}

/*
 * Call function when updating the ui
 * Semaphore must be available
 */
esp_err_t update_test_state(burn_in_testing_state_t state) {
	burn_in_testing_state_t cur_state = ERROR_VAL;
	esp_err_t ret = ESP_FAIL;

	if (test_vals_acquire(1)) {
		ret = ESP_OK;
		cur_state = brn_val.brn_state;
		brn_val.brn_state = state;

		if (state == CANCEL_BURNIN_TEST) {
			// end the timer and clear the detail view
			brn_val.ui_timer = 0;
			update_timer_counter(ui_timer, 0);

		}
		test_vals_release();

		if (cur_state != state) {
			ESP_LOGI(tag,
					"%s, Updating testing state|Current: %s|Updated: %s|___________",
					__FUNCTION__,
					burnin_state_to_str(cur_state), burnin_state_to_str(state));

		}

	} else {
		ESP_LOGE(tag,
				"%s, Updating testing state| Could not acquire semaphore to update test state",
				__FUNCTION__);

	}
	return ret;
}

/*
 * Call function when not in UI thread
 * Function updates the current state to RUNNING_COOLDOWN_TEST
 *  and Resets the timer for the ui
 *  Ret the success of the operations
 */
esp_err_t start_cooldown() {
	burn_in_testing_state_t cur_state = get_burn_in_state();
	esp_err_t ret = ESP_FAIL;

	if (cur_state == ERROR_VAL) {
		ret = ESP_FAIL;
		ESP_LOGE(tag, "%s, Could not Start cooldown", __FUNCTION__);

	} else if (cur_state == FINISHED_BURNIN_TEST) {
		// Acquire the ui semaphore to update timer
		if (ui_acquire() == ESP_OK) {
			burn_in_cooldown_start(ui_timer);
			ui_release();
			// Update the state
			if (update_test_state(RUNNING_COOLDOWN_TEST) == ESP_OK) {
				ret = ESP_OK;
				ESP_LOGI(tag, "Starting Cooldown");

			}
		}
	} else if (cur_state == RUNNING_COOLDOWN_TEST) {
		// Already in state
		// TODO: check the timer to verify that it is running
		ESP_LOGW(tag, "Already running Cooldown Test");
	} else if (cur_state == STARTING_VALVE_TEST) {
			// Acquire the ui semaphore to update timer
		if (ui_acquire() == ESP_OK) {
			//TODO: create the burnin timer countdown
			burn_in_test_start(ui_timer);
			ui_release();
			// Update the state
			if (update_test_state(RUNNING_VALVE_TEST) == ESP_OK) {
				ret = ESP_OK;
				ESP_LOGI(tag, "Starting Valve Test");

			}
		}
		ESP_LOGW(tag, "Already running Cooldown Test");

	} else {
		ESP_LOGW(tag, "Calling Start_Cooldown from incorrect state");

	}
	return ret;
}

/*
 * Call function when not in UI thread
 * Function updates the current state to RUNNING_COOLDOWN_TEST
 *  and Resets the timer for the ui
 *  @return - the success of the operations
 */
esp_err_t start_burnin() {
	burn_in_testing_state_t cur_state = get_burn_in_state();
	esp_err_t ret = ESP_FAIL;

	if (cur_state == ERROR_VAL) {
		ret = ESP_FAIL;
		ESP_LOGW(tag, "%s, Could not Start burn in; In Error State",
				__FUNCTION__);

	} else if (cur_state == STARTING_BURNIN_TEST) {
		// Aquire the ui semaphore to update timer

		if (update_test_state(RUNNING_BURNIN_TEST) == ESP_OK) {
			ret = ESP_OK;
			ESP_LOGI(tag, "%s, Running burn in", __FUNCTION__);

		}
		if (ui_acquire() == ESP_OK) {
			burn_in_test_start(ui_timer);

			ui_release();
			// Update the state

		} else {
			ESP_LOGI(tag, "%s, Start_burnin failed to get ui Semaphore",
					__FUNCTION__);

		}

	} else if (cur_state == RUNNING_BURNIN_TEST) {
		// Already in state
		// TODO: check the timer to verify that it is running
		ESP_LOGW(tag, "%s, Already running burnin Test", __FUNCTION__);

	} else if (cur_state == CANCEL_BURNIN_TEST) {
		if (update_test_state(STARTING_BURNIN_TEST) == ESP_OK) {
			ret = ESP_OK;
			ESP_LOGI(tag, "%s, Running burn in", __FUNCTION__);

		} else {

			ESP_LOGE(tag, "%s, Failed to change state", __FUNCTION__);
		}

	} else {
		ESP_LOGW(tag, "%s, Calling Start_Cooldown from incorrect state",
				__FUNCTION__);

	}
	return ret;
}

burn_in_testing_state_t get_burn_in_state(void) {
	burn_in_testing_state_t ret = ERROR_VAL;
	if (test_vals_acquire(1)) {
// We do not need to update values if the test is cancelled and we are not restarting or no values have changed
//		ESP_LOGI(tag, "Current State: %d",brn_val.brn_state);
		ret = brn_val.brn_state;
		test_vals_release();
	}
	return ret;
}

esp_err_t ui_timer_finished(void) {
	burn_in_testing_state_t current_state = ERROR_VAL;
	burn_in_testing_state_t next_state = ERROR_VAL;
	esp_err_t ret = ESP_FAIL;
	ESP_LOGD(tag, "%s, Test timer finished", __FUNCTION__);

	// Timer State change
	// TODO: add global timer struct to update the state
	if (test_vals_acquire(10)) {
		current_state = brn_val.brn_state;
		test_vals_release();

		switch (current_state) {
			case STARTING_BURNIN_TEST:
				// Ignore timer
				next_state = STARTING_BURNIN_TEST;
				break;
			case RUNNING_BURNIN_TEST:
				// Burn in has finished and state should be updated
				next_state = FINISHED_BURNIN_TEST;
				ret = ESP_OK;
				break;
			case FINISHED_BURNIN_TEST:
				// No change in state
				next_state = FINISHED_BURNIN_TEST;
				ret = ESP_OK;
				break;
			case RUNNING_COOLDOWN_TEST:
				// Cooldown in has finished and state should be updated
				next_state = FINISHED_BURNIN_CYCLE;
				ret = ESP_OK;
				break;
			case FINISHED_BURNIN_CYCLE:
				// Already in the cool down cycle
				next_state = FINISHED_BURNIN_CYCLE;
				ret = ESP_OK;
				break;
			case STARTING_VALVE_TEST:
				// Ignore timer
				next_state = STARTING_VALVE_TEST;
				break;
			case RUNNING_VALVE_TEST:
				// Valve test has finished update the state
				next_state = FINISHED_VALVE_TEST;
				ret = ESP_OK;
				break;
			case FINISHED_VALVE_TEST:
				// Move to Finished burnin state
				next_state = FINISHED_BURNIN_CYCLE;
				ret = ESP_OK;
				break;
			case CANCEL_BURNIN_TEST:
				next_state = CANCEL_BURNIN_TEST;
				// State should not be in this state
				ret = ESP_FAIL;
				break;
			default:
				ESP_LOGE(tag,
						"Error: Hit default state when updating timer State: %d",
						current_state);
		}
		ret = update_test_state(next_state);
	} else {
		ESP_LOGE(tag,
				"Could not acquire Semaphore to update timer state change: Current State:%d",
				current_state);

	}

	return ret;

}

bool test_vals_acquire(uint32_t timeout_ms) {
	BaseType_t result = pdFALSE;
	if (semaphore_burnin_display_values != NULL) {
		TickType_t ticks_to_wait = pdMS_TO_TICKS(timeout_ms);
		result = xSemaphoreTake(semaphore_burnin_display_values, ticks_to_wait);
	}

	return (result == pdTRUE) ? true : false;
}

void test_vals_release(void) {
	xSemaphoreGive(semaphore_burnin_display_values);
}

/**
 * Retrieve the UI Blower Burn in Struct
 * Used to make changes to the ui values directly
 * Note: must have mutex to call this
 */
burn_in_ui_value_t* get_test_vals(void) {
	return &brn_val;
}

const char* burnin_state_to_str(burn_in_testing_state_t state) {
	switch (state) {
		case ERROR_VAL:
			return "ERROR_VAL";
		case STARTING_BURNIN_TEST:
			return "STARTING_BURNIN_TEST";
		case RUNNING_BURNIN_TEST:
			return "RUNNING_BURNIN_TEST";
		case FINISHED_BURNIN_TEST:
			return "FINISHED_BURNIN_TEST";
		case RUNNING_COOLDOWN_TEST:
			return "RUNNING_COOLDOWN_TEST";
		case FINISHED_BURNIN_CYCLE:
			return "FINISHED_BURNIN_CYCLE";
		case STARTING_VALVE_TEST:
			return "STARTING_VALVE_TEST";
		case RUNNING_VALVE_TEST:
			return "RUNNING_VALVE_TEST";
		case FINISHED_VALVE_TEST:
			return "FINISHED_VALVE_TEST";

		default:
			return "UNKNOWN_STATE";
	}
}

static void print_blower_vals(blower_test_value_t *b_val) {
	printf("Blower DevID: %s\n", b_val->name);
	printf("Blower ChipID: %s\n", b_val->chip_id);
	printf("   Offset: %d\n", b_val->offset);
	printf("   Range: %d\n", b_val->range);
	printf("   VAS Offset: %d\n", b_val->vas_offset);
	printf("   QC Offset: %d\n", b_val->qc_offset);
	printf("   Num Points: %d\n", b_val->num_point);

	for (size_t i = 0; i < NUM_OF_TEST; i++) {
		printf("  %d Offset: %d\n", i, b_val->burn_in_offset[i]);

	}

}

/**
 * Called by update _test_values if the connection attributes need to be updated
 *
 */
esp_err_t update_connect_values(void) {

	if (brn_val.app_connect_changed) {

		ESP_LOGD(tag, "%s, Updating burn in values", __FUNCTION__);
		// Update the ip:
		lv_label_set_text(ui_ESPadressLabel, brn_val.conect_info.ip_v4);
		// Update the ip:
		lv_label_set_text(ui_ESPNameLabel, brn_val.conect_info.node_name);
	}

	return ESP_OK;
}

//#define TESTING_CHART 1
#ifndef TESTING_CHART

esp_err_t update_detail_values(int dev_id) {
	esp_err_t ret = ESP_FAIL;
	ESP_LOGI(tag, " Updating details page for %s passed index:%d",
			test_blower_device_names[dev_id], dev_id);
	if (test_vals_acquire(10)) {

		// set the values for the selected blower
		blower_details_lv_obj_map_t *b_map = &brn_ui_map.detail_lv;
		blower_test_value_t *b_vals = &brn_val.blowers[dev_id];
		print_blower_vals(b_vals);

		//TODO: Check if there was an error
		int error_id = 0;
		// error_msg == 1 failed for range
		char *f_range = "Error:\n Over Range Limit";
		// error_msg == 2 failed for TEB
		char *f_TEB = "Error:\n Over TEB Limit";

		// error_msg == 3 failed for both range and TEB
		char *f_both = "Error:\n Over Range Limit\nOver TEB Limit";
		char *failed_msg[4] = { "  ", f_range, f_TEB, f_both };

		if (b_vals->state == FAILED_BLOWER_TEST) {
			if (b_vals->range > 11) {
				error_id += 1;
			}
			if (b_vals->max_val > 80 || b_vals->min_val < -80) {
				error_id += 2;
			}
			lv_label_set_text(ui_ErrorLable, failed_msg[error_id]);
			lv_obj_clear_flag(ui_ErrorLable, LV_OBJ_FLAG_HIDDEN);

		} else {
			lv_obj_add_flag(ui_ErrorLable, LV_OBJ_FLAG_HIDDEN);
		}
		lv_label_set_text_fmt(ui_QC_Value, "QC Value:  %d", b_vals->qc_offset);
		lv_label_set_text_fmt(ui_VasValue, "VAS Value:  %d",
				b_vals->vas_offset);

		// Update the title
		const char *name = b_vals->name;
		lv_label_set_text(b_map->name_label, name);
		ESP_LOGD(tag, " Updating chart Title %s", name);

// Update the chart
// Add the original 2 vals
//		size_t data_points_count = b_vals->num_point;

		ESP_LOGD(tag, " Populating chart");

		create_chart_with_data(b_map->chart, b_vals);
		/* TODO: set the VAS VALE and THe QC value on detail screen */
		ret = ESP_OK;
		test_vals_release();
	}
	ESP_LOGI(tag, " Finished updating detail page: %d", ret);
	return ret;
}

#else

		 esp_err_t update_detail_values(int dev_id) {
		 esp_err_t ret = ESP_FAIL;
		 ESP_LOGI(tag, " Updating details page for %s passed index:%d",
		 test_blower_device_names[dev_id], dev_id);
		 if (test_vals_acquire(10)) {

		 // set the values for the selected blower
		 blower_details_lv_obj_map_t *b_map = &brn_ui_map.detail_lv;
		 blower_test_value_t *b_vals = &brn_val.blowers[dev_id];

		 sprintf(b_vals->chip_id, "1234567");
		 b_vals->is_testing = true;
		 b_vals->values_changed = true;
		 b_vals->offset = 2;
		 b_vals->range = 10;
		 b_vals->vas_offset = -5;
		 b_vals->qc_offset = 5;
		 b_vals->min_val = -5;
		 b_vals->max_val = 5;
		 b_vals->num_point = 6;

		 for (int i = 0; i < b_vals->num_point; i++) {

		 b_vals->burn_in_offset[i] = (lv_rand(-5, 5));
		 }
		 print_blower_vals(b_vals);
		 //

		 // Update the title
		 const char *name = b_vals->name;
		 lv_label_set_text(b_map->name_label, name);
		 ESP_LOGD(tag, " Updating chart Title %s", name);

		 // Update the chart
		 // Add the original 2 vals
		 //		size_t data_points_count = b_vals->num_point;

		 ESP_LOGD(tag, " Populating chart");

		 create_chart_with_data(b_map->chart, b_vals);
		 ret = ESP_OK;
		 test_vals_release();
		 }
		 ESP_LOGI(tag, " Finished updating detail page: %d", ret);
		 return ret;
		 }

		 #endif  // TESTING_CHART

/** ______ Private Functions _____ */

/**
 *Sets array to default values that are considered to be not set
 */
static void set_array_to_default(int *arr, int length) {
	for (int i = 0; i < length; i++) {
		arr[i] = DEF_OFFSET_VAL;
	}
}

static void setup_burnin_test_struct(void) {
	ESP_LOGI(tag, " Setting up burnin blower structs to default");
	brn_val.brn_state = CANCEL_BURNIN_TEST;
	brn_val.values_changed = true;
	for (int i = 0; i < 4; i++) {
		brn_val.blowers[i].is_testing = false;
		brn_val.blowers[i].values_changed = true;

		strcpy(brn_val.blowers[i].name, test_blower_device_names[i]);
		strcpy(brn_val.blowers[i].chip_id, default_chip_id);

		brn_val.blowers[i].offset = DEF_OFFSET_VAL;
		brn_val.blowers[i].range = DEF_OFFSET_VAL;
		brn_val.blowers[i].vas_offset = DEF_OFFSET_VAL;
		brn_val.blowers[i].qc_offset = DEF_OFFSET_VAL;
		set_array_to_default(brn_val.blowers[i].burn_in_offset,
		NUM_OF_TEST);
		brn_val.blowers[i].min_val = DEF_OFFSET_VAL;
		brn_val.blowers[i].max_val = DEF_OFFSET_VAL;
		brn_val.blowers[i].num_point = 0;
		brn_val.blowers[i].state = UNINIT_BLOWER_TEST;

	}
	brn_val.cur_screen = START_SCR;
	brn_val.power_state = false;
	brn_val.app_con = NA_APP_STATUS;

	// set default ui_connection info
	brn_val.conect_info.wifi_connected = false;
	brn_val.conect_info.ip_v4 = ip_v4;
	brn_val.conect_info.usb_connected = false;
	brn_val.conect_info.mqtt_connected = false;
	brn_val.conect_info.node_name = node_name;
	brn_val.conect_info.modbus_connected = false;
	brn_val.conect_info.blt_connected = false;
	brn_val.app_connect_changed = false;
	// Set up Detail screen
	brn_val.detail_scrn.device_index = NA_INDEX;
	brn_val.detail_scrn.vas_val = 0;
	brn_val.detail_scrn.qc_val = 0;
	brn_val.detail_scrn.cur_val = 0;
	brn_val.detail_scrn.len = 0;

	brn_val.ui_timer = -1;

}

static void setup_burnin_ui_structs(void) {
	// Detailed label pointer mapping
	ESP_LOGD(tag, "%s, Initiated ui struct", __FUNCTION__);

	brn_ui_map.detail_lv.name_label = ui_BlowerHeaderLabel;
	brn_ui_map.detail_lv.chart = ui_Blower_Val_Chart;
	brn_ui_map.detail_lv.pre_offset = NULL;
	brn_ui_map.detail_lv.post_offset = NULL;
	brn_ui_map.detail_lv.min_offset_label = NULL;
	brn_ui_map.detail_lv.max_offset_label = NULL;

	// Supply A pointer Mapping
	brn_ui_map.blower_lv[0].panel = ui_BloweSAPanel;
	brn_ui_map.blower_lv[0].name_label = ui_BlowerLabel3;
	brn_ui_map.blower_lv[0].chip_id_label = ui_ChipIDValLabel3;
	brn_ui_map.blower_lv[0].offset_label = ui_OffsetValLabel3;
	brn_ui_map.blower_lv[0].range_label = ui_RangeValLabel3;
	brn_ui_map.blower_lv[0].status_label = ui_PassedLabel3;

	// Exhaust A pointer Mapping
	brn_ui_map.blower_lv[1].panel = ui_BlowerEAPanel;
	brn_ui_map.blower_lv[1].name_label = ui_BlowerLabel1;
	brn_ui_map.blower_lv[1].chip_id_label = ui_ChipIDValLabel1;
	brn_ui_map.blower_lv[1].offset_label = ui_OffsetValLabel1;
	brn_ui_map.blower_lv[1].range_label = ui_RangeValLabel1;
	brn_ui_map.blower_lv[1].status_label = ui_PassedLabel1;

	// Supply B pointer Mapping
	brn_ui_map.blower_lv[2].panel = ui_BlowerSAPanel2;
	brn_ui_map.blower_lv[2].name_label = ui_BlowerLabel4;
	brn_ui_map.blower_lv[2].chip_id_label = ui_ChipIDValLabel4;
	brn_ui_map.blower_lv[2].offset_label = ui_OffsetValLabel4;
	brn_ui_map.blower_lv[2].range_label = ui_RangeValLabel4;
	brn_ui_map.blower_lv[2].status_label = ui_PassedLabel4;

	// Exhaust B pointer Mapping
	brn_ui_map.blower_lv[3].panel = ui_BlowerSAPanel;
	brn_ui_map.blower_lv[3].name_label = ui_BlowerLabel2;
	brn_ui_map.blower_lv[3].chip_id_label = ui_ChipIDValLabel2;
	brn_ui_map.blower_lv[3].offset_label = ui_OffsetValLabel2;
	brn_ui_map.blower_lv[3].range_label = ui_RangeValLabel2;
	brn_ui_map.blower_lv[3].status_label = ui_PassedLabel2;

}

static void update_label_with_int(lv_obj_t *label, int value) {
	char str[16];
	sprintf(str, "%d", value);
	lv_label_set_text(label, str);
}

static void update_blower_test_val_ui(blower_test_label_map_t *b_lv,
		blower_test_value_t *b_vals) {
	int tmp = (b_vals->offset == DEF_OFFSET_VAL) ? 0 : b_vals->offset;
	update_label_with_int(b_lv->offset_label, tmp);
	update_label_with_int(b_lv->range_label, b_vals->range);
}

static void default_blower_test_val_ui(blower_test_label_map_t *b_lv,
		blower_test_value_t *b_vals) {

	lv_label_set_text(b_lv->name_label, b_vals->name);
	lv_label_set_text(b_lv->offset_label, " ");
	lv_label_set_text(b_lv->range_label, " ");
	lv_label_set_text(b_lv->chip_id_label, " ");
}

void update_blower_id_ui(blower_test_label_map_t *b_lv,
		blower_test_value_t *b_vals) {

	lv_label_set_text(b_lv->name_label, b_vals->name);
	lv_label_set_text(b_lv->chip_id_label, b_vals->chip_id);
}

/** _____RH HANDLER Functions______*/

esp_err_t handle_ip_addr(char *ip_v4) {
	// Copy into config for setting to be updated next time update is called
	esp_err_t ret = ESP_FAIL;
	ESP_LOGI(tag, " Updating Settings IP to %s ",
			ip_v4);
	if (test_vals_acquire(10)) {
		// set the values for the selected blower
		strcpy(brn_val.conect_info.ip_v4, ip_v4);
		// Set the update marker
		brn_val.app_connect_changed = true;
		ret = ESP_OK;
		test_vals_release();
	}
	ESP_LOGI(tag, " Finished updating detail page: %d", ret);
	return ret;
}

esp_err_t handle_mqtt_status(bool mqtt_connected) {
	// Copy into config for setting to be updated next time update is called
	esp_err_t ret = ESP_FAIL;
	ESP_LOGI(tag, " Updating MQTT Connected to %d ",
			mqtt_connected);
	if (test_vals_acquire(10)) {
		// set the values for the selected blower
		brn_val.conect_info.mqtt_connected = mqtt_connected;
		// Set the update marker
		brn_val.app_connect_changed = true;
		ret = ESP_OK;
		test_vals_release();
	}
	ESP_LOGI(tag, " Finished updating detail page: %d", ret);
	return ret;
}

esp_err_t handle_modbus_power(bool modbus_connected) {
	// Copy into config for setting to be updated next time update is called
	esp_err_t ret = ESP_FAIL;
	ESP_LOGI(tag, " Updating Modbus Connected to %d ",
			modbus_connected);
	if (test_vals_acquire(10)) {
		// set the values for the selected blower
		brn_val.conect_info.modbus_connected = modbus_connected;
		// Set the update marker
		brn_val.app_connect_changed = true;
		ret = ESP_OK;
		test_vals_release();
	}
	ESP_LOGI(tag, " Finished updating detail page: %d", ret);
	return ret;
}

esp_err_t handle_node_name(char *node_name) {
	// Copy into config for setting to be updated next time update is called
	esp_err_t ret = ESP_FAIL;
	ESP_LOGI(tag, " Updating Node Name to %s ",
			node_name);
	if (test_vals_acquire(10)) {
		// set the values for the selected blower
		strcpy(brn_val.conect_info.node_name, node_name);
		// Set the update marker
		brn_val.app_connect_changed = true;
		ret = ESP_OK;
		test_vals_release();
	}
	ESP_LOGI(tag, " Finished updating detail page: %d", ret);
	return ret;
}

esp_err_t handle_serial_status(bool usb_connected) {
	// Copy into config for setting to be updated next time update is called
	esp_err_t ret = ESP_FAIL;
	ESP_LOGI(tag, " Updating USB Connected to %d ",
			usb_connected);
	if (test_vals_acquire(10)) {
		// set the values for the selected blower
		brn_val.conect_info.usb_connected = usb_connected;
		// Set the update marker
		brn_val.app_connect_changed = true;
		ret = ESP_OK;
		test_vals_release();
	}
	ESP_LOGI(tag, " Finished updating detail page: %d", ret);
	return ret;
}

esp_err_t handle_blt_status(bool blt_connected) {
	// Copy into config for setting to be updated next time update is called
	esp_err_t ret = ESP_FAIL;
	ESP_LOGI(tag, " Updating Blue Tooth Connected to %d ",
			blt_connected);
	if (test_vals_acquire(10)) {
		// set the values for the selected blower
		brn_val.conect_info.blt_connected = blt_connected;
		// Set the update marker
		brn_val.app_connect_changed = true;
		ret = ESP_OK;
		test_vals_release();
	}
	ESP_LOGI(tag, " Finished updating detail page: %d", ret);
	return ret;
}

static void init_colors(void) {
	lv_default = lv_color_hex(0x000000);
	lv_red = lv_color_hex(0xeb0404);
	lv_green = lv_color_hex(0x00db24);
	lv_dark = lv_color_hex(0x000024);
	lv_light = lv_color_hex(0xe0e0e0);
	lv_blue = lv_color_hex(0x002189);
}

void update_blower_status_ui(lv_obj_t *lv_label,
		blower_testing_state_t state) {
	const char *state_str;
	lv_color_t text_color;
	lv_color_t bg_color;

	switch (state) {
		case UNINIT_BLOWER_TEST:
			state_str = "No Blower";
			text_color = lv_dark;
			bg_color = lv_light;
			break;
		case STARTING_BLOWER_TEST:
			state_str = "Starting";
			text_color = lv_blue;
			bg_color = lv_light;
			break;
		case RUNNING_BLOWER_TEST:
			state_str = "Running";
			text_color = lv_default;
			bg_color = lv_light;
			break;
		case SUCCESS_BLOWER_TEST:
			state_str = "Passed";
			text_color = lv_green;
			bg_color = lv_light;
			break;
		case FAILED_BLOWER_TEST:
			state_str = "Failed";
			text_color = lv_red;
			bg_color = lv_light;
			break;
		default:
			state_str = "";
			text_color = lv_default;
			bg_color = lv_dark;
			break;
	}
	lv_label_set_text(lv_label, state_str);

//		lv_obj_set_style_bg_color(lv_label, bg_color,
//				LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(lv_label, text_color,
			LV_PART_MAIN | LV_STATE_DEFAULT);
//	    lv_style_set_text_color(&style, text_color);
//	    lv_style_set_bg_color(&style, bg_color);
//	    lv_obj_set_style(lv_label, &style);
}

void create_chart_with_data(lv_obj_t *chart,
		blower_test_value_t *b_vals) {

	size_t data_points_count = b_vals->num_point;
	int *data_points = b_vals->burn_in_offset;

	if (!data_points || data_points_count == 0) {
		ESP_LOGV(tag,
				"[%s, %d] Populating chart failed burn in address=%p, num_vals=%d",
				__FUNCTION__, __LINE__, (void* )data_points,
				data_points_count);
	}
	ESP_LOGI(tag, " Populating chart values");
	ESP_LOGD(tag, " value 0 = %d", data_points[0]);

	// Clear out chart data DEF_OFFSET_VAL are not displayed
	memset(chart_data_burnin, DEF_OFFSET_VAL,
			sizeof(chart_data_burnin));
	memset(chart_data_prev_val, DEF_OFFSET_VAL,
			sizeof(chart_data_prev_val));

	// Setting the previous calibration values in the array
	chart_data_prev_val[0] = b_vals->vas_offset;
	chart_data_prev_val[1] = b_vals->qc_offset;

	lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
	lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_CIRCULAR);




	int16_t min_value =
			(b_vals->min_val > b_vals->vas_offset) ? b_vals->vas_offset : b_vals->min_val;
	min_value =
				(b_vals->min_val > b_vals->qc_offset) ? b_vals->qc_offset : b_vals->min_val;

	 min_value =
					(b_vals->min_val < -200 || b_vals->min_val > 200) ? -80 : b_vals->min_val;


	int16_t max_value =
				(b_vals->max_val < b_vals->qc_offset) ? b_vals->qc_offset : b_vals->max_val;
	max_value =
				(b_vals->max_val < b_vals->vas_offset) ? b_vals->vas_offset : b_vals->max_val;
	max_value =
				(b_vals->max_val > 200) ? 80 : b_vals->max_val;

	// Added to keep historic values in view
	for (size_t i = 0; i < b_vals->num_point; i++) {
		min_value =
				(min_value < data_points[i]) ? min_value :
												data_points[i];
		max_value =
				(max_value > data_points[i]) ? max_value :
												data_points[i];

		ESP_LOGD(tag,
				"%d     Chart Scale(%d): Min:%d Max:%d data_point:%d",
				i,
				b_vals->num_point, min_value, max_value,
				data_points[i]);

	}

	int16_t range = max_value - min_value;

//			(b_vals->range == DEF_OFFSET_VAL) ?
//												20 :
//												max_value - min_value + 10;

	range =
			(range < 10) ? 10 : range+1;
	range =
			(range > 160) ? 160 : range-1;

	ESP_LOGI(tag, " Chart Scale: Min:%d Max:%d Range:%d", min_value,
			max_value,
			range);

	//Calculate the number of ticks needed
	//  - Want at least 5 ticks but if range is very large use a larger range
	int16_t tick_step =
			(range / TICK_MIN < TICK_MIN) ? TICK_MIN : range / TICK_MIN;

	int16_t y_min = (((min_value - tick_step +1) / tick_step));
	int16_t y_max = ((max_value + tick_step - 1) / tick_step)
			* tick_step;



	lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, y_min, y_max);
	lv_chart_set_range(chart, LV_CHART_AXIS_SECONDARY_Y, y_min, y_max);

	uint16_t num_ticks_y = (y_max - y_min) / tick_step + 1;

	ESP_LOGI(tag, " Chart tick: Steps:%d, Min:%d Max:%d num ticks:%d", tick_step, y_min,
				y_max, num_ticks_y);

	lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 5, 1,
			num_ticks_y, 1,
			true, 30);

	for (size_t i = 0; i < 10; i++) {
		if (i < 2) {
			ESP_LOGD(tag, "[%s, %d] 1 adding ser1, ser2= (%d, %d)",
					__FUNCTION__,
					__LINE__, (int16_t )chart_data_prev_val[i],
					LV_CHART_POINT_NONE);

			lv_chart_set_next_value(chart, ser1,
					(int16_t) chart_data_prev_val[i]);
			lv_chart_set_next_value(chart, ser2, LV_CHART_POINT_NONE);

		}
		else if (i < data_points_count + 2) {
			ESP_LOGD(tag, "[%s, %d] 2 adding ser1, ser2= (%d, %d)",
					__FUNCTION__,
					__LINE__, LV_CHART_POINT_NONE,
					(int16_t )data_points[i - 2]);

			lv_chart_set_next_value(chart, ser1,
					(int16_t) LV_CHART_POINT_NONE);
			lv_chart_set_next_value(chart, ser2,
					(int16_t) data_points[i - 2]);
		}
		else {
			ESP_LOGD(tag, "[%s, %d] 3 adding ser1, ser2= (%d, %d)",
					__FUNCTION__,
					__LINE__, LV_CHART_POINT_NONE, LV_CHART_POINT_NONE);

			lv_chart_set_next_value(chart, ser1, LV_CHART_POINT_NONE);
			lv_chart_set_next_value(chart, ser2, LV_CHART_POINT_NONE);

		}

		ESP_LOGD(tag, "[%s, %d] loop %d)", __FUNCTION__, i);

	}
	ESP_LOGD(tag, "[%s, %d]Finished adding points", __FUNCTION__,
			__LINE__);

	lv_chart_refresh(chart); /*Required after direct set*/

}

static void detail_view_chart_init(lv_obj_t *chart) {
	ESP_LOGD(tag, "%s, Initiated ui Chart", __FUNCTION__);

	ser1 = lv_chart_add_series(chart, lv_color_make(0xff, 0x66, 0x66),
			LV_CHART_AXIS_PRIMARY_Y);
	lv_chart_set_x_start_point(chart, ser1, 0);
	ser2 = lv_chart_add_series(chart, lv_color_make(0x33, 0x33, 0xff),
			LV_CHART_AXIS_SECONDARY_Y);
	lv_chart_set_x_start_point(chart, ser2, 0);
	lv_chart_set_point_count(chart, 10);
	return;
}

/** ______TESTING FUNCTIONS____*/

// For testing
void setup_blower_random_struct(blower_test_value_t *b_vals) {
	brn_val.brn_state = STARTING_BURNIN_TEST;
	brn_val.values_changed = true;
//	srand(time(NULL));   // Initialization, should only be called once.

	for (int i = 0; i < 4; i++) {
		brn_val.blowers[i].is_testing = true;
		brn_val.blowers[i].values_changed = true;

		strcpy(brn_val.blowers[i].name, test_blower_device_names[i]);
		strcpy(brn_val.blowers[i].chip_id, default_chip_id);
		int min_v = 100;
		int max_v = -100;

//		set_array_to_default(brn_val.blowers[i].burn_in_offset, NUM_OF_TEST);
		for (int ii = 0; ii < NUM_OF_TEST; ii++) {
			int tmp = ((rand() % 10) - 5);
			min_v = (min_v < tmp) ? min_v : tmp;
			max_v = (max_v > tmp) ? max_v : tmp;

			brn_val.blowers[i].burn_in_offset[ii] = tmp;
		}
		brn_val.blowers[i].offset =
				brn_val.blowers[i].burn_in_offset[9];
		brn_val.blowers[i].range = max_v - min_v;
		brn_val.blowers[i].vas_offset =
				brn_val.blowers[i].burn_in_offset[0];
		brn_val.blowers[i].qc_offset =
				brn_val.blowers[i].burn_in_offset[1];
		brn_val.blowers[i].min_val = min_v;
		brn_val.blowers[i].max_val = max_v;
		brn_val.blowers[i].num_point = 3;

	}
}
