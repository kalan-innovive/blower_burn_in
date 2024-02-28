/*
 * wifi_cmd.h
 *
 *  Created on: Feb 26, 2024
 *      Author: ekalan
 */

#ifndef COMPONENTS_CONSOLE_CMD_INCLUDE_WIFI_CMD_H_
#define COMPONENTS_CONSOLE_CMD_INCLUDE_WIFI_CMD_H_

typedef struct {
	struct arg_str *ssid;
	struct arg_str *password;
	struct arg_int *channel;
	struct arg_end *end;
} wifi_connect_args_t;

// Register WiFi functions
void register_wifi(void);



#endif /* COMPONENTS_CONSOLE_CMD_INCLUDE_WIFI_CMD_H_ */
