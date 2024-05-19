

//
// Created by Erik Klan on 1/23/24.
//



#include "register.h"
#include "reg_types.h"
#include "dev_types.h"


void default_register(register_value_t *reg, int id) {
    reg->addr = (blower_reg_addr) (id);
    reg->val = DEFAULT_REG_VAL;
    reg->write_val = DEFAULT_REG_VAL;
    reg->curr_value_status = REG_NOT_UPDATED;
    reg->update_status = UKNOWN_REG_STATUS;

}

reg_list *create_reg_list(int len) {
    reg_list *p = (reg_list *) malloc(sizeof(reg_list));
    if (p == NULL) {
        return p;
    }
    p->reg_list = (register_value_t *) malloc(sizeof(register_value_t) * len);
    if (!p->reg_list) {
        free(p);
        return NULL;
    }
    return p;
}

void destruct_reg_list(reg_list *reg_list) {
    if (reg_list == NULL) {
        return;
    }
    if (reg_list->reg_list == NULL) {
        free(reg_list->reg_list);
        reg_list->reg_list = NULL;
    }
    free(reg_list);
    reg_list = NULL;
}
//
//int default_dev_handle(device_valve_blower_t *b_handle) {
//    // create the device blower memory
//    if (b_handle == NULL) {
//        return -1;
//    }
//    b_handle->blower_type = UKNOWN_TYPE;
//    b_handle->chip_id = DEF_DEV_HANDLE_CHIPID;
//    b_handle->chip_id_valid = DEF_DEV_CHIPID_VALID;
//    default_register(&(b_handle->cur_press), REG_CURR_PRESS);
//    default_register(&(b_handle->target_press), REG_TARG_PRESS);
//    default_register(&(b_handle->cal), REG_CALIBRATE);
//    default_register(&(b_handle->raw_press), REG_RAW_PRESS);
//    default_register(&(b_handle->valve_position), REG_RPM_A);
//    return 1;
//}

//
//device_valve_blower_t *create_device_handle() {
//    device_valve_blower_t *p = (device_valve_blower_t *) malloc(sizeof(device_valve_blower_t));
//    if (p == NULL) {
//        return NULL;
//    }
//
//    default_dev_handle(p);
//    return p;
//}
//
//void destruct_device_handle(device_valve_blower_t *rec) {
//    free(rec);
//    rec = NULL;
//}



