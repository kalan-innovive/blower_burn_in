/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once


typedef enum {
	BLOWER_BURN_IN,
	APP_TYPE_LEN,
} app_types;

// Define a struct to hold the WiFi credentials
typedef struct {
    char ssid[32];
    char password[64];
} wifi_credentials_t;

//typedef struct {
//	uint8_t num_wifi_records[8];
//	uint8_t primary_credential;
//	wifi_credentials_t saved_wifi_credential;
//} wifi_credentials_list;

// Define a struct to hold the WiFi credentials


typedef struct {
	uint8_t mqtt_connected;
	uint8_t console_connected;
	uint8_t eh_server_connected;
	uint8_t bluetooth_connected;
	uint8_t wifi_connected;
} node_com_ports;

typedef struct {
    char host[32];
    char password[64];
    char node_name[64];
	unsigned node_number;
} mqtt_credentials_t;

typedef struct {
    char ipv4[32];
    char password[64];
    char mac_addr[64];
} net_settings_t;


typedef struct {
    char eh_server[32];
    char eh_encr[64];
} server_settings_t;

typedef struct {
    uint8_t TEB;
    uint8_t range;
    uint8_t num_cycles;
    uint8_t num_cycle_shown;
    uint8_t on_time;
    uint8_t off_time;
} app_burn_in;



typedef struct {
    bool need_hint;
    wifi_credentials_t wifi;
    mqtt_credentials_t mqtt;
    net_settings_t net;
    uint8_t app_type;
    server_settings_t eh;
    app_burn_in burn_in;
    node_com_ports comm_ports;
} sys_param_t;


esp_err_t settings_read_parameter_from_nvs(void);
esp_err_t settings_write_parameter_to_nvs(void);
sys_param_t *settings_get_parameter(void);
esp_err_t save_wifi_credentials(wifi_credentials_t *credentials);
