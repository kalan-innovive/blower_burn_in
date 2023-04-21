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



static const char *TAG = "main";

QueueHandle_t task_queue_handle;

void app_main(void)
{
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
    ESP_LOGI(TAG, "Display LVGL demo");
    bsp_display_backlight_on();
//    ESP_ERROR_CHECK(ui_main_start());

    // Initialize Values from settings
//    const board_res_desc_t *brd = bsp_board_get_description();
//    app_rack_init(brd->PMOD2->row1[1], brd->PMOD2->row1[2], brd->PMOD2->row1[3]);






	// Create Event Handler task
//	task_queue_handle = xQueueCreate(10, sizeof(queue_task_t));
//    xTaskCreate(event_handler_task, "event_handler_task", 4096, NULL, 5, NULL);

    while (true) {
        printf("Hello from app_main!\n");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
