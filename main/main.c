#include <stdio.h>
#include <string.h>
#include <time.h>
#include "stdlib.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "freertos/queue.h"

#include "esp_netif.h"
#include "esp_heap_caps.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_check.h"

#include "settings.h"
#include "app_event_handler.h"
//#include "../gui/ui_burnin/ui_main.h"
#include "ui.h"
//#include "bsp/esp-box.h"
//#include "lvgl.h"


#include "bsp_board.h"
#include "bsp/esp-bsp.h"

#include "inno_connect.h"
//
//#include "ui_blower_burn_in.h"
//#include "serial_inno.h"
//#include "rack_device.h"
//
//#include "burn_in.h"
//#include "burnin_valve.h"
//#include "ui.h"
//#include "mqtt_handler.h"

#include "apprackcomm.h"

static const char* tag = "app_main";



QueueHandle_t task_queue_handle;
QueueHandle_t uart_rx_queue;
QueueHandle_t uart_tx_queue;
QueueHandle_t rack_queue;

//TaskHandle_t rack_task_handle;
//TaskHandle_t uart_rx_handle;
//TaskHandle_t burn_in_handle;

TaskHandle_t rackcomm_task_handle;

mqtt_handler_config_t app_cfg;
int wifi_conn=0;



void app_main(void) {
//	const char* tes = NULL;

	srand(time(NULL));   // Initialization, should only be called once.
	esp_log_level_set("inno_common", ESP_LOG_WARN);
	esp_log_level_set("inno_connect", ESP_LOG_WARN);
	esp_log_level_set("inno_wifi_connect", ESP_LOG_WARN);

	esp_log_level_set("wifi", ESP_LOG_WARN);
	esp_log_level_set("mqtt_handler", ESP_LOG_WARN);

	ESP_LOGI(tag, "Compile time: %s %s", __DATE__, __TIME__);
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
 // Default setup for running app
	//		ESP_LOG_NONE,       /*!< No log output */
	//	    ESP_LOG_ERROR,      /*!< Critical errors, software module can not recover on its own */
	//		ESP_LOG_WARN,       /*!< Error conditions from which recovery measures have been taken */
	//	    ESP_LOG_INFO,       /*!< Information messages which describe normal flow of events */
	//	    ESP_LOG_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
	//	    ESP_LOG_VERBOSE     /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */

	esp_log_level_set("msg16", ESP_LOG_ERROR);
	esp_log_level_set("serial_inno", ESP_LOG_ERROR);
	esp_log_level_set("burn-in", ESP_LOG_WARN);
	esp_log_level_set("UI_blower-BI", ESP_LOG_WARN);
	esp_log_level_set("UI_Timer", ESP_LOG_WARN);
	esp_log_level_set("UI_EVENT", ESP_LOG_DEBUG);
	esp_log_level_set("UI_Detail", ESP_LOG_DEBUG);
	esp_log_level_set("spi_master", ESP_LOG_WARN);
	esp_log_level_set("efuse", ESP_LOG_WARN);
	esp_log_level_set("GC_task", ESP_LOG_WARN);
	esp_log_level_set("user_event_loop", ESP_LOG_WARN);
	esp_log_level_set("offset_data", ESP_LOG_WARN);
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	bsp_display_backlight_on();

	ui_init();
//	esp_err_t err_wifi = inno_connect();
//	if (err_wifi==ESP_OK) {
//		wifi_conn = 1;
//	}


	xTaskCreate(&apprackcomm_task, "AppComm", 1024 * 6,
			NULL, 5, NULL);



}
