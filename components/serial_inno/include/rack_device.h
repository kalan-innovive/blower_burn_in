/*
 * rack_device.h
 *
 *  Created on: May 9, 2023
 *      Author: ekalan
 */

#ifndef COMPONENTS_SERIAL_INNO_INCLUDE_RACK_DEVICE_H_
#define COMPONENTS_SERIAL_INNO_INCLUDE_RACK_DEVICE_H_
#include <stdint.h>

typedef struct {
    uint16_t revision;
    uint16_t rack_type;
    uint16_t rows;
    uint16_t columns;
    uint16_t sides;
    uint16_t checksum;
    uint16_t rack_rev;
    uint16_t chip_idh;
    uint16_t chip_idl;
    uint16_t modbus_addr;
} rack_registers_t;

void rack_task(void *pvParameters);
void handle_rack_req(msg16_t *request, msg16_t *response);
void setup_rack_dev_default(void);



#endif /* COMPONENTS_SERIAL_INNO_INCLUDE_RACK_DEVICE_H_ */
