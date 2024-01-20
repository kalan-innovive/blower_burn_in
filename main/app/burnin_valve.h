/*
 * burnin_valve.h
 *
 *  Created on: Jan 10, 2024
 *      Author: ekalan
 */

#ifndef MAIN_APP_BURNIN_VALVE_H_
#define MAIN_APP_BURNIN_VALVE_H_

#include "esp_err.h"
#include "msg16.h"


typedef void *burnin_handle_t;
/**
 * @brief Supported button type
 *
 */
typedef enum {
	FAN_BLOWER,
    VALVE_BLOWER,
    FUNCTIONAL_CUSTOM
} burnin_type_t;

/**
 * @brief Supported button type
 *
 */
typedef enum {
	UKNOWN_REG_STATUS,
	REG_NOT_UPDATED,
    REG_UPDATED,
	WRITE_REG,
	REG_WRITTEN,
	REG_VALID
} reg_staus_type_t;


typedef enum {
	cal_init,
	cal_stopping_fan,
	cal_wait,
	cal_running,
	cal_complete,
	cal_success,
	cal_failed
}calibration_state_t;

typedef struct {
	dev_id id;
	dev_id contr_id;
	uint32_t start; // @suppress("Type cannot be resolved")
	uint32_t stop;
	uint32_t time_out;
	calibration_state_t state;
	int itterations;
	int cal_val;
}calibration_conf_t;

typedef struct{
	dev_id id;
	dev_id contr_id;
	TickType_t start;
	TickType_t stop;
	TickType_t time_out;
	unsigned control_pressure;
	unsigned control_pwm;
	unsigned max_blowby;
	unsigned rec_len;
	unsigned *records;
	unsigned max_press_val;
	calibration_state_t state;
	int iterations;
	int blow_by_val;
} blowby_conf_t;


/**
 * @brief Register value
 *
 */
typedef struct {
	blower_reg_addr reg;                        /**< blower device type, The corresponding type runs different flows */
	int  curr_value;							/**< Register Value*/
	reg_staus_type_t curr_value_status;                 /**< flag for updated values */
	reg_staus_type_t reg_valid;					 /**< flag if the reg is valid */
	unsigned last_updated;					/**< time register was updated */
} register_value_t;

/**
 * @brief Register list
 *
 */
//typedef struct {
//	int  len;							/**< Register Value*/
//	register_value_t r_list[24];                 /**< flag for updated values */
//} reg_list;



/**
 * @brief Burnin configuration
 *
 */
typedef struct {
	dev_id type;                           	/**< blower device type, The corresponding type runs different flows */
	blower_t blower_type;					/**< blower type, The corresponding type runs different flows */
	unsigned chip_id;                    	/**< Chip id , 0 default to 0 */
	unsigned chip_id_valid;                 /**< Chip id valid, if 0 default invalid */
    register_value_t cur_press;			/**< current pressure, */
    register_value_t target_press;			/**< target pressure, */
    register_value_t cal;			/**< target pressure, */
    register_value_t raw_press;			/**< target pressure, */
    register_value_t rpm;			/**< current pwm value, */

} burnin_controll_fan_config_t;


/**
 * @brief Burnin configuration
 *
 */
typedef struct {
	dev_id type;                           	/**< blower device type, The corresponding type runs different flows */
	blower_t blower_type;					/**< blower type, The corresponding type runs different flows */
	unsigned chip_id;                    	/**< Chip id , 0 default to 0 */
	unsigned chip_id_valid;                 /**< Chip id valid, if 0 default invalid */
    register_value_t cur_press;			/**< current pressure, */
    register_value_t target_press;			/**< target pressure, */
    register_value_t cal;			/**< target pressure, */
    register_value_t raw_press;			/**< target pressure, */
    register_value_t curr_pwm;			/**< current pwm value, */

} burnin_valve_config_t;


/**
 * @brief Burnin configuration
 *
 */
typedef struct {
	burnin_type_t type;                           /**< button type, The corresponding button configuration must be filled */
	unsigned start_time;                     /**< Trigger time(ms) for long press, if 0 default to BUTTON_LONG_PRESS_TIME_MS */
	unsigned stop_time;                    /**< Trigger time(ms) for short press, if 0 default to BUTTON_SHORT_PRESS_TIME_MS */
    union {
//    	burnin_blower_config_t fan_config;  /**< gpio button configuration */
    	burnin_valve_config_t valve_config;    /**< adc button configuration */
    	burnin_controll_fan_config_t control_config;   /**< custom button configuration */
    }; /**< button configuration */
} burnin_conf_t;

#endif /* MAIN_APP_BURNIN_VALVE_H_ */
