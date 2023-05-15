#include <stdio.h>
//#include "esp_heap_caps.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_check.h"


#include "freertos/queue.h"
#include "settings.h"
#include "app_event_handler.h"
#include "gui/ui_main.h"


#include "bsp_board.h"
#include "bsp/esp-bsp.h"

#include "ui_blower_burn_in.h"
#include "esp_err.h"

#include "string.h"
#include <time.h>
#include "stdlib.h"
#include "burn_in.h"


static const char *TAG = "main";

QueueHandle_t task_queue_handle;

#define TESTING_SER_INNO 1
#ifdef TESTING_SER_INNO
#include "serial_inno_test.h"

void run_ser_inno_tests(void *pvParameter){

//	if (serial_inno_unit_tests() == ESP_OK){
//		ESP_LOGI(TAG, "Unit test successful");
//	}
//	else{
//		ESP_LOGI(TAG, "Unit test Failed");
//	}
	if (serial_inno_system_tests() == ESP_OK){
		ESP_LOGI(TAG, "--------System test successful-------");
	}
	else{
		ESP_LOGI(TAG, "System test Failed");
	}
//	if (serial_inno_in_system_tests() == ESP_OK){
//		ESP_LOGI(TAG, "--------In-System test successful-------");
//	}
//	else{
//		ESP_LOGI(TAG, "In System test Failed");
//	}
	vTaskDelay(10000 / portTICK_PERIOD_MS);

}
#endif

void app_main(void)
{
	const char* tes = NULL;

	srand(time(NULL));   // Initialization, should only be called once.

    ESP_LOGI(TAG, "Compile time: %s %s", __DATE__, __TIME__);
    /* Initialize NVS. */

    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            err = nvs_flash_init();
        }
        ESP_ERROR_CHECK(err);
        ESP_ERROR_CHECK(settings_read_parameter_from_nvs());
    /* Initialize Board. */

    bsp_i2c_init();
    bsp_display_start();
    bsp_board_init();
#ifndef TESTING_SER_INNO

    ESP_LOGI(TAG, "Display LVGL demo");
    bsp_display_backlight_on();
    ESP_ERROR_CHECK(ui_main_start());
#endif
#ifdef TESTING_SER_INNO

    run_ser_inno_tests(NULL);

#endif



}
