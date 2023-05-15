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
#include "esp_log.h"

#define UART_BUFFER_SIZE 24
#define UART_RX_QUEUE_SIZE 10
#define BUF_SIZE (1024)
QueueHandle_t uart_rx_queue;
QueueHandle_t rack_queue;

static const char* tag = "serial_inno";

//
//void uart_rx_task(void *pvParameters)
//{
//    uart_event_t event;
//    size_t buffered_size;
//    uint8_t* dtmp = (uint8_t*) malloc(BUF_SIZE);
//
//    uint8_t uart_buffer[UART_BUFFER_SIZE]={0};
//    uint8_t msg_buffer[128] = {0};
//    size_t uart_pos = 0;
//    size_t msg_pos = 0;
//    for(;;) {
//        //Waiting for UART event.
//        if(xQueueReceive(UART_SERIAL_INNO, (void * )&event, (TickType_t)portMAX_DELAY)) {
//            bzero(dtmp, BUF_SIZE);
//            ESP_LOGI(tag, "uart[%d] event:", UART_SERIAL_INNO);
//            switch(event.type) {
//                //Event of UART receving data
//                /*We'd better handler data event fast, there would be much more data events than
//                other types of events. If we take too much time on data event, the queue might
//                be full.*/
//                case UART_DATA:
//                    ESP_LOGI(tag, "[UART DATA]: %d", event.size);
//                    uart_read_bytes(UART_SERIAL_INNO, dtmp, event.size, portMAX_DELAY);
//                    ESP_LOGI(tag, "[DATA EVT]:");
//                    uart_write_bytes(UART_SERIAL_INNO, (const char*) dtmp, event.size);
//                    break;
//                //Event of HW FIFO overflow detected
//                case UART_FIFO_OVF:
//                    ESP_LOGI(tag, "hw fifo overflow");
//                    // If fifo overflow happened, you should consider adding flow control for your application.
//                    // The ISR has already reset the rx FIFO,
//                    // As an example, we directly flush the rx buffer here in order to read more data.
//                    uart_flush_input(UART_SERIAL_INNO);
//                    xQueueReset(uart_rx_queue);
//                    break;
//                //Event of UART ring buffer full
//                case UART_BUFFER_FULL:
//                    ESP_LOGI(tag, "ring buffer full");
//                    // If buffer full happened, you should consider increasing your buffer size
//                    // As an example, we directly flush the rx buffer here in order to read more data.
//                    uart_flush_input(UART_SERIAL_INNO);
//                    xQueueReset(UART_SERIAL_INNO);
//                    break;
//                //Event of UART RX break detected
//                case UART_BREAK:
//                    ESP_LOGI(tag, "uart rx break");
//                    break;
//                //Event of UART parity check error
//                case UART_PARITY_ERR:
//                    ESP_LOGI(tag, "uart parity error");
//                    break;
//                //Event of UART frame error
//                case UART_FRAME_ERR:
//                    ESP_LOGI(tag, "uart frame error");
//                    break;
//                //UART_PATTERN_DET
//                case UART_PATTERN_DET:
//                    uart_get_buffered_data_len(UART_SERIAL_INNO, &buffered_size);
//                    int pos = uart_pattern_pop_pos(UART_SERIAL_INNO);
//                    ESP_LOGI(tag, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
////                    if (pos == -1) {
////                        // There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
////                        // record the position. We should set a larger queue size.
////                        // As an example, we directly flush the rx buffer here.
////                        uart_flush_input(UART_SERIAL_INNO);
////                    } else {
////                        uart_read_bytes(UART_SERIAL_INNO, dtmp, pos, 100 / portTICK_PERIOD_MS);
////                        uint8_t pat[PATTERN_CHR_NUM + 1];
////                        memset(pat, 0, sizeof(pat));
////                        uart_read_bytes(EX_UART_NUM, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
////                        ESP_LOGI(TAG, "read data: %s", dtmp);
////                        ESP_LOGI(TAG, "read pat : %s", pat);
////                    }
//                    break;
//                //Others
//                default:
//                    ESP_LOGI(TAG, "uart event type: %d", event.type);
//                    break;
//            }
//        }
//    }
//    free(dtmp);
//    dtmp = NULL;
//    vTaskDelete(NULL);
//}

void uart_rx_task(void *pvParameters) {
    uint8_t uart_buffer[UART_BUFFER_SIZE]={0};
    uint8_t msg_buffer[128] = {0};
    size_t uart_pos = 0;
    size_t msg_pos = 0;

    while (true) {
        // read from UART buffer
//		vTaskDelay(1000 / portTICK_PERIOD_MS);

        int read_len = uart_read_bytes(UART_SERIAL_INNO, uart_buffer + uart_pos, UART_BUFFER_SIZE - uart_pos, 10 / portTICK_PERIOD_MS);
        if (read_len <= 0) {

            continue;
        }
        ESP_LOGI(tag, "Read length: :%d", read_len);
    	int pos = 0;
    	char buf[40]= {'\0'};
        for (int i = 0; i<UART_BUFFER_SIZE;i++){
        		pos += sprintf(&buf[pos], "%02x:",uart_buffer[i] );
		}
        ESP_LOGI(tag,  "Uart_pos:%d|msg_pos:%d|read_len:%d|",uart_pos, msg_pos, read_len);
		ESP_LOGI(tag, "RESP BUF: 0X%s", buf);
//		vTaskDelay(1000 / portTICK_PERIOD_MS);


        // process buffer
        for (size_t i = 0; i < read_len; i++) {
            uint8_t byte = uart_buffer[uart_pos + i];
    		ESP_LOGI(tag, "%d, Byte:%02x", i, byte);


            if (msg_pos == 0 && byte != 0x7e) {
                // ignore bytes before start flag
        		ESP_LOGI(tag, "Ignoring Byte");
                continue;
            }
            if (msg_pos == 0 && byte == 0x7e) {
				// Found start byte
				ESP_LOGI(tag, "Start Flag");
				msg_buffer[msg_pos++] = byte;
				continue;
			}

            if (byte == 0x7e) {
        		ESP_LOGI(tag, "End Flag found, creating msg16_t, reseting ");

                // end of message, decode and add to queue
                msg_buffer[msg_pos] = 0x00;
                msg16_t* msg = malloc(sizeof(msg16_t));
                if (msg != NULL) {
                    size_t msg_len = unpack_msg16(msg_buffer, msg_pos, msg);
                    if (msg_len == 0){
                		ESP_LOGW(tag, "Discarding msg unable to unpack");
                    }

//                    xQueueSend(uart_rx_queue, ( void * ) &msg, portMAX_DELAY);
                    if (msg->dev_id == 0x11 && msg->type == READ_REQ) {
						xQueueSend(rack_queue, &msg, portMAX_DELAY);
					} else {
						xQueueSend(uart_rx_queue, &msg, portMAX_DELAY);
					}
                }
                msg_pos = 0;
            } else if (byte == 0xef && msg_pos > 0 && msg_buffer[msg_pos - 1] == 0xef) {
        		ESP_LOGI(tag, "Received escape Flag, Byte: 0X%02x | 0X%02x", byte, byte+1);
                // escape character
                msg_buffer[msg_pos - 1] = byte ^ 0x20;
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
            memmove(uart_buffer, uart_buffer + uart_pos, UART_BUFFER_SIZE - uart_pos);
        }
    }
}

int uart_tx_task(uint8_t* buf, size_t len) {
	// send the buffer check that it was sent
	int pos = 0;
	char buf_s[40]= {'\0'};
	// Print out the message and compare the values
	for (int i = 0; i<len;i++){
		pos += sprintf(&buf_s[pos], "%02x:",buf[i] );
	}
	ESP_LOGI(tag, "Sending data:%s length %d| Bytes written",buf_s, len);

	int num_byt_written = uart_write_bytes(UART_SERIAL_INNO, (const char*)buf, len);
    esp_err_t err = uart_wait_tx_done(UART_SERIAL_INNO, TRANSMIT_WAIT_TIME);

    return (err == ESP_OK) ? num_byt_written : err;

}



static int set_;

void setup_driver() {
	if (set_) {
		// TODO change to return espok
		return;
	}
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    #if CONFIG_UART_ISR_IN_IRAM
        intr_alloc_flags = ESP_INTR_FLAG_IRAM;
    #endif
    uart_driver_install(UART_SERIAL_INNO, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags);
    uart_param_config(UART_SERIAL_INNO, &uart_config);
    uart_set_pin(UART_SERIAL_INNO,43, 44,  UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
//    uart_set_pin(UART_NUM_1, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

//    uart_driver_install(UART_NUM_0, UART_BUFFER_SIZE,UART_BUFFER_SIZE,0,NULL,0);

    // Setup the queue
    uart_rx_queue = xQueueCreate(5, sizeof(msg16_t*));
    rack_queue = xQueueCreate(5, sizeof(msg16_t*));


    set_ = 1;

}



int transact_read(const msg16_t* request, msg16_t* response, TickType_t timeout) {
    // pack the request message into bytes
    uint8_t packed_request[128]= {0};
    size_t len, packed_size = 0;
    int num_bytes = 0;
    int ret = 0;
    packed_size = pack_msg16((const msg16_t*) request, packed_request, &len);

    /* Todo: check if the queeue is empty first /*/

    // send the packed message
    num_bytes = uart_tx_task(packed_request, packed_size);

	ESP_LOGI(tag, "Bytes written-%d",num_bytes);


    // wait for response
    msg16_t* received_msg;

    while (xQueueReceive(uart_rx_queue, &received_msg, timeout) == pdTRUE) {
    	ret++;


		// validate response
		if (received_msg->type != (READ_RESP) || received_msg->dev_id != request->dev_id) {
			// invalid response
//			free(received_msg);
			ret =  -1;
		} else {

			// fill out response struct
			response->type = received_msg->type;
			response->dev_id = received_msg->dev_id;
			response->addr = received_msg->addr;
			response->len = received_msg->len;
			for (int i=0; i<response->len; i++){
				response->msg_val[i]= received_msg->msg_val[i];
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
    free(received_msg);

    return ret;
}

int get_chipid(int devid, unsigned *chipid) {
	int ret = 0;
	msg16_t msg_req = {
			.type = READ_REQ,
			.dev_id = devid ,
			.addr = REG_CHIPIDH,
			.len = 1
		};
	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.msg_val[0] = 0;

	unsigned tmp = 0;
	clear_uart_rx_queue();
	ret = transact_read(&msg_req,&msg_resp, 50);
	if (ret<1){
		ESP_LOGW(tag, "Transact Error: %d", ret);

	}
	tmp = (msg_resp.msg_val[0] & 0xffff) << 16;
	ESP_LOGI(tag, "Returning ChipID High 0x%04x", msg_resp.msg_val[0]);

	clear_uart_rx_queue();


	msg_req.addr = REG_CHIPIDL;

	ret = transact_read(&msg_req,&msg_resp, 30);
	if (ret<1){
		ESP_LOGW(tag, "Transact Error: %d", ret);

	}
	ESP_LOGI(tag, "Returning ChipID Low 0x%04x", msg_resp.msg_val[0]);

	tmp += (msg_resp.msg_val[0]) & 0x0000ffff;
	ESP_LOGI(tag, "Returning ChipID %d", tmp);
	*chipid = tmp;

	return ret;



}


int get_raw_pressure(int devid, int *raw_pressure) {
	int ret = 0;
	msg16_t msg_req = {
			.type = READ_REQ,
			.dev_id = devid ,
			.addr = REG_RAW_PRESS,
			.len = 1
		};

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.msg_val[0] = 0xFFFF;

	unsigned tmp = 0;
	int num_cleared = clear_uart_rx_queue();

	if (num_cleared > 0) {
		ESP_LOGW(tag, "Cleared %d messages from the Queue", num_cleared);
	}
	ret = transact_read(&msg_req,&msg_resp, 500);
	if (ret<1){
		ESP_LOGW(tag, "Transact Error: %d", ret);

	}
	tmp = (int16_t) msg_resp.msg_val[0] ;
	ESP_LOGI(tag, "Returning Raw Pressure  %d", (int16_t) msg_resp.msg_val[0]);

	*raw_pressure = tmp;

	return ret;
}


int clear_uart_rx_queue() {
    msg16_t* msg;
    int num_cleared = 0;
    while (xQueueReceive(uart_rx_queue, &msg, 0) == pdTRUE) {

    	++num_cleared;
        free(msg);
    }
    return num_cleared;
}



