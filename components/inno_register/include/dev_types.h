//
// Created by Erik Klan on 1/31/24.
//

#ifndef UNIT_TESTS_DEV_TYPES_H
#define UNIT_TESTS_DEV_TYPES_H


#include "reg_types.h"
#include "msg16_types.h"


typedef enum {
    DEV_SUPA = 0x01,
    DEV_EXHA = 0x03,
    DEV_SUPB = 0x21,
    DEV_EXB = 0x23,
    DEV_UNDEF = 0xff,
    DEV_RACK = 0x11,
} dev_id;
const static char *dev_id_repr[] = {
        "SupplyA",
        "ExhaustA",
        "SupplyB",
        "ExhaustB",
        "Rack",
};


// array of device blowers
const static dev_id dev_id_list[] = {
        DEV_SUPA,
        DEV_EXHA,
        DEV_SUPB,
        DEV_EXB
};
const static char *blower_id_str[] = {
        "SupplyA",
        "ExhaustA",
        "SupplyB",
        "ExhaustB",
};

const static char *blower_id_abrev[] = {
        "SA",
        "EA",
        "SB",
        "EB",
};


typedef enum {
    FAN = 0x00,
    VALVE = 0x01,
    CONTROL = 0xFE,
    UKNOWN_TYPE = 0xFFFF,
} blower_t;


/**
 * @brief Burnin configuration
 *
 */
typedef struct {
    dev_id type;                            /**< blower device type, The corresponding type runs different flows */
    blower_t blower_type;                    /**< blower type, The corresponding type runs different flows */
    unsigned chip_id;                        /**< Chip id , 0 default to 0 */
    unsigned chip_id_valid;                 /**< Chip id valid, if 0 default invalid */
    register_value_t cur_press;            /**< current pressure, */
    register_value_t target_press;            /**< target pressure, */
    register_value_t cal;            /**< target pressure, */
    register_value_t raw_press;            /**< target pressure, */
    register_value_t curr_pwm;            /**< current pwm value, */

} control_blower_t;

// Create custom config decleration for custom burnin
typedef struct {
    dev_id type;                            /**< blower device type, The corresponding type runs different flows */
    blower_t blower_type;                    /**< blower type, The corresponding type runs different flows */
    unsigned chip_id;                        /**< Chip id , 0 default to 0 */
    unsigned chip_id_valid;                 /**< Chip id valid, if 0 default invalid */
    register_value_t cur_press;            /**< current pressure, */
    register_value_t target_press;            /**< target pressure, */
    register_value_t cal;            /**< target pressure, */
    register_value_t raw_press;            /**< target pressure, */
    register_value_t valve_position;            /**< current valve position, */

} device_valve_blower_t;

// Create custom config decleration for custom burnin
typedef struct {
    dev_id type;                            /**< blower device type, The corresponding type runs different flows */
    blower_t blower_type;                    /**< blower type, The corresponding type runs different flows */
    unsigned chip_id;                        /**< Chip id , 0 default to 0 */
    unsigned chip_id_valid;                 /**< Chip id valid, if 0 default invalid */
    register_value_t cur_press;            /**< current pressure, */
    register_value_t target_press;            /**< target pressure, */
    register_value_t cal;            /**< target pressure, */
    register_value_t raw_press;            /**< target pressure, */
    register_value_t valve_position;            /**< current valve position, */

} device_fan_blower_t;

typedef int (*transact_func_t)(const msg16_t *request, msg16_t *response, uint32_t timeout);

int default_dev_handle(device_valve_blower_t *b_handle);

void destruct_device_handle(device_valve_blower_t **rec);

device_valve_blower_t *create_device_handle();

int update_device_blower_register(device_valve_blower_t *dev, transact_func_t transact);

const static char *get_dev_repr_full(dev_id dev) {
    for (int i = 0; i < sizeof(dev_id_list) / sizeof(dev_id); i++) {
        if (dev_id_list[i] == dev) {
            return blower_id_str[i];
        }
    }
    return "Unknown";
}

typedef union {
    device_fan_blower_t fan_blower;            /**< fan blower configuration */
    device_valve_blower_t valve_blower;        /**< valve blower configuration */
    control_blower_t control_blower;        /**< valve blower configuration */
} device_handle_union_t;


#endif //UNIT_TESTS_DEV_TYPES_H
