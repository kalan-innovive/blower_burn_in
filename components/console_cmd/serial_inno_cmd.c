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


static char* flag_none= "None";
static char* flag_timout= "error:timeout";
static char* flag_transact= "error:transaction";
static char* flag_badarg = "error:bad_arg";

static void register_check_id(void);
static void register_check_id_moc(void);
static void register_get_dev_info(void);
static void register_run_cal_test(void);
static void register_set_uuid(void);
static void register_get_uuid(void);
static void register_set_sensor_type(void);
static void register_set_blower_type(void);


static int consl_check_id(int argc, char **argv);
static int consl_check_id_moc(int argc, char **argv);
static int consl_get_dev_info(int argc, char **argv);
static int consl_run_cal_test(int argc, char **argv);
static int consl_uuid(int argc, char **argv);
static int consl_blower_type(int argc, char **argv);
static int consl_sensor_type(int argc, char **argv);

static int parse_uuid_args(cJSON *json, blower_update *b);
static int parse_id_args(cJSON *json, blower_update *b);

static int get_uuid_args(cJSON *json, blower_update *b);
static int set_uuid_args(cJSON *json, blower_update *b);

static void print_response(cJSON *json);
static int verify_args(int argc, char **argv);
static int reset_blower_info(blower_update *b);


/******************************************************
 * Helper Functions
 */

static void print_response(cJSON *json){
	char* ret_str = NULL;
	vTaskDelay(1);
	printf("\n");
	vTaskDelay(1);
	ret_str = cJSON_PrintUnformatted(json);

    printf("%s\n", ret_str);
	vTaskDelay(1);
	cJSON_free(ret_str);
	cJSON_Delete(json);
    printf("\n");
	vTaskDelay(1);
}


static int reset_blower_info(blower_update *b){
	if (!b){
		return 1;
	}
	b->devid = 0;
	b->uuid = 0;
	b->chipid = 0;

	b->hw_ver = 0;
	b->sw_ver_maj = 0;
	b->sw_ver_min = 0;
	b->p_sensor_type = 0;
	b->blower_type = 0;

	b->is_updated = 0;
	b->valid = 0;
	return 0;

}

static int verify_args(int argc, char **argv)
{
	int ret = 0;


	if (argc != 2) {
		printf("Error: Incorrect number of arguments.\n");
		return 0; // Return an error
	}

	// Check that we can load the argument into a json object
	cJSON *json = cJSON_Parse(argv[1]);
	if (!json) {
		printf("Error: Parsing JSON.\n");
		return 0; // Return an error
	}
	return 1;
}

/******************************************************
 * Register Functions
 */

void register_serialinno(void){
	reset_blower_info(&blowerinfo);
	register_check_id();
	register_check_id_moc();
	register_get_dev_info();
	register_run_cal_test();
	register_set_uuid();
	register_get_uuid();
	register_set_sensor_type();
	register_set_blower_type();


}

static void register_check_id_moc(void){
	const esp_console_cmd_t cmd = {
	        .command = "check_id_moc",
	        .help = "Checks if an ID is on the system",
	        .hint = "{\"id\":255}",
	        .func = &consl_check_id_moc,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static void register_check_id(void){
	const esp_console_cmd_t cmd = {
	        .command = "check_id",
	        .help = "Checks if an ID is on the system",
	        .hint = "{\"id\":255, \"msg_id\":123, \"uuid\":\"04010001\"}",
	        .func = &consl_check_id,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
static void register_get_dev_info(void){
	const esp_console_cmd_t cmd = {
	        .command = "check_id",
	        .help = "Gets the configuration info",
	        .hint = "{\"id\":255}",
	        .func = &consl_get_dev_info,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static void register_run_cal_test(void){
	const esp_console_cmd_t cmd = {
	        .command = "cal",
	        .help = "Runs a calibration test,",
	        .hint = "{\"id\":255,\"cmd\":\"cal/test\" \"uuid\":\202010001}",
	        .func = &consl_run_cal_test,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static void register_set_uuid(void){
	const esp_console_cmd_t cmd = {
	        .command = "uuid/set",
	        .help = "Set cmd in json to uuid/set to set the uuid, Default operation uuid/get",
	        .hint = "{\"id\":255,\"cmd\":\"uuid/set\",\"uuid\":24040001}",
	        .func = &consl_uuid,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static void register_set_sensor_type(void){
	const esp_console_cmd_t cmd = {
	        .command = "set_sensor",
	        .help = "Runs a calibration test, inserts UUID if present",
	        .hint = "{\"id\":255, \"uuid\":\"04010001\"}",
			.func = &consl_sensor_type,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static void register_get_uuid(void){
	const esp_console_cmd_t cmd = {
	        .command = "uuid/get",
	        .help = "Retrieves the uuid from the device id",
	        .hint = "{\"id\":255}",
			.func = &consl_uuid,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static void register_set_blower_type(void){
	const esp_console_cmd_t cmd = {
	        .command = "set_blower_type",
	        .help = "Not Implemented yet ",
	        .hint = "{\"id\":255, \"blower_type\":1}",
	        .func = &consl_blower_type,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}


/******************************************************
 * API Functions Parsers
 */

/**
 * Parses blower version args into a blower_version struct
 * returns 0 for passed and 1 for fail
 * sets the flag to badarg if incorrect argument
 */
static int parse_blower_version_args(cJSON *json, blower_update *b_ver){
	if (!b_ver || !json ) {
		printf("Error: Parsing JSON.\n");
		// add the error flag to the JSON

		return 1; // Return an error
	}
	int ret = 1;
	char *flag_p = flag_badarg;
	b_ver->devid = 0;
	// Sets all values to default
	b_ver->hw_ver = -1;
	b_ver->sw_ver_maj = -1;
	b_ver->sw_ver_min = -1;
	b_ver->p_sensor_type = -1;
	b_ver->blower_type = -1;
	b_ver->is_updated = 0;
	b_ver->valid = 0;

	// Check required args
	cJSON *id = cJSON_GetObjectItemCaseSensitive(json, "id");
	if (cJSON_IsNumber(id) ) {
		b_ver->devid = id->valueint;
		ret = 0;
		flag_p = flag_none;
	}
	cJSON_AddStringToObject(json, "flag", flag_p);


	return ret;
}


/**
 * Parses uuid args into a blower_version struct
 * returns 0 for passed and 1 for fail
 * sets the flag to badarg if incorrect argument
 */
static int parse_uuid_args(cJSON *json, blower_update *b){
	if (!b || !json ) {
		printf("Error: Parsing JSON.\n");
		return 1; // Return an error
	}

	cJSON *uuid = cJSON_GetObjectItemCaseSensitive(json, "uuid");
	if (cJSON_IsNumber(uuid) ) {
		b->uuid = uuid->valueint;
	} else {
		// TODO: Add the bad arg to the flags
		return 1;
	}
	return 0;
}

/**
 * Parses id args into a blower_version struct
 * returns 0 for passed and 1 for fail
 * sets the flag to badarg if incorrect argument
 */
static int parse_id_args(cJSON *json, blower_update *b){
	if (!b || !json ) {
		printf("Error: Parsing JSON.\n");
		// add the error flag to the JSON
		return 1; // Return an error
	}

	char *flag_p = flag_badarg;

	cJSON *id = cJSON_GetObjectItemCaseSensitive(json, "id");
	if (cJSON_IsNumber(id) ) {
		b->devid = id->valueint;
		flag_p = flag_none;

	}

	cJSON_AddStringToObject(json, "flag", flag_p);
	return 0;
}


/**
 * Parses Check id args into a blower_version struct
 * returns 0 for passed and 1 for fail
 * sets the flag to badarg if incorrect argument
 */
static int parse_check_id_args(cJSON *json, blower_update *b){

	if (!b || !json ) {
		printf("Error: Parsing JSON.\n");
		return 1; // Return an error
	}

	int ret = 0;
	char *flag_p = flag_badarg;

	cJSON *id = cJSON_GetObjectItemCaseSensitive(json, "id");
	if (cJSON_IsNumber(id) ) {
		b->devid = id->valueint;
		ret = 0;
		flag_p = flag_none;

	}
	// TODO check the return value
	parse_uuid_args(json, b);

	cJSON_AddStringToObject(json, "flag", flag_p);

	return ret;
}



/******************************************************
 * API Functions
 */

/**
 * Sets the blower version arguments in the b_version and json
 * Returns 0 on passing
 * Verify pointers before
 */
static int set_blower_version_args(cJSON *json, blower_version *b_ver){
	unsigned val = 0;
	unsigned v_maj, v_min = 0;
	int comm_ret = 0;



	comm_ret = get_hw_version(b_ver->devid, &val);
	if (comm_ret ) {
		// FixMe: implement method should return hex and convert to decimal
		b_ver->hw_ver = (int) val;
		cJSON_AddNumberToObject(json, "hw_version", 107);
	}

	comm_ret = get_sw_version(b_ver->devid, &v_maj, &v_min);
	if (comm_ret ) {
		b_ver->sw_ver_maj = (int) (v_maj);
		cJSON_AddNumberToObject(json, "sw_ver_maj", v_maj);
		//Todo: implement min version or build id
		b_ver->sw_ver_min = (int) v_min;
		cJSON_AddNumberToObject(json, "sw_ver_min", v_min);
	}
	comm_ret = get_sensor_type(b_ver->devid, &val);
	if (comm_ret ) {
		b_ver->p_sensor_type = (int) (val);
		cJSON_AddNumberToObject(json, "sensor_type", val);

	}

	comm_ret = get_blower_type(b_ver->devid, &val);
	if (comm_ret ) {
		b_ver->blower_type = (int) (val);
		cJSON_AddNumberToObject(json, "sensor_type", val);
	}

	if (b_ver->blower_type>=0 && b_ver->sw_ver_maj>0 && b_ver->sw_ver_min==0){
		char ver_str[60];
		char *b_type = (b_ver->blower_type==0xFF ||b_ver->blower_type==0x1 ) ? "F": "V";
		sprintf(ver_str, "%s %d.%d", b_type, b_ver->sw_ver_maj, b_ver->sw_ver_maj);
		cJSON_AddStringToObject(json, "sw_version", ver_str);
	} else {
		cJSON_AddStringToObject(json, "sw_version", "None");

	}
	return 0;
}

/**
 * Checks the device id chip id and uuid
 * Returns 0 on passing
 * Verify pointers before
 */
static int set_check_id_args(cJSON *json, blower_update *b){

	int ret, ret1, ret2;
	char str_chipid[60] = "None";



	cJSON *uuid_json = cJSON_GetObjectItemCaseSensitive(json, "uuid");
	if (uuid_json == NULL && !cJSON_IsNumber(uuid_json)) {

		printf("Warning: Expected uuid as a number.\n");
	} else {
		b->uuid = (unsigned) uuid_json->valueint;
		b->uuid = (b->uuid == 0xFFFFFFFF)? 0:b->uuid;
		// Only set the uuid if provided and valid
		if (set_uuid(b->devid, b->uuid)) {
		get_uuid(b->devid, &(b->uuid));
	}

	// send the message wit for response.
	ret = check_dev_id(b->devid);
	ret1 = get_chipid(b->devid, &b->chipid);
	ret2 = get_raw_pressure(b->devid, &b->offset);


	if (ret == 1 && ret1 == 1 && ret2 == 1){

		sprintf(str_chipid, "%u", b->chipid);
		blowerinfo.valid = 1;


		cJSON_AddStringToObject(json, "chipID", str_chipid);
		cJSON_AddNumberToObject(json, "offset", b->offset);

	}
	else if (ret == -1 || ret1 == -1 || ret2 == -1){
		blowerinfo.valid = 0;
		blowerinfo.is_updated = 1;
		cJSON_AddStringToObject(json, "chipID", "");
		cJSON_AddNumberToObject(json, "offset", 0);
		cJSON *flag= cJSON_GetObjectItemCaseSensitive(json, "flag");
		cJSON_Delete(flag);
		cJSON_AddStringToObject(json, "flag", flag_transact);

	}
	else if (ret == 0 || ret1 == 0 || ret2 == 0) {
		blowerinfo.valid = 0;
		blowerinfo.is_updated = 1;
		cJSON_AddStringToObject(json, "chipID", "");
		cJSON_AddNumberToObject(json, "offset", 0);
		cJSON *flag= cJSON_GetObjectItemCaseSensitive(json, "flag");
		cJSON_Delete(flag);
		cJSON_AddStringToObject(json, "flag", flag_timout);
	}
	return 0;
}

/**
 * Checks the uuid of the board chip id and uuid
 * Returns 0 on passing 1 on Fail
 * Verify pointers before
 */
static int set_uuid_args(cJSON *json, blower_update *b){

	int ret;

	cJSON *uuid_json = cJSON_GetObjectItemCaseSensitive(json, "uuid");
	if (uuid_json == NULL ) {
		// delete the object
		cJSON_AddNumberToObject(json, "uuid", 0);

	}
	else if (!cJSON_IsNumber(uuid_json)) {
		printf("Warning: Expected uuid as a number.\n");
		return 1;
	}

	ret = set_uuid(b->devid, b->uuid);
    cJSON_ReplaceItemInObjectCaseSensitive(json, "uuid", cJSON_CreateNumber(0));

	printf("set_uuid_args:   Updating uuid:%u\n", (unsigned) b->uuid);


	return (ret==1)?0:1;
}




static int get_uuid_args(cJSON *json, blower_update *b){

	int ret;

	cJSON *uuid_json = cJSON_GetObjectItemCaseSensitive(json, "uuid");
	if (uuid_json == NULL ) {
		// delete the object
		cJSON_AddNumberToObject(json, "uuid", 0);
	}
	cJSON_ReplaceItemInObjectCaseSensitive(json, "uuid", cJSON_CreateNumber(b->uuid));
	ret = get_uuid(b->devid, &b->uuid);
	printf("   Updating uuid:%u\n", (unsigned) b->uuid);
	cJSON_ReplaceItemInObjectCaseSensitive(json, "uuid", cJSON_CreateNumber(b->uuid));

	return (ret==1)?0:1;
}

///**
// * Checks the device id chip id and uuid
// * Returns 0 on passing
// * Verify pointers before
// */
//static int set_check_id_args(cJSON *json, blower_update *b){
//
//	int ret, ret1, ret2;
//	char str_chipid[60] = "None";
//
//
//
//	cJSON *uuid_json = cJSON_GetObjectItemCaseSensitive(json, "uuid");
//	if (uuid_json == NULL && !cJSON_IsNumber(uuid_json)) {
//
//		printf("Warning: Expected uuid as a number.\n");
//	} else {
//		b->uuid = (unsigned) uuid_json->valueint;
//		printf("   Updating uuid:%u\n", (unsigned) b->uuid);
//
//		set_uuid(b->devid, b->uuid);
//		get_uuid(b->devid, &(b->uuid));
//	}
//
//	// send the message wit for response.
//	ret = check_dev_id(b->devid);
//	ret1 = get_chipid(b->devid, &b->chipid);
//	ret2 = get_raw_pressure(b->devid, &b->offset);
//
//
//	if (ret == 1 && ret1 == 1 && ret2 == 1){
//
//		sprintf(str_chipid, "%u", b->chipid);
//		blowerinfo.valid = 1;
//
//
//		cJSON_AddStringToObject(json, "chipID", str_chipid);
//		cJSON_AddNumberToObject(json, "offset", b->offset);
//
//	}
//	else if (ret == -1 || ret1 == -1 || ret2 == -1){
//		blowerinfo.valid = 0;
//		blowerinfo.is_updated = 1;
//		cJSON_AddStringToObject(json, "chipID", "");
//		cJSON_AddNumberToObject(json, "offset", 0);
//		cJSON *flag= cJSON_GetObjectItemCaseSensitive(json, "flag");
//		cJSON_Delete(flag);
//		cJSON_AddStringToObject(json, "flag", flag_transact);
//
//	}
//	else if (ret == 0 || ret1 == 0 || ret2 == 0) {
//		blowerinfo.valid = 0;
//		blowerinfo.is_updated = 1;
//		cJSON_AddStringToObject(json, "chipID", "");
//		cJSON_AddNumberToObject(json, "offset", 0);
//		cJSON *flag= cJSON_GetObjectItemCaseSensitive(json, "flag");
//		cJSON_Delete(flag);
//		cJSON_AddStringToObject(json, "flag", flag_timout);
//	}
//	return 0;
//}


/******************************************************
 * CMD Line Callback Functions
 */


static int consl_run_cal_test(int argc, char **argv){
	printf("Error: Not Implemented.\n");
	return 0;
}

static int consl_uuid(int argc, char **argv){
	// Acting as set uuid for know
	int ret = 1;
	static const char* uuid_get_str = "uuid/get";
	static const char* uuid_set_str = "uuid/set";

	if (argc != 2) {
		printf("Error: Incorrect number of arguments. expects 2  got %d\n", argc);
		for (int i =0; i<argc;i++) {
			printf("Arg %d - %s", i, argv[i]);
		}
		return 1; // Return an error
	}

	// Check that we can load the argument into a json object
	cJSON *json = cJSON_Parse(argv[1]);
	if (!json) {
		printf("Error: Parsing JSON.\n");
		return 1; // Return an error
	}
	blowerinfo.devid = 0;
	blowerinfo.uuid = 0;
	blowerinfo.valid =0;
	blowerinfo.is_updated =0;
	printf("UUID Options parse argv |%s|%s||%s|  result:%d:%d \n",
			uuid_get_str, uuid_set_str, argv[0],
			strcmp(argv[0],uuid_get_str ), strcmp(argv[0],uuid_set_str));


	// Parse the ID arg first should we check the ID?
	if (parse_id_args(json, &blowerinfo) == 0){
		printf("Parsed id Arg %d. ", blowerinfo.devid);
		printf("uuid Args %s. \n", argv[0]);

		if (strcmp(argv[0], uuid_set_str) == 0) {
			// Parse args; Set UUID; Read UUID
			if (parse_uuid_args(json, &blowerinfo) == 0) {

				if (set_uuid_args(json, &blowerinfo) == 0) {
					printf("Set uuid  JSON.\n");

					if (get_uuid_args(json, &blowerinfo) == 0) {
						printf("consl_uuid get uuid  JSON.\n");
						blowerinfo.valid = 1;
						blowerinfo.is_updated = 1;
						ret = 0;
					}
				}
			}
		}

		if (strcmp(argv[0], uuid_get_str) ==0) {
			printf("Running UUID get\n");
			if (get_uuid_args(json, &blowerinfo) == 0) {
				printf("consl_uuid get uuid  JSON.\n");
				blowerinfo.valid = 1;
				blowerinfo.is_updated = 1;
				ret = 0;
			}
		}
	}
	if (ret == 1){
		printf("UUID cmd failed argv |%s|%s||%s|  result:%d:%d \n",
				uuid_get_str, uuid_set_str, argv[0],
				strcmp(argv[0],uuid_get_str ), strcmp(argv[0],uuid_set_str));

	}

	printf("Uuid- %d. \n", blowerinfo.uuid);

	print_response(json);

    return ret;
}


static int consl_blower_type(int argc, char **argv){
	printf("Error: Not Implemented.\n");
	return 0;
}
static int consl_sensor_type(int argc, char **argv){
	printf("Error: Not Implemented.\n");
	return 0;
}

/**
 * Expected Json request
 *  arg required = {"id": 1}
 *  arg preffered = {"cmd": "check_version", "id": 1, "msg_id":435}
 * response sent through printf()
 * resp_suc: {"cmd": "check_version", "id": 1,  "hw_version":0xELE107, "sw_ver_maj":10,
 * 		"sw_ver_minor":9, "sw_version":"F-10.9", "blower_type":1, "sensor_type":2, "flag":"None", "msg_id":435}
 * resp_timeout: {"cmd": "check_version", "id": 1, "msg_id":435, "flag":"timeout"}
 */
static int consl_get_dev_info(int argc, char **argv)
{

	int ret = 0;
	blower_version b_ver;

	if (argc != 2) {
		printf("Error: Incorrect number of arguments. expects 2  got %d\n", argc);
		return 1; // Return an error
	}

	// Check that we can load the argument into a json object
	cJSON *json = cJSON_Parse(argv[1]);
	if (!json) {
		printf("Error: Parsing JSON.\n");
		return 1; // Return an error
	}

	ret = parse_blower_version_args(json, &b_ver);

	if (ret) {
		b_ver.valid = 1;
		ret = set_blower_version_args(json, &b_ver);
		if (ret) {
			b_ver.valid = 0;
		}
	}

	b_ver.is_updated = 1;
	print_response(json);
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
static int consl_check_id(int argc, char **argv)
{
	int ret = 0;


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


	ret = parse_check_id_args(json, &blowerinfo);
	if (ret) {
		ret = set_check_id_args(json, &blowerinfo);
	}

	print_response(json);

    return 0;
}



static int consl_check_id_moc(int argc, char **argv)
{

	unsigned uuid = 0;
	unsigned chipID = 87639689;
	int offset = 35;

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
	blowerinfo.devid = id->valueint;

	char str_chipid[25];


	// send the message wit for response.

	sprintf(str_chipid, "%u", chipID);
	blowerinfo.chipid = chipID;
	blowerinfo.offset = offset;
	blowerinfo.uuid = uuid;
	blowerinfo.valid = 1;
	blowerinfo.is_updated = 1;

	cJSON_AddStringToObject(json, "chipID", str_chipid);
	cJSON_AddNumberToObject(json, "offset", 0);
	cJSON_AddStringToObject(json, "flag", flag_none);

	print_response(json);
    return 0;
}



