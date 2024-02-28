#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
//#include "led_strip.h"
#include "sdkconfig.h"

#include "spi_mod.h"
#include "freertos/queue.h"
#include "ms45sensor.h"
#include "driver/uart.h"
#include "pressureuartdriver.h"
//#include "rs485_uart_inno_driver.h"

#include "pins.h"

static const char *TAG = "main";
#define test  portTICK_RATE_MS

#define BUF_SIZE (1024)
#if MS45_SENSOR

	// Create the queues for uart and spi tasks
	QueueHandle_t read_spi_queue;
	QueueHandle_t uart1_queue ;
	QueueHandle_t result_queue;

#endif
//void app_main()
//{
//	esp_log_level_set(TAG, ESP_LOG_INFO);
//
//#if MS45_SENSOR
//
//	// Create the queues for uart and spi tasks
//	read_spi_queue = xQueueCreate(10, sizeof(uint32_t));
//	uart1_queue = xQueueCreate(10, sizeof(uint32_t));
//
//	result_queue = xQueueCreate(10,sizeof(p_result_handle_t));
//
//
////	init_uart_w_queue();
//	init_uart();
//	xTaskCreate(rx_task, "SPI_pressure_", 1024 * 4, NULL,
//		configMAX_PRIORITIES, NULL);
//
//
////	//Create the SPI task pass the queue _spi_queue
////	xTaskCreate(MS45_sensor_task, "SPI_pressure_", 1024 * 4, NULL,
////	configMAX_PRIORITIES-8, NULL);
//
//	//Create a task to handler UART event from ISR
////	xTaskCreate(uart_event_task, "uart_event_task", 1024 * 4,
////			(void*) read_spi_queue,
////			configMAX_PRIORITIES - 9, NULL);
//
//	// Send data to be parsed every couple seconds
//	// Remove comment to debug by connecting rx and tx pins together
//	xTaskCreate(tx_task_spi_test, "uart_tx_task", 1024 * 4, NULL,
//	configMAX_PRIORITIES - 10, NULL);
//
//#endif
//
//#if RS485_COM
//
//	QueueHandle_t result_queue = xQueueCreate(10,
//			sizeof(modbus_msg));
//
//	init_uart_rs485();
//
//	//Create the RS485 process task task pass the queue
////	xTaskCreate(MS45_sensor_task, "SPI_pressure_", 4096,
////			(void*) read_rs485_queue,
////			configMAX_PRIORITIES, NULL);
//
//	//Create a task to handler UART event from ISR
//	xTaskCreate(rs485_uart_inno_task, "rs485_uart_inno_task", 2048,
//			(void*) result_queue,
//			configMAX_PRIORITIES - 9, NULL);
//
//	// Send data to be parsed every couple seconds for testing
//	// Remove comment to debug by connecting rx and tx pins together
//	xTaskCreate(tx_task, "uart_tx_task", 1024 * 2, NULL,
//	configMAX_PRIORITIES, NULL);
//
//#endif
//
//}

