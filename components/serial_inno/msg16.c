/*
 * msg16.c
 *
 *  Created on: Apr 19, 2023
 *      Author: ekalan
 */
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
//#include "stdlib.h"

//#include <unistd.h>
#include "esp_err.h"
#include "msg16.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"

static const char* tag = "msg16";

#define FLAG 0x07e
#define ESC 0x07d
#define CHK_LEN 2
#define GET_PAYLOAD_LEN(frame_len) (frame_len-5)
#define GET_FRAME_LEN(payload_len) (payload_len+5)

unsigned calc_chk(uint8_t * mem, unsigned len);

static size_t pack_data(const msg16_t* msg16, uint8_t* packed_msg);
static size_t pack_read_resp_data(const msg16_t* msg16, uint8_t* packed_msg);
static size_t pack_read_req_data(const msg16_t* msg16, uint8_t* packed_msg);
static size_t pack_write_req_data(const msg16_t* msg16, uint8_t* packed_msg);
static size_t pack_write_resp_data(const msg16_t* msg16, uint8_t* packed_msg);


unsigned calc_chk(uint8_t * mem, unsigned len) {
    unsigned chk = 0;
    uint8_t *p = mem;
    while (len--) {
        chk += *p++;
    }
    return (unsigned) chk;
}
void add_chk(void * buf, unsigned buf_len) {
    uint8_t * p;
    uint16_t chk;
    p = buf;
    chk = (uint16_t)calc_chk(p,buf_len);
    p[buf_len] = chk;
    p[buf_len+1] = chk>>8;
    // return the new length
    return;
}
#define READ_PAYLOAD_LEN 3
#define READ_RESP_PAYLOAD_LEN 2
#define READ_WRITE_LEN 3




static size_t pack_read_resp_data(const msg16_t* msg16, uint8_t* packed_msg){
	//Payload length
	size_t len_ = 0;
	packed_msg[len_++] = msg16->len *2;
	//Pack data
	for(int ii =0; ii < msg16->len;ii++){
	// Base register + the number of reads requested
	// Base register lsb first
		packed_msg[len_++] = (uint8_t) (msg16->msg_val[ii] & 0xff);
		packed_msg[len_++] = (uint8_t) ((msg16->msg_val[ii] >> 8) & 0xff);
		ESP_LOGI(tag, "Read Resp packing payload len:%d|Val:0x%02x|packed bytes:%d ", msg16->len *2, msg16->msg_val[ii], len_);
	}
	return len_;

}


static size_t pack_write_resp_data(const msg16_t* msg16, uint8_t* packed_msg){
	//Payload length
	size_t len_ = 0;
	// Pack device id
	packed_msg[len_++] = msg16->dev_id;
	// Pack 0 length
	packed_msg[len_++] = 0x00;
	return len_;
}


static size_t pack_write_req_data(const msg16_t* msg16, uint8_t* packed_msg){
	//Payload length
	size_t len_ = 0;
	packed_msg[len_++] = READ_PAYLOAD_LEN;
	//Pack data
	for(int ii =0; ii < msg16->len;ii++){
		// Base register + the number of reads requested
		// Base register lsb first
		packed_msg[len_++] = (uint8_t) (msg16->addr & 0xff);
		packed_msg[len_++] = (uint8_t) ((msg16->addr >> 8) & 0xff);
	}
	// length
	packed_msg[len_++] = (uint8_t) msg16->len;
	return len_;

}

static size_t pack_read_req_data(const msg16_t* msg16, uint8_t* packed_msg){
	//Payload length
	size_t len_ = 0;
	packed_msg[len_++] = READ_PAYLOAD_LEN;
	//Pack data
	// Base register + the number of reads requested
	// Base register lsb first
	packed_msg[len_++] = (uint8_t) (msg16->addr & 0xff);
	packed_msg[len_++] = (uint8_t) ((msg16->addr >> 8) & 0xff);
	// length
	packed_msg[len_++] = (uint8_t) msg16->len;
	return len_;

}
static size_t pack_data(const msg16_t* msg16, uint8_t* packed_msg){
	switch (msg16->type){
	case READ_REQ:
		ESP_LOGI(tag, "packing msg type: READ_REQ");
		return pack_read_req_data( msg16,  packed_msg);
	case WRITE_REQ:
		ESP_LOGI(tag, "packing msg type: WRITE_REQ");
		return pack_write_req_data( msg16,  packed_msg);
	case READ_RESP:
		ESP_LOGI(tag, "packing msg type: READ_RESP");
		return pack_read_resp_data( msg16,  packed_msg);
	case WRITE_RESP:
		ESP_LOGI(tag, "packing msg type: READ_REQ");
		return pack_write_resp_data( msg16,  packed_msg);
	default:
		ESP_LOGW(tag, "Error packing data msg type: %d", msg16->type);
	}
	return 0;
}

size_t pack_msg16(const msg16_t* msg16, uint8_t* packed_msg, size_t *packed_msg_size) {
    // pack the msg16 struct into bytes with escape characters
    size_t i = 0;

    // start byte
    packed_msg[i++] = 0x7e;
    // device ID
    packed_msg[i++] = (uint8_t) (msg16->dev_id & 0xff);
	ESP_LOGI(tag, "%d. Dev_id: %02x",i,  msg16->dev_id);
    // type
//    packed_msg[i++] = (uint8_t) ((msg16->type >> 8) & 0xff);
    packed_msg[i++] = (uint8_t) (msg16->type & 0xff);
	ESP_LOGI(tag, "%d. Type: %02x",i,  msg16->type);


    size_t len_ = pack_data(msg16, &packed_msg[i]);

    if (len_ == 0){
    	ESP_LOGW(tag, "Packed data returned 0; i:%d return len:%d",i,  len_);

//    	return 0;
    }
    i += len_;
	ESP_LOGI(tag, "Packed %d bytes new index:%d", len_, i);


    unsigned c_sum = calc_chk(&packed_msg[1], i);
    // Checksum lsb first
	ESP_LOGI(tag, "%d. Checksum: %02x",i,  c_sum);

    packed_msg[i++] = (uint8_t) (c_sum & 0xff);
    packed_msg[i++] = (uint8_t) ((c_sum >> 8) & 0xff);
    // end byte
    packed_msg[i++] = 0x7e;
    *packed_msg_size = i;
    return i;
}

//void send_msg16(msg16_t* msg16, char* response, size_t response_size) {
//    // pack the msg16 struct into bytes with escape characters
//    uint8_t packed_msg[128];
//    size_t packed_size, msg_len;
//    msg_len = pack_msg16(msg16, packed_msg, &packed_size);
//    if (packed_size != msg_len){
//        	// Need to add error to queue
//    	return;
//     }
//
//    // send the packed message and receive the response
//    // ...
//
//    // for this example, we just copy the packed message to the response
//    snprintf(response, response_size, "write|%d|%d|%d|%d", msg16->dev_id, msg16->addr, msg16->len, msg16->msg_val[0]);
//}



static size_t unpack_data(msg16_t* msg16, uint8_t* packed_msg){
	switch (msg16->type){
	case READ_REQ:
		ESP_LOGI(tag, "packing msg type: READ_REQ");
		return pack_read_req_data( msg16,  packed_msg);
	case WRITE_REQ:
		ESP_LOGI(tag, "packing msg type: WRITE_REQ");
		return pack_write_req_data( msg16,  packed_msg);
	case READ_RESP:
		ESP_LOGI(tag, "packing msg type: READ_RESP");
		return pack_read_resp_data( msg16,  packed_msg);
	case WRITE_RESP:
		ESP_LOGI(tag, "packing msg type: READ_REQ");
		return pack_write_resp_data( msg16,  packed_msg);
	default:
		ESP_LOGW(tag, "Error packing data msg type: %d", msg16->type);
	}
	return 0;
}

/*
 * Unpacks msg_16
 * Param:
 *  - Pointer to message buffer
 *   - length of the buffer
 *   - Pointer to msg16 struct to fill
 *  @Return
 *   - number of bytes packed or zero if message fails
 */
size_t unpack_msg16(uint8_t* packed_msg, size_t packed_msg_size, msg16_t* msg16) {
    // unpack the packed message into a msg16 struct
    size_t i = 0;
    for (int j =0; j<packed_msg_size;j++){
        	ESP_LOGI(tag, "%d. Val: %02x", j, packed_msg[j]);
    	}

    // start byte
    if (packed_msg[i] != 0x7e) {
        // error
    	ESP_LOGI(tag, "%d. Illegal start byte : 0X%02x",i, packed_msg[i]);

        return 0;
    }
    i++;


    // device ID
    msg16->dev_id = (uint16_t) packed_msg[i++];
	ESP_LOGI(tag, "Unpacked dev_id| %d", msg16->dev_id);

    // type
    msg16->type = ((uint16_t) packed_msg[i++] ) ;
	ESP_LOGI(tag, "Unpacked type| %d", msg16->type);


    // length
	msg16->len = (uint16_t)packed_msg[i++]/2;

	ESP_LOGI(tag, "Unpacked len| %d", msg16->len);

	for (int j =0; j<msg16->len;j++){
		msg16->msg_val[j] =  (uint16_t) packed_msg[i++] & 0xffff;
		msg16->msg_val[j] |= packed_msg[i++] << 8;
    	ESP_LOGI(tag, "Unpacked Val: %d, %d", msg16->msg_val[j], (int16_t)msg16->msg_val[j]);

	}
	uint16_t cal_sum = (uint16_t) calc_chk(&packed_msg[1], i-1);
	uint16_t ret_sum = (uint16_t) (packed_msg[i] | packed_msg[i+1] << 8);
	i +=2;
	ESP_LOGI(tag, "Unpacked CHK: X%02x == X%02x", cal_sum, ret_sum);

	if (cal_sum != ret_sum){
		return 0;
	}
	return i;


}







