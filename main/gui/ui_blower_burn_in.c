/*
 * ui_blower_burn_in.c
 *
 *  Created on: Apr 23, 2023
 *      Author: ekalan
 */

#include "ui_blower_burn_in.h"
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
//#include "main.h"
//#include "lvgl.h"

//#include "lvgl/lvgl.h"
#include <stddef.h>
#include <limits.h>
#include <math.h>

// Used for debug and dev
#include <time.h>
#include "stdlib.h"


static void init_colors(void);
static void set_array_to_default(int *arr, int length);
static void update_blower_test_val_ui(blower_test_label_map_t *b_lv, blower_test_value_t *b_vals);
static void setup_burnin_ui_structs(void);
static void setup_burnin_test_struct(void);
static void update_label_with_int(lv_obj_t *label, int value);
void create_chart_with_data(lv_obj_t *chart, int16_t *data_points, size_t data_points_count);





static lv_color_t lv_green ;
static lv_color_t lv_red;
static lv_color_t lv_default;
static lv_color_t lv_light;
static lv_chart_series_t *series;

static burn_in_lv_obj_map_t brn_ui_map;
static burn_in_test_value_t brn_val;

SemaphoreHandle_t semaphore_burnin_display_values = NULL;

const char test_blower_device_names[4][16] = {"Supply A", "Exhaust A","Supply B", "Exhaust B"};
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


//static void setup_burnin_test_struct(void){
//	brn_val.brn_state = STARTING_BURNIN_TEST;
//	brn_val.values_changed = true;
//	for (int i = 0; i<4; i++) {
//		brn_val.blowers[i].is_testing = false;
//		brn_val.blowers[i].values_changed = true;
//
//		strcpy(brn_val.blowers[i].name, test_blower_device_names[i]);
//		strcpy(brn_val.blowers[i].chip_id, default_chip_id);
//
//		brn_val.blowers[i].offset = DEF_OFFSET_VAL;
//		brn_val.blowers[i].range = DEF_OFFSET_VAL;
//		brn_val.blowers[i].pre_rec_offset = DEF_OFFSET_VAL;
//		brn_val.blowers[i].post_rec_offset = DEF_OFFSET_VAL;
//		set_array_to_default(brn_val.blowers[i].burn_in_offset, NUM_OF_TEST);
//		brn_val.blowers[i].min_val = DEF_OFFSET_VAL;
//		brn_val.blowers[i].max_val = DEF_OFFSET_VAL;
//
//	}
//}

// For testing
static void setup_burnin_test_struct(void){
	brn_val.brn_state = STARTING_BURNIN_TEST;
	brn_val.values_changed = true;
	srand(time(NULL));   // Initialization, should only be called once.

	for (int i = 0; i<4; i++) {
		brn_val.blowers[i].is_testing = false;
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

	}
}



static void setup_burnin_ui_structs(void){
	// Detailed label pointer mapping

	brn_ui_map.detail_lv.name_label = ui_BlowerHeaderLabel;
	brn_ui_map.detail_lv.chart = ui_Blower_Val_Chart;
	brn_ui_map.detail_lv.pre_offset = ui_BloweSAPanel;
	brn_ui_map.detail_lv.post_offset = ui_BloweSAPanel;
	brn_ui_map.detail_lv.min_offset_label = ui_BloweSAPanel;
	brn_ui_map.detail_lv.max_offset_label = ui_BloweSAPanel;

	// Supply A pointer Mapping
	brn_ui_map.blower_lv[0].panel = ui_BloweSAPanel;
	brn_ui_map.blower_lv[0].name_label = ui_BlowerLabel3;
	brn_ui_map.blower_lv[0].chip_id_label = ui_ChipIDValLabel3;
	brn_ui_map.blower_lv[0].offset_label = ui_OffsetValLabel3;
	brn_ui_map.blower_lv[0].range_label = ui_RangeValLabel3;
	brn_ui_map.blower_lv[0].status_label = ui_PassedLabelSA;

	// Exhaust A pointer Mapping
	brn_ui_map.blower_lv[1].panel = ui_BlowerEAPanel;
	brn_ui_map.blower_lv[1].name_label = ui_BlowerLabel1;
	brn_ui_map.blower_lv[1].chip_id_label = ui_ChipIDLabel1;
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


	bool res = ESP_OK;
	semaphore_burnin_display_values = xSemaphoreCreateBinary();

//	res = test_vals_acquire(0);

	if (semaphore_burnin_display_values == NULL) {
		ESP_LOGE(tag, "Could not create Semaphore.");
		res = ESP_FAIL;
	} else {
		ESP_LOGI(tag, "Created Semaphore.");

		setup_burnin_test_struct();
		xSemaphoreGive(semaphore_burnin_display_values);
	}
	// Initialize the series
    series = lv_chart_add_series(brn_ui_map.detail_lv.chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);

	ESP_LOGI(tag, "Blower burn in UI structure initialized %s", (res)?"Success":"Failed");
	return (res) ? ESP_OK : ESP_FAIL;


}


static void update_label_with_int(lv_obj_t *label, int value) {
    char str[16];
    sprintf(str, "%d", value);
    lv_label_set_text(label, str);
}

static void update_blower_test_val_ui(blower_test_label_map_t *b_lv, blower_test_value_t *b_vals){
	update_label_with_int(b_lv->offset_label, b_vals->offset);
	update_label_with_int(b_lv->range_label, b_vals->range);
}


static void update_blower_id_ui(blower_test_label_map_t *b_lv, blower_test_value_t *b_vals){

	lv_label_set_text(b_lv->name_label, b_vals->name);
	lv_label_set_text(b_lv->chip_id_label, b_vals->chip_id);
}



static void init_colors(void){
	lv_green = lv_color_hex(0x00E946);
	lv_red = lv_color_hex(0xFF0000);
	lv_default = lv_color_hex(0x00DB24);
	lv_light = lv_color_hex(0xffffff);
}

static void update_blower_status_ui(lv_obj_t *lv_label, blower_test_state_t state){
	 const char *state_str;



	 lv_color_t text_color;
	    lv_color_t bg_color;

	    switch (state) {
	        case UNINIT_BLOWER_TEST:
	            state_str = "Uninitialized";
	            text_color = lv_default;
	            bg_color = lv_light;
	            break;
	        case STARTING_BLOWER_TEST:
	            state_str = "Starting";
	            text_color = lv_default;
	            bg_color = lv_green;
	            break;
	        case RUNNING_BLOWER_TEST:
	            state_str = "Running";
	            text_color = lv_green;
	            bg_color = lv_light;
	            break;
	        case SUCCESS_BLOWER_TEST:
	            state_str = "Success";
	            text_color = lv_default;
	            bg_color = lv_green;
	            break;
	        case FAILED_BLOWER_TEST:
	            state_str = "Failed";
	            text_color = lv_light;
	            bg_color = lv_red;
	            break;
	        default:
	            state_str = "";
	            text_color = lv_default;
	            bg_color = lv_light;
	            break;
	    }
	    lv_label_set_text(lv_label, state_str);

		lv_obj_set_style_bg_color(lv_label, bg_color,
				LV_PART_MAIN | LV_STATE_DEFAULT);
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
	if (test_vals_acquire(4)){
		// We do not need to update values if the test is cancelled and we are not restarting or no values have changed
		if(brn_val.brn_state < CANCEL_BURNIN_TEST && brn_val.values_changed){
		    for (int i = 0; i < 4; i++) {
		    	blower_test_label_map_t *b_map = &brn_ui_map.blower_lv[i];
		    	blower_test_value_t *b_vals = &brn_val.blowers[i];

		    	// Check if any blower values changed
		    	if (b_vals->values_changed && b_vals->is_testing) {
		    		update_blower_test_val_ui(b_map, b_vals);
		    		update_blower_status_ui(b_map->status_label, b_vals->state);
		    		update_blower_id_ui(b_map, b_vals);

		    	}
		    }
		}
	}
	return ESP_OK;
}

static void print_blower_vals(blower_test_value_t *b_val) {
	printf("Blower DevID: %s\n", b_val->name);
	printf("Blower ChipID: %s\n", b_val->chip_id);
	printf("   Offset: %d\n", b_val->offset);
	printf("   Range: %d\n", b_val->range);
	printf("   VAS Offset: %d\n", b_val->pre_rec_offset);
	printf("   QC Offset: %d\n", b_val->post_rec_offset);
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
    	print_blower_vals(b_vals);
    	// Update the title
    	const char *name = b_vals->name;
    	lv_label_set_text(b_map->name_label, name);
		ESP_LOGI(tag, " Updating chart Title %s", name);


    	// Update the chart
    	 // Add the original 2 vals
    	size_t data_points_count = NUM_OF_TEST +2;
        int16_t vals[data_points_count];

		vals[0] = (int16_t) b_vals->pre_rec_offset;
		vals[1] = (int16_t) b_vals->post_rec_offset;

		for (size_t i = 0; i < NUM_OF_TEST; i++) {
				// Copy the chart values adding the initial values post production and qc value to the beginning
				vals[i+2] =  b_vals->burn_in_offset[i];
		}
		ESP_LOGI(tag, " Populating chart");

    	create_chart_with_data(b_map->chart, vals, data_points_count);
    	ret = ESP_OK;
    	test_vals_release();
	}
	ESP_LOGI(tag, " Finished updating detail page: %d", ret);
	return ret;
}


void create_chart_with_data(lv_obj_t *chart, int16_t *data_points, size_t data_points_count) {
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
//    char ticks_buf[256] = {0};
//    for (uint16_t i = 0; i < num_ticks; i++) {
//        char tick_label[16];
//        lv_snprintf(tick_label, sizeof(tick_label), "%d", y_min + i * tick_step);
//        strcat(ticks_buf, tick_label);
//        if (i != num_ticks - 1) {
//            strcat(ticks_buf, "\n");
//        }
//    }
    lv_coord_t maj = 0;
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 5, 1, num_ticks, 1, true, 20);
//    lv_chart_set_y_tick_texts(chart, ticks_buf, LV_CHART_AXIS_TICK_LABEL_AUTO_RECOLOR, 0);

//    lv_chart_set_y_tick_length(chart, 0, 0);

//    lv_chart_clear_series(chart, series);

    lv_coord_t *y_ser = lv_chart_get_y_array(chart, series);


    for (size_t i = 0; i < data_points_count; i++) {
//    	[i] = data_points[i];
//        lv_chart_set_next_value(chart, series, data_points[i]);
    	y_ser[i] = data_points[i];
    }
	lv_chart_refresh(chart); /*Required after direct set*/

}


