#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "include/msg16.h"

#define UART_BUFFER_SIZE 1024
#define UART_RX_QUEUE_SIZE 10

QueueHandle_t uart_rx_queue;

void uart_rx_task(void *pvParameters) {
    uint8_t uart_buffer[UART_BUFFER_SIZE];
    uint8_t msg_buffer[128];
    size_t uart_pos = 0;
    size_t msg_pos = 0;

    while (true) {
        // read from UART buffer
        size_t read_len = uart_read_bytes(UART_NUM_0, uart_buffer + uart_pos, UART_BUFFER_SIZE - uart_pos, pdMS_TO_TICKS(100));
        if (read_len <= 0) {
            continue;
        }

        // process buffer
        for (size_t i = 0; i < read_len; i++) {
            uint8_t byte = uart_buffer[uart_pos + i];

            if (msg_pos == 0 && byte != 0x7e) {
                // ignore bytes before start flag
                continue;
            }

            if (byte == 0x7e) {
                // end of message, decode and add to queue
                msg_buffer[msg_pos] = 0x00;
                msg16_t* msg = malloc(sizeof(msg16_t));
                if (msg != NULL) {
                    unpack_msg16(msg_buffer, msg_pos, msg);
                    xQueueSend(uart_rx_queue, &msg, portMAX_DELAY);
                }
                msg_pos = 0;
            } else if (byte == 0xef && msg_pos > 0 && msg_buffer[msg_pos - 1] == 0xef) {
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

void setup_driver() {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
//    uart_driver_install(UART_NUM_0, UART_BUFFER_SIZE * 2,-1,-1);
}
