/*
 * console_cmd.h
 *
 *  Created on: Feb 26, 2024
 *      Author: ekalan
 */

#ifndef COMPONENTS_INNO_CONNECT_INCLUDE_CONSOLE_CMD_H_
#define COMPONENTS_INNO_CONNECT_INCLUDE_CONSOLE_CMD_H_

#include <stddef.h>

//#include "sdkconfig.h"
#include "esp_err.h"

/**
 * @brief Parameters for console req:
 *
 * @note setup required interfaces first
 *
 */
typedef struct {
//	QueueHandle_t *event_q_handle;
	int using_system;
	int using_wifi;
	int using_bluetooth;
	int using_nvs;
	int using_sensor;
	int using_serialinno;

} console_setup_config_t;

#define CONSOLE_SETUP_CONFIG_DEFAULT() \
{                                         \
        .using_system = 0,            \
        .using_wifi = 0,        \
        .using_bluetooth = 0,          \
        .using_nvs = 0,               \
        .using_sensor = 0,                   \
		.using_serialinno = 0,                   \
}


// Register console functions
esp_err_t setup_console_cmds(console_setup_config_t *cons_conf);


#endif /* COMPONENTS_INNO_CONNECT_INCLUDE_CONSOLE_CMD_H_ */
