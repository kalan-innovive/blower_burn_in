//
// Created by Erik Klan on 1/31/24.
//

#ifndef UNIT_TESTS_REG_TYPES_H
#define UNIT_TESTS_REG_TYPES_H


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


typedef enum {
    REG_STATUS,               //0
    REG_CURR_PRESS,           //1
    REG_TARG_PRESS,           //2
    REG_RPM_A,                //3
    REG_RPM_B,                //4
    REG_CMD,                  //5
    REG_CALIBRATE,            //6
    REG_DIAGIDfan,            //7
    REG_SERVO_DEG,            //8
    REG_HC_DETECT,            //9
    REG_RAW_PRESS,            //10 signed pressure in 0.001 Inch H2O prior to calibration (signed)
    REG_RAW_SAMPLE,            //11 averaged ADC or pressure sample in native output
    REG_RAW_SAMPLE_MIN,        //12 minimum native sample over last averaging period
    REG_RAW_SAMPLE_MAX,        //13 maximum native sample over last averaging period
    REG_CURR_PWM,            //14 current pwm value [0-100]
    REG_CURR_FANS,            //15 current number of enabled fanse
    REG_FAIL_PWM,            //16 pwm value [0-100] to use on pressure sensor fail
    REG_FAIL_FANS,            //17 number of fans to use on pressure sensor fail
    REG_CURRENT_TEMP,                //18 units are 0.1C (signed)
    REG_PRESSURE_SENSOR_TYPE,//19 0 = MPXV7002, 1 = MS4515DS3BS002, 0xffff = unknown
    REG_PWM_TEST_MODE,            //20 0xffff = normal operation, 0-100 PWM value, others ?
    REG_VALVE_POS,            //21 0xffff = normal operation, 0-1000 PWM value, others ?
    REG_VALVE_POS_MAX,            //22 0xffff = normal operation, 0-1000 PWM value, others ?
    REG_VALVE_POS_MIN,            //22 0xffff = normal operation, 0-1000 PWM value, others ?

    REG_HWTYPE = 0x1000,        //0x1000
    REG_VERSION,              //0x1001
    REG_INVOKE_BOOTLOADERH,   //0x1002
    REG_INVOKE_BOOTLOADERL,   //0x1003
    REG_CHIPIDH,              //0x1004
    REG_CHIPIDL,              //0x1005
    REG_MODBUS_ADDR,          //0x1006
    REG_BLOWER_TYPE,          //0x1007 0 == fan, 1 == valve- airflow related registers

    REG_NV_STORE_LOW = 0xA000,          //0xA000
    REG_NV_STORE_HIGH = (REG_NV_STORE_LOW + NUM_NV_STORE - 1)
} blower_reg_addr;

typedef enum {
//    REG_CHIPIDH=0x1004,
//    REG_CHIPIDL=0x1005,
//    REG_MODBUS_ADDR=0x1006,
    INFO_REVISION = 0xA000,
    INFO_RACKTYPE = 0xA001,
    INFO_ROWS = 0xA002,
    INFO_COLUMNS = 0xA003,
    INFO_SIDES = 0xA004,
    INFO_CHECKSUM = 0xA005,
    INFO_RACKREV = 0xA006,

} rack_reg_addr;


/**
 * @brief enum describes what needs to be done to the register
 *
 */
typedef enum {
    UKNOWN_REG_STATUS,
    REG_NOT_UPDATED,
    REG_UPDATE,
    WRITE_REG,
    REG_WRITTEN,
    REG_VALID
} reg_staus_t;

// create array of register status strings
static const char *reg_status_str[] = {
        "UKNOWN_REG_STATUS",
        "REG_NOT_UPDATED",
        "REG_UPDATE",
        "WRITE_REG",
        "REG_WRITTEN",
        "REG_VALID"
};

/**
 * @brief Register value
 * set the write value to write the register
 * Returns 255 or 0xff if written in write register
 */
typedef struct {
    blower_reg_addr addr;                        /**< blower device type, The corresponding type runs different flows */
    int val;                                /**< interpreted value of the register as an int read only*/
    uint16_t reg_val;                          /** Value held in register read only*/
    uint16_t write_val;
    reg_staus_t curr_value_status;        /**< flag for updated values */
    reg_staus_t update_status;                /**< check  */
    uint32_t last_updated;                    /**< time register was updated from blower */
} register_value_t;


/**
 * @brief Register list
 * Holds a list of read write registers for the burnin
 *
 */
typedef struct {
    register_value_t *reg_list;                        /**< blower device type, The corresponding type runs different flows */
    int len;                            /**< Register Value*/
    int num_updated;                 /**< flag for updated values */
    int num_valid;                     /**< flag if the reg is valid */
    unsigned last_updated;                    /**< time register was updated */

} reg_list;


#endif //UNIT_TESTS_REG_TYPES_H
