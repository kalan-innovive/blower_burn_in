/*
 * apprackcomm.c
 *
 *      Author: ekalan
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//#include "esp_system.h"
//#include "freertos/queue.h"

#include "esp_log.h"

#include "serial_inno_cmd.h"
#include "apprackcomm.h"
//#include "inno_connect.h"
#include "serial_inno.h"
#include "console_cmd.h"
#include "ui.h"
#include "bsp_board.h"
#include "bsp/esp-bsp.h"


int run_apprack = 0;
static const char* tag = "app_rackcomm";

//extern QueueHandle_t task_queue_handle;
//extern QueueHandle_t uart_rx_queue;
//extern QueueHandle_t uart_tx_queue;
//extern QueueHandle_t rack_queue;
//
//extern TaskHandle_t rack_task_handle;
TaskHandle_t uart_rx_handle;
//extern TaskHandle_t burn_in_handle;






static void setup_task() {
	esp_log_level_set("msg16", ESP_LOG_WARN);
	esp_log_level_set("serial_inno", ESP_LOG_WARN);
	ESP_LOGI(tag, "Setting up Serial Inno and console");

	setup_driver();
	// Start the task for receiving on InnoModbus
	xTaskCreate(&uart_rx_task, "uart_rx_", 1024 * 6,
	NULL, 6, NULL);
	ESP_LOGI(tag, "Finished up Serial Inno and console");
	console_setup_config_t conf = CONSOLE_SETUP_CONFIG_DEFAULT();
	conf.using_serialinno = 1;

	setup_console_cmds(&conf);
	run_apprack = 1;
}

void stop_appracktask(){
	run_apprack = 0;
}

blower_update blowerinfo ={0,0,0,0,0,0};
blower_version blow_version = DEFAULT_BLOWER_VER_VALUES;

//const char[5]* ={1,3,0x21, 0x23, 0x11}
void apprackcomm_task(void *pvParameter){
	setup_task();
	vTaskDelay(1000);
	ESP_LOGI(tag, "Starting Task");

	while(run_apprack){
//		ESP_LOGI(tag, "Task Loop");
		vTaskDelay(100 );
		if (blowerinfo.is_updated) {
			if (bsp_display_lock(0)) {

				if (blowerinfo.valid) {
					lv_label_set_text_fmt(ui_deviceLabel, "%d", blowerinfo.devid);
					lv_label_set_text_fmt(ui_chipidLabel,"%u",blowerinfo.chipid);
					lv_label_set_text_fmt(ui_offsetLabel,"%u mInH2O",blowerinfo.offset);
					lv_label_set_text_fmt(ui_uuidLabel,"%u", blowerinfo.uuid);


				}
				else if(!blowerinfo.valid){
					lv_label_set_text(ui_deviceLabel,"NA");
					lv_label_set_text(ui_chipidLabel,"");
					lv_label_set_text(ui_offsetLabel, "");
					lv_label_set_text(ui_uuidLabel,"");

				}
				blowerinfo.is_updated = 0;
				bsp_display_unlock();
			}
		}
		//TODO: update the blower version struct
//		if (blow_version.is_updated) {
//			if (bsp_display_lock(0)) {
//
//				if (blowerinfo.valid) {
//					lv_label_set_text_fmt(ui_deviceLabel, "%d", blowerinfo.devid);
//					lv_label_set_text_fmt(ui_chipidLabel,"%u",blowerinfo.chipid);
//					lv_label_set_text_fmt(ui_offsetLabel,"%u mInH2O",blowerinfo.offset);
//					lv_label_set_text_fmt(ui_uuidLabel,"%u", blowerinfo.uuid);
//
//				}
//				else if(!blowerinfo.valid){
//					lv_label_set_text(ui_deviceLabel,"NA");
//					lv_label_set_text(ui_chipidLabel,"");
//					lv_label_set_text(ui_offsetLabel, "");
//					lv_label_set_text(ui_uuidLabel,"");
//
//				}
//				blowerinfo.is_updated = 0;
//				bsp_display_unlock();
//			}
//		}


	};
//	delete_Task(NULL);
}
