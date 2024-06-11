/*
 * serial_inno_cmd.c
 *
 *  Created on: Feb 26, 2024
 *      Author: ekalan
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "esp_log.h"
#include "esp_console.h"
#include "esp_chip_info.h"
#include "esp_sleep.h"
#include "esp_flash.h"
#include "driver/rtc_io.h"
#include "driver/uart.h"
#include "argtable3/argtable3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "system_cmd.h"
#include "sdkconfig.h"
#include "cJSON.h"
#include "serial_inno.h"
#include "serial_inno_cmd.h"



static void register_check_id(void);
//static void register_get_chipid(void);
//static void register_get_rawpressure(void);
//static void register_get_registers(void);
//static void register_get_blowertype(void);
//static void register_get_connecteddevices(void);

static void register_check_id_moc(void);
static void register_get_dev_info(void);
static void register_run_cal_test(void);
static void register_check_id();


void register_serialinno(void){
	register_check_id();
	register_check_id_moc();
//	register_get_chipid();
//	register_get_rawpressure();
//	register_get_registers();
//	register_get_blowertype();
//	register_get_connecteddevices();


}
//	static const char *return = "NOPE\n";

static char* flag_none= "None";
static char* flag_timout= "error:timeout";
static char* flag_transact= "error:transaction";
static char* flag_badarg = "error:bad_arg";

/**
 * Parses blower version args into a blower_version struct
 * returns 0 for passed and 1 for fail
 * sets the flag to badarg if incorrect argument
 */
static int parse_blower_version_args(cJSON *json, blower_version *b_ver){
	if (!b_ver || !json ) {
		printf("Error: Parsing JSON.\n");
		// add the error flag to the JSON

		return 1; // Return an error
	}
	int ret = 0;
	// Check required args
	cJSON *id = cJSON_GetObjectItemCaseSensitive(json, "id");
	if (!cJSON_IsNumber(id) ) {
		cJSON_AddStringToObject(json, "flag", flag_badarg);
		ret = 1;
	}
	b_ver->devid = 0;
	// Sets all values to default
	b_ver->hw_ver = -1;
	b_ver->sw_ver_maj = -1;
	b_ver->sw_ver_min = -1;
	b_ver->p_sensor_type = -1;
	b_ver->blower_type = -1;
	b_ver->is_updated = 0;
	b_ver->valid = 0;
	return ret;
}

/**
 * Expected Json request
 *  arg required = {"id": 1}
 *  arg preffered = {"cmd": "check_version", "id": 1, "msg_id":435}
 * response sent through printf()
 * resp_suc: {"cmd": "check_version", "id": 1,  "hw_version":0xELE107, "sw_ver_maj":10,
 * 		"sw_ver_minor":9, "sw_version":"F10.9", "blower_type":"Fan", "sensor_type":"HSC2", "flag":"None", "msg_id":435}
 * resp_timeout: {"cmd": "check_version", "id": 1, "msg_id":435, "flag":"timeout"}
 */
static int check_version(int argc, char **argv)
{

	static char* ret_str = NULL;
	int ret = 0;
	int comm_ret = 0;
	unsigned val = 0;


	ret = parse_blower_version_args();
	blower_version b_ver;


	if (argc != 2) {
		printf("Error: Incorrect number of arguments.\n");
		return 1; // Return an error
	}

	// Check that we can load the argument into a json object
	cJSON *json = cJSON_Parse(argv[1]);
	ret = parse_blower_version_args(json, &b_ver);
	if (ret) {



	}
	comm_ret = get_hw_version(b_ver, &val);
	if (comm_ret ) {
		b_ver.hw_ver = (int) val;
	}

	comm_ret = get_sw_version(b_ver, &val);
	if (comm_ret ) {
		b_ver.sw_ver_maj = (int) (val && 0xFF);
	}
	comm_ret = get_sw_version(b_ver, &val);
	if (comm_ret ) {
		b_ver.sw_ver_min = (int) (val && 0xFF);
	}




	if (uuid_json == NULL && !cJSON_IsNumber(uuid_json)) {

		printf("Warning: Expected uuid as a number.\n");
	} else {
		uuid = uuid_json->valueint;
		printf("   Updating uuid:%u\n", (unsigned) uuid);

		set_uuid(devid, uuid);
		get_uuid(devid, &uuid);
	}


	devid = id->valueint;


	// send the message wit for response.
	ret = check_dev_id(devid);
	ret1 = get_chipid(devid, &chipID);
	ret2 = get_raw_pressure(devid, &offset);


//	float raw_avg = 0;
//	for (int i=0;i<5;i++) {
//		ret2 = get_raw_pressure(devid, &offset);
//		raw_avg+=offset;
//		if (ret2) {
//			break;
//
//		}
//		raw_avg+=offset;
//	}

//	ret2 = get_raw_pressure(devid, &offset);

	if (ret == 1 && ret1 == 1 && ret2 == 1){

		sprintf(str_chipid, "%u", chipID);
		blowerinfo.devid = devid;
		blowerinfo.chipid = chipID;
		blowerinfo.offset = offset;
		blowerinfo.valid = 1;
		blowerinfo.uuid = uuid;


		cJSON_AddStringToObject(json, "chipID", str_chipid);
		cJSON_AddNumberToObject(json, "offset", offset);
		cJSON_AddStringToObject(json, "flag", flag_none);

	}
	else if (ret == -1 || ret1 == -1 || ret2 == -1){
		blowerinfo.valid = 0;
		blowerinfo.is_updated = 1;
		cJSON_AddStringToObject(json, "chipID", "");
		cJSON_AddNumberToObject(json, "offset", 0);
		cJSON_AddStringToObject(json, "flag", flag_transact);

	}
	else if (ret == 0 || ret1 == 0 || ret2 == 0) {
		blowerinfo.valid = 0;
		blowerinfo.is_updated = 1;
		cJSON_AddStringToObject(json, "chipID", "");
		cJSON_AddNumberToObject(json, "offset", 0);
		cJSON_AddStringToObject(json, "flag", flag_timout);
	}

	blowerinfo.is_updated = 1;
	printf("\n");

	vTaskDelay(1);
	printf("\n");
	vTaskDelay(1);

	ret_str = cJSON_PrintUnformatted(json);
    printf("%s\n", ret_str);
	vTaskDelay(1);
    printf("\n");
	vTaskDelay(1);


	cJSON_Delete(json);
    return 0;
}
/**
 * Expected Json request
 *  arg = {"id": 1, "msg_id":435}
 * response sent through printf()
 * resp_suc: {"cmd": "check_id", "id": 1,  "offset":-2, "chipID":"12345678", "flag":"None"}
 * resp_timeout: {"cmd": "check_id", "id": 1, "msg_id":435, "flag":"timeout"}
 */
static int check_id(int argc, char **argv)
{
	int devid = 0;
	int ret = 0;
	int ret1 = 0;
	int ret2 = 0;
	unsigned uuid = 0;
	unsigned chipID = 87639689;
	int offset = 0;
	static char* ret_str = NULL;
	char str_chipid[25];
	blowerinfo.devid = 0;
	blowerinfo.chipid = 0;
	blowerinfo.offset = 0;
	blowerinfo.valid = 0;
	blowerinfo.uuid = 0;

	if (argc != 2) {
		printf("Error: Incorrect number of arguments.\n");
		return 1; // Return an error
	}

	// Check that we can load the argument into a json object
	cJSON *json = cJSON_Parse(argv[1]);
	if (!json) {
		printf("Error: Parsing JSON.\n");
		return 1; // Return an error
	}
	cJSON *id = cJSON_GetObjectItemCaseSensitive(json, "id");
	if (!cJSON_IsNumber(id) ) {
		cJSON_Delete(json);
		printf("Error: Expected id and msg_id to be numbers.\n");
		return 1; // Return an error
	}
	cJSON *uuid_json = cJSON_GetObjectItemCaseSensitive(json, "uuid");
	if (uuid_json == NULL && !cJSON_IsNumber(uuid_json)) {

		printf("Warning: Expected uuid as a number.\n");
	} else {
		uuid = uuid_json->valueint;
		printf("   Updating uuid:%u\n", (unsigned) uuid);

		set_uuid(devid, uuid);
		get_uuid(devid, &uuid);
	}


	devid = id->valueint;


	// send the message wit for response.
	ret = check_dev_id(devid);
	ret1 = get_chipid(devid, &chipID);
	ret2 = get_raw_pressure(devid, &offset);


//	float raw_avg = 0;
//	for (int i=0;i<5;i++) {
//		ret2 = get_raw_pressure(devid, &offset);
//		raw_avg+=offset;
//		if (ret2) {
//			break;
//
//		}
//		raw_avg+=offset;
//	}

//	ret2 = get_raw_pressure(devid, &offset);

	if (ret == 1 && ret1 == 1 && ret2 == 1){

		sprintf(str_chipid, "%u", chipID);
		blowerinfo.devid = devid;
		blowerinfo.chipid = chipID;
		blowerinfo.offset = offset;
		blowerinfo.valid = 1;
		blowerinfo.uuid = uuid;


		cJSON_AddStringToObject(json, "chipID", str_chipid);
		cJSON_AddNumberToObject(json, "offset", offset);
		cJSON_AddStringToObject(json, "flag", flag_none);

	}
	else if (ret == -1 || ret1 == -1 || ret2 == -1){
		blowerinfo.valid = 0;
		blowerinfo.is_updated = 1;
		cJSON_AddStringToObject(json, "chipID", "");
		cJSON_AddNumberToObject(json, "offset", 0);
		cJSON_AddStringToObject(json, "flag", flag_transact);

	}
	else if (ret == 0 || ret1 == 0 || ret2 == 0) {
		blowerinfo.valid = 0;
		blowerinfo.is_updated = 1;
		cJSON_AddStringToObject(json, "chipID", "");
		cJSON_AddNumberToObject(json, "offset", 0);
		cJSON_AddStringToObject(json, "flag", flag_timout);
	}

	blowerinfo.is_updated = 1;
	printf("\n");

	vTaskDelay(1);
	printf("\n");
	vTaskDelay(1);

	ret_str = cJSON_PrintUnformatted(json);
    printf("%s\n", ret_str);
	vTaskDelay(1);
    printf("\n");
	vTaskDelay(1);


	cJSON_Delete(json);
    return 0;
}

static void register_check_id(void){
	const esp_console_cmd_t cmd = {
	        .command = "check_id",
	        .help = "Checks if an ID is on the system",
	        .hint = "{\"id\":11}",
	        .func = &check_id,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
static void register_get_dev_info(void){
	const esp_console_cmd_t cmd = {
	        .command = "check_id",
	        .help = "Checks if an ID is on the system",
	        .hint = "{\"id\":11}",
	        .func = &check_id,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static void register_register_run_cal_test(void){
	const esp_console_cmd_t cmd = {
	        .command = "cal_test",
	        .help = "Runs a calibration test, inserts UUID if present",
	        .hint = "{\"id\":255, \"uuid\":\"04010001\"}",
	        .func = &check_id,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static int check_id_moc(int argc, char **argv)
{
	int devid = 0;
	int ret = 0;
	int ret1 = 0;
	int ret2 = 0;
	unsigned uuid = 0;
	unsigned chipID = 87639689;
	int offset = 35;
	static char* ret_str = NULL;

	if (argc != 2) {
		printf("Error: Incorrect number of arguments.\n");
		return 1; // Return an error
	}

	// Check that we can load the argument into a json object
	cJSON *json = cJSON_Parse(argv[1]);
	if (!json) {
		printf("Error: Parsing JSON.\n");
		return 1; // Return an error
	}
	cJSON *id = cJSON_GetObjectItemCaseSensitive(json, "id");
	if (!cJSON_IsNumber(id) ) {
		cJSON_Delete(json);
		printf("Error: Expected id and msg_id to be numbers.\n");
		return 1; // Return an error
	}
	devid = id->valueint;

	char str_chipid[25];


	// send the message wit for response.

	sprintf(str_chipid, "%u", chipID);
	blowerinfo.devid = devid;
	blowerinfo.chipid = chipID;
	blowerinfo.offset = offset;
	blowerinfo.uuid = uuid;
	blowerinfo.valid = 1;
	blowerinfo.is_updated = 1;
	vTaskDelay(1);
	printf(" \r\n ");
	vTaskDelay(1);
	cJSON_AddStringToObject(json, "chipID", str_chipid);
	cJSON_AddNumberToObject(json, "offset", 0);
	cJSON_AddStringToObject(json, "flag", flag_none);

	ret_str = cJSON_PrintUnformatted(json);
    printf("%s\n", ret_str);

	cJSON_Delete(json);
	vTaskDelay(1);
	printf("\r\n");
    return 0;
}

static void register_check_id_moc(void){
	const esp_console_cmd_t cmd = {
	        .command = "check_id_moc",
	        .help = "Checks if an ID is on the system",
	        .hint = "{\"id\":255}",
	        .func = &check_id_moc,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

