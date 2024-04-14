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
	unsigned chipID = 0;
	int offset = 0;
	char* flag_none= "None";
	char* flag_timout= "error:timeout";
	char* flag_transact= "error:transaction";
	char* flag = flag_none;

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
	ret = check_dev_id(devid);
	ret1 = get_chipid(devid, &chipID);
	ret2 = get_raw_pressure(devid, &offset);

	float raw_avg = 0;
//	for (int i=0;i<5;i++) {
//		ret2 = get_raw_pressure(devid, &offset);
//		raw_avg+=offset;
//		if (ret2) {
//			break;
//
//		}
//		raw_avg+=offset;
//	}

	ret2 = get_raw_pressure(devid, &offset);

	if (ret == 1 && ret1 == 1 && ret2 == 1){

		sprintf(str_chipid, "%u", chipID);
		blowerinfo.devid = devid;
		blowerinfo.chipid = chipID;
		blowerinfo.offset = offset;
		blowerinfo.valid = 1;
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
	printf(" \n ");
	printf(" \r\n ");
	vTaskDelay(5);
	printf(" \r\n ");
	vTaskDelay(10);


    printf("%s\n", cJSON_Print(json));

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

static int check_id_moc(int argc, char **argv)
{
	int devid = 0;
	int ret = 0;
	int ret1 = 0;
	int ret2 = 0;
	unsigned chipID = 87639689;
	int offset = 35;
	char* flag_none= "None";

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
	blowerinfo.uuid = 0;
	blowerinfo.valid = 1;
	blowerinfo.is_updated = 1;
	cJSON_AddStringToObject(json, "chipID", str_chipid);
	cJSON_AddNumberToObject(json, "offset", 0);
	cJSON_AddStringToObject(json, "flag", flag_none);
	printf(" \r\n ");
	vTaskDelay(1);
	printf(" \r\n ");
	vTaskDelay(1);

    printf(" \n%s\n ", cJSON_Print(json));

	cJSON_Delete(json);
    return 0;
}

static void register_check_id_moc(void){
	const esp_console_cmd_t cmd = {
	        .command = "check_id_moc",
	        .help = "Checks if an ID is on the system",
	        .hint = "{\"id\":11}",
	        .func = &check_id_moc,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

