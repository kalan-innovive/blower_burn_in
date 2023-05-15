/*
 * rack_device.c
 *
 *  Created on: May 9, 2023
 *      Author: ekalan
 */
#include "msg16.h"
#include "rack_device.h"
#include "serial_inno.h"

#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"

extern QueueHandle_t rack_queue;
rack_registers_t rack_regs;

void handle_rack_req(msg16_t *request, msg16_t *response){
	response->type = request->type;
	response->dev_id = request->dev_id;
	response->addr = request->addr;
	response->len = request->len;
	uint16_t base = request->addr;
	for (int i=0; i<request->len; i++){
		switch (base) {
			case INFO_REVISION:
				response->msg_val[i] = rack_regs.revision;
				break;
			case INFO_RACKTYPE:
				response->msg_val[i] = rack_regs.rack_type;
				break;
			case INFO_ROWS:
				response->msg_val[i] = rack_regs.rows;
				break;
			case INFO_COLUMNS:
				response->msg_val[i] = rack_regs.columns;
				break;
			case INFO_SIDES:
				response->msg_val[i] = rack_regs.sides;
				break;
			case INFO_CHECKSUM:
				response->msg_val[i] = rack_regs.checksum;
				break;
			case INFO_RACKREV:
				response->msg_val[i] = rack_regs.rack_rev;
				break;
			case REG_CHIPIDH:
				response->msg_val[i] = rack_regs.chip_idh;
				break;
			case REG_CHIPIDL:
				response->msg_val[i] = rack_regs.chip_idl;
				break;
			case REG_MODBUS_ADDR:
				response->msg_val[i] = rack_regs.modbus_addr;
				break;
			default:
				// invalid address, send error response
				response->msg_val[0] = 0;
				response->len = 0;
				break;
		}
	}

}

void setup_rack_dev_default(){
	rack_regs.revision = 1;
	rack_regs.rack_type = 0;
	rack_regs.rows = 8;
	rack_regs.columns = 3;
	rack_regs.sides = 2;
	rack_regs.checksum = 0x6543;
	rack_regs.rack_rev = 1;
	rack_regs.chip_idh = 0x678;
	rack_regs.chip_idl = 0x123;
	rack_regs.modbus_addr = 0x11;
}


void set_rack_dev_sides(uint16_t sides){
	rack_regs.sides = sides;

}

void set_rack_dev_type(uint16_t r_type){
	rack_regs.rack_type = r_type;

}
void rack_task(void *pvParameters) {




    while (1) {
        // wait for request in rack queue
        msg16_t* request;
        if (xQueueReceive(rack_queue, &request, portMAX_DELAY) == pdFALSE) {
            continue;
        }

        // process request
        msg16_t response;


        // pack and send response
        uint8_t packed_response[128];
        size_t msg_len, packed_size = 0;
        packed_size = pack_msg16(&response, packed_response, &msg_len);
//        uart_write_bytes(UART_NUM_0, (const char*) packed_response, strlen((const char*) packed_response));
        uart_tx_task(packed_response, packed_size);

        // free request
        free(request);
    }
}


