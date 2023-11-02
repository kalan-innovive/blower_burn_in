/*
 * app_rack.c
 *
 *  Created on: Apr 19, 2023
 *      Author: ekalan
 */

//#include <string.h>
//#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"

#include "app_rack.h"

static const char *TAG = "app_rack";

// TODO: Create rack config struct
// 7 Register values
// current Connected devices
// status: on off
/* Blower Test struct
 *  - Attached
 *  - Blower reg: dev_id, reg: offset
 *  - VAS Value
 *  - QC_Value
 *  - status: pass, fail, running, uninitiated
 *  -
 *
 */


// TODO: Create rack queue add to main
// TODO: Create mutex for rack config


/* TODO: Create rack config task
 * Run task until Blowers are attached
 * Setup Rack Config struct
 * run periodically to check connection of blowers
 */


/* TODO: Create rack task
 * Run task as long as config does not change
 * Task Should update the rack struct periodically
 * - responds to controller when attached
 * - Has to be able to
 * run periodically to check connection of blowers
 */

// Task to setup Rack config
/*
 *
 *
 */
esp_err_t app_rack_init(gpio_num_t u_tx, gpio_num_t u_rx) {
	// TODO: Initialize serial inno comm
	// TODO: start rack config check
	ESP_LOGE(TAG, "APP_RACK functionality not available");
	return ESP_OK;
}
