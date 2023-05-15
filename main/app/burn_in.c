/*
 * burn_in.c
 *
 *  Created on: May 1, 2023
 *      Author: ekalan
 */


#include <stdio.h>
//#include "esp_heap_caps.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_check.h"


#include "freertos/queue.h"
#include "app_event_handler.h"
#include "gui/ui_main.h"

#include "bsp_board.h"
#include "bsp/esp-bsp.h"

#include "ui_blower_burn_in.h"
#include "burn_in.h"
#include "esp_err.h"

#include "string.h"
#include <time.h>
#include "stdlib.h"

static const char *TAG = "burn in";

//QueueHandle_t task_queue_handle;
#define NUM_TESTS 2
static int test_cycle = 1;
//static int num_points = 3;
static blower_test_state_t test_blower[4];


// For testing
void setup_blower_random_(burn_in_test_value_t *brn_val){
	brn_val->values_changed = true;

	for (int i = 0; i<NUM_TESTS; i++) {
		brn_val->blowers[i].is_testing = true;
		brn_val->blowers[i].values_changed = true;

		strcpy(brn_val->blowers[i].name, test_blower_device_names[i]);
		char chipid[16];
		unsigned c_id = rand()%1000000;
		sprintf(chipid, "%ul", c_id);
		strcpy(brn_val->blowers[i].chip_id, chipid);
		int min_v = 100;
		int max_v = -100;
		int seed = (rand() % 140) - 70;

//		set_array_to_default(brn_val.blowers[i].burn_in_offset, NUM_OF_TEST);
		for (int ii = 0; ii< 3; ii++){
			int tmp  = ((rand() % 6) - 3)+ seed;
			min_v = (min_v < tmp) ? min_v : tmp;
			max_v = (max_v > tmp) ? max_v : tmp;

			brn_val->blowers[i].burn_in_offset[ii] = tmp;
		}
		brn_val->blowers[i].offset =brn_val->blowers[i].burn_in_offset[2];
		brn_val->blowers[i].range = max_v - min_v ;
		brn_val->blowers[i].pre_rec_offset = brn_val->blowers[i].burn_in_offset[0];
		brn_val->blowers[i].post_rec_offset = brn_val->blowers[i].burn_in_offset[1];
		brn_val->blowers[i].min_val = min_v;
		brn_val->blowers[i].max_val = max_v;
		brn_val->blowers[i].state = RUNNING_BLOWER_TEST;
		brn_val->blowers[i].num_point = 3;

	}
}

// For testing
void setup_blower_demo_(burn_in_test_value_t *brn_val){
	brn_val->values_changed = true;
	unsigned chipid[4] = {334910768,387443231, 442381775, 314496356};
	int testing[4] = {1,1, 0, 0};
	int vas_v[4] = {1,1, 0, 0};
	int QC_v[4] = {1,1, 0, 0};




	for (int i = 0; i<NUM_TESTS; i++) {
		brn_val->blowers[i].is_testing = true;
		brn_val->blowers[i].values_changed = true;

		strcpy(brn_val->blowers[i].name, test_blower_device_names[i]);
		char chipid[16];
		unsigned c_id = chipid[i];
		sprintf(chipid, "%ul", c_id);
		strcpy(brn_val->blowers[i].chip_id, chipid);
		int min_v = 1000;
		int max_v = -1000;
		int seed = QC_v[i];
		int tmp  = ((rand() % 9) - 3)+ seed;
//		brn_val->blowers[i].pre_rec_offset = vas_v[i];
		brn_val->blowers[i].burn_in_offset[0] = vas_v[i];
//		brn_val->blowers[i].post_rec_offset = QC_v[i];
		brn_val->blowers[i].burn_in_offset[1] = QC_v[i];
//		brn_val->blowers[i].offset = tmp;
		brn_val->blowers[i].burn_in_offset[2] = tmp;

//		set_array_to_default(brn_val.blowers[i].burn_in_offset, NUM_OF_TEST);
		for (int ii = 0; ii< 3; ii++){
			tmp  = brn_val->blowers[i].burn_in_offset[ii];
			min_v = (min_v < tmp) ? min_v : tmp;
			max_v = (max_v > tmp) ? max_v : tmp;

		}
		brn_val->blowers[i].offset =brn_val->blowers[i].burn_in_offset[2];
		brn_val->blowers[i].range = max_v - min_v ;
		brn_val->blowers[i].pre_rec_offset = brn_val->blowers[i].burn_in_offset[0];
		brn_val->blowers[i].post_rec_offset = brn_val->blowers[i].burn_in_offset[1];
		brn_val->blowers[i].min_val = min_v;
		brn_val->blowers[i].max_val = max_v;
		brn_val->blowers[i].state = RUNNING_BLOWER_TEST;
		brn_val->blowers[i].num_point = 3;

	}
}

// For testing
void update_blower_random_val(burn_in_test_value_t *brn_val){
//	if (brn_val->brn_state != RUNNING_BURNIN_TEST){
//		return;
//	}
	brn_val->values_changed = true;

	for (int i = 0; i<NUM_TESTS; i++) {
		blower_test_value_t *blower = &brn_val->blowers[i];
		blower->is_testing = true;
		blower->values_changed = true;


		int min_v = 10000;
		int max_v = -10000;
		// add seed value to the current offset to get random value close to original
		int cur_offset = blower->offset;
		int seed =((rand() % 9) - 3)+ cur_offset;
		// If the num points is past the array length reorder the array

		if (blower->num_point == NUM_OF_TEST) {
			for (int ii=2; ii<NUM_OF_TEST-1;ii++){
				printf("Adjusting array: blower value %d. set to %d\n", ii, blower->burn_in_offset[ii+1]);
				blower->burn_in_offset[ii] = blower->burn_in_offset[ii+1];
			}

		}
		blower->num_point++;
		blower->num_point = (blower->num_point > NUM_OF_TEST) ? NUM_OF_TEST : blower->num_point;
		int n_points = blower->num_point;

		blower->burn_in_offset[n_points -1] =seed;
//		int array_len = (num_points >= NUM_OF_TEST) ? NUM_OF_TEST: num_points;
		int passing = 1;

//		set_array_to_default(brn_val.blowers[i].burn_in_offset, NUM_OF_TEST);
		for (int ii = 0; ii< n_points; ii++){
			int tmp  = brn_val->blowers[i].burn_in_offset[ii];
			min_v = (min_v < tmp) ? min_v : tmp;
			max_v = (max_v > tmp) ? max_v : tmp;
			passing &= (abs(tmp) >= 80)? 0:1;

		}
		blower->offset = seed;
		blower->range = max_v - min_v ;
		blower->min_val = min_v;
		blower->max_val = max_v;
		passing &= ( blower->range >= 9 )? 0:1;
		if (passing){
			blower->state = (n_points >= REQ_POINTS_PASS)  ?SUCCESS_BLOWER_TEST:RUNNING_BLOWER_TEST;
		} else {
			blower->state =FAILED_BLOWER_TEST;
		}

	}
}

void burn_in_task(void *pvParameter)
{


    // Initialize Values from settings
//    const board_res_desc_t *brd = bsp_board_get_description();
//    app_rack_init(brd->PMOD2->row1[1], brd->PMOD2->row1[2], brd->PMOD2->row1[3]);

// SA 7e:01:20:03:06:10:01:3b:00:7e
//    334910768
//    calibration -52
//	  current -48

// EA 7e:03:20:03:06:10:01:3d:00:7e
//    387443231
//    calibration -9
//	  current -12
// SB 7e:21:20:03:06:10:01:5b:00:7e
// EB 7e:23:20:03:06:10:01:5d:00:7e
	// Create Event Handler task
//	task_queue_handle = xQueueCreate(10, sizeof(queue_task_t));
//    xTaskCreate(event_handler_task, "event_handler_task", 4096, NULL, 5, NULL);

    // Testing UI
    // Create test for SA EA



    int count = 0;
    int cur_state = 0;
    int prev_state;
	vTaskDelay(5000 / portTICK_PERIOD_MS);

    if(test_vals_acquire(10)){
		//A	quired ui update the values
		burn_in_test_value_t* b_val;
		b_val = get_test_vals();
//		setup_blower_random_(b_val);
		setup_blower_demo_(b_val);
		b_val->values_changed = true;
		b_val->brn_state = STARTING_BLOWER_TEST;
		test_vals_release();

	}
	vTaskDelay(5000 / portTICK_PERIOD_MS);


    while (true) {

		burn_in_test_state_t state = get_test_state();
		if (state == -1){
			continue;
		}else if (state == STARTING_BURNIN_TEST && test_cycle > 0){
			// This state is changed when the user presses start
			if(test_vals_acquire(10)){
				//Acquired ui update the values
				burn_in_test_value_t* b_val;
				b_val = get_test_vals();
				update_blower_random_val(b_val);
				test_vals_release();
				ESP_LOGI(TAG, "Simulating power supply on and starting burn in teset");

				if (start_burnin() == ESP_OK) {
					ESP_LOGI(TAG, "Simulating started burn in teset");
				}

			}

		}else if (state == RUNNING_BURNIN_TEST){
			// Do something
//				update_test_state(RUNNING_COOLDOWN_TEST);

		}else if (state == FINISHED_BURNIN_TEST){
			// Set the running cooldown state ui should update the timer
//			update_test_state(RUNNING_COOLDOWN_TEST);
			esp_err_t ret = start_cooldown();
			if (ret == ESP_OK) {
				ESP_LOGI(TAG, "Simulating turning off burn in test");
			}
		}
		else if (state == RUNNING_COOLDOWN_TEST){
			// Do something
//				update_test_state(RUNNING_COOLDOWN_TEST);

		}
		else if (state == FINISHED_BURNIN_CYCLE){

			// Simulate rack on event
			// Only update if we successfully updated the state
			esp_err_t ret = update_test_state(STARTING_BURNIN_TEST);
			if (ret == ESP_OK){
				test_cycle++;
				ESP_LOGI(TAG, "Finished cycle %d", test_cycle);

			}

		} else if (state == CANCEL_BURNIN_TEST){

			if(test_vals_acquire(10)){
				//Aquired ui update the values
				// TODO: adjust values to new value
				burn_in_test_value_t* b_val;
				b_val = get_test_vals();
				update_blower_random_val(b_val);
				test_vals_release();
				ESP_LOGI(TAG, "Simulating restart after Cancel|cycle %d", test_cycle);

				update_test_state(STARTING_BURNIN_TEST);

			}
		}

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    	count++;
    	update_test_values();


    }
}
