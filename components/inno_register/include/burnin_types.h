//
// Created by Erik Klan on 1/22/24.
//

#ifndef BLOWER_BURN_IN_BURNIN_TYPES_H
#define BLOWER_BURN_IN_BURNIN_TYPES_H
#ifdef UNIT_TESTING

#include "esp_log.h"
#include "freertos/task.h"

#include <stdio.h>

#else

#include <esp_log.h>
#include "esp_err.h"

#include "freertos/FreeRTOS.h"

#endif

#include "register.h"
#include "msg16_types.h"

#define MILLS_IN_MINUTE (60*1000)

/**
 * @brief Type of burnin being run on blower
 */
typedef enum {
    FAN_BLOWER,
    VALVE_BLOWER,
    FUNCTIONAL_CUSTOM
} burnin_type_t;

/** const string representation of burnin type */
static const char *burnin_type_reper[] = {
        "Fan Blower",
        "Valve Blower",
        "Control Blower"
};


typedef enum {
    burnin_test_Error,     /*? Set by burn in handler and shows error with task code */
    burnin_test_init,           /*? Task is initialization phase; only set by burn in handler */
    burnin_test_passed,         /*? Set by task when finished and passed */
    burnin_test_failed,         /*? Set by task if finished and failed */

} task_result_t;

// create task_result_t string array
static const char *task_result_str[] = {
        "burnin_test_Error",
        "burnin_test_init",
        "burnin_test_passed",
        "burnin_test_failed",
};

/**
 * @brif struct contains burnin testing passed failed and warning
 *
 */
typedef struct {
    int test_setup;
    int test_calibrate;
    int test_check_valve;
    int test_blowby_low;
    int test_blowby_high;
    int test_ach_low_hvac_high;
    int test_ach_low_hvac_low;
    int test_ach_high_hvac_high;
    int test_ach_high_hvac_low;
    int test_record_test;
} burnin_valve_results_t;


/**
 * @brief Struct that contains register record of given length
 */
typedef struct {
    int len;
    int *records;
    int *status;
    int index;
} reg_record_t;

reg_record_t *create_record(int len);

void destruct_record(reg_record_t *rec);


/** ********************************Types for the VALVE BURNIN TASK  ********************************/
// Create an enum that defines the burnin state
typedef enum {
    burnin_init, /** Initial state of the burnin task */
    burnin_setup, /** Start the setup task */
    burnin_check_valve, /** Start the position task */
    burnin_calibrate, /** start the calibration task */
    burnin_blowby_low, /**start the blowby low tasks */
    burnin_blowby_high, /**start the blowby high tasks */
    burnin_ach_low_hvac_high, /**start the ach low hvac high tasks */
    burnin_ach_low_hvac_low, /**start the ach low hvac low tasks */
    burnin_ach_high_hvac_high, /**start the ach high hvac high tasks */
    burnin_ach_high_hvac_low, /**start the ach high hvac low tasks */
    burnin_record_test, /**Record the data for the tests */
    burnin_passed, /** Burnin passed wait for off cycle */
    burnin_warning, /** Burnin had warnings send warning to display */
    burnin_finished, /** Burnin Task has finished*/
    burnin_failed, /** Burnin failed send failed to display */
    burnin_length,
} burnin_state_t;

/** String array representation of burnin_state_t */
static const char *burnin_state_repr[] = {
        "burnin_init",
        "burnin_setup",
        "burnin_check_valve",
        "burnin_calibrate",
        "burnin_blowby_low",
        "burnin_blowby_high",
        "burnin_ach_low_hvac_high",
        "burnin_ach_low_hvac_low",
        "burnin_ach_high_hvac_high",
        "burnin_ach_high_hvac_low",
        "burnin_record_test",
        "burnin_passed",
        "burnin_warning",
        "burnin_finished",
        "burnin_failed",
};




/** ********************************Types for the VALVE PWM POSITIONING TASK  ********************************/

// Create position thread state
typedef enum {
    position_init,
    position_set_control,
    position_set_pwm,
    position_wait,
    position_passed,
    position_failed,
} position_state_t;

// Create a const sring aray for position_state_t
static const char *position_state_str[] = {
        "position_init",
        "position_set_control",
        "position_set_pwm",
        "position_wait",
        "position_passed",
        "position_failed",
};

typedef struct {
    int control_pwm_len; /** length of the control pwm array */
    int *control_pwm;/** Pwm control list to iterate through  */
    reg_record_t *pwm0; /** Record for PWM0 */
    int pwm0_index; /** Current index of PWM0  */
    reg_record_t *pwm25; /** Record for PWM25 */
    int pwm25_index; /** Current index of PWM25  */
    reg_record_t *pwm50;    /** Record for PWM50 */
    int pwm50_index; /** Current index of PWM50  */
    reg_record_t *pwm75; /** Record for PWM75 */
    int pwm75_index; /** Current index of PWM75  */
    reg_record_t *pwm100; /** Record for PWM100 */
    int pwm100_index; /** Current index of PWM100  */
} pwm_record_t;

pwm_record_t *create_pwm_record_t(int num_samples, const int contr[], size_t contr_len);

void destruct_pwm_record_t(pwm_record_t *p);

/**
 * @brief struct that holds the configuration and results for the valve positioning task
 */
typedef struct {
    device_valve_blower_t *vdev;    /** Device handle for the valve */
    device_valve_blower_t *cdev;    /** Device handle for the control */
    dev_id id;
    dev_id contr_id;
    TickType_t start;               /** Set when task starts running */
    TickType_t stop;                /** Set when task should stop running */
    TickType_t time_out;            /** Time out for entire task */
    TickType_t loop_time;           /** Time for each loop plus transactions */
    pwm_record_t *pwm_record;       /** Record of the pwm values */
    position_state_t state;         /** Current state of the task */
    task_result_t result_state;     /** Result of the task lets caller know when it is finished */
    int iterations;
} valve_positioning_conf_t;



/** ********************************Types for the VALVE ACH HVAC TASK  ********************************/
#define ACH_CONF_DEFAULT_LOOP_TIME 1000
#define ACH_CONF_DEFAULT_TIMEOUT 10000
#define ACH_CONF_DEFAULT_REC_LEN 10
#define ACH_CONF_MAP_ROWS 4
#define ACH_CONF_REC_LEN(timeout, loop_time) ((timeout / loop_time) + 1)

#define ACH_CONF_HVAC_MAX_PRESS 1700    // Max hvac pressure in mInH2O
#define ACH_CONF_HVAC_MIN_PRESS 1200    // Min hvac pressure in  mInH2O
#define ACH_VALVE_MIN_PRESS 140         // Valve max pressure in mInH2O
#define ACH_VALVE_MAX_PRESS 1200        // Valve min pressure in mInH2O


typedef enum {
    ach_unknown = -1,
    ach_init,
    ach_setup,
    ach_setup_complete,
    ach_set,
    ach_reached,
    ach_maintain,
    ach_warning,
    ach_passed,
    ach_failed,
} ach_state_t;

// Create a const sring aray for ach_state_t
static const char *ach_state_str[] = {
        "ach_unknown",
        "ach_init",
        "ach_setup",
        "ach_setup_complete",
        "ach_set",
        "ach_reached",
        "ach_maintain",
        "ach_warning",
        "ach_passed",
        "ach_failed",
};

// Typedf for Type of ACH map
typedef enum {
    ACH_MAP_MIN_MIN,
    ACH_MAP_MIN_MAX,
    ACH_MAP_MAX_MIN,
    ACH_MAP_MAX_MAX,
} ach_map_type_t;

const static char *ach_map_type_str[4] = {
        "Map-Valve:min-HVAC:min",
        "Map-Valve:min-HVAC:max",
        "Map-Valve:max-HVAC:min",
        "Map-Valve:max-HVAC:max",
};

// Struct that holds the ach configuration
typedef struct {

    int valve_press;
    int hvac_press;
    ach_map_type_t type;
} setpoint_map_t;

typedef struct {
    setpoint_map_t setpoints[ACH_CONF_MAP_ROWS];
} ach_map_t;

const static ach_map_t ach_map = {
        .setpoints = {
                {ACH_VALVE_MIN_PRESS, ACH_CONF_HVAC_MIN_PRESS, ACH_MAP_MIN_MIN},
                {ACH_VALVE_MIN_PRESS, ACH_CONF_HVAC_MAX_PRESS, ACH_MAP_MIN_MAX},
                {ACH_VALVE_MAX_PRESS, ACH_CONF_HVAC_MIN_PRESS, ACH_MAP_MAX_MIN},
                {ACH_VALVE_MAX_PRESS, ACH_CONF_HVAC_MAX_PRESS, ACH_MAP_MAX_MAX}}
};

typedef struct {
    device_valve_blower_t *vdev;    /** Device handle for the valve */
    device_valve_blower_t *cdev;    /** Device handle for the control */
    dev_id id;
    dev_id contr_id;
    TickType_t start;
    TickType_t stop;
    TickType_t time_out;
    TickType_t loop_time;
    unsigned control_pressure;
    unsigned valve_setpoint;
    unsigned control_pwm;
    unsigned max_press_val;
    unsigned min_press_val;
    unsigned rec_len;
    ach_state_t ach_state;
    reg_record_t *records;
    int rec_index;
    int rec_warn_index;
    task_result_t state;
    task_result_t time_state;
    int iterations;
} ach_conf_t;

ach_conf_t *create_ach_conf(const setpoint_map_t *map);

void destruct_ach_conf(ach_conf_t *c);

/** ********************************Types for the VALVE BLOW-BY TASK   ********************************/
#define BLOWBY_CONF_DEF_TIMEOUT (2 * MILLS_IN_MINUTE)  // 2 minutes
#define BLOWBY_CONF_DEF_CONTROL_PRESS 1700
#define BLOWBY_CONF_DEF_CONTROL_PWM 85
#define BLOWBY_CONF_DEF_MAX_BLOWBY 40
#define BLOWBY_CONF_DEF_LOOP_TIME 1000 // 1 second loop time
#define BLOWBY_CONF_DEF_REC_LEN ACH_CONF_REC_LEN(BLOWBY_CONF_DEF_TIMEOUT, BLOWBY_CONF_DEF_LOOP_TIME)

/**
 * @brief enum holding the current blowby state
 */
typedef enum {
    blowby_init,
    blowby_set_control,
    blowby_wait_contr,
    blowby_contr_set,
    blowby_valve_set,
    blowby_valve_wait,
    blowby_valve_passed,
    blowby_valve_failed,
} blowby_state_t;

/** string reperarray for blowby_state_t */
static const char *blowby_state_str[] = {
        "blowby_init",
        "blowby_set_control",
        "blowby_wait_contr",
        "blowby_contr_set",
        "blowby_valve_set",
        "blowby_valve_wait",
        "blowby_valve_passed",
        "blowby_valve_failed",
};

typedef struct {
    device_valve_blower_t *vdev;    /** Device handle for the valve */
    device_valve_blower_t *cdev;    /** Device handle for the control */
    dev_id id;
    dev_id contr_id;
    TickType_t start;
    TickType_t stop;
    TickType_t time_out;
    unsigned control_pressure;
    unsigned control_pwm;
    unsigned max_blowby;
    unsigned rec_index;
    reg_record_t *records;
    unsigned max_press_val;
    blowby_state_t state;
    int iterations;
    int blow_by_val;
    unsigned loop_time;
} blowby_conf_t;

blowby_conf_t *create_blowby_conf();

void destruct_blowby_conf(blowby_conf_t *c);

/** ********************************Types for the VALVE CALIBRATION TASK  ********************************/
#define CAL_CONF_MAX_PRESS 80
#define CAL_CONF_TIMEOUT 10000
#define CAL_CONF_DEFAULT_VAL 0xff

/**
 * @brief state that holds the current callibration state
 */
typedef enum {
    cal_init,
    cal_stopping_fan,
    cal_wait,
    cal_running,
    cal_complete,
    cal_success,
    cal_failed,
    cal_started,
    cal_finished
} calibration_state_t;

/** string reperarray for calibration_state_t */
static const char *calibration_state_str[] = {
        "cal_init",
        "cal_stopping_fan",
        "cal_wait",
        "cal_running",
        "cal_complete",
        "cal_success",
        "cal_failed",
        "cal_started",
        "cal_finished"
};

typedef struct {
    device_valve_blower_t *vdev;    /** Device handle for the valve */
    device_valve_blower_t *cdev;    /** Device handle for the control */
    dev_id id;
    dev_id contr_id;
    uint32_t start;
    uint32_t stop;
    uint32_t time_out;
    calibration_state_t state;
    task_result_t burnin_state;
    int max_press;
    int iterations;
    int cal_val;
} calibration_conf_t;

void log_calibration_conf_t(calibration_conf_t *p);

calibration_conf_t *create_calibration_config();

void destruct_calibration_config(calibration_conf_t *c);
//
//typedef struct ParentStruct {
//    // other shared members
//    int commonMember;
//} ParentStruct;
//
//typedef union conf_handle {
//    calibration_conf_t *calib_conf;
//    valve_positioning_conf_t *pos_conf;
//    blowby_conf_t *blowby_conf;
//    ach_conf_t *ach_conf;
//} conf_handle_t;

#endif //BLOWER_BURN_IN_BURNIN_TYPES_H
