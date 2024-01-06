#include <stdio.h>
#include <string.h>
#include <time.h>
#include "stdlib.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "esp_netif.h"
#include "esp_heap_caps.h"
#include "esp_err.h"
#include "esp_event.h"
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

#include "inno_connect.h"

#include "ui_blower_burn_in.h"
#include "serial_inno.h"
#include "rack_device.h"

#include "burn_in.h"
#include "ui.h"
#include "mqtt_handler.h"

#define TESTING_INNO_COMPONENTS 0

#ifdef TESTING_INNO_COMPONENTS

#define TESTING_SER_INNO 1
//#define TESTING_MQTT_HANDLER_INNO 1
//#define TESTING_EH_LOOP 1

#ifdef TESTING_MQTT_HANDLER_INNO
#include "test_mqtt_handler.h"
#endif  //TESTING_MQTT_HANDLER_INNO

#endif	// TESTING_INNO_COMPONENTS

static const char *TAG = "main";
static esp_err_t run_blower_burn_in_app(void);


QueueHandle_t task_queue_handle;
QueueHandle_t uart_rx_queue;
QueueHandle_t uart_tx_queue;
QueueHandle_t rack_queue;

TaskHandle_t rack_task_handle;
TaskHandle_t uart_rx_handle;
TaskHandle_t burn_in_handle;

mqtt_handler_config_t app_cfg;
int wifi_conn=0;

#ifdef TESTING_EH_LOOP		// Testing Event Loop includes an variables

#include "test_event_handler.h"

static esp_err_t run_event_handler_tests() {
//	esp_log_set_level_master(ESP_LOG_DEBUG); // disables all logs globally. esp_log_level_set has no effect at the moment.

	esp_log_level_set("test_eh", ESP_LOG_INFO);
	esp_log_level_set("event", ESP_LOG_INFO);
	esp_log_level_set(TAG, ESP_LOG_INFO);
	esp_log_level_set("user_event_loops", ESP_LOG_DEBUG);
	esp_log_level_set("test_eh", ESP_LOG_DEBUG);
	esp_log_level_set("TRANSPORT_BASE", ESP_LOG_WARN);
	esp_log_level_set("esp-tls", ESP_LOG_WARN);
	esp_log_level_set("TRANSPORT", ESP_LOG_WARN);
	esp_log_level_set("OUTBOX", ESP_LOG_WARN);
	esp_log_level_set("wifi", ESP_LOG_WARN);
	esp_log_level_set("GC_task", ESP_LOG_DEBUG);
	esp_log_level_set("GB_TEST", ESP_LOG_DEBUG);

	esp_err_t err;

	ESP_LOGI(TAG, "\n\n_____ Starting EH tests______");
	ESP_LOGI(TAG, "     _____ Starting EH Unit tests______");
	// Run Unit tests
	ESP_ERROR_CHECK(test_event_handler_unit());

	ESP_LOGI(TAG, "\n     _____ Finished EH Unit tests______");
	vTaskDelay(1000 / portTICK_PERIOD_MS);

	ESP_LOGI(TAG, "     _____ Starting EH System tests______");

	ESP_ERROR_CHECK(test_event_handler_system());

	ESP_LOGI(TAG, "\n     _____ Finished EH System tests with ______"
			);
	ESP_LOGI(TAG, "_____ Finished EH tests______");
	vTaskDelay(500 / portTICK_PERIOD_MS);

	return ESP_OK;
}

#endif	//TESTING_EH_LOOP

#ifdef TESTING_SER_INNO
#include "serial_inno_test.h"
#define TESTING_BLOWER_COMM 100
#define USING_CONTROL_LINE 100



void run_ser_inno_tests(void *pvParameter){
	esp_log_level_set("msg16", ESP_LOG_DEBUG);
	esp_log_level_set("serial_inno", ESP_LOG_DEBUG);
	setup_driver();
	// Start the task for receiving
//	xTaskCreate(&uart_rx_task, "uart_rx_task", 2048,
//				NULL, configMAX_PRIORITIES, &uart_rx_handle);
//	xTaskCreate(&rack_task, "rack_task", 2048,
//					NULL, 6, &rack_task_handle);
	vTaskDelay(1000 / portTICK_PERIOD_MS);
#ifdef TESTING_BLOWER_COMM
	if (serial_inno_blower_tests() == ESP_OK){
		ESP_LOGI(TAG, "Blower comm test successful");

	}
#else

//	if (serial_inno_unit_tests() == ESP_OK){
//		ESP_LOGI(TAG, "Unit test successful");
//	}
//	else{
//		ESP_LOGI(TAG, "Unit test Failed");
//	}
//	if (serial_inno_system_tests() == ESP_OK){
//		ESP_LOGI(TAG, "--------System test successful-------");
//	}
//	else{
//		ESP_LOGI(TAG, "System test Failed");
//	}
//	if (serial_inno_in_system_tests() == ESP_OK){
//		ESP_LOGI(TAG, "--------In-System test successful-------");
//	}
//	else{
//		ESP_LOGI(TAG, "In System test Failed");
//	}
#endif

	vTaskDelay(100000 / portTICK_PERIOD_MS);

}
#endif // TESTING_SER_INNO

#ifdef TESTING_MQTT_HANDLER_INNO
static esp_err_t run_mqtt_test(void) {

	esp_log_level_set("MQTT_CLIENT", ESP_LOG_DEBUG);
	esp_log_level_set(TAG, ESP_LOG_INFO);
	esp_log_level_set("TRANSPORT_BASE", ESP_LOG_DEBUG);
	esp_log_level_set("esp-tls", ESP_LOG_DEBUG);
	esp_log_level_set("TRANSPORT", ESP_LOG_DEBUG);
	esp_log_level_set("OUTBOX", ESP_LOG_DEBUG);
	esp_log_level_set("mqtt_client", ESP_LOG_INFO);
	esp_log_level_set("test_mqtt", ESP_LOG_INFO);
	esp_log_level_set("mqtt_handler", ESP_LOG_INFO);
	esp_log_level_set("user_event_loop", ESP_LOG_INFO);

//	heap_trace_init_standalone();
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	ESP_ERROR_CHECK(example_connect());

	ESP_LOGI(TAG, "\n\n_____ Starting MQTT tests______");
	ESP_LOGI(TAG, "     _____ Starting MQTT Unit tests______");

	run_mqtt_unit_tests();

	ESP_LOGI(TAG, "     _____ Finished MQTT Unit tests______");
	vTaskDelay(500 / portTICK_PERIOD_MS);

	ESP_LOGI(TAG, "     _____ Starting MQTT System tests______");

	run_mqtt_system_tests();

	ESP_LOGI(TAG, "     _____ Finished MQTT System tests______");
	ESP_LOGI(TAG, "_____ Finished MQTT tests______");
	vTaskDelay(500 / portTICK_PERIOD_MS);

	return ESP_OK;
}
#endif //TESTING_MQTT_HANDLER_INNO

#ifndef TESTING_INNO_COMPONENTS
/**
 * Setup all components for the blower burn in app
 */
static esp_err_t run_blower_burn_in_app(void) {

	esp_err_t ret = ESP_OK;
	ESP_LOGI(TAG, "Setting up Serial Inno and burn in task");
	setup_driver();
	// Start the task for receiving on InnoModbus
	xTaskCreate(&uart_rx_task, "uart_rx_", 1024 * 3,
	NULL, 12, &uart_rx_handle);

	// Start the task for Blower Burn-in
	xTaskCreate(&burn_in_task, "burn_in_", 1024 * 4,
	NULL, 6, &burn_in_handle);

	vTaskDelay(100 / portTICK_PERIOD_MS);
	// Setup the event handler
	init_event_handler();

	// Setup mqtt client and configuration
	app_cfg.config_type = CONFIG_TYPE_DEFAULT;
	app_cfg.event_base = get_event_handler_base();
	app_cfg.eh_handler = get_event_handler_loop();

	if (wifi_conn) {

		ESP_ERROR_CHECK(setup_mqtt_default(&app_cfg));
		ESP_ERROR_CHECK(setup_mqtt_setup(&app_cfg));
		set_ui_ip(get_ip());
		set_ui_esp_name(app_cfg.node_name);
	}

	ESP_LOGI(TAG, "Setup MQTT handler Completed ");
	vTaskDelay(1000 / portTICK_PERIOD_MS);

	return ret;
}

#endif // not defined TESTING_INNO_COMPONENTS

void app_main(void) {
//	const char* tes = NULL;

	srand(time(NULL));   // Initialization, should only be called once.
	esp_log_level_set("inno_common", ESP_LOG_WARN);
	esp_log_level_set("inno_connect", ESP_LOG_DEBUG);
	esp_log_level_set("inno_wifi_connect", ESP_LOG_DEBUG);

	esp_log_level_set("wifi", ESP_LOG_WARN);
	esp_log_level_set("mqtt_handler", ESP_LOG_WARN);

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
#ifdef TESTING_INNO_COMPONENTS
#ifdef TESTING_EH_LOOP
	run_event_handler_tests();

#endif  // TESTING_EH_LOOP

#ifdef TESTING_MQTT_HANDLER_INNO
	run_mqtt_test();
#endif //TESTING_MQTT_HANDLER_INNO

#ifdef TESTING_SER_INNO
    run_ser_inno_tests(NULL);
#endif //TESTING_SER_INNO

#else // Default setup for running app
	//		ESP_LOG_NONE,       /*!< No log output */
	//	    ESP_LOG_ERROR,      /*!< Critical errors, software module can not recover on its own */
	//		ESP_LOG_WARN,       /*!< Error conditions from which recovery measures have been taken */
	//	    ESP_LOG_INFO,       /*!< Information messages which describe normal flow of events */
	//	    ESP_LOG_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
	//	    ESP_LOG_VERBOSE     /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */

	esp_log_level_set("msg16", ESP_LOG_ERROR);
	esp_log_level_set("serial_inno", ESP_LOG_ERROR);
	esp_log_level_set("burn-in", ESP_LOG_DEBUG);
	esp_log_level_set("UI_blower-BI", ESP_LOG_DEBUG);
	esp_log_level_set("UI_Timer", ESP_LOG_WARN);
	esp_log_level_set("UI_EVENT", ESP_LOG_DEBUG);
	esp_log_level_set("UI_Detail", ESP_LOG_DEBUG);
	esp_log_level_set("spi_master", ESP_LOG_WARN);
	esp_log_level_set("efuse", ESP_LOG_WARN);
	esp_log_level_set("GC_task", ESP_LOG_WARN);
	esp_log_level_set("user_event_loop", ESP_LOG_WARN);
	esp_log_level_set("offset_data", ESP_LOG_DEBUG);
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	bsp_display_backlight_on();

	ESP_ERROR_CHECK(ui_main_start());
	esp_err_t err_wifi = inno_connect();
	if (err_wifi==ESP_OK) {
		wifi_conn = 1;
	}

	ESP_ERROR_CHECK(run_blower_burn_in_app());
#endif

}
