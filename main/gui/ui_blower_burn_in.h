#ifndef _BLOWER_BURN_IN_UI_H
#define _BLOWER_BURN_IN_UI_H
#include <stdbool.h>
#include "esp_err.h"
#include "main.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEF_OFFSET_VAL 0xFFFE
#define NUM_OF_TEST 12
#define REQ_POINTS_PASS 6

//#define SA_DEVID 0x1
//#define EA_DEVID 0x3
//#define SB_DEVID 0x21
//#define EB_DEVID 0x23

typedef enum {
	SA_INDEX,
	EA_INDEX,
	SB_INDEX,
	EB_INDEX
} blower_ui_index;

typedef enum {
	UNINIT_BLOWER_TEST, // Blowers that can not be contacted
	STARTING_BLOWER_TEST, // Setting up blower can read the default value and the chipid
	RUNNING_BLOWER_TEST, // Blowers that have been updated from the server and are running the burn in
	SUCCESS_BLOWER_TEST, // Blowers that have finished and passed tests
	FAILED_BLOWER_TEST   	// Blowers that failed test for reason
} blower_test_state_t;

// This should hold the current state
typedef enum {
	ERROR_VAL = -1,
	STARTING_BURNIN_TEST, 	// Screen 1 waiting for start to be pressed
	RUNNING_BURNIN_TEST, // Screen 2 Collecting samples and saving values before controller starts controlleing
	FINISHED_BURNIN_TEST,// 10 minut timer is done waiting for blowers to be turned off
	RUNNING_COOLDOWN_TEST,	// Rack off 30 min timer started
	FINISHED_BURNIN_CYCLE,// 30 min Timer finished waiting for Rack to be turned on
	CANCEL_BURNIN_TEST
} burn_in_test_state_t;

const char* t_state_to_str(burn_in_test_state_t state);

typedef struct {
	bool is_testing;		// Flag to signal we are testing this blower
	bool values_changed; 	// Flag to update values
	char name[16];		// Name of Device ID
	char chip_id[16];	// ChipId value of blower
	int offset;			// Current Offset of test
	int range;			// Current Range of all offsets
	int vas_offset;		// Value when sensor was placed on the board
	int qc_offset;	// Value when board was tested in QC
	int burn_in_offset[NUM_OF_TEST]; // Last 10 burn in values
	int min_val;			// 1/2 ALLowable range + mean or 80
	int max_val;			// 1/2 ALLowable range - mean or -80
	int num_point;
	blower_test_state_t state;

} blower_test_value_t;

typedef struct {
	burn_in_test_state_t brn_state; 	// Burn in state
	bool values_changed; 			// Flag Showing if any values have changed
	blower_test_value_t blowers[4]; 	// 4 possible test blowers
} burn_in_test_value_t;

typedef struct {
	lv_obj_t *panel;			// Container objct to try and hide
	lv_obj_t *name_label;		// Blower Device name label
	lv_obj_t *chip_id_label;	// ChipId label
	lv_obj_t *offset_label;		// Current Offset label
	lv_obj_t *range_label;		// Current Range label
	lv_obj_t *status_label;		// Current Test status label
} blower_test_label_map_t;

typedef struct {
	lv_obj_t *name_label;		// Blower Device name label
	lv_obj_t *chart;			// ChipId label
	lv_obj_t *pre_offset;		// Last Offset from VAS
	lv_obj_t *post_offset;		// Last Offset QC
	lv_obj_t *min_offset_label;	// Current Min Offset label
	lv_obj_t *max_offset_label;	// Current Max Offset label
} blower_details_lv_obj_map_t;

typedef struct {
	blower_details_lv_obj_map_t detail_lv; 	// Burn in state
	blower_test_label_map_t blower_lv[4]; // Flag Showing if any values have changed
} burn_in_lv_obj_map_t;

esp_err_t update_detail_values(int dev_id);

/**
 * @brief Setup the testing values struct
 *
 * Default burnin_test_struct
 */
burn_in_test_value_t* get_test_vals(void);
burn_in_test_state_t get_test_state(void);
esp_err_t init_test_vals(void);
bool test_vals_acquire(uint32_t timeout_ms); // Must be succesful to get the test vals
void test_vals_release(void);
esp_err_t update_test_values(void);
esp_err_t update_test_state(burn_in_test_state_t state);
esp_err_t test_timer_finished(void);
void setup_blower_random_struct(blower_test_value_t *b_vals);
esp_err_t start_cooldown(void);
esp_err_t start_burnin(void);

extern char test_blower_device_names[4][16];

#ifdef __cplusplus
}
#endif

#endif
