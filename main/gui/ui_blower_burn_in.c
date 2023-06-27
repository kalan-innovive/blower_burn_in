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
//#include "lvgl/lvgl.h"



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
extern lv_timer_t *ui_timer;

static void init_colors(void);
static void set_array_to_default(int *arr, int length);
static void update_blower_test_val_ui(blower_test_label_map_t *b_lv, blower_test_value_t *b_vals);
static void setup_burnin_ui_structs(void);
static void setup_burnin_test_struct(void);
static void update_label_with_int(lv_obj_t *label, int value);
void create_chart_with_data(lv_obj_t *chart, int *data_points, size_t data_points_count);
static void print_blower_vals(blower_test_value_t *b_val);
static void default_blower_test_val_ui(blower_test_label_map_t *b_lv, blower_test_value_t *b_vals);



static lv_color_t lv_green ;
static lv_color_t lv_red;
static lv_color_t lv_default;
static lv_color_t lv_light;
static lv_color_t lv_dark;
static lv_color_t lv_blue;


static lv_chart_series_t *series;

static burn_in_lv_obj_map_t brn_ui_map;
static burn_in_test_value_t brn_val;

SemaphoreHandle_t semaphore_burnin_display_values = NULL;

char test_blower_device_names[4][16] = {"Supply A", "Exhaust A","Supply B", "Exhaust B"};
const char default_chip_id[16] = {"000"};
const char *tag = "UI_blower-BI";


bool test_vals_acquire(uint32_t timeout_ms) {
	BaseType_t result = pdFALSE;
	if (semaphore_burnin_display_values != NULL){
		TickType_t ticks_to_wait = pdMS_TO_TICKS(timeout_ms);
		result = xSemaphoreTake(semaphore_burnin_display_values, ticks_to_wait);
	}

    return (result == pdTRUE) ? true : false;
}


void test_vals_release(void) {
    xSemaphoreGive(semaphore_burnin_display_values);
}

burn_in_test_value_t* get_test_vals(void){
	return &brn_val;
}


static void set_array_to_default(int *arr, int length) {
    for (int i = 0; i < length; i++) {
        arr[i] = DEF_OFFSET_VAL;
    }
}

const char* t_state_to_str(burn_in_test_state_t state) {
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
		case CANCEL_BURNIN_TEST:
			return "CANCEL_BURNIN_TEST";
		default:
			return "UNKNOWN_STATE";
	}
}

static void setup_burnin_test_struct(void){
	ESP_LOGI(tag, " Setting up burnin blower structs to default");
	brn_val.brn_state = CANCEL_BURNIN_TEST;
	brn_val.values_changed = true;
	for (int i = 0; i<4; i++) {
		brn_val.blowers[i].is_testing = false;
		brn_val.blowers[i].values_changed = true;

		strcpy(brn_val.blowers[i].name, test_blower_device_names[i]);
		strcpy(brn_val.blowers[i].chip_id, default_chip_id);

		brn_val.blowers[i].offset = DEF_OFFSET_VAL;
		brn_val.blowers[i].range = DEF_OFFSET_VAL;
		brn_val.blowers[i].pre_rec_offset = DEF_OFFSET_VAL;
		brn_val.blowers[i].post_rec_offset = DEF_OFFSET_VAL;
		set_array_to_default(brn_val.blowers[i].burn_in_offset, NUM_OF_TEST);
		brn_val.blowers[i].min_val = DEF_OFFSET_VAL;
		brn_val.blowers[i].max_val = DEF_OFFSET_VAL;
		brn_val.blowers[i].num_point = 0;
		brn_val.blowers[i].state = UNINIT_BLOWER_TEST;


	}
}

// For testing
void setup_blower_random_struct(blower_test_value_t *b_vals){
	brn_val.brn_state = STARTING_BURNIN_TEST;
	brn_val.values_changed = true;
//	srand(time(NULL));   // Initialization, should only be called once.

	for (int i = 0; i<4; i++) {
		brn_val.blowers[i].is_testing = true;
		brn_val.blowers[i].values_changed = true;

		strcpy(brn_val.blowers[i].name, test_blower_device_names[i]);
		strcpy(brn_val.blowers[i].chip_id, default_chip_id);
		int min_v = 100;
		int max_v = -100;

//		set_array_to_default(brn_val.blowers[i].burn_in_offset, NUM_OF_TEST);
		for (int ii = 0; ii< NUM_OF_TEST; ii++){
			int tmp  = ((rand() % 10) - 5);
			min_v = (min_v < tmp) ? min_v : tmp;
			max_v = (max_v > tmp) ? max_v : tmp;

			brn_val.blowers[i].burn_in_offset[ii] = tmp;
		}
		brn_val.blowers[i].offset =brn_val.blowers[i].burn_in_offset[9];
		brn_val.blowers[i].range = max_v - min_v ;
		brn_val.blowers[i].pre_rec_offset = brn_val.blowers[i].burn_in_offset[0];
		brn_val.blowers[i].post_rec_offset = brn_val.blowers[i].burn_in_offset[1];
		brn_val.blowers[i].min_val = min_v;
		brn_val.blowers[i].max_val = max_v;
		brn_val.blowers[i].num_point = 3;


	}
}



static void setup_burnin_ui_structs(void){
	// Detailed label pointer mapping

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
/*
 * Call after the display has been initialized
 *
 */
esp_err_t init_test_vals(void){
	// Initialise the ui structs

	ESP_LOGI(tag, "Initializing Blower burn in UI structure");
	setup_burnin_ui_structs();
	init_colors();



	esp_err_t res = ESP_OK;
	semaphore_burnin_display_values = xSemaphoreCreateBinary();

//	res = test_vals_acquire(0);

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
    series = lv_chart_add_series(brn_ui_map.detail_lv.chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);

	ESP_LOGI(tag, "Blower burn in UI structure initialized %s", ((res==ESP_OK) ? "Success" : "Failed"));
	return res;


}


static void update_label_with_int(lv_obj_t *label, int value) {
    char str[16];
    sprintf(str, "%d", value);
    lv_label_set_text(label, str);
}

static void update_blower_test_val_ui(blower_test_label_map_t *b_lv, blower_test_value_t *b_vals){
	int tmp = ( b_vals->offset == DEF_OFFSET_VAL) ? 0: b_vals->offset;
	update_label_with_int(b_lv->offset_label,tmp);
	update_label_with_int(b_lv->range_label, b_vals->range);
}


static void default_blower_test_val_ui(blower_test_label_map_t *b_lv, blower_test_value_t *b_vals){

	lv_label_set_text(b_lv->name_label, b_vals->name);
	lv_label_set_text(b_lv->offset_label, " ");
	lv_label_set_text(b_lv->range_label, " ");
	lv_label_set_text(b_lv->chip_id_label, " ");
}


static void update_blower_id_ui(blower_test_label_map_t *b_lv, blower_test_value_t *b_vals){

	lv_label_set_text(b_lv->name_label, b_vals->name);
	lv_label_set_text(b_lv->chip_id_label, b_vals->chip_id);
}



static void init_colors(void){
	lv_default = lv_color_hex(0x000000);
	lv_red = lv_color_hex(0xeb0404);
	lv_green = lv_color_hex(0x00db24);
	lv_dark = lv_color_hex(0x000024);
	lv_light = lv_color_hex(0xe0e0e0);
	lv_blue = lv_color_hex(0x002189);
}

static void update_blower_status_ui(lv_obj_t *lv_label, blower_test_state_t state){
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

/*
 * Call function when updating the ui
 */
esp_err_t update_test_values(void){
//	ESP_LOGI(tag, "Updating testing values: %d, values changed %d",brn_val.brn_state, brn_val.values_changed);

	if (test_vals_acquire(1)){
		// We do not need to update values if the test is cancelled and we are not restarting or no values have changed
//		if(brn_val.brn_state < CANCEL_BURNIN_TEST && brn_val.values_changed){
		if( brn_val.values_changed){

			ESP_LOGI(tag, "Burn in state: %d, values changed %d",brn_val.brn_state, brn_val.values_changed);

		    for (int i = 0; i < 4; i++) {
		    	blower_test_label_map_t *b_map = &brn_ui_map.blower_lv[i];
		    	blower_test_value_t *b_vals = &brn_val.blowers[i];

		    	// Check if any blower values changed
//		    	if (b_vals->values_changed && b_vals->is_testing) {
		    	if (b_vals->values_changed ){
		    		ESP_LOGI(tag, "Updating Blower %s", test_blower_device_names[i]);
//		    		print_blower_vals(b_vals);
		    		if (b_vals->is_testing){
						update_blower_test_val_ui(b_map, b_vals);
			    		update_blower_id_ui(b_map, b_vals);

		    		} else {
		    			default_blower_test_val_ui(b_map, b_vals);
		    		}
					update_blower_status_ui(b_map->status_label, b_vals->state);

		    		b_vals->values_changed = false;

		    	}
		    }
		}
		brn_val.values_changed = false;
		test_vals_release();
		return ESP_OK;

	}
	return ESP_FAIL;
}

/*
 * Call function when updating the ui
 */
esp_err_t update_test_state(burn_in_test_state_t state){
	burn_in_test_state_t cur_state = ERROR_VAL;
	esp_err_t ret  = ESP_FAIL;

	if (test_vals_acquire(1)){
		ret =  ESP_OK;
		cur_state = brn_val.brn_state;
		brn_val.brn_state = state;
		test_vals_release();

		if (cur_state != state){
			ESP_LOGI(tag, "Updating testing state|Current: %s|Updated: %s|",t_state_to_str(cur_state), t_state_to_str(state));
		}

	} else {
		ESP_LOGI(tag, "Updating testing state|Could not acquire semaphore to update test state");

	}
	return ret;
}


/*
 * Call function when not in UI thread
 * Function updates the current state to RUNNING_COOLDOWN_TEST
 *  and Resets the timer for the ui
 *  Ret the success of the operations
 */
esp_err_t start_cooldown(){
	burn_in_test_state_t cur_state = get_test_state();
	esp_err_t ret  = ESP_FAIL;

	if (cur_state == ERROR_VAL){
		ret =  ESP_FAIL;
		ESP_LOGW(tag, "Could not Start cooldown");

	}else if  (cur_state == FINISHED_BURNIN_TEST){
		// Aquire the ui semaphore to update timer
		if (ui_acquire() == ESP_OK){
			burn_in_cooldown_start(ui_timer);
			ui_release();
			// Update the state
			if (update_test_state(RUNNING_COOLDOWN_TEST) == ESP_OK) {
				ret = ESP_OK;
				ESP_LOGI(tag, "Starting Cooldown");

			}
		}

	}else if  (cur_state == RUNNING_COOLDOWN_TEST){
		// Already in state
		// TODO: check the timer to verify that it is running
		ESP_LOGW(tag, "Already running Cooldown Test");

	}else {
		ESP_LOGW(tag, "Calling Start_Cooldown from incorrect state");

	}
	return ret;
}

/*
 * Call function when not in UI thread
 * Function updates the current state to RUNNING_COOLDOWN_TEST
 *  and Resets the timer for the ui
 *  Ret the success of the operations
 */
esp_err_t start_burnin(){
	burn_in_test_state_t cur_state = get_test_state();
	esp_err_t ret  = ESP_FAIL;

	if (cur_state == ERROR_VAL){
		ret =  ESP_FAIL;
		ESP_LOGW(tag, "Could not Start burnin");

	}else if  (cur_state == STARTING_BURNIN_TEST){
		// Aquire the ui semaphore to update timer
		if (ui_acquire() == ESP_OK){
			if (update_test_state(RUNNING_BURNIN_TEST) == ESP_OK) {
				ret = ESP_OK;
				ESP_LOGI(tag, "Starting burnin");
				burn_in_test_start(ui_timer);
			}
			ui_release();
			// Update the state

		} else{
			ESP_LOGI(tag, "start_burnin failed to get ui Semaphore");

		}

	}else if  (cur_state == RUNNING_BURNIN_TEST){
		// Already in state
		// TODO: check the timer to verify that it is running
		ESP_LOGW(tag, "Already running burnin Test");

	}else {
		ESP_LOGW(tag, "Calling Start_Cooldown from incorrect state");

	}
	return ret;
}

burn_in_test_state_t get_test_state(void){
	blower_test_state_t ret =ERROR_VAL;
	if (test_vals_acquire(1)){
		// We do not need to update values if the test is cancelled and we are not restarting or no values have changed
//		ESP_LOGI(tag, "Current State: %d",brn_val.brn_state);
		ret =  brn_val.brn_state;
		test_vals_release();
	}
	return ret;
}

esp_err_t test_timer_finished(void){
	burn_in_test_state_t current_state = ERROR_VAL;
	burn_in_test_state_t next_state = ERROR_VAL;
	esp_err_t ret = ESP_FAIL;
	ESP_LOGI(tag, "Test timer finished");


	// Timer State change
	// TODO: add global timer struct to update the state
	if (test_vals_acquire(10)){
		current_state = brn_val.brn_state;
		test_vals_release();
		switch (current_state){
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
		case CANCEL_BURNIN_TEST:
			next_state = CANCEL_BURNIN_TEST;
			// State should not be in this state
			ret = ESP_FAIL;
			break;
		default:
			ESP_LOGE(tag, "Error: Hit default state when updating timer State: %d", current_state);
		}
		update_test_state(next_state);
	} else {
		ESP_LOGW(tag, "Could not acquire Semaphore to update timer state change: Current State:%d",  current_state);

	}

	return ret;

}

static void print_blower_vals(blower_test_value_t *b_val) {
	printf("Blower DevID: %s\n", b_val->name);
	printf("Blower ChipID: %s\n", b_val->chip_id);
	printf("   Offset: %d\n", b_val->offset);
	printf("   Range: %d\n", b_val->range);
	printf("   VAS Offset: %d\n", b_val->pre_rec_offset);
	printf("   QC Offset: %d\n", b_val->post_rec_offset);
	printf("   Num Points: %d\n", b_val->num_point);

	for (size_t i = 0;i<NUM_OF_TEST; i++){
		printf("  %d Offset: %d\n", i, b_val->burn_in_offset[i]);

	}

}

esp_err_t update_detail_values(int dev_id){
	esp_err_t ret = ESP_FAIL;
	ESP_LOGI(tag, " Updating details page for %s passed index:%d", test_blower_device_names[dev_id], dev_id);
	if (test_vals_acquire(10)){

			// set the values for the selected blower
		blower_details_lv_obj_map_t *b_map = &brn_ui_map.detail_lv;
    	blower_test_value_t *b_vals = &brn_val.blowers[dev_id];
//    	print_blower_vals(b_vals);
    	// Update the title
    	const char *name = b_vals->name;
    	lv_label_set_text(b_map->name_label, name);
		ESP_LOGD(tag, " Updating chart Title %s", name);


    	 // Update the chart
    	 // Add the original 2 vals
    	size_t data_points_count = b_vals->num_point;

		ESP_LOGD(tag, " Populating chart");

    	create_chart_with_data(b_map->chart, b_vals->burn_in_offset, data_points_count);
    	ret = ESP_OK;
    	test_vals_release();
	}
	ESP_LOGI(tag, " Finished updating detail page: %d", ret);
	return ret;
}


void create_chart_with_data(lv_obj_t *chart, int *data_points, size_t data_points_count) {
    if (!data_points || data_points_count == 0) {
        return;
    }

//    lv_obj_t *chart = lv_chart_create(parent, NULL);
//    lv_obj_set_size(chart, 240, 240);
//    lv_obj_align(chart, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_CIRCULAR);

    int16_t min_value = INT16_MAX;
    int16_t max_value = INT16_MIN;



    for (size_t i = 0; i < data_points_count; i++) {
        if (data_points[i] < min_value) {
            min_value = data_points[i];
        }
        if (data_points[i] > max_value) {
            max_value = data_points[i];
        }
    }

    int16_t range = max_value - min_value;

    int16_t tick_step = 5;
    int16_t y_min = (min_value / tick_step) * tick_step;
    int16_t y_max = ((max_value + tick_step - 1) / tick_step) * tick_step;
    lv_chart_set_point_count(chart, data_points_count);

    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, y_min, y_max);

    uint16_t num_ticks = (y_max - y_min) / tick_step + 1;

    lv_coord_t maj = 0;
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 5, 1, num_ticks, 1, true, 30);


    lv_coord_t *y_ser = lv_chart_get_y_array(chart, series);


    for (size_t i = 0; i < data_points_count; i++) {
//    	[i] = data_points[i];
//        lv_chart_set_next_value(chart, series, data_points[i]);
    	if (data_points[i] == DEF_OFFSET_VAL){
    		continue;
    	}
//    	printf("%d LVChart add %d", i, data_points[i]);
    	y_ser[i] = (int16_t) data_points[i];
    }
	lv_chart_refresh(chart); /*Required after direct set*/

}


