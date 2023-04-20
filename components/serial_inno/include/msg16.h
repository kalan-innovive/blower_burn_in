/*
 * msg16.h
 *
 *  Created on: Apr 19, 2023
 *      Author: ekalan
 */

#ifndef COMPONENTS_SERIAL_INNO_INCLUDE_MSG16_H_
#define COMPONENTS_SERIAL_INNO_INCLUDE_MSG16_H_
#include <stdint.h>


typedef struct {
    uint16_t type;
    uint16_t dev_id;
    uint16_t addr;
    uint16_t len;
    uint16_t* msg_val;
} msg16_t;

void pack_msg16(msg16_t* msg16, uint8_t* packed_msg, size_t packed_msg_size);
void send_msg16(msg16_t* msg16, char* response, size_t response_size);
void unpack_msg16(const uint8_t* packed_msg, size_t packed_msg_size, msg16_t* msg16);



#endif /* COMPONENTS_SERIAL_INNO_INCLUDE_MSG16_H_ */
