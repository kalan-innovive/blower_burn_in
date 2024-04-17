#ifndef _BLOWER_BURN_IN_UI_H
#define _BLOWER_BURN_IN_UI_H
#include <stdbool.h>
#include "esp_err.h"
#include "main.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DEF_OFFSET_VAL
#define DEF_OFFSET_VAL 0xFFFE
#endif

#define NUM_OF_TEST 10
#define REQ_POINTS_PASS 4
#define TICK_MIN 5

//#define SA_DEVID 0x1
//#define EA_DEVID 0x3
//#define SB_DEVID 0x21
//#define EB_DEVID 0x23

typedef enum {
	SA_INDEX,
	EA_INDEX,
	SB_INDEX,
	EB_INDEX,
	NA_INDEX
} blower_ui_index;

typedef enum {
	NA_APP_STATUS = -1,
	APP_READY = 0,
	APP_BUSY,
	APP_NOT_READY
} app_connection_status;

/**
 * All information to display the wifi and modbus information
 */
typedef struct {
	bool wifi_connected;
	char *ip_v4;
	bool usb_connected;
	bool mqtt_connected;
	char *node_name;
	bool modbus_connected;
	bool blt_connected;
} ui_connection_info_t;

typedef enum {
	SETTINGS_SCR,
	START_SCR,
	INFO_SCR,
	DETAILED_SCR
} cur_screen_index;

typedef enum {
	UNINIT_BLOWER_TEST, // Blowers that can not be contacted
	STARTING_BLOWER_TEST, // Setting up blower can read the default value and the chipid
	RUNNING_BLOWER_TEST, // Blowers that have been updated from the server and are running the burn in
	SUCCESS_BLOWER_TEST, // Blowers that have finished and passed tests
	FAILED_BLOWER_TEST   	// Blowers that failed test for reason
} blower_testing_state_t;

// This should hold the current state
typedef enum {
	ERROR_VAL = -1,
	STARTING_BURNIN_TEST, 	// Screen 1 waiting for start to be pressed
	RUNNING_BURNIN_TEST, // Screen 2 Collecting samples and saving values before controller starts controlleing
	FINISHED_BURNIN_TEST, // 10 minut timer is done waiting for blowers to be turned off
	RUNNING_COOLDOWN_TEST,	// Rack off 30 min timer started
	FINISHED_BURNIN_CYCLE,// 30 min Timer finished waiting for Rack to be turned on
	STARTING_VALVE_TEST,	// Finished burnin test start 3hr timer
	RUNNING_VALVE_TEST, // Running Valve exercise test Move Valve back and forth
	FINISHED_VALVE_TEST, // Update the finished screen
	CANCEL_BURNIN_TEST
} burn_in_testing_state_t;

const char* burnin_state_to_str(burn_in_testing_state_t state);

/**
 * Blower values for  UI
 */
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
	blower_testing_state_t state;

} blower_test_value_t;

typedef struct {
	blower_ui_index device_index;
	int vas_val;
	int qc_val;
	int cur_val;
	int len;
	int *val_array;

} detail_blower_scr_t;

/**
 * Main UI struct to pass information between
 */
typedef struct {
	burn_in_testing_state_t brn_state; 	// Burn in state
	bool values_changed; 			// Flag Showing if any values have changed
	blower_test_value_t blowers[4]; 	// 4 possible test blowers
	cur_screen_index cur_screen;
	bool power_state;
	app_connection_status app_con; // Used to test if the app Components are ready busy or failed
	bool app_connect_changed;
	ui_connection_info_t conect_info;
	detail_blower_scr_t detail_scrn;
	int ui_timer;

} burn_in_ui_value_t;

//typedef struct {
//	burn_in_test_state_t brn_state; 	// Burn in state
//	bool values_changed; 			// Flag Showing if any values have changed
//	blower_test_value_t blowers[4]; 	// 4 possible test blowers
//
//} burn_in_ui_value_t;

/**
 * Privately used structures
 */
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

/**
 * @brief Setup the testing values struct
 *
 * Default burnin_test_struct
 */
burn_in_ui_value_t* get_test_vals(void);
burn_in_testing_state_t get_burn_in_state(void);
const char* burnin_state_to_str(burn_in_testing_state_t state);
esp_err_t init_test_vals(void);
bool test_vals_acquire(uint32_t timeout_ms); // Must be successful to get the test values
void test_vals_release(void);
esp_err_t update_test_values(void);
esp_err_t update_test_state(burn_in_testing_state_t state);
esp_err_t ui_timer_finished(void);
void setup_blower_random_struct(blower_test_value_t *b_vals);
esp_err_t start_cooldown(void);
esp_err_t start_burnin(void);
void create_chart_with_data(lv_obj_t *chart, blower_test_value_t *b_vals);
// Todo: this should be static
esp_err_t update_detail_values(int dev_id);
void update_blower_status_ui(lv_obj_t *lv_label,
		blower_testing_state_t state);
void update_blower_id_ui(blower_test_label_map_t *b_lv,
		blower_test_value_t *b_vals);
// Handler Functions
esp_err_t handle_ip_addr(char *ip_v4);
esp_err_t handle_mqtt_status(bool mqtt_connected);
esp_err_t handle_modbus_power(bool modbus_connected);
esp_err_t handle_node_name(char *node_name);
esp_err_t handle_serial_status(bool usb_connected);
esp_err_t handle_blt_status(bool blt_connected);

// Should be static?
esp_err_t update_connect_values(void);

extern char test_blower_device_names[4][16];

#ifdef __cplusplus
}
#endif

#endif
