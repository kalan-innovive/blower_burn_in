#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
//#include "app_event_handler.h"
#include "esp_event.h"

#include "include/msg16.h"
#include "serial_inno.h"

#include <stdio.h>

#include "driver/gpio.h"
#include "sdkconfig.h"


#define UART_BUFFER_SIZE 256
#define UART_RX_QUEUE_SIZE 15
#define BUF_SIZE (1024)

extern QueueHandle_t uart_rx_queue;
extern QueueHandle_t uart_tx_queue;
extern QueueHandle_t rack_queue;
SemaphoreHandle_t transactSemaphore;

static const char *tag = "serial_inno";

static int set_;
#define F_DE_RS485 (1ULL<<GPIO_NUM_9)


typedef struct {
    uint8_t * buf;
    int buf_sz;
    int esc_next;
    int len;
} comm_frame_t;

static void transact_wrapper_init();
static void transact_wrapper_deinit();

/*
 * Receives MSG16_REQUEST events from EH
 * Sends Request out waits for a response
 * Post response to MSG16_RESPONSE
 * typedef enum {
	MSG16_EVENT_REQUEST,
	MSG16_EVENT_RESPONSE,
	MSG16_EVENT_TIMEOUT,
	MSG16_EVENT_CONNECTED,
	MSG16_EVENT_DISCONECTED,
	MSG16_EVENT_QUEUE_FULL,
} serial_event_id_t;
 */
//static void serial_event_handler(void *handler_arg, esp_event_base_t base,
//		int32_t id, void *event_data) {
//
//
//
//}


void uart_rx_task(void *pvParameters) {

	uint8_t msg_buffer[256] = { 0 };
	comm_frame_t com_frame;
	com_frame.buf = msg_buffer;
	com_frame.buf_sz = sizeof(msg_buffer);
	com_frame.esc_next = 0;
	com_frame.len = 0;
	comm_frame_t *s = &com_frame;
	uint8_t b;

	// block while receiving data until a FLAG is received
	while(true){
		// Check if there is a transmit
		// Return 0 on timeout
		uint8_t len = uart_read_bytes(UART_SERIAL_INNO, &b, 1, 100 / portTICK_PERIOD_MS);
		if (len == 0) {
			// Reset the buffer
			s->len = 0;
			continue;
		}
		ESP_LOGD(tag, "%d, Byte:%02x", s->len, b);
		if (s->len <= 1 && b == FLAG) {

				ESP_LOGD(tag, "Start Flag ");
				s->buf[s->len] = b;
				s->len = 1;
		}

		// on flag, return length of frame if valid
		// this can return a zero length frame
		else if (b == FLAG) {
			s->esc_next = 0;
			s->buf[s->len] = b;
			s->len++;

			msg16_t msg16;
			msg16.type = 0;
			msg16.len = 0;
			msg16.dev_id = 0;

			ESP_LOGD(tag, "End Flag Found ");

			ESP_LOGD(tag, "Received Frame: len:%d, ", s->len);
			ESP_LOG_BUFFER_HEXDUMP(tag, s->buf, s->len, ESP_LOG_DEBUG);


			size_t msg_len = unpack_msg16(s->buf, s->len, &msg16);

			if (msg_len !=  s->len) {
				ESP_LOGW(tag, "Discarding msg unable to unpack: Error=%d expected=%d", msg_len, s->len);
				// TODO: send message decode error
			} else {
				// TODO: send event instead of using queue
//				char buf_s[126] = { '\0' };
//				int pos = 0;
//				// Print out the message and compare the values
//				for (int i = 0; i < s->len; i++) {
//					pos += sprintf(&buf_s[pos], "%02x:", s->buf[i]);
//				}
				ESP_LOGD(tag, "Received length %d| Packed msg16 type %d", s->len, msg16.type);

				xQueueSend(uart_rx_queue, (void*)&msg16, portMAX_DELAY);
				ESP_LOGI(tag, "Done, Finished sending msg16: Type: %d, dev=%d, Addr=%d",
						msg16.type, msg16.dev_id, msg16.addr);
			}
			s->len = 0;
			s->esc_next = 0;
		}
		// on escape, mark the next byte to be xor'd
		else if (b == ESC) {
			s->esc_next = 1;
			ESP_LOGD(tag, "Escape Flag, %02X ", b);

		}
		// non-special character, add to buffer
		else {
			if (s->esc_next) {

				b ^= 0x20;
				s->esc_next = 0;
				ESP_LOGD(tag, "Escape Character, %02X ", b);

			}
			// only write to buffer if we have space
			// else, set the error flag so we know we are missing data
			if (s->len < s->buf_sz) {
				s->buf[s->len] = b;
				s->len++;
			} else {
				// Reset the buffer
				s->len = 0;
			}
		}
		// Check for a notification without waiting
		if (ulTaskNotifyTake(pdTRUE, 0) > 0){
			ESP_LOGW(tag, "Closing tx_task ");
			vTaskDelete(NULL);
		}
	}

}




int uart_tx_task(uint8_t *buf, size_t len) {
	// send the buffer check that it was sent
	int pos = 0;
	char buf_s[257] = { '\0' };

	// Print out the message and compare the values
	for (int i = 0; i < len; i++) {
		pos += sprintf(&buf_s[pos], "%02x:", buf[i]);
	}

	ESP_LOGD(tag, "Sending data:%s length %d| Bytes written", buf_s, len);
	int num_byt_written = uart_write_bytes(UART_SERIAL_INNO, (const char*) buf,
			len);

	ESP_LOGD(tag, "Sent data complete num_bytes written %d", num_byt_written);

	vTaskDelay(0);

	return num_byt_written;

}

void setup_driver() {
	if (set_) {
		// TODO change to return espok
		return;
	}
	// Setup the queue
	uart_rx_queue = xQueueCreate(6, sizeof(msg16_t));
//	rack_queue = xQueueCreate(10, sizeof(msg16_t));
	ESP_LOGI(tag, "Creating transact wrapper");
	transact_wrapper_init();
	ESP_LOGI(tag, "Finished transact wrapper");
	vTaskDelay(100);


	ESP_LOGI(tag, "Uart Config on UART1 ");

	uart_config_t uart_config = { .baud_rate = 115200, .data_bits =
			UART_DATA_8_BITS, .parity = UART_PARITY_DISABLE, .stop_bits =
			UART_STOP_BITS_1, .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
			.rx_flow_ctrl_thresh = 122,
			.source_clk = UART_SCLK_DEFAULT, };
	int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
        intr_alloc_flags = ESP_INTR_FLAG_IRAM;
    #endif
	uart_driver_install(UART_SERIAL_INNO, BUF_SIZE, 0, 0, NULL,
			intr_alloc_flags);
	vTaskDelay(100);

	ESP_LOGI(tag, "Uart Driver installed on UART1 ");

	uart_param_config(UART_SERIAL_INNO, &uart_config);
	ESP_LOGI(tag, "Uart Config installed on UART1 ");
	vTaskDelay(100);


	uart_set_pin(UART_SERIAL_INNO, 43, 44, 14, -1);
	ESP_LOGI(tag, "Uart Pins installed on UART1 ");
	vTaskDelay(100);


	uart_set_mode(UART_SERIAL_INNO, UART_MODE_RS485_HALF_DUPLEX);
	ESP_LOGI(tag, "Uart Mode installed on UART1 ");
	vTaskDelay(100);

	set_ = 1;

}


void deinit_driver() {
    if (!set_) {
        // Driver has not been set up, so there's nothing to deinitialize
        return;
    }

    // Deinitialize the UART driver
    uart_driver_delete(UART_SERIAL_INNO);

    // Deinitialize the semaphore or any other synchronization primitives
    transact_wrapper_deinit();

    // Delete the queues if they were created
    if (uart_rx_queue != NULL) {
        vQueueDelete(uart_rx_queue);
        uart_rx_queue = NULL;
    }

    // Reset the set flag to indicate the driver is no longer initialized
    set_ = 0;
}


/*
 * Creates a read transaction on the modbus inno network
 * @Param: request struct, response struct, timout value
 * Timeout is reset on every return
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out
 */

int transact_read(const msg16_t *request, msg16_t *response, TickType_t timeout) {
	// pack the request message into bytes
	uint8_t packed_request[256]; // = { 0 };
	size_t len, packed_size = 0;
	int num_bytes = 0;
	int ret = 0;
	packed_size = pack_msg16((const msg16_t*) request, packed_request, &len);

	/* Todo: check if the queeue is empty first /*/

	// send the packed message
	num_bytes = uart_tx_task(packed_request, packed_size);
	ESP_LOGI(tag, "Transact Read type%d, dev ID=%d, addr=%d, len=%d timeout=%lu ",
			request->type, request->dev_id, request->addr, request->len, timeout);

	if (num_bytes == 0){
			return 0;
		}
	if (timeout == 0) {
		timeout = (TickType_t)((request->len) * 4 /10) + 6 ;
	}


	msg16_t msg16;
	msg16.type = 0;
	msg16.dev_id = 0xFF;
	msg16.addr = 0;
	msg16.len = 0;
	while (xQueueReceive(uart_rx_queue, (void*)&msg16, timeout) == pdTRUE) {



		// validate response
		if (msg16.type != (READ_RESP) || msg16.dev_id != request->dev_id) {
			//invalid response
			ESP_LOGW(tag, "Msg_type %d != %d, Received DevID:%d Request DevID:%d",
					msg16.type,READ_RESP, msg16.dev_id, request->dev_id);
			ret = -1;
		} else {

			// fill out response struct
			response->type = msg16.type;
			response->dev_id = msg16.dev_id;
			response->addr = msg16.addr;
			response->len = msg16.len;
			for (int i = 0; i < response->len; i++) {
				response->payload[i] = msg16.payload[i];
			}
			ESP_LOGI(tag, "Transact Read Response type%d, dev ID=%d, addr=%d, len=%d",
						request->type, request->dev_id, request->addr, request->len);
			ret = 1;
			break;
		}
		ESP_LOGW(tag, "Queue Received type%d, dev ID=%d, addr=%d, len=%d Discarding Message",
						msg16.type, msg16.dev_id, msg16.addr, msg16.len);
	}


	if (ret == 0) {
		// timeout
		ESP_LOGW(tag, "Transact Timed out; Timeout period=%lu", timeout);
	}



	return ret;
}

/*
 * Creates a write transaction on the modbus inno network
 * @Param: request struct, response struct, timout value
 * Timeout is reset on every return
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out
 */

int transact_write(const msg16_t *request, msg16_t *response,
		TickType_t timeout) {
	// pack the request message into bytes
	uint8_t packed_request[256]; // = { 0 };
	size_t len, packed_size = 0;
	int num_bytes = 0;
	int ret = 0;
	packed_size = pack_msg16((const msg16_t*) request, packed_request, &len);

	ESP_LOGI(tag, "Transact Write type%d, dev ID=%d, addr=%d, len=%d",
				request->type, request->dev_id, request->addr, request->len);

	/* Todo: check if the queeue is empty first /*/

	// send the packed message
	num_bytes = uart_tx_task(packed_request, packed_size);
	if (num_bytes == 0){
		return 0;
	}

	msg16_t msg16;
	msg16.type = 0;
	msg16.dev_id = 0xFF;
	msg16.addr = 0;
	msg16.len = 0;
	while (xQueueReceive(uart_rx_queue, (void*)&msg16, timeout) == pdTRUE) {

		ESP_LOGI(tag, "Queue Received type%d, dev ID=%d, addr=%d, len=%d",
				msg16.type, msg16.dev_id, msg16.addr, msg16.len);

		// validate response
		if (msg16.type != (WRITE_RESP) || msg16.dev_id != request->dev_id) {
			//invalid response
			ESP_LOGW(tag, "Msg_type %d != %d, Received DevID:%d Request DevID:%d",
					msg16.type, WRITE_RESP, msg16.dev_id, request->dev_id);
			ret = -1;
		} else {

			// fill out response struct
			response->type = msg16.type;
			response->dev_id = msg16.dev_id;
			response->addr = request->addr;
			response->len = msg16.len;
			for (int i = 0; i < response->len; i++) {
				response->payload[i] = msg16.payload[i];
			}
			ESP_LOGI(tag, "Transact Write Response type%d, dev ID=%d, addr=%d, len=%d",
							request->type, request->dev_id, request->addr, request->len);
			ret = 1;
			break;
		}
	}


	return ret;
}




static void transact_wrapper_init() {
    transactSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(transactSemaphore); // Initially release the semaphore
}

static void transact_wrapper_deinit() {
    if (transactSemaphore != NULL) {
        vSemaphoreDelete(transactSemaphore);
        transactSemaphore = NULL;
    }
}

int transact(const msg16_t *request, msg16_t *response, TickType_t timeout) {
	// Check if the request pointer is not null
	    if (request == NULL) {
	        return -1; // Or any other error code indicating null pointer
	    }

	    if (xSemaphoreTake(transactSemaphore, timeout) == pdTRUE) {
	        // Semaphore obtained, check request type and perform the appropriate transaction
	        int result = -1; // Default to an error code
	        switch (request->type) {
	            case READ_REQ:
	                result = transact_read(request, response, timeout);
	                break;
	            case WRITE_REQ:
	                result = transact_write(request, response, timeout);
	                break;
	            default:
	                // Invalid request type
	                result = -2; // Or any other error code indicating invalid request type
	                break;
	        }

	        xSemaphoreGive(transactSemaphore); // Release the semaphore for other tasks
	        return result;
	    } else {
	        // Timeout occurred while waiting for the semaphore
	        return -3;
	    }
}



int clear_uart_rx_queue() {
	msg16_t msg;
	int num_cleared = 0;
	while (xQueueReceive(uart_rx_queue, &msg, 0) == pdTRUE) {

		++num_cleared;
//        free(msg);
	}
	return num_cleared;
}


