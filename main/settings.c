///*
//
// */
//
//#include <string.h>
//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
//#include "esp_log.h"
//#include "esp_check.h"
//#include "bsp_board.h"
//#include "nvs_flash.h"
//#include "nvs.h"
//#include "settings.h"
//
//static const char *TAG = "settings";
//
//#define NAME_SPACE "sys_param"
//#define NVS_NAMESPACE "wifi_credentials"
//
//#define KEY "param"
//
//static sys_param_t g_sys_param = {0};
////static wifi_credentials_t credentials = {0};
//
////TODO:: set wifi credentials from config and from cmd console
//
//static const wifi_credentials_t g_default_wifi_param = {
//		.ssid = "innowifi",
//		.password = "Inn0wifi!",
//};
//
//static const mqtt_credentials_t g_default_mqtt_param = {
//		.host = "mqtt.innovive.com",
//		.password = "innovive",
//		.node_name = "esp233",
//};
//
//static const net_settings_t g_default_net_param = {
//		.ipv4 = "255.255.255.255",
//		.password = "",
//		.mac_addr = "00-00-00-00-00-00",
//};
//
//static const server_settings_t g_default_eh_param = {
//		.eh_server = "eh.innovive.com",
//		.eh_encr = "",
//};
//
//static const app_burn_in g_default_burn_in_param = {
//		.TEB = 40,
//		.range = 15,
//		.num_cycles = 4,
//		.num_cycle_shown = 8,
//		.on_time = 10,
//		.off_time = 30,
//};
//
//static const node_com_ports g_default_comm_param = {
//    .mqtt_connected = 1,
//    .console_connected = 1,
//    .eh_server_connected = 1,
//    .bluetooth_connected = 1,
//    .wifi_connected = 1,
//};
//
//static const sys_param_t g_default_sys_param = {
//    .need_hint = 1,
//    .wifi = g_default_wifi_param,
//    .mqtt = g_default_mqtt_param,
//	.net = g_default_net_param,
//	.app_type = BLOWER_BURN_IN,
//	.eh = g_default_eh_param,
//	.burn_in = g_default_burn_in_param,
//	.comm_ports = g_default_comm_param,
//};
//
//
//
//// Save the WiFi credentials to NVS storage
//esp_err_t save_wifi_credentials(wifi_credentials_t *credentials)
//{
//    esp_err_t err = ESP_OK;
//
//    // Initialize NVS
//    err = nvs_flash_init();
//    if (err != ESP_OK) {
//        ESP_LOGE("NVS", "Error initializing NVS: %s", esp_err_to_name(err));
//        return err;
//    }
//
//    // Open NVS handle
//    nvs_handle_t nvs_handle;
//    err = nvs_open(NAME_SPACE, NVS_READWRITE, &nvs_handle);
//    if (err != ESP_OK) {
//        ESP_LOGE("NVS", "Error opening NVS handle: %s", esp_err_to_name(err));
//        return err;
//    }
//
//    // Write WiFi SSID to NVS
//    err = nvs_set_str(nvs_handle, "ssid", credentials->ssid);
//    if (err != ESP_OK) {
//        ESP_LOGE("NVS", "Error setting WiFi SSID: %s", esp_err_to_name(err));
//        return err;
//    }
//
//    // Write WiFi password to NVS
//    err = nvs_set_str(nvs_handle, "wifi_password", credentials->password);
//    if (err != ESP_OK) {
//        ESP_LOGE("NVS", "Error setting WiFi password: %s", esp_err_to_name(err));
//        return err;
//    }
//
//    // Commit changes to NVS
//    err = nvs_commit(nvs_handle);
//    if (err != ESP_OK) {
//        ESP_LOGE("NVS", "Error committing changes to NVS: %s", esp_err_to_name(err));
//        return err;
//    }
//
//    // Close NVS handle
//    nvs_close(nvs_handle);
//
//    return ESP_OK;
//}
//
//static esp_err_t settings_check(sys_param_t *param)
//{
//    esp_err_t ret;
//    ESP_GOTO_ON_FALSE(param->app_type < APP_TYPE_LEN, ESP_ERR_INVALID_ARG, reset, TAG, "Application selection incorrect");
//    return ret;
//reset:
//    ESP_LOGW(TAG, "Set to default");
//    memcpy(&g_sys_param, &g_default_sys_param, sizeof(sys_param_t));
//    return ret;
//}
//
//esp_err_t settings_read_parameter_from_nvs(void)
//{
//    nvs_handle_t my_handle = 0;
//    esp_err_t ret = nvs_open(NAME_SPACE, NVS_READONLY, &my_handle);
//    if (ESP_ERR_NVS_NOT_FOUND == ret) {
//        ESP_LOGW(TAG, "Not found, Set to default");
//        memcpy(&g_sys_param, &g_default_sys_param, sizeof(sys_param_t));
//        settings_write_parameter_to_nvs();
//        return ESP_OK;
//    }
//
//    ESP_GOTO_ON_FALSE(ESP_OK == ret, ret, err, TAG, "nvs open failed (0x%x)", ret);
//
//    size_t len = sizeof(sys_param_t);
//    ret = nvs_get_blob(my_handle, KEY, &g_sys_param, &len);
//    ESP_GOTO_ON_FALSE(ESP_OK == ret, ret, err, TAG, "can't read param");
//    nvs_close(my_handle);
//
//    settings_check(&g_sys_param);
//    return ret;
//err:
//    if (my_handle) {
//        nvs_close(my_handle);
//    }
//    return ret;
//}
//
//esp_err_t settings_write_parameter_to_nvs(void)
//{
//    ESP_LOGI(TAG, "Saving settings");
//    settings_check(&g_sys_param);
//    nvs_handle_t my_handle = {0};
//    esp_err_t err = nvs_open(NAME_SPACE, NVS_READWRITE, &my_handle);
//    if (err != ESP_OK) {
//        ESP_LOGI(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
//    } else {
//        err = nvs_set_blob(my_handle, KEY, &g_sys_param, sizeof(sys_param_t));
//        err |= nvs_commit(my_handle);
//        nvs_close(my_handle);
//    }
//    return ESP_OK == err ? ESP_OK : ESP_FAIL;
//}
//
//sys_param_t *settings_get_parameter(void)
//{
//    return &g_sys_param;
//}
