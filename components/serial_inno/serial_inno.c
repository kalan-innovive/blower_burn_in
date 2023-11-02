#include "esp_log.h"

#include "freertos/FreeRTOS.h"
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
//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
//#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define UART_BUFFER_SIZE 256
#define UART_RX_QUEUE_SIZE 15
#define BUF_SIZE (1024)

extern QueueHandle_t uart_rx_queue;
extern QueueHandle_t rack_queue;

static const char *tag = "serial_inno";


typedef struct {
    uint8_t * buf;
    int buf_sz;
    int esc_next;
    int len;
} comm_frame_t;


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
//
////				s->esc_next = 0;
////				s->len = 0;
//			} else {
//				// Discard byte need start byte first
//				ESP_LOGW(tag, "Discarding Byte:%02x", b);
//				continue;
//			}
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
			ESP_LOG_BUFFER_HEXDUMP(tag, s->buf, s->len+1, ESP_LOG_DEBUG);


			size_t msg_len = unpack_msg16(s->buf, s->len, &msg16);

			if (msg_len !=  s->len) {
				ESP_LOGW(tag, "Discarding msg unable to unpack: Error=%d", msg_len);
				// TODO: send message decode error
			} else {
				// TODO: send event instead of using queue
//				char buf_s[126] = { '\0' };
//				int pos = 0;
//				// Print out the message and compare the values
//				for (int i = 0; i < s->len; i++) {
//					pos += sprintf(&buf_s[pos], "%02x:", s->buf[i]);
//				}
				ESP_LOGD(tag, "Received length %d| Packked msg16 type %d", s->len, msg16.type);

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
			vTaskDelete(NULL);
		}
	}

}




void uart_rx_task0(void *pvParameters) {
	uint8_t uart_buffer[UART_BUFFER_SIZE] = { 0 };
	uint8_t msg_buffer[128] = { 0 };
	size_t uart_pos = 0;
	size_t msg_pos = 0;
	int esc_next = 0;
	int read_len = 0;
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
//			vTaskDelay(1000 / portTICK_PERIOD_MS);

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

//		ESP_LOGD(tag, "RESP BUF: 0X%s", buf);UART_SERIAL_INNO

		// process buffer
		for (size_t i = 0; i < read_len; i++) {
			// get one byte at a time from the buffer
			byte = uart_buffer[uart_pos + i];
    		ESP_LOGD(tag, "%d, Byte:%02x", i, byte);

			if (msg_pos == 0 && byte != FLAG) {
				// ignore bytes before start flag
				ESP_LOGW(tag, "Ignoring Byte %d: 0x%x of read_len:%d", i, byte,
						read_len);
				continue;
			}
			if (msg_pos == 0 && byte == FLAG) {
				// Found start byte
				ESP_LOGD(tag, "Start Flag");
				msg_buffer[msg_pos++] = byte;
				continue;
			}

			if (byte == FLAG) {
				// end of message, decode and add to queue
				ESP_LOGD(tag, "End Flag found, creating msg16_t, reseting ");
				msg_buffer[msg_pos] = FLAG;
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
					}
				} else {
					ESP_LOGW(tag, "Can not allocate memory");
				}
				msg_pos = 0;
			}
			// If the byte is th ESCAPE sequence do not copy byte and xor the next byte
			else if (byte == ESC && msg_pos > 0) {
				// Set esc flag and increment position
				esc_next = 1;
				ESP_LOGE(tag, "Received escape Flag, Byte: 0X%02x | 0X%02x",
						byte, byte + 1);
			}


			else {
				if (esc_next){
								// escape character
					byte = byte ^ 0x20;
					//VTaskDelay(1000 / portTICK_PERIOD_MS);
				}
				 if (msg_pos < sizeof(msg_buffer)){
					 msg_buffer[msg_pos++] = byte;
				 }


			}

		}

		// move remaining bytes to start of buffer
		if (msg_pos == 0) {
			uart_pos = 0;
		} else if (msg_buffer[msg_pos - 1] == 0x7e) {
			// message ended at end of buffer, move remaining bytes to start of buffer
			uart_pos = 0;
			msg_pos = 0;
		}  else {
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
//	rack_queue = xQueueCreate(10, sizeof(msg16_t));

	uart_config_t uart_config = { .baud_rate = 115200, .data_bits =
			UART_DATA_8_BITS, .parity = UART_PARITY_DISABLE, .stop_bits =
			UART_STOP_BITS_1, .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
			.source_clk = UART_SCLK_DEFAULT, };
	int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
        intr_alloc_flags = ESP_INTR_FLAG_IRAM;
    #endif
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

	if (num_bytes == 0){
			return 0;
		}

	// wait for response
//	msg16_t *r_msg = malloc(sizeof(msg16_t));
//	if (r_msg == NULL) {
//		return 0;
//	}
	msg16_t msg16;
	msg16.type = 0;
	msg16.dev_id = 0xFF;
	msg16.addr = 0;
	msg16.len = 0;
	while (xQueueReceive(uart_rx_queue, (void*)&msg16, timeout) == pdTRUE) {

		ESP_LOGI(tag, "Queue Received type%d, dev ID=%d, addr=%d, len=%d",
				msg16.type, msg16.dev_id, msg16.addr, msg16.len);

		// validate response
		if (msg16.type != (READ_RESP) || msg16.dev_id != request->dev_id) {
			//invalid response
			ESP_LOGW(tag, "Msg_type %d, Received DevID:%d Request DevID:%d",
					msg16.type, msg16.dev_id, request->dev_id);
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
			ret = 1;
			break;
		}
	}


//	while (xQueueReceive(uart_rx_queue, r_msg, timeout) == pdTRUE) {
//
//		ESP_LOGI(tag, "Queue Received type%d, dev ID=%d, addr=%d, len=%d",
//				r_msg->type, r_msg->dev_id, r_msg->addr, r_msg->len);
//
//		// validate response
//		if (r_msg->type != (READ_RESP) || r_msg->dev_id != request->dev_id) {
//			//invalid response
//			ESP_LOGW(tag, "Msg_type %d, Received DevID:%d Request DevID:%d",
//					r_msg->type, r_msg->dev_id, request->dev_id);
//			ret = -1;
//		} else {
//
//			// fill out response struct
//			response->type = r_msg->type;
//			response->dev_id = r_msg->dev_id;
//			response->addr = r_msg->addr;
//			response->len = r_msg->len;
//			for (int i = 0; i < response->len; i++) {
//				response->payload[i] = r_msg->payload[i];
//			}
//			ret = 1;
//			break;
//		}
//	}
	if (ret == 0) {
		// timeout
		ESP_LOGW(tag, "Transact Timed out; Timeout period=%lu", timeout);
	}

//	// free received message
//	if (r_msg != NULL) {
//		free(r_msg);
//	}

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
	if (num_bytes == 0){
		return 0;
	}

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
				response->payload[i] = r_msg->payload[i];
			}
			ret = 1;
			break;
		}
	}
	if (ret == 0) {
		// timeout
		ESP_LOGW(tag, "Transact Timed out; Timeout period=%lu", timeout);
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
//int get_chipid(int devid, unsigned *chipid) {
//	int ret = 0;
//	TickType_t timeout = 30 / portTICK_PERIOD_MS;
//
//	// TODO: Read both at the same time
////	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid, .addr = REG_CHIPIDH,
////			.len = 1 };
//	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid, .addr = REG_CHIPIDH,
//				.len = 1 };
//	msg16_t msg_resp;
//	msg_resp.len = 0;
//	msg_resp.payload[0] = 0;
//
//	unsigned tmp = 0;
//	clear_uart_rx_queue();
//	ret = transact_read(&msg_req, &msg_resp, timeout);
//	if (ret < 1) {
////		ESP_LOGW(tag, "Transact Error: %d", ret);
//
//	}
//	tmp = (msg_resp.payload[0] & 0xffff) << 16;
//	ESP_LOGI(tag, "Returning ChipID High 0x%04x", msg_resp.payload[0]);
//
//	clear_uart_rx_queue();
//
//	msg_req.addr = REG_CHIPIDL;
//
//	ret = transact_read(&msg_req, &msg_resp, timeout);
//	if (ret < 1) {
//		ESP_LOGW(tag, "Transact Error: %d", ret);
//
//	}
//	ESP_LOGI(tag, "Returning ChipID Low 0x%04x", msg_resp.payload[0]);
//
//	tmp += (msg_resp.payload[0]) & 0x0000ffff;
//	ESP_LOGI(tag, "Returning ChipID %d", tmp);
//	*chipid = tmp;
//
//	return ret;
//
//}

int get_chipid(int devid, unsigned *chipid) {
	int ret = 0;
	TickType_t timeout = 30 / portTICK_PERIOD_MS;


	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid, .addr = REG_CHIPIDH,
				.len = 2 };


	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0;

	unsigned high, low, c_id;

	clear_uart_rx_queue();

	//Run transaction verify that the response was received without error
	ret = transact_read(&msg_req, &msg_resp, timeout);
	// Why ret
	if (ret < 1) {
		ESP_LOGW(tag, "Transact Error: %d", ret);

	}
	if (msg_resp.len != 2) {
		ESP_LOGW(tag, "Chip ID Transact Error %d: %d", ret, msg_resp.len);

	}
	high = (msg_resp.payload[0] & 0xffff) << 16;
	low = (msg_resp.payload[1] & 0xffff);
	c_id = high + low;
	ESP_LOGI(tag, "Returning ChipID High 0x%04x Low 0x%04x  ChipID:%u", msg_resp.payload[0],msg_resp.payload[1], c_id);

	*chipid = c_id;

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
	msg_resp.payload[0] = 0xFFFF;

	unsigned tmp = 0;
	int num_cleared = clear_uart_rx_queue();

	if (num_cleared > 0) {
		ESP_LOGW(tag, "Cleared %d messages from the Queue", num_cleared);
	}

	ret = transact_read(&msg_req, &msg_resp, timeout);
	if (ret < 1) {
		ESP_LOGW(tag, "Transact Error: %d", ret);

	}
	tmp = (int16_t) msg_resp.payload[0];
	ESP_LOGI(tag, "Returning Raw Pressure  %d", (int16_t ) msg_resp.payload[0]);

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
	msg_resp.payload[0] = 0x00;

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

