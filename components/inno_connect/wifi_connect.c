/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Common functions for protocol examples, to establish Wi-Fi or Ethernet connection.

 This example code is in the Public Domain (or CC0 licensed, at your option.)

 Unless required by applicable law or agreed to in writing, this
 software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 CONDITIONS OF ANY KIND, either express or implied.
 */

#include <string.h>
#include "stdint.h"
#include "inno_connect.h"
#include "example_common_private.h"
#include "esp_log.h"
#include "esp_wifi.h"
//#include "esp_event_loop.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_netif.h"
#include "freertos/semphr.h"

static esp_err_t connect_to_known_network(wifi_config_t wifi_config);

#if CONFIG_INNO_CONNECT_WIFI

static const char *TAG = "inno_wifi_connect";
static esp_netif_t *s_example_sta_netif = NULL;
static SemaphoreHandle_t s_semph_get_ip_addrs = NULL;
#if CONFIG_EXAMPLE_CONNECT_IPV6
static SemaphoreHandle_t s_semph_get_ip6_addrs = NULL;
#endif

#if CONFIG_EXAMPLE_WIFI_SCAN_METHOD_FAST
#define EXAMPLE_WIFI_SCAN_METHOD WIFI_FAST_SCAN
#elif CONFIG_EXAMPLE_WIFI_SCAN_METHOD_ALL_CHANNEL
#define EXAMPLE_WIFI_SCAN_METHOD WIFI_ALL_CHANNEL_SCAN
#endif

#if CONFIG_EXAMPLE_WIFI_CONNECT_AP_BY_SIGNAL
#define EXAMPLE_WIFI_CONNECT_AP_SORT_METHOD WIFI_CONNECT_AP_BY_SIGNAL
#elif CONFIG_EXAMPLE_WIFI_CONNECT_AP_BY_SECURITY
#define EXAMPLE_WIFI_CONNECT_AP_SORT_METHOD WIFI_CONNECT_AP_BY_SECURITY
#endif

#if CONFIG_EXAMPLE_WIFI_AUTH_OPEN
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_EXAMPLE_WIFI_AUTH_WEP
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_EXAMPLE_WIFI_AUTH_WPA_PSK
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_EXAMPLE_WIFI_AUTH_WPA2_PSK
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_EXAMPLE_WIFI_AUTH_WPA_WPA2_PSK
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_EXAMPLE_WIFI_AUTH_WPA2_ENTERPRISE
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_ENTERPRISE
#elif CONFIG_EXAMPLE_WIFI_AUTH_WPA3_PSK
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_EXAMPLE_WIFI_AUTH_WPA2_WPA3_PSK
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_EXAMPLE_WIFI_AUTH_WAPI_PSK
#define EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

/** *******************************************************************
 *	Prototypes
 */
/** *******************************************************************/

static esp_err_t connect_to_known_network(wifi_config_t wifi_config);
static void inno_handler_on_sta_got_ip(void *arg,
		esp_event_base_t event_base, int32_t event_id, void *event_data);
static void inno_handler_on_wifi_connect(void *esp_netif,
		esp_event_base_t event_base, int32_t event_id, void *event_data);
static void inno_handler_on_wifi_disconnect(void *arg,
		esp_event_base_t event_base, int32_t event_id, void *event_data);

static int s_retry_num = 0;

wifi_net_login known_networks[] = {
		{ "Erik's iPhone", "7342173e" },
		{ "innowf", "Inn0wifi!" },
		{ "Wytec-Office", "Innovive" },
};

/** *******************************************************************
 *
 */
/** *******************************************************************
 * Sets up wifi configuration  and tries to connect
 */
esp_err_t inno_wifi_connect(void) {
	ESP_LOGI(TAG, "Start inno_connect.");
	connect_wifi_start();

	//@formatter:off;
	wifi_config_t wifi_config = {
			.sta = {
				.scan_method = EXAMPLE_WIFI_SCAN_METHOD,
				.sort_method = EXAMPLE_WIFI_CONNECT_AP_SORT_METHOD,
				.threshold.rssi = CONFIG_EXAMPLE_WIFI_SCAN_RSSI_THRESHOLD,
				.threshold.authmode = EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD,
				},
			};
		//@formatter:on;



#if CONFIG_INNO_WIFI_SSID_PWD_FROM_STDIN
    example_configure_stdin_stdout();
    char buf[sizeof(wifi_config.sta.ssid)+sizeof(wifi_config.sta.password)+2] = {0};
    ESP_LOGI(TAG, "Please input ssid password:");
    fgets(buf, sizeof(buf), stdin);
    int len = strlen(buf);
    buf[len-1] = '\0'; /* removes '\n' */
    memset(wifi_config.sta.ssid, 0, sizeof(wifi_config.sta.ssid));

    char *rest = NULL;
    char *temp = strtok_r(buf, " ", &rest);
    strncpy((char*)wifi_config.sta.ssid, temp, sizeof(wifi_config.sta.ssid));
    memset(wifi_config.sta.password, 0, sizeof(wifi_config.sta.password));
    temp = strtok_r(NULL, " ", &rest);
    if (temp) {
        strncpy((char*)wifi_config.sta.password, temp, sizeof(wifi_config.sta.password));
    } else {
        wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
    }
#endif
	return inno_wifi_sta_do_connect(wifi_config, true);
}

esp_err_t inno_wifi_sta_do_connect(wifi_config_t wifi_config, bool wait) {

	if (wait) {
		s_semph_get_ip_addrs = xSemaphoreCreateBinary();
		if (s_semph_get_ip_addrs == NULL) {
			return ESP_ERR_NO_MEM;
		}
	}
	s_retry_num = 0;
	ESP_ERROR_CHECK(
			esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &inno_handler_on_wifi_disconnect, NULL));
	ESP_ERROR_CHECK(
			esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &inno_handler_on_sta_got_ip, NULL));
	ESP_ERROR_CHECK(
			esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED,
					&inno_handler_on_wifi_connect, s_example_sta_netif));

#if CONN_KNOWN_WIFI_NETWORKS
	esp_err_t ret = connect_to_known_network(wifi_config);
#else

	ESP_LOGI(TAG, "Connecting to %s...", wifi_config.sta.ssid);
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

	// Set the wifi password from the credentials
	//	esp_err_t ret = esp_wifi_connect();
#endif
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "WiFi connect failed! ret:%x", ret);
		return ret;
	}
	if (wait) {
		ESP_LOGI(TAG, "Waiting for IP(s)");
		xSemaphoreTake(s_semph_get_ip_addrs, portMAX_DELAY);

		ESP_LOGD(TAG, "Retry count %d", s_retry_num);

		if (s_retry_num > CONFIG_INNO_WIFI_CONN_MAX_RETRY) {
			ESP_LOGW(TAG, "Reached max retry for connections");
			return ESP_FAIL;
		}
	}
	ESP_LOGD(TAG, "Successful on count %d", s_retry_num);

	return ESP_OK;
}

static esp_err_t connect_to_known_network(wifi_config_t wifi_config) {

	// Start the scan
	wifi_scan_config_t scan_config = { .show_hidden = false };
	ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));

	// Get the number of networks found
	uint16_t ap_count = 0;
	ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));

	wifi_ap_record_t *ap_records = (wifi_ap_record_t*) malloc(
			sizeof(wifi_ap_record_t) * ap_count);
	ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_count, ap_records));
	int found_id = 0;

	for (int i = 0; i < ap_count; i++) {
		wifi_ap_record_t *ap = &ap_records[i];

		ESP_LOGI(TAG, "%d. Wifi scan found : %s", i, ap->ssid);

		for (int j = 0; j < sizeof(known_networks) / sizeof(wifi_net_login);
				j++) {
			if (strcmp((char*) ap->ssid, known_networks[j].ssid) == 0) {
				ESP_LOGI(TAG, "    Known network found: %s", ap->ssid);

				// Connect to the first found network
				if (found_id == 0) {
					wifi_config_t wifi_config;
					memset(&wifi_config, 0, sizeof(wifi_config_t));

					strcpy((char*) wifi_config.sta.ssid,
							known_networks[j].ssid);
					strcpy((char*) wifi_config.sta.password,
							known_networks[j].pass);

					ESP_ERROR_CHECK(
							esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
					esp_err_t ret = esp_wifi_connect();
					// Check if the connection was successful
					// If it was we set found to true and do not try connect again
					// Otherwise handle errors
					switch (ret) {
					case (ESP_OK):
						ESP_LOGI(TAG, "    Connected to AP: %s",
								wifi_config.sta.ssid);
						found_id = 1;
						break;
					case (ESP_ERR_WIFI_NOT_INIT):
						ESP_LOGW(TAG,
								"    Error Connecting: ESP_ERR_WIFI_NOT_INIT  ap:%s",
								wifi_config.sta.ssid);
						break;
					case (ESP_ERR_WIFI_NOT_STARTED):
						ESP_LOGW(TAG,
								"    Error Connecting: ESP_ERR_WIFI_NOT_STARTED  ap:%s",
								wifi_config.sta.ssid);
						break;
					case (ESP_ERR_WIFI_CONN):
						ESP_LOGW(TAG,
								"    Error Connecting: ESP_ERR_WIFI_CONN   ap:%s",
								wifi_config.sta.ssid);
						break;
					case (ESP_ERR_WIFI_SSID):
						ESP_LOGW(TAG,
								"    Error Connecting: ESP_ERR_WIFI_SSID ap:%s:%s",
								wifi_config.sta.ssid, wifi_config.sta.password);
						break;

					}

				}
			}
		}
	}
	free(ap_records);

	if (found_id == 0) {
		ESP_LOGW(TAG, "No known networks found");
		return ESP_FAIL;
	}

	return ESP_OK;
}



static void inno_handler_on_wifi_disconnect(void *arg,
		esp_event_base_t event_base, int32_t event_id, void *event_data) {
	s_retry_num++;
	if (s_retry_num > CONFIG_INNO_WIFI_CONN_MAX_RETRY) {
		ESP_LOGI(TAG, "WiFi Connect failed %d times, stop reconnect.",
				s_retry_num);
		/* let example_wifi_sta_do_connect() return */
		if (s_semph_get_ip_addrs) {
			xSemaphoreGive(s_semph_get_ip_addrs);
		}

		return;
	}
	ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
	esp_err_t err = esp_wifi_connect();
	if (err == ESP_ERR_WIFI_NOT_STARTED) {
		return;
	}
	//TODO: signal that wifi is disconnected
	ESP_ERROR_CHECK(err);
}

static void inno_handler_on_wifi_connect(void *esp_netif,
		esp_event_base_t event_base, int32_t event_id, void *event_data) {

}

static void inno_handler_on_sta_got_ip(void *arg,
		esp_event_base_t event_base, int32_t event_id, void *event_data) {
	s_retry_num = 0;
	ip_event_got_ip_t *event = (ip_event_got_ip_t*) event_data;
	if (!example_is_our_netif(INNO_NETIF_DESC_STA, event->esp_netif)) {
		return;
	}
	ESP_LOGI(TAG, "Got IPv4 event: Interface \"%s\" address: " IPSTR,
			esp_netif_get_desc(event->esp_netif),
			IP2STR(&event->ip_info.ip));
	if (s_semph_get_ip_addrs) {
		xSemaphoreGive(s_semph_get_ip_addrs);
	}
	else {
		ESP_LOGI(TAG, "- IPv4 address: " IPSTR ",",
				IP2STR(&event->ip_info.ip));
	}
}


void connect_wifi_start(void) {
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	esp_netif_inherent_config_t esp_netif_config =
	ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();
	// Warning: the interface desc is used in tests to capture actual connection details (IP, gw, mask)
	esp_netif_config.if_desc = INNO_NETIF_DESC_STA;
	esp_netif_config.route_prio = 128;
	s_example_sta_netif = esp_netif_create_wifi(WIFI_IF_STA,
			&esp_netif_config);
	esp_wifi_set_default_wifi_sta_handlers();

	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_start());
}

void connect_wifi_stop(void) {
	esp_err_t err = esp_wifi_stop();
	if (err == ESP_ERR_WIFI_NOT_INIT) {
		return;
	}
	ESP_ERROR_CHECK(err);
	ESP_ERROR_CHECK(esp_wifi_deinit());
	ESP_ERROR_CHECK(
			esp_wifi_clear_default_wifi_driver_and_handlers(
					s_example_sta_netif));
	esp_netif_destroy(s_example_sta_netif);
	s_example_sta_netif = NULL;
}



esp_err_t example_wifi_sta_do_disconnect(void) {
	ESP_ERROR_CHECK(
			esp_event_handler_unregister(WIFI_EVENT,
					WIFI_EVENT_STA_DISCONNECTED,
					&inno_handler_on_wifi_disconnect));
	ESP_ERROR_CHECK(
			esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP,
					&inno_handler_on_sta_got_ip));
	ESP_ERROR_CHECK(
			esp_event_handler_unregister(WIFI_EVENT,
					WIFI_EVENT_STA_CONNECTED,
					&inno_handler_on_wifi_connect));

	if (s_semph_get_ip_addrs) {
		vSemaphoreDelete(s_semph_get_ip_addrs);
	}

	return esp_wifi_disconnect();
}

void connect_wifi_shutdown(void) {
	example_wifi_sta_do_disconnect();
	connect_wifi_stop();
}
//
//static char wifi_ssid_set[32];
//static char wifi_passwd_set[64];
//
//int set_wifi_cradentail(char *ssid, char *passwd) {
//	if (strlen(ssid) > 23 || strlen(passwd) > 63) {
//		return 0;
//	}
//	strcpy((char*) wifi_ssid_set, ssid);
//	strcpy((char*) wifi_passwd_set, ssid);
//	ESP_LOGI(TAG, "Setting wifi credentials %s, %s", ssid, passwd);
//	return 1;
//
//}

//static void event_handler(void* arg, esp_event_base_t event_base,
//                                int32_t event_id, void* event_data)
//{
//    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
//        esp_wifi_connect();
//    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
//        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
//            esp_wifi_connect();
//            s_retry_num++;
//            ESP_LOGI(TAG, "retry to connect to the AP");
//        } else {
//            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
//        }
//        ESP_LOGI(TAG,"connect to the AP fail");
//    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
//        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
//        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
//        s_retry_num = 0;
//        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
//    }
//}



//bool is_wifi_connected_getter() {
//    return is_wifi_connected;
//}





#endif /* CONFIG_INNO_CONNECT_WIFI */
