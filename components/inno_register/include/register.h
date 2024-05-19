//
// Created by Erik Klan on 1/31/24.
//

#ifndef UNIT_TESTS_REGISTER_H
#define UNIT_TESTS_REGISTER_H


#include "dev_types.h"
#include "reg_types.h"

#define DEFAULT_REG_VAL 0xFF
#define DEF_DEV_HANDLE_CHIPID 0
#define DEF_DEV_CHIPID_VALID 0

void default_register(register_value_t *reg, int id);


/**
 * @brief create register List
    * @param: length of the list
    * @return array of registers

 */
reg_list *create_reg_list(int len);

/**
 * @brief destroy register List
    * @param reg_list
 */
void destruct_reg_list(reg_list *reg_list);


#endif //UNIT_TESTS_REGISTER_H
