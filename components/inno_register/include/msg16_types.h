//
// Created by Erik Klan on 1/31/24.
//

#ifndef UNIT_TESTS_MSG16_TYPES_H
#define UNIT_TESTS_MSG16_TYPES_H
/*
 * msg16.h
 *
 *  Created on: Apr 19, 2023
 *      Author: ekalan
 */

#ifndef COMPONENTS_SERIAL_INNO_INCLUDE_MSG16_H_
#define COMPONENTS_SERIAL_INNO_INCLUDE_MSG16_H_

#if __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>


#define READ_REQ 0x20
#define WRITE_REQ 0x21
#define READ_RESP 0xa0
#define WRITE_RESP 0xa1

#define REG_HWTYPE_RETURN 0xE107
#define NUM_NV_STORE 16
#define RECORD_VERSION 3

#define FLAG 0x07e
#define ESC 0x07d
#define CHK_LEN 2
#define READ_PAYLOAD_LEN 3
#define READ_RESP_PAYLOAD_LEN 2
#define READ_WRITE_LEN 3
#define GET_PAYLOAD_LEN(frame_len) (frame_len-5)
#define GET_FRAME_LEN(payload_len) (payload_len+5)
#define CALIBRATE_CMD 0x5678
#define RESET_PWM_CMD 0xffff


// pre-release gets 0xFF for version
// last released version was 8
#define SW_VERSION 8
#define HW_VERSION 107  // FIXME - should have a board rev included here
#define MSG_16_PAYLOAD_LEN 64

typedef struct {
    /* Type:READ_REQ, READ_RESP, WRITE_REQ, WRITE_REP*/
    uint16_t type;
    /* Type: one of dev_id values*/
    uint16_t dev_id;
    /* Address of register considered the base address for multi read and write*/
    uint16_t addr;
    /* Length:
     * READ_REQ= Number of consecutive registers requested stored in payload
     * READ_RESP: Number of registers in response starting at base address
     * WRITE_REQ : Number consecutive registers to write stored in payload
     * WRITE_RESP: Number of consecutive registers in payload
     */
    uint16_t len;
    /* Array holder of values set to Null if not using */
    uint16_t payload[MSG_16_PAYLOAD_LEN];
} __attribute__((packed)) msg16_t;

msg16_t *create_msg16(uint16_t type, uint16_t dev_id, uint16_t addr, uint16_t len, uint16_t *payload);

int destruct_msg16(msg16_t **msg);

#if __cplusplus
}
#endif

#endif /* COMPONENTS_SERIAL_INNO_INCLUDE_MSG16_H_ */

#endif //UNIT_TESTS_MSG16_TYPES_H
