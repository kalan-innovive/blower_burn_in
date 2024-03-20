#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include "stdlib.h"
#include <stdbool.h>
#include "esp_types.h"
#include "esp_event.h"
#include "esp_err.h"
#include <cJSON.h>
//#include <cjson/cJSON.h>
typedef enum {
	MSG16_REQUEST,
	MSG16_RESPONSE,
	SETTINGS_REQUEST,
	SERVER_EH_RESPONSE,
	SERVER_EH_REQUEST,
	DB_REQUEST,
	DB_GET_PRE_POST_BURNIN,
	DB_SET_CALIBRATION,
	DB_LIST_BUCKETS,
	APP_EVENT_ID_LEN,
} eh_event_id_t;

typedef enum {
	MSG16_EVENT_REQUEST,
	MSG16_EVENT_RESPONSE,
	MSG16_EVENT_TIMEOUT,
	MSG16_EVENT_CONNECTED,
	MSG16_EVENT_DISCONECTED,
	MSG16_EVENT_QUEUE_FULL,
} serial_event_id_t;

typedef enum {
	CONSOLE_EVENT_REQ,
	CONSOLE_EVENT_RESPONSE,
	CONSOLE_EVENT_TIMEOUT,
	CONSOLE_EVENT_CONNECTED,
	CONSOLE_EVENT_DISCONECTED,
	CONSOLE_EVENT_QUEUE_FULL,
} console_event_id_t;

typedef enum {
	SETTINGS_EVENT_REQ,
	SETTINGS_EVENT_RESPONSE,
	SETTINGS_EVENT_CONNECTED,
	SETTINGS_EVENT_DISCONECTED,
	SETTINGS_EVENT_QUEUE_FULL,
} settings_event_id_t;

typedef enum {
	DB_RESP,
	DB_REQ,
	DB_RESP_UNPACKED
} db_event_t;

typedef enum {
	NAN_CMD = 0,
	PRE_POST_BURN,
	SET_CAL,
	CAL_30_MIN,
} server_event_t;

#define MAX_CALIBRATION_VALUE_LEN (10)

extern const char *cal_pre_post_burn_str;
extern const char *cal_30_min_str;
extern const char *qc_val_str;
extern const char *burnin_val_str;
extern const char *chip_id_str;
extern const char *cmd_str;
extern const char *node_name_str;

///**
// * @brief NMEA Parser Handle
// *
// */
//typedef void *nmea_parser_handle_t;
//
///**
// * @brief Configuration of NMEA Parser
// *
// */
//typedef struct {
//	struct {
//		uint32_t event_queue_size; /*!< UART event queue size */
//	} uart; /*!< UART specific configuration */
//} nmea_parser_config_t;
///**
// * @brief GPS object
// *
// */
//typedef struct {
//	int valid;
//} gps_t;

/*
 * Object to hold message values for resp from database
 *
 */
typedef struct {
	unsigned chipID;
	char *msg;
	uint8_t burnin_len;
	int vas_cal_val;
	int qc_cal_val;
	int num_burnin;
	int burnin_val[MAX_CALIBRATION_VALUE_LEN];
	int range;
	int result;
	char *result_msg;
} db_resp_pre_post_burnin_t;

typedef enum {
	IP_ADDR, 		// Data=char*
	SSID_STR,		// Data = char*
	WIFI_PASSWD,	// Data=char*
	MQTT_CONF,  	// Data=mqtt_conf_str*
	MQTT_STATUS,	// Data=bool
	MODBUS_POWER,	// Data=bool
	NODE_NAME,		// Data=char*
	SERIAL_STATUS,	// Data=bool
	BLT_STATUS		// Data=bool
} setting_descriptor_t;

/*
 * Object to hold Settings values
 *
 */
typedef struct {
	setting_descriptor_t type;
	void *data;
	void *config_struct;
	bool valid;

} settings_req_t;

const char* get_eh_event_id_string(eh_event_id_t event_id);

//typedef struct {
//    uint16_t type;
//    uint16_t dev_id;
//    uint16_t addr;
//    uint16_t len;
//    uint16_t* msg_val;
//} msg16_t;
typedef struct {
	int type;  // type dependint on the event id
	int msg_id;
	void *msg_struct;
	int valid;
} eh_event_t;

typedef struct {
	int msg_id;
	void (*respons_cb)(void *result);
	void *request;
} app_request_t;

typedef struct {
	char *json_msg;
	int id;
	void (*respons_cb)(app_request_t *result);
} queue_task_t;

void event_handler_task(void *pvParams);
esp_err_t init_event_handler();
esp_err_t deinit_event_handler();
esp_event_base_t get_event_handler_base(void);
esp_event_loop_handle_t get_event_handler_loop(void);
eh_event_t* server_eh_process_resp(eh_event_t *event);
esp_err_t db_pre_post_parser(cJSON *root, db_resp_pre_post_burnin_t **db_ppb);
void print_ppb(db_resp_pre_post_burnin_t *ppb);
void delete_event(eh_event_t *event);
int to_valuint(const cJSON *json);

#ifdef __cplusplus
}
#endif
