#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_types.h"
#include "esp_event.h"
#include "esp_err.h"
#include "mqtt_client.h"

// Assuming we have a type defined as:
typedef void (*eh_handler_t)(void);

typedef void (*mqtt_config)(void);

// Enum for the config type
typedef enum {
	CONFIG_TYPE_CUSTOM,
	CONFIG_TYPE_DEFAULT,
	CONFIG_TYPE_NVME,
} config_type_t;

typedef enum {
	RESP_TOPIC,
	INFO_TOPIC,
	UPDATE_TOPIC,
	PING_TOPIC,
	CONFIG_TOPIC,
} sub_topic_t;

typedef enum {
	STATUS_UP,
	STATUS_DOWN,
	STATUS_BUSY,
	PING_RESTARTING,
	STATUS_UNKOWN,
} client_status_t;

// The MQTT handler configuration structure
typedef struct {
	config_type_t config_type;   // Config type: either custom, default, or NVME
	eh_handler_t eh_handler;                 // Pointer to an event handler
	esp_mqtt_client_config_t *mqtt_config; // Pointer to an MQTT client configuration structure
	esp_event_base_t event_base;             // Event base for ESP events
	unsigned node_number;
	char *node_name;
	char *up_topic;
	char *up_message;
	char *last_will_topic;
	char *eh_topic;
	const char **sub_topics;
	const int *sub_topic_ids;
	size_t sub_topic_len;
	int status;

	const char *prog_name;
	const char *ver;

} mqtt_handler_config_t;

/**
 * @brief Desstrowys the mqtt task and cleans up
 *
 * @param esp_mqtt_client_handle_t - client handle to destroy
 *
 * @return esp_err_t
 *  - ESP_OK: Success
 *  - ESP_ERR_INVALIG_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 *
 */
esp_err_t teardown_mqtt(esp_mqtt_client_handle_t client);

/**
 * @brief Creates a topic list for esp given an array of topics and the espname
 *
 * @param unsigned userNode- node number 5 ->esp5
 * @param const char **topics topic array to concat  "up" -> esp5/up
 * @param size_t numTopics
 *
 *
 * @return const char**
 *  - Array of completed topics
 */
const char** create_esp_subscriptions(unsigned int userNode,
		const char **topics, size_t numTopics);

esp_err_t setup_mqtt_default(mqtt_handler_config_t *app_cfg);
esp_err_t request_ppb_vals(unsigned chipID);
esp_err_t set_calibration_val(unsigned chipID, int val);

#ifdef __cplusplus
}
#endif
