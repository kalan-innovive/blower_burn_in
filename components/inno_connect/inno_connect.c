#include <stdio.h>
#include "inno_connect.h"
/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>
#include "inno_connect.h"
#include "example_common_private.h"
#include "sdkconfig.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"

static const char *TAG = "inno_connect";
static char IP_addr[150];



/**
 * @brief Checks the netif description if it contains specified prefix.
 * All netifs created withing common connect component are prefixed with the module TAG,
 * so it returns true if the specified netif is owned by this module
 */
bool example_is_our_netif(const char *prefix, esp_netif_t *netif)
{
	return strncmp(prefix, esp_netif_get_desc(netif), strlen(prefix) - 1) == 0;
}

esp_netif_t* get_example_netif_from_desc(const char *desc)
{
	esp_netif_t *netif = NULL;
	while ((netif = esp_netif_next(netif)) != NULL) {
		if (strcmp(esp_netif_get_desc(netif), desc) == 0) {
			return netif;
		}
	}
	return netif;
}

void example_print_all_netif_ips(const char *prefix)
{
	// iterate over active interfaces, and print out IPs of "our" netifs
	esp_netif_t *netif = NULL;
	esp_netif_ip_info_t ip;
	for (int i = 0; i < esp_netif_get_nr_of_ifs(); ++i) {
		netif = esp_netif_next(netif);
		if (example_is_our_netif(prefix, netif)) {
			ESP_LOGI(TAG, "Connected to %s", esp_netif_get_desc(netif));
			ESP_ERROR_CHECK(esp_netif_get_ip_info(netif, &ip));

			sprintf(IP_addr, "%d.%d.%d.%d", IP2STR(&ip.ip));
			ESP_LOGI(TAG, "Setting IPv4 address: %s", IP_addr);
			ESP_LOGI(TAG, "- IPv4 address: " IPSTR ",", IP2STR(&ip.ip));

		}
	}
}

const char* get_ip(void) {
	ESP_LOGI(TAG, "Retturning IPv4 address: %s", IP_addr);
	return IP_addr;

}

//static void set_ip(char *ip_str) {
//	strcpy(IP_addr, ip_str);
//	ESP_LOGI(TAG, "Setting IPv4 address: %s", IP_addr);
//
//}


/**
 * Connects to all forms of transportation
 * Wifi
 * Bluetooth
 * Serial
 *
 */
esp_err_t inno_connect(void)
{


#if CONFIG_INNO_CONNECT_WIFI
	ESP_LOGI(TAG, "Connecting to wifi");
	esp_err_t err = ESP_FAIL;
//	set_wifi_cradentail("Erik's iPhone","7342173e");

#ifdef SSID_LIST_FROM_CFG

	char wf_ssid[3][24] = {
	CONFIG_INNO_WIFI_SSID0,
	CONFIG_INNO_WIFI_SSID1,
	CONFIG_INNO_WIFI_SSID2
	};

	char wf_passwd[3][64] = {
	CONFIG_INNO_WIFI_PASSWORD0,
	CONFIG_INNO_WIFI_PASSWORD1,
	CONFIG_INNO_WIFI_PASSWORD2,
	};

	for (int i = 0; i < 3; i++) {
		// Set the wifi Credentials in the list
		set_wifi_cradentail(wf_ssid[i], wf_passwd[i]);
		// Try connecting
		if (inno_wifi_connect() == ESP_OK) {
			err = ESP_OK;
			ESP_LOGI(TAG, "Connected to ssid  %s", wf_ssid[i]);

			break;

		}
		ESP_LOGW(TAG, "Could not connect to ssid  %s", wf_ssid[i]);

	}

#else

//	set_wifi_cradentail(CONFIG_INNO_WIFI_SSID0, CONFIG_INNO_WIFI_PASSWORD0);
	if (inno_wifi_connect() != ESP_OK) {
		ESP_LOGE(TAG, "Could not connect to ssid  %s", CONFIG_INNO_WIFI_SSID0);
		vTaskDelay(10);

		return ESP_FAIL;
	}

#endif

	ESP_LOGI(TAG, "Registering Shutdown callback ");
	vTaskDelay(10);
	err = esp_register_shutdown_handler(&connect_wifi_shutdown);
	ESP_LOGI(TAG, "esp_register_shutdown_handler returned %s",
			esp_err_to_name(err));
	vTaskDelay(10);

//	ESP_ERROR_CHECK(esp_register_shutdown_handler(&connect_wifi_shutdown));
#endif

#if CONFIG_INNO_CONNECT_WIFI
	example_print_all_netif_ips(INNO_NETIF_DESC_STA);
#endif

	return ESP_OK;
}

esp_err_t inno_disconnect(void)
{

#if CONFIG_INNO_CONNECT_WIFI
	connect_wifi_shutdown();
	ESP_ERROR_CHECK(
			esp_unregister_shutdown_handler(&connect_wifi_shutdown));
#endif
	return ESP_OK;
}
