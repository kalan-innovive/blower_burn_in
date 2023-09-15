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

static const char *tag = "msg16";



static unsigned calc_chk(uint8_t *mem, unsigned len);
static unsigned  add_chk(void *buf, unsigned buf_len);
static unsigned valid_frame(uint8_t * buf, unsigned buf_len);



unsigned calc_chk(uint8_t *mem, unsigned len) {
	unsigned chk = 0;
	uint8_t *p = mem;
	while (len--) {
		ESP_LOGI(tag, "Checksum len=%d, val=%x, adding val=%x  ", len, chk, *p);
		chk += *p++;
	}
	return (unsigned) chk;
}
static unsigned  add_chk(void *buf, unsigned buf_len) {
	uint8_t *p;
	uint16_t chk;
	p = buf;
	chk = (uint16_t) calc_chk(p, buf_len);
	p[buf_len] = chk;
	p[buf_len + 1] = chk >> 8;
	// return the new length
	return 2;
}

static unsigned valid_frame(uint8_t * buf, unsigned buf_len) {
    uint16_t chk, rx_chk;
    // first check for minimum frame length
    // then verify the destination ID
    // then check the payload length is good
    // then verify the frame checksum
    if (buf_len<GET_FRAME_LEN(0)) {
        return 0;
    }

    if (GET_PAYLOAD_LEN(buf_len)!=buf[2]) {
        return 0;
    }
    chk = calc_chk(buf,buf_len-2);
    rx_chk = buf[buf_len-2] | (buf[buf_len-1]<<8);
    if (rx_chk != chk) {
        return 0;
    }
    return 1;
}

/**
 * *********************************************************************
 *
 * Unpacking msg_16 struct Functions
 */
static size_t pack_read_req_payload(const msg16_t *msg16, uint8_t *packed_msg);
static size_t pack_read_resp_payload(const msg16_t *msg16, uint8_t *packed_msg);
static size_t pack_write_req_payload(const msg16_t *msg16, uint8_t *packed_msg);
static size_t pack_write_resp_payload(const msg16_t *msg16, uint8_t *packed_msg);
static size_t pack_payload(const msg16_t *msg16, uint8_t *packed_msg);



static size_t pack_read_req_payload(const msg16_t *msg16, uint8_t *packed_msg) {
	//Payload length
	size_t len_ = 0;
	packed_msg[0] = READ_PAYLOAD_LEN;
	len_++;
	//Pack data
	// Base register + the number of reads requested
	// Base register lsb first
	packed_msg[1] = (uint8_t) (msg16->addr & 0xff);
	packed_msg[2] = (uint8_t) ((msg16->addr >> 8) & 0xff);
	len_ += 2;

	// length
	packed_msg[3] = (uint8_t) msg16->len;
	len_++;

	ESP_LOGI(tag, "Payload: type=%x addr=%x%x, len=%x", packed_msg[0],
			packed_msg[1], packed_msg[2], packed_msg[3]);
	return len_;

}

static size_t pack_read_resp_payload(const msg16_t *msg16, uint8_t *packed_msg) {
	//Payload length
	size_t len_ = 0;
	packed_msg[len_++] = msg16->len * 2;
	//Pack data
	for (int ii = 0; ii < msg16->len; ii++) {
		// Base register + the number of reads requested
		// Base register lsb first
		packed_msg[len_++] = (uint8_t) (msg16->payload[ii] & 0xff);
		packed_msg[len_++] = (uint8_t) ((msg16->payload[ii] >> 8) & 0xff);
		ESP_LOGI(tag,
				"Read Resp packing payload len:%d|Val:0x%02x|packed bytes:%d ",
				msg16->len * 2, msg16->payload[ii], len_);
	}
	return len_;

}

static size_t pack_write_resp_payload(const msg16_t *msg16, uint8_t *packed_msg) {
	//Payload length
	size_t len_ = 0;
	// Pack device id
	packed_msg[len_++] = msg16->dev_id;
	// Pack 0 length
	packed_msg[len_++] = 0x00;
	return len_;
}

static size_t pack_write_req_payload(const msg16_t *msg16, uint8_t *packed_msg) {
	//Payload length
	size_t len_ = 0;
	packed_msg[len_++] = READ_PAYLOAD_LEN;
	//Pack data
	for (int ii = 0; ii < msg16->len; ii++) {
		// Base register + the number of write requested
		// Base register lsb first
		packed_msg[len_++] = (uint8_t) (msg16->addr & 0xff);
		packed_msg[len_++] = (uint8_t) ((msg16->addr >> 8) & 0xff);
	}
	// length
	packed_msg[len_++] = (uint8_t) msg16->len;
	return len_;

}


static size_t pack_payload(const msg16_t *msg16, uint8_t *packed_msg) {
	switch (msg16->type) {
		case READ_REQ:
			ESP_LOGI(tag, "packing msg type: READ_REQ");
			return pack_read_req_payload(msg16, packed_msg);
		case WRITE_REQ:
			ESP_LOGI(tag, "packing msg type: WRITE_REQ");
			return pack_write_req_payload(msg16, packed_msg);
		case READ_RESP:
			ESP_LOGI(tag, "packing msg type: READ_RESP");
			return pack_read_resp_payload(msg16, packed_msg);
		case WRITE_RESP:
			ESP_LOGI(tag, "packing msg type: READ_REQ");
			return pack_write_resp_payload(msg16, packed_msg);
		default:
			ESP_LOGW(tag, "Error packing data msg type: %d", msg16->type);
	}
	return 0;
}

/*
 * Pack buffer from msg_16 struct
 * Param:
 *  - Pointer to message buffer
 *   - length of the buffer
 *   - Pointer to msg16 with completed msg
 *  @Return
 *   - number of bytes packed or zero if message fails
 */
size_t pack_msg16(const msg16_t *msg16, uint8_t *packed_msg,
		size_t *packed_msg_size) {

	// pack the msg16 struct into bytes with escape characters
	size_t i = 0;
	if (packed_msg==NULL || msg16==NULL) {
		ESP_LOGW(tag, "Passed Null pointer");
		return 0;
	}

	// start byte
	packed_msg[i++] = 0x7e;
	// device ID
	packed_msg[i++] = (uint8_t) (msg16->dev_id & 0xff);
	ESP_LOGD(tag, "%d. Dev_id: %02x", i, msg16->dev_id);
	// type
//    packed_msg[i++] = (uint8_t) ((msg16->type >> 8) & 0xff);
	packed_msg[i++] = (uint8_t) (msg16->type & 0xff);
	ESP_LOGD(tag, "%d. Type: %02x", i, msg16->type);

	size_t len_ = pack_payload(msg16, &packed_msg[i]);

	if (len_ == 0) {
		ESP_LOGW(tag, "Packed data returned 0; i:%d return len:%d", i, len_);
    	return 0;
	}
	i += len_;
	ESP_LOGD(tag, "Packed %d bytes new index:%d", len_, i);
	i += add_chk(&packed_msg[1], len_+ READ_WRITE_LEN);
//	unsigned c_sum = calc_chk(&packed_msg[1], i - 1);
//	// Checksum lsb first
//	ESP_LOGD(tag, "%d. Checksum: %02x", i, c_sum);
//
//	packed_msg[i++] = (uint8_t) (c_sum & 0xff);
//	packed_msg[i++] = (uint8_t) ((c_sum >> 8) & 0xff);
	// end byte
	packed_msg[i++] = 0x7e;
	*packed_msg_size = i;
	return i;
}

/**
 * *********************************************************************
 * Unpacking msg_16 struct Function Prototypes
 */

static size_t unpack_read_req_payload( msg16_t *m, uint8_t *packed_msg);
static size_t unpack_read_resp_payload( msg16_t *m, uint8_t *packed_msg);
static size_t unpack_write_req_payload(msg16_t *m, uint8_t *packed_msg);
static size_t unpack_write_resp_payload( msg16_t *m, uint8_t *packed_msg);
static size_t unpack_payload(msg16_t *msg16, uint8_t *packed_msg);



static size_t unpack_read_req_payload( msg16_t *m, uint8_t *packed_msg) {
	if (packed_msg[3]==3) {
		m->addr = packed_msg[0];
		m->addr |= packed_msg[1]<<8;
		m->len = packed_msg[2];
		ESP_LOGD(tag, "Unpacked len= %d| addr=%d", m->len, m->addr);
		return 3;
	}
	return 0;
}


static size_t unpack_read_resp_payload( msg16_t *m, uint8_t *packed_msg) {
	int j=0;
	if (packed_msg[3]%2==0) {
		uint8_t *p = &packed_msg[4];
		m->len = (uint16_t) *p++ / 2;
		ESP_LOGD(tag, "Unpacked len| %d", m->len);
		for (j = 0; j < m->len; j++) {
			m->payload[j] = (uint16_t) *p++ & 0xffff;
			m->payload[j] |= *p++ << 8;
			ESP_LOGD(tag, "    Unpacked Val: u=%d, s=%d", (uint16_t)m->payload[j],
					(int16_t )m->payload[j]);
		}
	}
	return j;
}


static size_t unpack_write_req_payload(msg16_t *m, uint8_t *packed_msg) {
	if (packed_msg[3]==3) {
		m->addr = packed_msg[4];
		m->addr |= packed_msg[5]<<8;
		m->len = packed_msg[6];
		ESP_LOGD(tag, "Unpacked len= %d| addr=%d", m->len, m->addr);
		return 3;
	}
	return 0;
}


static size_t unpack_write_resp_payload( msg16_t *m, uint8_t *packed_msg) {
	int j=0;
	if (packed_msg[3]>=4) {
		uint16_t count = (uint16_t) (packed_msg[3] - 2);
		m->addr = packed_msg[4];
		m->addr |= packed_msg[5]<<8;
		if (count%2 !=0) {
			ESP_LOGD(tag, "Unpacked len must be >4 and even| %d", count);
			m->len = 0;
		} else {
			m->len = count/2;

			ESP_LOGD(tag, "Unpacked len=%d, addr=0X04%x", m->len, m->addr);
			uint8_t *p = &packed_msg[6];

			for (j = 0; j < m->len; j++) {
				m->payload[j] = (uint16_t) packed_msg[4] & 0xffff;
				m->payload[j] |= *p++ << 8;
				ESP_LOGD(tag, "    Unpacked Val: u=%d, s=%d", (uint16_t)m->payload[j],
						(int16_t )m->payload[j]);
			}
		}
	}
	return j;
}


static size_t unpack_payload(msg16_t *msg16, uint8_t *packed_msg) {
	switch (msg16->type) {
		case READ_REQ:
			ESP_LOGI(tag, "Unpacking: READ_REQ");
			return unpack_read_req_payload(msg16, packed_msg);
		case WRITE_REQ:
			ESP_LOGI(tag, "Unpacking: WRITE_REQ");
			return unpack_write_req_payload(msg16, packed_msg);
		case READ_RESP:
			ESP_LOGI(tag, "Unpacking: READ_RESP");
			return unpack_read_resp_payload(msg16, packed_msg);
		case WRITE_RESP:
			ESP_LOGI(tag, "Unpacking: READ_REQ");
			return unpack_write_resp_payload(msg16, packed_msg);
		default:
			ESP_LOGW(tag, "Error packing data msg type: %d", msg16->type);
	}
	return 0;
}

/*
 * Unpacks buffer into msg_16 struct
 * Param:
 *  - Pointer to message buffer
 *   - length of the buffer
 *   - Pointer to msg16 struct to fill
 *  @Return
 *   - number of bytes packed or zero if message fails
 */
size_t unpack_msg16(uint8_t *packed_msg, size_t packed_msg_size, msg16_t *msg16) {
	// unpack the packed message into a msg16 struct
	size_t i = 0;
	uint8_t *p = packed_msg;

	// TODO: Check the length of the buffer
	// Check if the buffer is NULL
	if (msg16==NULL) {
		ESP_LOGW(tag, "Passed Null buffer");
		return 0;
	} else if (!valid_frame(packed_msg, packed_msg_size)) {
		return 0;
	}

	// start byte
	if (*p != 0x7e) {
		// error
		ESP_LOGW(tag, "%d. Illegal start byte : 0X%02x", i, *p);

		return 0;
	}
	p++;

	// device ID
	msg16->dev_id = (uint16_t) *p++;
	ESP_LOGD(tag, "Unpacked dev_id| %d", msg16->dev_id);

	// type
	msg16->type = ((uint16_t) *p++);
	ESP_LOGD(tag, "Unpacked type| %d", msg16->type);

	// length
	msg16->len = (uint16_t) *p++ / 2;
	ESP_LOGD(tag, "Unpacked len| %d", msg16->len);

	i = unpack_payload(msg16, packed_msg);
	if (i==0){
		ESP_LOGI(tag, "Unpacking Error length == 0");
		return 0;
	}
//	uint16_t cal_sum = (uint16_t) calc_chk(&packed_msg[1], packed_msg_size - 1);
//	uint16_t ret_sum = (uint16_t) (packed_msg[i] | packed_msg[i + 1] << 8);
//	i += 2;
//	ESP_LOGD(tag, "Unpacked CHK: X%02x == X%02x", cal_sum, ret_sum);
//
//	if (cal_sum != ret_sum) {
//		return 0;
//	}
	return i;

}

