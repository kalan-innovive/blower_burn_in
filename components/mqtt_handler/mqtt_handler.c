#include "mqtt_handler.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_mac.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "app_event_handler.h"
#include "esp_heap_caps.h"
#include <cJSON.h>

// Assuming we have an event base defined as:
//ESP_EVENT_DEFINE_BASE(MY_CUSTOM_EVENT_BASE);

//// And an event with the id
//typedef enum {
//    DB_REQUEST,
//    // more events here
//} my_custom_event_id_t;

static const char *topic_notify = "esp/up";
static const char *topic_last_will = "esp/down";
static const char *topic_db_set = "eh/set";
static char espname[15] = "esp000";

static int is_mqtt_connected = 0;

//static char node_name[25];

const char* createNodeNameTopic(const char *userNode, const char *topic);
static void free_topic_arrays(const char **nodeNames, size_t len);

static const char *TAG = "mqtt_handler";
static esp_mqtt_client_handle_t client;

/**
 * Send request for pre post burn in vals
 *
 */
esp_err_t request_ppb_vals(unsigned chipID) {
	const char *ppb_jsn =
			"{\"node_name\": \"%s\", \"cmd\": \"calibration/pre_post_burnin\", \"chip_id\": %u}";
	int ppb_jsn_len = strlen(ppb_jsn);
	const char *topic = "esp/get";
	char msg[100];
	int msg_id = 0;
	int len = sprintf(msg, ppb_jsn, espname, chipID);

	ESP_LOGI(TAG, "sending ppb request; topic=%s, msg=%s msg_id=%d",
			topic, msg, msg_id);

	if (is_mqtt_connected) {
		msg_id = esp_mqtt_client_publish(client, topic, msg, 0, 1, 0);
	}

	ESP_LOGI(TAG, "Sent ppb request; topic=%s, msg=%s msg_id=%d",
			topic, msg, msg_id);

	return (len > ppb_jsn_len) ? ESP_OK : ESP_FAIL;

}

/**
 * Send server event handler calibration val
 *
 */
esp_err_t set_calibration_val(unsigned chipID, int val) {
	const char *cal_jsn =
			"{\"node_name\": \"%s\", \"cmd\": \"calibration\", \"chip_id\": %u, \"value\":%d}";
	int cal_jsn_len = strlen(cal_jsn);
	const char *topic = "esp/set";
	char msg[150];
	int msg_id = 0;
	int len = sprintf(msg, cal_jsn, espname, chipID, val);

	ESP_LOGI(TAG, "sending cal_jsn request; topic=%s, msg_id=%d, msg=%s ",
			topic, msg_id, msg);

	if (is_mqtt_connected) {
		msg_id = esp_mqtt_client_publish(client, topic, msg, 0, 1, 0);
	}

	ESP_LOGI(TAG, "Sent ppb request; topic=%s, msg=%s msg_id=%d",
			topic, msg, msg_id);

	return (len > cal_jsn_len) ? ESP_OK : ESP_FAIL;

}

/**
 * Send server event handler calibration val
 *
 */
esp_err_t set_cal_burnin_val(unsigned chipID, int val) {
	const char *cal_jsn =
			"{\"node_name\": \"%s\", \"cmd\": \"calibration/burn_in_val\", \"chip_id\": %u, \"value\":%d}";
	int cal_jsn_len = strlen(cal_jsn);
	const char *topic = "esp/set";
	char msg[150];
	int msg_id = 0;
	int len = sprintf(msg, cal_jsn, espname, chipID, val);

	ESP_LOGI(TAG, "sending cal_jsn request; topic=%s, msg_id=%d, msg=%s ",
			topic, msg_id, msg);

	if (is_mqtt_connected) {
		msg_id = esp_mqtt_client_publish(client, topic, msg, 0, 1, 0);
	}

	ESP_LOGI(TAG, "Sent ppb request; topic=%s, msg=%s msg_id=%d",
			topic, msg, msg_id);

	return (len > cal_jsn_len) ? ESP_OK : ESP_FAIL;

}

/**
 * Log error nonzero message
 */
static void log_error_if_nonzero(const char *message, int error_code) {
	if (error_code != 0) {
		ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
	}
}

const char* create_const_str(const char *str) {
	if (str == NULL) {
		return NULL;
	}

	size_t length = strlen(str) + 1;  // +1 to include null terminator
	char *buffer = (char*) (malloc(length * sizeof(char)));

	if (buffer != NULL) {
		strcpy(buffer, str);
	}

	return buffer;
}

const char* node_up_message(const mqtt_handler_config_t *config) {
	if (config == NULL || config->node_name == NULL || config->ver == NULL) {
		return NULL; // Unable to construct the message
	}

	cJSON *root = cJSON_CreateObject();
	if (root == NULL) {
		return NULL; // Unable to create the JSON object
	}

	cJSON_AddStringToObject(root, "node_name", config->node_name);
	cJSON_AddStringToObject(root, "prog", config->prog_name);
	cJSON_AddStringToObject(root, "ver", config->ver);
	cJSON_AddStringToObject(root, "status", config->ver);

	char *json_string = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);

	return json_string;
}

void print_mqtt_handler_config(const mqtt_handler_config_t *config) {
	if (config == NULL) {
		ESP_LOGI(TAG, "Config is NULL.");
		return;
	}

	ESP_LOGI(TAG, "Config Type: %d", config->config_type);
	ESP_LOGI(TAG, "Node Number: %u", config->node_number);

	if (config->node_name != NULL) {
		ESP_LOGI(TAG, "Node Name: %s", config->node_name);
	} else {
		ESP_LOGW(TAG, "Node Name: NULL");
	}

	if (config->up_topic != NULL) {
		ESP_LOGI(TAG, "Up Topic: %s", config->up_topic);
	} else {
		ESP_LOGW(TAG, "Up Topic: NULL");
	}

	if (config->last_will_topic != NULL) {
		ESP_LOGI(TAG, "Last Will Topic: %s", config->last_will_topic);
	} else {
		ESP_LOGW(TAG, "Last Will Topic: NULL");
	}

	if (config->eh_topic != NULL) {
		ESP_LOGI(TAG, "Event Handler Topic: %s", config->eh_topic);
	} else {
		ESP_LOGW(TAG, "Event Handler Topic: NULL");
	}

	if (config->ver != NULL) {
		ESP_LOGI(TAG, "Version: %s", config->ver);
	} else {
		ESP_LOGW(TAG, "Version: NULL");
	}
	if (config->prog_name != NULL) {
		ESP_LOGI(TAG, "Prog: %s", config->prog_name);
	} else {
		ESP_LOGW(TAG, "Prog: NULL");
	}

	ESP_LOGI(TAG, "Subscription Topics:");
	for (size_t i = 0; i < config->sub_topic_len; ++i) {
		if (config->sub_topics[i] != NULL) {
			ESP_LOGI(TAG, "%d: %s", i, config->sub_topics[i]);
		} else {
			ESP_LOGW(TAG, "%d: NULL", i);
		}
	}
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
		int32_t event_id, void *event_data) {

	ESP_LOGD(TAG,
			"Event dispatched from event loop base=%s, event_id=%" PRIi32 "",
			base, event_id);

//	// Unpack vaiables for app_event handler
//	mqtt_handler_config_t *hand_conf = (mqtt_handler_config_t*) handler_args;
	// App configuration variables

	vTaskDelay(0);

	esp_mqtt_event_handle_t event = event_data;
	esp_mqtt_client_handle_t client = event->client;
	int msg_id;

	// de construct eh handler
	mqtt_handler_config_t *app_conf = handler_args;

	esp_event_base_t event_base = app_conf->event_base;
	char *node_name = app_conf->node_name;
	const char **sub_topics = app_conf->sub_topics;
	eh_handler_t eh_handle = app_conf->eh_handler;

	// EVENT Processing
	switch ((esp_mqtt_event_id_t) event_id) {
		case MQTT_EVENT_CONNECTED:

			ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
			print_mqtt_handler_config(app_conf);
			is_mqtt_connected = 1;

			// Publish on the notify topic to let subscribers know the device is up
			const char *up_msg = node_up_message(app_conf);
			app_conf->up_message = (char*) up_msg;
			msg_id = esp_mqtt_client_publish(client, app_conf->up_topic, up_msg,
					0,
					1, 0);
			ESP_LOGI(TAG,
					"Sent up topic message,topic=%s, payload=%s msg_id=%d",
					app_conf->up_topic, up_msg, msg_id);

			// Create the publish list of topics in the app_handler config
			for (int i = 0; i < app_conf->sub_topic_len; i++) {
				const char *t = sub_topics[i];
				ESP_LOGI(TAG, "Subscribed to topic=%s", t);
				esp_mqtt_client_subscribe(client, t, 0);
			}
			break;

		case MQTT_EVENT_DISCONNECTED:
			ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
			is_mqtt_connected = 0;

			// Free the array that was created in the connect event;
//		free_topic_arrays(hand_conf->post_topics, hand_conf->post_topic_len);

			break;

		case MQTT_EVENT_SUBSCRIBED:
			ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
//		msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0,
//				0);
//		ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
			break;

		case MQTT_EVENT_UNSUBSCRIBED:
			ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
			break;

		case MQTT_EVENT_PUBLISHED:
			ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
			break;

		case MQTT_EVENT_DATA:
			ESP_LOGI(TAG, "MQTT_EVENT_DATA");
			ESP_LOGI(TAG, "TOPIC=%.*s", event->topic_len, event->topic);
			ESP_LOGI(TAG, "DATA=%.*s", event->data_len, event->data);

			// All communication responses that are json are on esp#
			if (strncmp(event->topic, app_conf->node_name, event->topic_len)
					== 0) {

				// Create new event
				/*TODO: Make sure this is freed*/
				int len = event->data_len;
				char *json_msg = malloc(sizeof(char) * (len + 1));
				memcpy(json_msg, event->data, len);
				json_msg[(size_t) len] = '\0';
				ESP_LOGI(TAG, "%s, [%d] msg=%s", __func__, __LINE__, json_msg);

				eh_event_t e = {
						.type = DB_RESP,
						.msg_id = event->msg_id,
						.msg_struct = (void*) json_msg,
						.valid = 1,
				};
				esp_event_post_to(eh_handle, event_base, SERVER_EH_RESPONSE,
						&e, sizeof(e), portMAX_DELAY);

			} else if (strncmp(event->topic, sub_topics[PING_TOPIC],
					event->topic_len)
					== 0) {
				char pong_topic[25];
				sprintf(pong_topic, "%s/pong", espname);
				esp_mqtt_client_publish(client, pong_topic, "pong", 0, 1, 0);
			}
			break;
		case MQTT_EVENT_BEFORE_CONNECT:
			ESP_LOGI(TAG, "%s, MQTT_EVENT_BEFORE_CONNECT:", __FUNCTION__);

			ESP_LOGI(TAG, "%s :App Config    : %p", __FUNCTION__,
					(void* )app_conf);
			if (app_conf == NULL) {
				ESP_LOGE(TAG, "%s, App Config pointer is NULL", __FUNCTION__);

				break;
			}
			ESP_LOGI(TAG, "%s, Event handle=:%p", __FUNCTION__,
					(void* )eh_handle);
			ESP_LOGI(TAG, "%s, event base=:%s", __FUNCTION__, event_base);
			ESP_LOGI(TAG, "%s, Node name=:%s", __FUNCTION__, node_name);
			break;

		case MQTT_EVENT_ERROR:
			ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
			if (event->error_handle->error_type
					== MQTT_ERROR_TYPE_TCP_TRANSPORT) {
				log_error_if_nonzero("reported from esp-tls",
						event->error_handle->esp_tls_last_esp_err);
				log_error_if_nonzero("reported from tls stack",
						event->error_handle->esp_tls_stack_err);
				log_error_if_nonzero("captured as transport's socket errno",
						event->error_handle->esp_transport_sock_errno);
				ESP_LOGI(TAG, "Last errno string (%s)",
						strerror(
								event->error_handle->esp_transport_sock_errno));

			}
			break;

		default:
			ESP_LOGI(TAG, "Other event id:%d", event->event_id);
			break;
	}
}

/**
 * @brief sets up the mqtt service
 * @param mqtt_handler_config_t *app_mqtt_cfg configurattion to set up
 * @return esp_err_t
 */
esp_err_t setup_mqtt_default(mqtt_handler_config_t *app_cfg) {
//	heap_trace_start();
	esp_err_t err = ESP_OK;
//	esp_mqtt_client_config_t *m_cfg;
//	esp_mqtt_client_config_t m_configure;
//	char espname[15];
//	char last_will_msg[30];

	ESP_LOGI(TAG, "free heap size is %" PRIu32 ", minimum %" PRIu32,
			esp_get_free_heap_size(), esp_get_minimum_free_heap_size());

	// Check if we are using the default configuration
	if (app_cfg->config_type == CONFIG_TYPE_DEFAULT) {
		ESP_LOGI(TAG, "Setting up default mqt handler config");
		// Set client id from mac
		uint8_t mac[8];
		ESP_ERROR_CHECK(esp_base_mac_addr_get(mac));
		unsigned ssid = 0;
		for (int i = 0; i < 5; i++) {
			ssid += mac[i];
			ESP_LOGI(TAG, "mac[%d]=%x ssid=%x", i, mac[i], ssid);
			//* TODO: add configuration event for the ui handler

		}
		//	char client_id[64];
		unsigned int node_num = (ssid) & 0xff;
		node_num += (node_num < 2) ? 2 : 0;
//		uint8_t node_num = 1;
		ESP_LOGI(TAG, "%s : SSID=%u", __FUNCTION__, node_num);

		sprintf(espname, "esp%d", node_num);
		app_cfg->node_name = malloc(sizeof(char) * (strlen(espname) + 1));
		strcpy(app_cfg->node_name, espname);
		ESP_LOGI(TAG, "%s : Node Name=[%s]", __FUNCTION__, app_cfg->node_name);

		app_cfg->node_number = node_num;
		app_cfg->up_topic = (char*) topic_notify;
		app_cfg->last_will_topic = (char*) topic_last_will;
		app_cfg->eh_topic = (char*) topic_db_set;
		app_cfg->prog_name = "burn_in";
		app_cfg->ver = "1.0.0";

		// Create the sub topic list
		const char *topics[] = { "", "info", "update", "ping", "config" };
		size_t numTopics = sizeof(topics) / sizeof(topics[0]);
		ESP_LOGI(TAG, "%s :numTopics=[%d]", __FUNCTION__, numTopics);

		app_cfg->sub_topics = create_esp_subscriptions(node_num, topics,
				numTopics);
		if (app_cfg->sub_topics == NULL) {
			ESP_LOGE(TAG, "Memory allocation failed\n");
			return ESP_FAIL;
		}
		ESP_LOGI(TAG, "%s :Created sub_topics at=%p", __FUNCTION__,
				(void* )app_cfg->sub_topics);

		app_cfg->sub_topic_len = numTopics;

		// Copy the configuration to mqtt_config pointer
		app_cfg->mqtt_config = (esp_mqtt_client_config_t*) malloc(
				sizeof(esp_mqtt_client_config_t));
	}

	// Configuration basic info
	const char *username = "innovive";
	const char *client_id = espname;
	const char *password = "innovive";
	const char *last_will_topic = app_cfg->last_will_topic;
	const char *last_will_msg = espname;
//	const char *uri = "mqtt://mqtt.innovive.com";
	const char *uri = "mqtt://innovive:innovive@mqtt.innovive.com";
	int last_will_msg_len = strlen(last_will_msg);

	// Client configuration step
	esp_mqtt_client_config_t
	mqtt_cfg = {
			.broker = {
					.address = {
							.uri = uri,
					},
			},
			.credentials = {
					//					.username = username,
					.client_id = client_id,
			//					.authentication = {
//							.password = password,
//					},
					},
			.session = {
					.last_will = {
							.topic = last_will_topic,
							.msg = last_will_msg,
							.msg_len = last_will_msg_len,
							.qos = 1,
							.retain = true,
					},
					.disable_clean_session = false,
					.keepalive = 5,
					//				.protocol_ver= MQTT_PROTOCOL_V_3_1_1,
					.message_retransmit_timeout = 3,
			},
			.network = {
					.disable_auto_reconnect = false,
			}

	};

// Client initialization
//	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
	client = esp_mqtt_client_init(&mqtt_cfg);

	if (client == NULL) {
		ESP_LOGE(TAG, "%s: client Memory allocation failed\n", __FUNCTION__);
		return ESP_FAIL;
	}

	memcpy(app_cfg->mqtt_config, &mqtt_cfg, sizeof(esp_mqtt_client_config_t));
	vTaskDelay(100 / portTICK_PERIOD_MS);

	ESP_LOGI(TAG, "%s :Init mqtt_config: %p", __FUNCTION__, (void* )client);
	ESP_LOGI(TAG, "%s :App Config at : %p", __FUNCTION__, (void* )app_cfg);

	/* TODO: the last arg should be the custom event handler so that the mqtt handler knows where to send messages
	 *
	 */
	err = esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID,
			mqtt_event_handler, app_cfg);
	ESP_LOGI(TAG, "%s : esp_mqtt_client_register_event: err=[%d]", __FUNCTION__,
			err);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s : Error Registering MQTT event: err=%s", __FUNCTION__,
				esp_err_to_name(err));
		return err;
	}

	// Start the mqtt client
	err = esp_mqtt_client_start(client);
	ESP_LOGI(TAG, "%s : esp_mqtt_client_start: err=[%d]", __FUNCTION__, err);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "%s : Error Starting MQTT client: err=%s", __FUNCTION__,
				esp_err_to_name(err));
		return err;
	}

	return err;
}

/*TODO: change to accespt app_config and check if the default was used to free memory;*/
esp_err_t teardown_mqtt(esp_mqtt_client_handle_t client) {
	esp_err_t err = ESP_OK;

	err = esp_mqtt_client_stop(client);
	if (err != ESP_OK) {
		return err;
	}
	err = esp_mqtt_client_destroy(client);
	if (err != ESP_OK) {
		return err;
	}
//	heap_trace_stop();
	return err;
}

/**
 * @brief creates a const char* from node nameand appends a topic
 * @param mqtt_handler_config_t *app_mqtt_cfg configurattion to set up
 * @return esp_err_t
 */
const char* createNodeNameTopic(const char *userNode, const char *topic) {
	const char *prefix = "esp";
	size_t prefixLength = strlen(prefix);
	size_t userNodeLength = strlen(userNode);
	size_t topicLength = strlen(topic);
	size_t totalLength = prefixLength + userNodeLength + topicLength + 1; // +1 for null terminator

	char *nodeName = (char*) malloc(totalLength * sizeof(char));
	if (nodeName == NULL) {
		ESP_LOGE(TAG, "Memory allocation failed\n");
		exit(1);
	}

	strcpy(nodeName, prefix);
	strcat(nodeName, userNode);
	strcat(nodeName, topic);

	return nodeName;
}

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
		const char **topics, size_t numTopics) {
	const char *t_pnt = *topics;
	int ntopic = numTopics;

	ESP_LOGI(TAG, "%s : %d, %p, %d", __FUNCTION__, userNode, (void* ) &t_pnt,
			ntopic);
	vTaskDelay(10 / portTICK_PERIOD_MS);

	const char *prefix = "esp";
	size_t prefixLength = strlen(prefix);

	const char **nodeNames = (const char**) malloc(
			numTopics * sizeof(const char*));
	if (nodeNames == NULL) {
		ESP_LOGE(TAG, "Memory allocation failed\n");
		return NULL;
	}
	vTaskDelay(500 / portTICK_PERIOD_MS);

	for (size_t i = 0; i < numTopics; i++) {

		char userNodeStr[11]; // Maximum length of unsigned int (10 digits) + 1 for null terminator
		snprintf(userNodeStr, sizeof(userNodeStr), "%u/", userNode);

		size_t userNodeLength = strlen(userNodeStr);
		size_t topicLength = strlen(topics[i]);
		size_t totalLength = prefixLength + userNodeLength + topicLength + 1; // +1 for null terminator

		char *nodeName = (char*) malloc(totalLength * sizeof(char));
		if (nodeName == NULL) {
			ESP_LOGE(TAG, "%s : Memory allocation failed", __FUNCTION__);
//			fprintf(stderr, "Memory allocation failed\n");
			return NULL;
		}
		if (topicLength == 0) {
			sprintf(nodeName, "esp%u", userNode);

		} else {
			strcpy(nodeName, prefix);
			strcat(nodeName, userNodeStr);
			strcat(nodeName, topics[i]);
		}

		nodeNames[i] = nodeName;
		ESP_LOGI(TAG, "%s : Added [%d], %s", __FUNCTION__, i, nodeNames[i]);
	}

	return nodeNames;
}

static void free_topic_arrays(const char **nodeNames, size_t len) {
	for (size_t i = 0; i < len; i++) {
		free((void*) nodeNames[i]);
	}
	free(nodeNames);
}
