/*
 * pressureuartdriver.c
 *
 *  Created on: Aug 15, 2022
 *      Author: ekalan
 */
#include "pressureuartdriver.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "ms45sensor.h"
#include "helpers.h"
//#include "rs485_uart_inno_driver.h"
#include "pins.h"

#include "freertos/queue.h"

static const int RX_BUF_SIZE = 1024;
static const int RD_BUF_SIZE = 1024;

static const char *TAG = "uart_events";
extern QueueHandle_t read_spi_queue;
extern QueueHandle_t uart1_queue;



int sendUARTData(const char *data)
{
	const int len = strlen(data);
	char data_head[20] = { };
	strcpy(data_head, &data[len - 10]);
	const int txBytes = uart_write_bytes(UART_SERIAL, data, len);
	ESP_LOGI("uart_send", "Wrote %d bytes |%s|, %d", txBytes, data_head,
			strlen(data_head));
	return txBytes;
}

static void parse_results(const char *data) {
	// Check the size
	// Split on the | character
	// check the return value
	// return number or 0 if error
}

void rx_task(void *arg)
{
	static const char *RX_TASK_TAG = "RX_TASK";
	esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
	uint8_t *data = (uint8_t*) malloc(RX_BUF_SIZE + 1);
	ESP_LOGI(RX_TASK_TAG, "Entering Task ");

	while (1) {
		const int rxBytes = uart_read_bytes(UART_SERIAL, data, RX_BUF_SIZE,
				500 / portTICK_PERIOD_MS);
		ESP_LOGI(RX_TASK_TAG, "Task attempting to read");

		if (rxBytes > 0) {
			data[rxBytes] = 0;
			ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);

		} else {
			ESP_LOGI(RX_TASK_TAG, "RX task no data ");

		}
		vTaskDelay(5000 / portTICK_PERIOD_MS);

	}
	free(data);
}

void tx_task(void *arg)
{
	static int tx_num = 0;
	char *Txdata = (char*) malloc(100);
	char rs485array[] = { 0x7e, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
			0x7e, '\n' };

	while (1) {
		// Setup timer
		uint32_t start_timer = 0;
		sprintf(Txdata, "read|%d|", tx_num++);
		// Write the rs485 communication wait for buffer to be
		uart_write_bytes(UART_SERIAL, rs485array, 10);

		esp_err_t err = uart_wait_tx_idle_polling(UART_SERIAL);
		spin_us(300);
		uart_write_bytes(UART_SERIAL, Txdata, strlen(Txdata));
//		ESP_LOGI(TAG, "rs485array %s, ERR%d", rs485array, err);

		ESP_LOGI(TAG, "Transmitted Txdata %s", Txdata);

		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
	free(Txdata);
	Txdata = NULL;
	vTaskDelete(NULL);
}


void tx_task_spi_test(void *arg)
{
	static int tx_num = 0;
	char *Txdata = (char*) malloc(100);
	char rs485array[] = { 0x7e, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
			0x7e, '\n' };

	while (1) {
		// Setup timer
		uint32_t start_timer = 0;
		sprintf(Txdata, "read|%d|", tx_num++);
		// Write the rs485 communication wait for buffer to be
		uart_write_bytes(UART_SERIAL, rs485array, 10);
		esp_err_t err = uart_wait_tx_idle_polling(UART_SERIAL);
		spin_us(300);
		uart_write_bytes(UART_SERIAL, Txdata, strlen(Txdata));
		tx_num = (tx_num > 100) ? 0 : tx_num;
		// Sdd to the queue
		int num_reads = 10;
		xQueueSend(read_spi_queue, (void* ) &num_reads,
									(TickType_t ) 0);

		ESP_LOGI(TAG, "Txdata %s", Txdata);

		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
	free(Txdata);
	Txdata = NULL;
	vTaskDelete(NULL);
}

void init_uart(void)
{
	const uart_config_t uart_config = {
			.baud_rate = 115200,
			.data_bits = UART_DATA_8_BITS,
			.parity = UART_PARITY_DISABLE,
			.stop_bits = UART_STOP_BITS_1,
			.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
			.source_clk = UART_SCLK_APB,
	};
	int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

	uart_driver_install(UART_SERIAL, 0, 0, 0, NULL, 0);
	uart_param_config(UART_SERIAL, &uart_config);
//	uart_set_pin(UART_SERIAL, TXD_PIN_SER, RXD_PIN_SER, UART_PIN_NO_CHANGE,
//	UART_PIN_NO_CHANGE);

	uart_set_pin(UART_SERIAL, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
		UART_PIN_NO_CHANGE);

//	//Set uart pattern detect function.
//	uart_enable_pattern_det_baud_intr(UART_SERIAL, '|', 1, 9, 0,
//			0);
//	//Reset the pattern queue length to record at most 20 pattern positions.
//	uart_pattern_queue_reset(UART_SERIAL, 20);
}

void init_uart_w_queue(void)
{
	/* Configure parameters of an UART driver,
	 * communication pins and install the driver */
	uart_config_t uart_config = {
			.baud_rate = 115200,
			.data_bits = UART_DATA_8_BITS,
			.parity = UART_PARITY_DISABLE,
			.stop_bits = UART_STOP_BITS_1,
			.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
			.source_clk = UART_SCLK_APB,
	};
	//Install UART driver, and get the queue.
	uart_driver_install(UART_SERIAL, RX_BUF_SIZE * 2, RX_BUF_SIZE * 2, 20,
			&uart1_queue, 0);
	uart_param_config(UART_SERIAL, &uart_config);

	//Set UART log level
	esp_log_level_set(TAG, ESP_LOG_INFO);
	//Set UART pins (using UART0 default pins ie no changes.)
	uart_set_pin(UART_SERIAL, TXD_PIN_SER, RXD_PIN_SER, UART_PIN_NO_CHANGE,
	UART_PIN_NO_CHANGE);

	//Set uart pattern detect function.
	uart_enable_pattern_det_baud_intr(UART_SERIAL, '|', PATTERN_CHR_NUM, 9, 0,
			0);
	//Reset the pattern queue length to record at most 20 pattern positions.
	uart_pattern_queue_reset(UART_SERIAL, 20);
}

void uart_event_task(void *pvParameters)
{
	// Set the queue handles
//	QueueHandle_t read_spi_queue = (QueueHandle_t) pvParameters;

	static const char *TAG = "Event_TASK";

	uart_event_t event;
	size_t buffered_size;
	uint8_t *dtmp = (uint8_t*) malloc(RD_BUF_SIZE);
	ESP_LOGI(TAG, "Checking RX_buffer queue: %d", UART_SERIAL);

	for (;;) {
//		vTaskDelay(2000 / portTICK_PERIOD_MS);
		//Waiting for UART event.
		if (xQueueReceive(uart1_queue, (void*) &event,
				(TickType_t) 1000 / portTICK_PERIOD_MS)) {
			bzero(dtmp, RD_BUF_SIZE);
//			ESP_LOGI(TAG, "uart[%d] event:", UART_SERIAL);
			ESP_LOGI(TAG, "[UART DATA]: size:%d  timeout:%d, event-type:%d",
					event.size, event.timeout_flag, event.type);
			switch (event.type) {
			//Event of UART receving data
			/*We'd better handler data event fast, there would be much more data events than
			 other types of events. If we take too much time on data event, the queue might
			 be full.*/
			case UART_DATA:
				ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
				uart_read_bytes(UART_SERIAL, dtmp, event.size, portMAX_DELAY);
				ESP_LOGI(TAG, "[DATA EVT]:");
//				uart_write_bytes(EX_UART_NUM, (const char*) dtmp, event.size);
				break;
				//Event of HW FIFO overflow detected
			case UART_FIFO_OVF:
				ESP_LOGI(TAG, "hw fifo overflow");
				// If fifo overflow happened, you should consider adding flow control for your application.
				// The ISR has already reset the rx FIFO,
				// As an example, we directly flush the rx buffer here in order to read more data.
				uart_flush_input(UART_SERIAL);
				xQueueReset(uart1_queue);
				break;
				//Event of UART ring buffer full
			case UART_BUFFER_FULL:
				ESP_LOGI(TAG, "ring buffer full");
				// If buffer full happened, you should consider encreasing your buffer size
				// As an example, we directly flush the rx buffer here in order to read more data.
				uart_flush_input(UART_SERIAL);
				xQueueReset(uart1_queue);
				break;
				//Event of UART RX break detected
			case UART_BREAK:
				ESP_LOGI(TAG, "uart rx break");
				break;
				//Event of UART parity check error
			case UART_PARITY_ERR:
				ESP_LOGI(TAG, "uart parity error");
				break;
				//Event of UART frame error
			case UART_FRAME_ERR:
				ESP_LOGI(TAG, "uart frame error");
				break;
				//UART_PATTERN_DET
			case UART_PATTERN_DET:
				uart_get_buffered_data_len(UART_SERIAL, &buffered_size);
				int pos = uart_pattern_pop_pos(UART_SERIAL);
				ESP_LOGI(TAG,
						"[UART PATTERN DETECTED] pos: %d, buffered size: %d",
						pos, buffered_size);
				if (pos == -1) {
					// There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
					// record the position. We should set a larger queue size.
					// As an example, we directly flush the rx buffer here.
					uart_flush_input(UART_SERIAL);
				} else {
					uart_read_bytes(UART_SERIAL, dtmp, pos,
							100 / portTICK_PERIOD_MS);
					uint8_t pat[PATTERN_CHR_NUM + 1];
					memset(pat, 0, sizeof(pat));
					uart_read_bytes(UART_SERIAL, pat, PATTERN_CHR_NUM,
							100 / portTICK_PERIOD_MS);
					ESP_LOGI(TAG, "read data: %s", dtmp);
					ESP_LOGI(TAG, "read pat : %s", pat); // Looking for read|10|

					char str[20];
					char *ptr;
					strcpy(str, (char*) dtmp);
					int num_reads;
					num_reads = (int) strtol(str, &ptr, 10);
					ESP_LOGI(TAG, "Number of Reads : %d", num_reads);
					xQueueSend(read_spi_queue, (void* ) &num_reads,
							(TickType_t ) 0);
				}
				break;
				//Others
			default:
				ESP_LOGI(TAG, "uart event type: %d", event.type);
				break;
			}
		}
	}
	free(dtmp);
	dtmp = NULL;
	vTaskDelete(NULL);
}
