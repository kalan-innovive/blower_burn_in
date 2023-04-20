/*
 * msg16.c
 *
 *  Created on: Apr 19, 2023
 *      Author: ekalan
 */
#include <stdio.h>
#include <stdbool.h>
//#include <unistd.h>

#include "msg16.h"



static int get_next_word(uint8_t* packed_msg, size_t len, uint8_t *low, uint8_t *high){
	uint16_t wrd = 0;
	if (len < 2) {
		// need at least 2 byte to make word
		return 0;
	}
	while(len > 1){
		*low = (uint8_t) ((*packed_msg >> 8) & 0xff);
		*high = (uint8_t) ((*packed_msg >> 8) & 0xff);
		if(*low == 0xF) {
			return 1;
		}


	}
	return 1;
}
void pack_msg16(msg16_t* msg16, uint8_t* packed_msg, size_t packed_msg_size) {
    // pack the msg16 struct into bytes with escape characters
    size_t i = 0;

    // start byte
    packed_msg[i++] = 0x7e;

    // type
    packed_msg[i++] = (uint8_t) ((msg16->type >> 8) & 0xff);
    packed_msg[i++] = (uint8_t) (msg16->type & 0xff);

    // device ID
    packed_msg[i++] = (uint8_t) ((msg16->dev_id >> 8) & 0xff);
    packed_msg[i++] = (uint8_t) (msg16->dev_id & 0xff);

    // address
    packed_msg[i++] = (uint8_t) ((msg16->addr >> 8) & 0xff);
    packed_msg[i++] = (uint8_t) (msg16->addr & 0xff);

    // length
    packed_msg[i++] = (uint8_t) ((msg16->len >> 8) & 0xff);
    packed_msg[i++] = (uint8_t) (msg16->len & 0xff);

    // message values
    for (int j = 0; j < msg16->len; j++) {
        uint16_t msg_val = msg16->msg_val[j];

        // check for escape characters
        if (msg_val == 0x7e || msg_val == 0xef) {
            packed_msg[i++] = 0xef;
            packed_msg[i++] = (uint8_t) ((msg_val >> 8) & 0xff) ^ 0x20;
            packed_msg[i++] = (uint8_t) (msg_val & 0xff) ^ 0x20;
        } else {
            packed_msg[i++] = (uint8_t) ((msg_val >> 8) & 0xff);
            packed_msg[i++] = (uint8_t) (msg_val & 0xff);
        }
    }

    // end byte
    packed_msg[i++] = 0x7e;
}

void send_msg16(msg16_t* msg16, char* response, size_t response_size) {
    // pack the msg16 struct into bytes with escape characters
    uint8_t packed_msg[128];
    pack_msg16(msg16, packed_msg, sizeof(packed_msg));

    // send the packed message and receive the response
    // ...

    // for this example, we just copy the packed message to the response
    snprintf(response, response_size, "write|%d|%d|%d|%d", msg16->dev_id, msg16->addr, msg16->len, msg16->msg_val[0]);
}



void unpack_msg16(const uint8_t* packed_msg, size_t packed_msg_size, msg16_t* msg16) {
    // unpack the packed message into a msg16 struct
    size_t i = 0;

    // start byte
    if (packed_msg[i++] != 0x7e) {
        // error
        return;
    }

    // type
    msg16->type = ((uint16_t) packed_msg[i++] << 8) | (uint16_t) packed_msg[i++];

    // device ID
    msg16->dev_id = ((uint16_t) packed_msg[i++] << 8) | (uint16_t) packed_msg[i++];

    // address
    msg16->addr = ((uint16_t) packed_msg[i++] << 8) | (uint16_t) packed_msg[i++];

    // length
    msg16->len = ((uint16_t) packed_msg[i++] << 8) | (uint16_t) packed_msg[i++];

    // message values
    msg16->msg_val = malloc(msg16->len * sizeof(uint16_t));
    if (msg16->msg_val == NULL) {
        // error
        return;
    }

//    for (int j = 0; j < msg16->len
