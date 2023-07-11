#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "include/msg16.h"
#include "serial_inno.h"

#include <stdio.h>
//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
//#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define UART_BUFFER_SIZE 24
#define UART_RX_QUEUE_SIZE 10
#define BUF_SIZE (1024)

extern QueueHandle_t uart_rx_queue;
extern QueueHandle_t rack_queue;

static const char *tag = "serial_inno";

void uart_rx_task(void *pvParameters) {
	uint8_t uart_buffer[UART_BUFFER_SIZE] = { 0 };
	uint8_t msg_buffer[128] = { 0 };
	size_t uart_pos = 0;
	size_t msg_pos = 0;
	int read_len = 0;
	int pos = 0;
	char buf[UART_BUFFER_SIZE] = { '\0' };
	uint8_t byte;

	while (true) {
		// read from UART buffer
//		vTaskDelay(1000 / portTICK_PERIOD_MS);

		read_len = uart_read_bytes(UART_SERIAL_INNO, uart_buffer + uart_pos,
		UART_BUFFER_SIZE - uart_pos, 10 / portTICK_PERIOD_MS);
		if (read_len <= 0) {

			continue;
		}
		if (read_len > UART_BUFFER_SIZE) {
			ESP_LOGE(tag, "Exceeded Read length: :%d", read_len);
			ESP_LOGE(tag, "Uart_pos: :%d, msg_pos:%d", uart_pos, msg_pos);
			// Should we reset the position?
			uart_pos = 0;
			msg_pos = 0;
			vTaskDelay(100000 / portTICK_PERIOD_MS);

			continue;
		}
		ESP_LOGD(tag, "Read length: :%d", read_len);

		ESP_LOGD(tag, "Uart_pos:%d|msg_pos:%d|read_len:%d|", uart_pos, msg_pos,
				read_len);
		ESP_LOG_BUFFER_HEXDUMP(tag, buf, UART_BUFFER_SIZE, ESP_LOG_INFO);
//    	pos = 0;
//    	memset(buf, '\0', sizeof(buf));
//        for (int i = 0; i<UART_BUFFER_SIZE;i++){
//        		pos += sprintf(&buf[pos], "%02x:",uart_buffer[i] );
//		}

//		ESP_LOGD(tag, "RESP BUF: 0X%s", buf);

// process buffer
		for (size_t i = 0; i < read_len; i++) {
			byte = uart_buffer[uart_pos + i];
//    		ESP_LOGI(tag, "%d, Byte:%02x", i, byte);

			if (msg_pos == 0 && byte != 0x7e) {
				// ignore bytes before start flag
				ESP_LOGW(tag, "Ignoring Byte %d: 0x%x of read_len:%d", i, byte,
						read_len);
				continue;
			}
			if (msg_pos == 0 && byte == 0x7e) {
				// Found start byte
				ESP_LOGD(tag, "Start Flag");
				msg_buffer[msg_pos++] = byte;
				continue;
			}

			if (byte == 0x7e) {
				ESP_LOGD(tag, "End Flag found, creating msg16_t, reseting ");

				// end of message, decode and add to queue
				msg_buffer[msg_pos] = 0x00;
//                msg16_t* msg = (msg16_t*) malloc(sizeof(msg16_t));
				msg16_t msg_struct;
				msg16_t *msg = &msg_struct;
				if (msg != NULL) {
					size_t msg_len = unpack_msg16(msg_buffer, msg_pos, msg);
					if (msg_len == 0) {
						ESP_LOGW(tag, "Discarding msg unable to unpack");
//                		free(msg);
					} else {

						//                    xQueueSend(uart_rx_queue, ( void * ) &msg, portMAX_DELAY);
						if (msg->dev_id == 0x11 && msg->type == READ_REQ) {
							xQueueSend(rack_queue, &msg, portMAX_DELAY);
						} else {
							xQueueSend(uart_rx_queue, msg, portMAX_DELAY);
						}
//						free(msg);
					}
				} else {
					ESP_LOGW(tag, "Can not allocate memory");
				}
				msg_pos = 0;
			} else if (byte == 0xef && msg_pos > 0
					&& msg_buffer[msg_pos - 1] == 0xef) {
				ESP_LOGE(tag, "Received escape Flag, Byte: 0X%02x | 0X%02x",
						byte, byte + 1);
				// escape character
				msg_buffer[msg_pos - 1] = byte ^ 0x20;
				vTaskDelay(100000 / portTICK_PERIOD_MS);

			} else {
				// regular byte
				msg_buffer[msg_pos++] = byte;
			}
		}

		// move remaining bytes to start of buffer
		if (msg_pos == 0) {
			uart_pos = 0;
		} else if (msg_buffer[msg_pos - 1] == 0x7e) {
			// message ended at end of buffer, move remaining bytes to start of buffer
			uart_pos = 0;
			msg_pos = 0;
		} else if (msg_pos > 0 && msg_buffer[msg_pos - 1] == 0xef) {
			// escape character at end of buffer, move remaining bytes to start of buffer
			uart_pos = 1;
			msg_pos = 1;
			msg_buffer[0] = 0xef;
		} else {
			// message not complete, move remaining bytes to start of buffer
			uart_pos = read_len;
			memmove(uart_buffer, uart_buffer + uart_pos,
			UART_BUFFER_SIZE - uart_pos);
		}
	}
}

int uart_tx_task(uint8_t *buf, size_t len) {
	// send the buffer check that it was sent
	int pos = 0;
	char buf_s[40] = { '\0' };
	// Print out the message and compare the values
	for (int i = 0; i < len; i++) {
		pos += sprintf(&buf_s[pos], "%02x:", buf[i]);
	}
	ESP_LOGI(tag, "Sending data:%s length %d| Bytes written", buf_s, len);

	int num_byt_written = uart_write_bytes(UART_SERIAL_INNO, (const char*) buf,
			len);
//    esp_err_t err = uart_wait_tx_done(UART_SERIAL_INNO, TRANSMIT_WAIT_TIME);
//    return (err == ESP_OK) ? num_byt_written : err;
	return num_byt_written;

}

static int set_;

void setup_driver() {
	if (set_) {
		// TODO change to return espok
		return;
	}
	// Setup the queue
	uart_rx_queue = xQueueCreate(10, sizeof(msg16_t));
	rack_queue = xQueueCreate(10, sizeof(msg16_t));

	uart_config_t uart_config = { .baud_rate = 115200, .data_bits =
			UART_DATA_8_BITS, .parity = UART_PARITY_DISABLE, .stop_bits =
			UART_STOP_BITS_1, .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
			.source_clk = UART_SCLK_DEFAULT, };
	int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
        intr_alloc_flags = ESP_INTR_FLAG_IRAM;
    #endif
//	uart_driver_install(UART_SERIAL_INNO, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags);
	uart_driver_install(UART_SERIAL_INNO, BUF_SIZE, 0, 0, NULL,
			intr_alloc_flags);
	uart_param_config(UART_SERIAL_INNO, &uart_config);
	uart_set_pin(UART_SERIAL_INNO, 43, 44, UART_PIN_NO_CHANGE,
	UART_PIN_NO_CHANGE);
//    uart_set_pin(UART_NUM_1, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

//    uart_driver_install(UART_NUM_0, UART_BUFFER_SIZE,UART_BUFFER_SIZE,0,NULL,0);

	set_ = 1;

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
	uint8_t packed_request[128]; // = { 0 };
	size_t len, packed_size = 0;
	int num_bytes = 0;
	int ret = 0;
	packed_size = pack_msg16((const msg16_t*) request, packed_request, &len);

	/* Todo: check if the queeue is empty first /*/

	// send the packed message
	num_bytes = uart_tx_task(packed_request, packed_size);

	// wait for response
	msg16_t *r_msg = malloc(sizeof(msg16_t));
	if (r_msg == NULL) {
		return 0;
	}

	while (xQueueReceive(uart_rx_queue, r_msg, timeout) == pdTRUE) {

		ESP_LOGI(tag, "Queue Received type%d, dev ID=%d, addr=%d, len=%d",
				r_msg->type, r_msg->dev_id, r_msg->addr, r_msg->len);

		// validate response
		if (r_msg->type != (READ_RESP) || r_msg->dev_id != request->dev_id) {
			//invalid response
			ESP_LOGW(tag, "Msg_type %d, Received DevID:%d Request DevID:%d",
					r_msg->type, r_msg->dev_id, request->dev_id);
			ret = -1;
		} else {

			// fill out response struct
			response->type = r_msg->type;
			response->dev_id = r_msg->dev_id;
			response->addr = r_msg->addr;
			response->len = r_msg->len;
			for (int i = 0; i < response->len; i++) {
				response->msg_val[i] = r_msg->msg_val[i];
			}
			ret = 1;
			break;
		}
	}
	if (ret == 0) {
		// timeout
		ESP_LOGW(tag, "Transact Timed out");
	}

	// free received message
	if (r_msg != NULL) {
		free(r_msg);
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
	uint8_t packed_request[128]; // = { 0 };
	size_t len, packed_size = 0;
	int num_bytes = 0;
	int ret = 0;
	packed_size = pack_msg16((const msg16_t*) request, packed_request, &len);

	/* Todo: check if the queeue is empty first /*/

	// send the packed message
	num_bytes = uart_tx_task(packed_request, packed_size);

	// wait for response
	msg16_t *r_msg = malloc(sizeof(msg16_t));
	if (r_msg == NULL) {
		return 0;
	}

	while (xQueueReceive(uart_rx_queue, r_msg, timeout) == pdTRUE) {

		ESP_LOGI(tag, "Queue Received type%d, dev ID=%d, addr=%d, len=%d",
				r_msg->type, r_msg->dev_id, r_msg->addr, r_msg->len);

		// validate response
		if (r_msg->type != (READ_RESP) || r_msg->dev_id != request->dev_id) {
			//invalid response
			ESP_LOGW(tag, "Msg_type %d, Received DevID:%d Request DevID:%d",
					r_msg->type, r_msg->dev_id, request->dev_id);
			ret = -1;
		} else {

			// fill out response struct
			response->type = r_msg->type;
			response->dev_id = r_msg->dev_id;
			response->addr = r_msg->addr;
			response->len = r_msg->len;
			for (int i = 0; i < response->len; i++) {
				response->msg_val[i] = r_msg->msg_val[i];
			}
			ret = 1;
			break;
		}
	}
	if (ret == 0) {
		// timeout
		ESP_LOGW(tag, "Transact Timed out");
	}

	// free received message
	if (r_msg != NULL) {
		free(r_msg);
	}

	return ret;
}

/*
 * Creates a read transaction to get the chipID
 * @Param: int devid, unsigned *chipid
 * Stores chip id in pointer adress provided
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int get_chipid(int devid, unsigned *chipid) {
	int ret = 0;
	TickType_t timeout = 30 / portTICK_PERIOD_MS;

	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid, .addr = REG_CHIPIDH,
			.len = 1 };
	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.msg_val[0] = 0;

	unsigned tmp = 0;
	clear_uart_rx_queue();
	ret = transact_read(&msg_req, &msg_resp, timeout);
	if (ret < 1) {
//		ESP_LOGW(tag, "Transact Error: %d", ret);

	}
	tmp = (msg_resp.msg_val[0] & 0xffff) << 16;
	ESP_LOGI(tag, "Returning ChipID High 0x%04x", msg_resp.msg_val[0]);

	clear_uart_rx_queue();

	msg_req.addr = REG_CHIPIDL;

	ret = transact_read(&msg_req, &msg_resp, timeout);
	if (ret < 1) {
		ESP_LOGW(tag, "Transact Error: %d", ret);

	}
	ESP_LOGI(tag, "Returning ChipID Low 0x%04x", msg_resp.msg_val[0]);

	tmp += (msg_resp.msg_val[0]) & 0x0000ffff;
	ESP_LOGI(tag, "Returning ChipID %d", tmp);
	*chipid = tmp;

	return ret;

}

/*
 * Creates a read transaction to get the offset pressure
 * @Param: int devid, unsigned *raw_pressure
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int get_raw_pressure(int devid, int *raw_pressure) {
	int ret = 0;
	TickType_t timeout = 30 / portTICK_PERIOD_MS;
	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid,
			.addr = REG_RAW_PRESS, .len = 1 };

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.msg_val[0] = 0xFFFF;

	unsigned tmp = 0;
	int num_cleared = clear_uart_rx_queue();

	if (num_cleared > 0) {
		ESP_LOGW(tag, "Cleared %d messages from the Queue", num_cleared);
	}

	ret = transact_read(&msg_req, &msg_resp, timeout);
	if (ret < 1) {
		ESP_LOGW(tag, "Transact Error: %d", ret);

	}
	tmp = (int16_t) msg_resp.msg_val[0];
	ESP_LOGI(tag, "Returning Raw Pressure  %d", (int16_t ) msg_resp.msg_val[0]);

	*raw_pressure = tmp;

	return ret;
}

/*
 * Creates a read transaction for modbus id check, used to check if a modbus id is connected
 * @Param: int devid to check
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int check_dev_id(int devid) {
	ESP_LOGD(tag, "%s Checking Modbus id : %d", __FUNCTION__, devid);
	int ret = 0;
	TickType_t timeout = 30 / portTICK_PERIOD_MS;
	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid,
			.addr = REG_MODBUS_ADDR, .len = 1 };

	// Set up the msg set the msg to default and msg_val to 0 as we expect response 0xffff
	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.msg_val[0] = 0x00;

	int num_cleared = clear_uart_rx_queue();

	if (num_cleared > 0) {
		ESP_LOGW(tag, "Cleared %d messages from the Queue", num_cleared);
	}

	ret = transact_read(&msg_req, &msg_resp, timeout);
	if (ret == 1) {
		ESP_LOGD(tag, "Modbus id found for id 0x%x resp code: %d", devid, ret);

	} else if (ret == 0) {
		ESP_LOGD(tag, "Did not receive response from id 0x%x resp code: %d",
				devid, ret);

	} else if (ret == -1) {
		ESP_LOGE(tag,
				"Error sending message not receive response from id 0x%x resp code: %d",
				devid, ret);

	}
	return ret;
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

