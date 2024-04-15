/* Common functions for protocol examples, to establish Wi-Fi or Ethernet connection.

 This example code is in the Public Domain (or CC0 licensed, at your option.)

 Unless required by applicable law or agreed to in writing, this
 software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 CONDITIONS OF ANY KIND, either express or implied.
 */

#pragma once

#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_netif.h"


#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_INNO_CONNECT_WIFI
#define INNO_NETIF_DESC_STA "Inno_netif_sta"
#endif



/* Example default interface, prefer the ethernet one if running in example-test (CI) configuration */

#if CONFIG_INNO_CONNECT_WIFI
#define EXAMPLE_INTERFACE get_example_netif_from_desc(INNO_NETIF_DESC_STA)
#define get_example_netif() get_example_netif_from_desc(INNO_NETIF_DESC_STA)
#endif

#define CONN_KNOWN_WIFI_NETWORKS 1
typedef struct {
	char ssid[25];
	char pass[25];
}wifi_net_login;





/**
 * @brief Configure Wi-Fi or Ethernet, connect, wait for IP
 *
 * This all-in-one helper function is used for different connection types to
 * reduce the amount of boilerplate in Projects.
 *
 *
 * @return ESP_OK on successful connection
 */
esp_err_t inno_connect(void);

const char* get_ip(void);
/**
 * Counterpart to inno_connect, de-initializes Wi-Fi
 */
esp_err_t inno_disconnect(void);

/**
 * @brief Configure stdin and stdout to use blocking I/O
 *
 * This helper function is used in ASIO examples. It wraps installing the
 * UART driver and configuring VFS layer to use UART driver for console I/O.
 */
esp_err_t example_configure_stdin_stdout(void);

/**
 * @brief Returns esp-netif pointer created by inno_connect() described by
 * the supplied desc field
 *
 * @param desc Textual interface of created network interface, for example "sta"
 * indicate default WiFi station, "eth" default Ethernet interface.
 *
 */
esp_netif_t* get_example_netif_from_desc(const char *desc);

#if CONFIG_INNO_PROVIDE_WIFI_CONSOLE_CMD
/**
 * @brief Register wifi connect commands
 *
 * Provide a simple wifi_connect command in esp_console.
 * This function can be used after esp_console is initialized.
 */
void example_register_wifi_connect_commands(void);
#endif



#ifdef __cplusplus
}
#endif
