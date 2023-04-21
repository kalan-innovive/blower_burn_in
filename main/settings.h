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

// Define a struct to hold the WiFi credentials
typedef struct {
    char host[32];
    char password[64];
    char node_name[64];
} mqtt_credentials_t;


typedef struct {
    bool need_hint;
    wifi_credentials_t wifi;
    mqtt_credentials_t mqtt;
    uint8_t app_type;
} sys_param_t;

esp_err_t settings_read_parameter_from_nvs(void);
esp_err_t settings_write_parameter_to_nvs(void);
sys_param_t *settings_get_parameter(void);
esp_err_t save_wifi_credentials(wifi_credentials_t *credentials);
