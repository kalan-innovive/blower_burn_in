/*
 * test_mqtt_handler.c
 *
 *  Created on: Jun 13, 2023
 *      Author: ekalan
 */
#include "esp_err.h"
#include "test_mqtt_handler.h"
#include "mqtt_handler.h"
#include "unity.h"
#include "app_event_handler.h"
#include "test_event_handler.h"

mqtt_handler_config_t app_cfg;
static const char *TAG = "test_mqtt";

/**
 * --------------------------**
 * Prototypes STATIC
 * --------------------------**
 */
static esp_err_t test_create_esp_subscriptions(void);
static esp_err_t test_mqtt_handler_send_db_request(void);
static esp_err_t test_mqtt_handler_init(void);
static esp_err_t system_test_setup(void);
static esp_err_t test_server_set_calibration_val(void);

/**
 * Unit Test
 * @brief
 *  - Run Unit tests
 *  - Results are printed to serial console
 *  - Requires only the current module
 */
void run_mqtt_unit_tests(void) {

	T_LOG(TAG, "Testing test_create_esp_subscriptions");
	ESP_ERROR_CHECK(test_create_esp_subscriptions());

	T_LOG(TAG, "%s, Testing test_mqtt_handler_send_db_request", __FUNCTION__);
	ESP_ERROR_CHECK(test_mqtt_handler_send_db_request());

	T_LOG(TAG, "%s, Testing test_server_set_calibration_val", __FUNCTION__);
	ESP_ERROR_CHECK(test_server_set_calibration_val());
	vTaskDelay(500 / portTICK_PERIOD_MS);
// The assertions are inside the mock functions.
// If something goes wrong an assertion will fail.
}

/**
 * System Test
 * @brief
 *  - Run System tests
 *  - Results are printed to serial console
 *  - Requires Event handler
 */
void run_mqtt_system_tests(void) {
	// Testing setup
	T_LOG(TAG, "%s, Testing system_test_setup", __FUNCTION__);
	ESP_ERROR_CHECK(system_test_setup());
	vTaskDelay(500 / portTICK_PERIOD_MS);

	// Initializing mqtt client with default configuration
	T_LOG(TAG, "%s, Testing test_mqtt_handler_init", __FUNCTION__);
	ESP_ERROR_CHECK(test_mqtt_handler_init());
	vTaskDelay(3000 / portTICK_PERIOD_MS);

	// Testing the send_db_request to server and the response with event handler
	T_LOG(TAG, "%s, Testing test_mqtt_handler_send_db_request", __FUNCTION__);
	ESP_ERROR_CHECK(test_mqtt_handler_send_db_request());
	vTaskDelay(500 / portTICK_PERIOD_MS);

	T_LOG(TAG, "%s, Testing test_server_set_calibration_val", __FUNCTION__);
	ESP_ERROR_CHECK(test_server_set_calibration_val());

	vTaskDelay(500 / portTICK_PERIOD_MS);

}

/**
 * System Test Initializer
 * @brief
 *  - Sets up environment for needed in system tests
 */
static esp_err_t system_test_setup(void) {
	// Start the user event loop
//	esp_log_level_set("*", ESP_LOG_WARN);
//	esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
//	esp_log_level_set(TAG, ESP_LOG_VERBOSE);
//	esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
//	esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
//	esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
//	esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);
//	esp_log_level_set("event", ESP_LOG_INFO);
//	esp_log_level_set(TAG, ESP_LOG_INFO);
//	esp_log_level_set("user_event_loop", ESP_LOG_DEBUG);
//	esp_log_level_set("test_eh", ESP_LOG_DEBUG);
//	esp_log_level_set("wifi", ESP_LOG_INFO);
//	esp_log_level_set("GC_task", ESP_LOG_INFO);

// Create the default event loop
	init_event_handler();

	return ESP_OK;

}

/**
 * @ brief Tests creating a topic list for the mqtt driver
 * Passes if all strings sent to function are created correctly durring return
 * Create subscriptions is used to create dynamic strings if the esp_name is changed
 *
 */
static esp_err_t test_create_esp_subscriptions(void) {

	T_LOG(TAG, "\t\t%s : Starting--\n", __FUNCTION__);
	esp_err_t err = ESP_OK;
	unsigned int userNode = 1;
	const char *topics[] = { "up", "down", "ping" };
	size_t numTopics = sizeof(topics) / sizeof(topics[0]);

	char **subscribed_topics = create_esp_subscriptions(userNode, topics,
			numTopics);

	// Check each of the returned topics
	for (size_t i = 0; i < numTopics; i++) {
		char expected_topic[32];  // buffer to store the expected topic
		snprintf(expected_topic, sizeof(expected_topic), "esp%d/%s", userNode,
				topics[i]);

		_INNO_ASSERT_EQUAL_STRING_MESSAGE(expected_topic, subscribed_topics[i],
				"Generated topic");
	}
	for (size_t i = 0; i < numTopics; i++) {
		free((void*) subscribed_topics[i]);
	}
	free(subscribed_topics);
	T_LOG(TAG, "\t\t%s : Finished; Error%d--\n", __FUNCTION__, err);

	return ESP_OK;
// TODO: Don't forget to free the memory allocated by create_esp_subscriptions() if it was dynamically allocated.
}

/**
 * System Test
 * @brief
 *  - test the Creation and connection to server
 *  Passes if the client can connect to the server
 */
static esp_err_t test_mqtt_handler_init(void) {
	T_LOG(TAG, "%s :App Config at=%p", __FUNCTION__, (void* )&app_cfg);

	app_cfg.config_type = CONFIG_TYPE_DEFAULT;
	app_cfg.event_base = get_event_handler_base();
	app_cfg.eh_handler = get_event_handler_loop();

	esp_err_t err;
	err = setup_mqtt_default(&app_cfg);
	err = setup_mqtt_config(&app_cfg);

	_INNO_TEST_ASSERT(err != ESP_OK, __LINE__, "Setup returned error ");
	_INNO_TEST_ASSERT(app_cfg.mqtt_config != NULL, __LINE__,
			"Setup returned error");
	/* TODO: close the server down after */

	T_LOG(TAG, "\t\t____setup_mqtt_default : Finished____\n");
	return err;

}

/**
 * System Test and unit test
 * @brief
 *  - test the send receive of db message
 *  Test requires that there is a connection to the mqtt server that is configured
 *  and running correlated pytest for response
 *
 *   - Initiates message to the server
 *   - Reads the response sends to the event handler
 *   - verifies that the message was handled correctly
 *
 */
static esp_err_t test_mqtt_handler_send_db_request(void) {
	esp_err_t err = ESP_OK;
	// Start create an event to send to the server

	// Send the request to server expects the json string back representing the information
	err = request_ppb_vals(482184725);

	//Check the handler output is correct and has no errors

	return err;
}

/**
 * System Test and unit test
 * @brief
 *  - test the send set cal val
 *  Test requires that there is a connection to the mqtt server that is configured
 *  and running correlated pytest for response
 *
 *   - Initiates message to the server
 *   - verifies that the message was formated correctly
 *
 */
static esp_err_t test_server_set_calibration_val(void) {

	// Call the set_calibration_val function
	esp_err_t err = set_calibration_val(482184725, -36);

	return err;
}

/**
 * System Test
 * @brief
 *  - Run system tests
 *  - Results are printed to serial console
 *  - Requires that the event handler is workingg
 */
void test_setup_mqtt_system(void) {
//	system_test_setup(&app_cfg);
	vTaskDelay(500 / portTICK_PERIOD_MS);

	// Testing the initialization of the function
	ESP_ERROR_CHECK(test_mqtt_handler_init());
	T_LOG(TAG, "%s, Testing test_db_pre_post_parser", __FUNCTION__);
	vTaskDelay(500 / portTICK_PERIOD_MS);
	// Testing the sending and receiving functions
	if (test_create_esp_subscriptions() != ESP_OK) {
		return;
	}
	vTaskDelay(500 / portTICK_PERIOD_MS);

// The assertions are inside the mock functions.
// If something goes wrong an assertion will fail.
}

