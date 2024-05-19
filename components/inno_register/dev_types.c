//
// Created by Erik Klan on 1/31/24.
//

#include "dev_types.h"
#include "reg_types.h"
#include "register.h"
#include "msg16_types.h"

#ifndef testing
#define testing 0
#endif
#ifdef testing

#include <time.h>

int timenow() {
    long t = time(NULL);
    t = t % 1000;
    return (int) t;

}

#else
#include "esp_timer.h"
int timenow() {
    return esp_timer_get_time()/1000
}

#endif


int default_dev_handle(device_valve_blower_t *b_handle) {
    // create the device blower memory
    if (b_handle == NULL) {
        return -1;
    }
    b_handle->blower_type = UKNOWN_TYPE;
    b_handle->chip_id = 0;
    b_handle->chip_id_valid = 0;
    default_register(&(b_handle->cur_press), REG_CURR_PRESS);
    default_register(&(b_handle->target_press), REG_TARG_PRESS);
    default_register(&(b_handle->cal), REG_CALIBRATE);
    default_register(&(b_handle->raw_press), REG_RAW_PRESS);
    default_register(&(b_handle->valve_position), REG_RPM_A);
    return 1;
}


device_valve_blower_t *create_device_handle() {
    device_valve_blower_t *p = (device_valve_blower_t *) malloc(sizeof(device_valve_blower_t));
    if (p == NULL) {
        return NULL;
    }
    default_dev_handle(p);
    return p;
}

void destruct_device_handle(device_valve_blower_t **rec) {
    free(*rec);
    *rec = NULL;
}


int update_device_blower_register(device_valve_blower_t *dev, transact_func_t transact) {
    if (dev == NULL) {
        return -1;
    }
    // list of registers to update
    register_value_t *reg_list[5] = {
            &(dev->cur_press),
            &(dev->target_press),
            &(dev->cal),
            &(dev->raw_press),
            &(dev->valve_position)
    };
    int ret;
    //iterate through the list of registers and write any were the update status is WRITE_REG
    for (int i = 0; i < 5; i++) {
        // pointer to current register
        register_value_t *reg = reg_list[i];
        int tmp;
        if (reg_list[i]->update_status == WRITE_REG) {
            // Create a write message
            msg16_t msg_req = {.type = WRITE_REQ, .dev_id = dev->type, .addr =reg->addr, .len = 1};
            // Create a write responsse message
            msg16_t msg_resp;
            // Send the message
            ret = transact(&msg_req, &msg_resp, 1000);
            // Check if the message was successful
            if (ret == 1) {
                // Update the register value
//                reg->write_val = msg_resp.payload[0];
                reg_list[i]->update_status = REG_WRITTEN;

            } else {
                // Update the register value
                reg_list[i]->update_status = REG_NOT_UPDATED;
            }
        }
        if (reg->update_status == REG_UPDATE) {
            // Create a read message
            msg16_t msg_req = {.type = READ_REQ, .dev_id = dev->type, .addr =reg->addr, .len = 1};
            // Create a read responsse message
            msg16_t msg_resp;
            // Send the message
            ret = transact(&msg_req, &msg_resp, 1000);
            // Check if the message was successful
            if (ret == 1) {
                // Update the register value
                reg->reg_val = msg_resp.payload[0];
                reg->curr_value_status = REG_VALID;
                reg->last_updated = timenow();


            } else {
                // Update the status value
                reg->update_status = REG_NOT_UPDATED;
                reg->curr_value_status = REG_NOT_UPDATED;
                reg->val = DEFAULT_REG_VAL;
            }
            // Set the reg val to the correct interpretation
            if (reg->addr == REG_RPM_A && dev->blower_type == FAN) {
                tmp = (uint16_t) reg->reg_val;
                tmp = (tmp == 0) ? 0 : tmp / 2 * 60;
                reg->val = tmp;
            } else {
                reg->val = (int16_t) reg->reg_val;
            }

        }
    }

    return 1;
}

