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


static char* flag_none = "None";
static char* flag_timout= "error:timeout";
static char* flag_transact = "error:transaction";
static char* flag_badarg = "error:bad_arg";
static char* flag_cmd_unsuccessful = "error:cmd_unsuccessful";

static void register_check_id(void);
static void register_check_id_moc(void);
static void register_get_dev_info(void);
static void register_run_cal_test(void);
static void register_uuid(void);
//static void register_get_uuid(void);
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
static int parse_blower_version_args(cJSON *json, blower_update *b_ver);


static int get_uuid_args(cJSON *json, blower_update *b);
static int set_uuid_args(cJSON *json, blower_update *b);
void id_to_str(char *id_str, unsigned *id);
static int set_check_connected_devices(cJSON *json);
static int set_chipID_args(cJSON *json, blower_update *b);
static int set_offset_args(cJSON *json, blower_update *b);

static void print_response(cJSON *json);
static cJSON * verify_args(int argc, char **argv);
static int reset_blower_info(blower_update *b);
static void update_flag(cJSON *json, char *flag);


/******************************************************
 * Helper Functions
 */

#define DEV_COUNT 7
#define CHIPID_MAX_LEN 15
const static int DEV_LIST[DEV_COUNT]  = {0x00,0x01,0x03,0x21,0x23,0x17,0xFF};



void id_to_str(char *id_str,unsigned *id){
	//Copy the id to the string
	if (*id >= 0xFFFFFFFF) {
		*id = 0;
	}
	sprintf(id_str, "%u", *id);
}

static void print_response(cJSON *json){
	char* ret_str = NULL;
	vTaskDelay(1);
	printf("\n");
	vTaskDelay(1);
	ret_str = cJSON_PrintUnformatted(json);

    printf("%s\n", ret_str);
	vTaskDelay(1);
	cJSON_free(ret_str);

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


void print_blower_update(const blower_update *bu) {
    if (bu == NULL) {
        printf("blower_update structure is NULL.\n");
        return;
    }
    printf("\n\tblower_update structure:\n");
    printf("\t\tcmd:    %d\n", bu->cmd);
    printf("\t\tdevid:  %d\n", bu->devid);
    printf("\t\tchipid: %u\n", bu->chipid);
    printf("\t\tuuid:   %u\n", bu->uuid);
    printf("\t\toffset: %d\n", bu->offset);
    printf("\t\tsensor_type: %d\n", bu->p_sensor_type);
    printf("\t\tblower_type: %d\n", bu->blower_type);
    printf("\t\tsw_ver_maj:  %d\n", bu->sw_ver_maj);
    printf("\t\tsw_ver_min:  %d\n", bu->sw_ver_min);
    printf("\t\thw_ver:      %X\n", bu->hw_ver);
    printf("\t\tis_updated:  %d\n", bu->is_updated);
    printf("\t\tvalid:       %d\n\n", bu->valid);
}




static void update_flag(cJSON *json, char *flag) {
	// Check if the flag is set if not set it
	cJSON *flag_json = cJSON_GetObjectItemCaseSensitive(json, "flag");
	if (flag_json == NULL ) {
		cJSON_AddStringToObject(json, "flag", flag);
		return;
	}
	if (strcmp(flag , flag_none) == 0){
		return;
	}
	// Replace the flag
	cJSON_ReplaceItemInObjectCaseSensitive(json, "flag", cJSON_CreateString(flag));
}

static cJSON *verify_args(int argc, char **argv)
{
	cJSON *json = NULL;
	if (argc != 2) {
		printf("Error: Incorrect number of arguments.\n");
		return json; // Return an error
	}

	// Check that we can load the argument into a json object
	json = cJSON_Parse(argv[1]);
	if (!json) {
		printf("Error: Parsing JSON.\n");
		return 0; // Return an error
	}
	return json;
}

/******************************************************
 * Register Functions
 */

void register_serialinno(void){
	reset_blower_info(&blowerinfo);
	register_get_dev_info();
	register_run_cal_test();
	register_uuid();
	register_set_sensor_type();
	register_set_blower_type();
	register_check_id();
	register_check_id_moc();




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

//static struct {
//	struct arg_str *single;
//    struct arg_str *multiple;
//    struct arg_end *end;
//}checkid_args;

static void register_check_id(void){
	const esp_console_cmd_t cmd = {
	        .command = "check_id",
	        .help = "Checks if an ID is on the system",
	        .hint = "{\"cmd\":\"check_id\",\"id\":255}\n"
	        		"    - check_id {\"cmd\":\"check_id/all\"}\n"
	        		"    - check_id {\"cmd\":\"check_id/uuid\",\"id\":255}\n",
	        .func = &consl_check_id,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
static void register_get_dev_info(void){
	const esp_console_cmd_t cmd = {
	        .command = "dev_info",
	        .help = "Gets the configuration info",
	        .hint = "{\"id\":255}\n"
	    	        "    -dev_info {\"cmd\":\"dev_info\",\"id\":255}\n",
	        .func = &consl_get_dev_info,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static void register_run_cal_test(void){
	const esp_console_cmd_t cmd = {
	        .command = "cal",
	        .help = "Runs a calibration test,",
	        .hint = "{\"id\":255,\"cmd\":\"cal/run\"}",
	        .func = &consl_run_cal_test,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static void register_uuid(void){
	const esp_console_cmd_t cmd = {
	        .command = "uuid",
	        .help = "Get or set cmd in json set to uuid/set to set the uuid, Default operation uuid/get",
	        .hint = "{\"id\":255,\"cmd\":\"uuid/set\",\"uuid\":24040001}\n"
	        		"    - uuid {\"cmd\":\"uuid/get\",\"id\":255}\n"
	        		"    - uuid {\"id\":255}\n",
	        .func = &consl_uuid,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static void register_set_sensor_type(void){
	const esp_console_cmd_t cmd = {
	        .command = "set_sensor",
	        .help = "Updates the sensor type ",
	        .hint = "{\"id\":255, \"uuid\":04010001}",
			.func = &consl_sensor_type,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

//static void register_get_uuid(void){
//	const esp_console_cmd_t cmd = {
//	        .command = "uuid/get",
//	        .help = "Retrieves the uuid from the device id",
//	        .hint = "{\"id\":255}",
//			.func = &consl_uuid,
//	    };
//	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
//}

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

typedef enum {
	CHECK_ID,
	CHECK_ID_UUID,
	CHECK_ID_ALL,
	UUID_SET,
	UUID_GET,
	DEV_INFO,
	RUN_CAL,
	READ_CAL,
	CI_LEN
} serial_inno_args;

const char* cmd_id = "check_id";
const char* cmd_id_uuid = "check_id/uuid";
const char* cmd_id_all = "check_id/all";
const char* cmd_uuid_set = "uuid/set";
const char* cmd_uuid_get = "uuid/get";
const char* cmd_run_cal = "cal/run";
const char* cmd_read_cal = "cal/read";

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
	int ret = 0;
	b_ver->devid = 0;
	// Sets all values to default
	b_ver->hw_ver = 0;
	b_ver->sw_ver_maj = 0;
	b_ver->sw_ver_min = 0;
	b_ver->p_sensor_type = -1;
	b_ver->blower_type = -1;
	b_ver->is_updated = 0;
	b_ver->valid = 0;

	// Check required args
	ret += parse_id_args(json, b_ver);

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
	char *flag_p = flag_badarg;

	cJSON *cmd = cJSON_GetObjectItemCaseSensitive(json, "cmd");
	if (cmd !=NULL && cJSON_IsString(cmd) ) {
		if (strcmp(cmd->valuestring, cmd_uuid_set) == 0 ) {
			b->cmd = UUID_SET;
			flag_p = flag_none;
		}
		else if (strcmp(cmd->valuestring, cmd_uuid_get) == 0 ) {
			b->cmd = UUID_GET;
			cJSON *uuid = cJSON_GetObjectItemCaseSensitive(json, "uuid");
			if (cJSON_IsNumber(uuid) ) {
				b->uuid = uuid->valueint;
				flag_p = flag_none;
			}
		}
	}
	else {
		b->cmd = UUID_GET;
		flag_p = flag_none;
	}

	update_flag(json, flag_p);

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
	} else {
		printf("Error: ID must be int.\n");
	}

	update_flag(json, flag_p);

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

	char *flag_p = flag_badarg;
	b->cmd = CHECK_ID;


	// Check if uuid is set
	cJSON *cmd = cJSON_GetObjectItemCaseSensitive(json, "cmd");
	if (cmd !=NULL && cJSON_IsString(cmd) ) {
		if (strcmp(cmd->valuestring, cmd_id) == 0 ) {
			b->cmd = CHECK_ID;
			parse_id_args(json, b);
			flag_p = flag_none;
		} else if (strcmp(cmd->valuestring, cmd_id_all) == 0 ) {
			b->cmd = CHECK_ID_ALL;
			flag_p = flag_none;

		} else if (strcmp(cmd->valuestring, cmd_id_uuid) == 0 ) {
			b->cmd = CHECK_ID_UUID;
			parse_uuid_args(json, b);
			flag_p = flag_none;
		} else {
			b->cmd = CHECK_ID;
			parse_id_args(json, b);
			flag_p = flag_none;
		}

	}

	update_flag(json, flag_p);

	return 0;
}



/******************************************************
 * API Functions
 */

/**
 * Sets the blower version arguments in the b_version and json
 * Returns 0 on passing
 * Verify pointers before
 */
static int set_blower_version_args(cJSON *json, blower_update *b_ver){
	unsigned val = 0;
	unsigned v_maj, v_min = 0;
	int comm_ret = 0;
	char *flag_p = flag_none;

	printf("set_blower_version_args: Getting UUID");

	comm_ret += get_uuid_args(json, b_ver);
	printf("set_blower_version_args: Getting ChipID");

	comm_ret += set_chipID_args(json, b_ver);
	comm_ret += set_offset_args(json, b_ver);
	if (comm_ret != 0) {
		flag_p = flag_transact;
	}

	printf("set_blower_version_args: Getting HW Ver");

	comm_ret = get_hw_version(b_ver->devid, &val);
	if (comm_ret ) {
		// FixMe: implement method should return hex and convert to decimal
		b_ver->hw_ver = (int) val;
		cJSON_AddNumberToObject(json, "hw_version", val);
	}
	printf("set_blower_version_args: Getting SW Ver");

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
		cJSON_AddNumberToObject(json, "blower_type", val);
	}
	int vali = 0;
	comm_ret = get_caibration(b_ver->devid, &vali);
	if (comm_ret ) {
		b_ver->offset = (vali);
		cJSON_AddNumberToObject(json, "calibration", vali);
	}

	if (b_ver->blower_type>=0 && b_ver->sw_ver_maj>0 && b_ver->sw_ver_min==0){
		char ver_str[60];
		char *b_type = (b_ver->blower_type==0xFF ||b_ver->blower_type==0x0 ) ? "Fan": "Valve";
		sprintf(ver_str, "%s-%d.%d", b_type, b_ver->sw_ver_maj, b_ver->sw_ver_min);

		cJSON_AddStringToObject(json, "sw_version", ver_str);
	} else {
		cJSON_AddStringToObject(json, "sw_version", "None");

	}
	update_flag(json, flag_p);
	return 0;
}





/**
 * Requires: arg = {"cmd":"check_id/all"}
 * resp_suc: {"cmd":"check_id/all","id_list":[1,3],"chipID_list":["12345678","543211234"],"dev_count":2,"flag":"None"}
 * resp_suc: {"cmd":"check_id/all","id_list":[],"chipID_list":[],"dev_count":0,"flag":"None"}
 * resp_timeout: {"cmd":"check_id/all","id_list":[],"chipID_list":[],"dev_count":0,"flag":"bad_arg"}
 */
static int set_check_connected_devices(cJSON *json)
{
	if (!json) {
		return 1; // Return an error
	}

	char chipID_list[DEV_COUNT][CHIPID_MAX_LEN];
	int devID_list[DEV_COUNT];

	int count= 0;
	unsigned chipID_tmp =0;
	cJSON *id_list = cJSON_CreateArray();
	cJSON *chipID = cJSON_CreateArray();
	cJSON_AddItemToObject(json, "id_list", id_list);
	cJSON_AddItemToObject(json, "chipID_list", chipID);

	// Iterate through devices
	for (int i=0;i<DEV_COUNT; i++) {
		printf(" %d,count=%d  Checking Device ID 0X%0X, ", i, count, DEV_LIST[i]);

		if( get_chipid(DEV_LIST[i], &chipID_tmp) != 0) {
			id_to_str(chipID_list[count], &chipID_tmp);
			devID_list[count] = DEV_LIST[i];
			printf(" ChipID: %u, str_reper %s\n", chipID_tmp, chipID_list[count]);
			cJSON_AddItemToArray(id_list, cJSON_CreateNumber(DEV_LIST[i]));
			cJSON_AddItemToArray(chipID, cJSON_CreateString(chipID_list[count]));

			count++;
		}
		else{
			printf(" Not Found\n");
		}
	}
	printf(" Found %d Devices\n", count);


	// Add fields to the JSON object

	cJSON_AddNumberToObject(json, "dev_count", count);
	update_flag(json,flag_none);

    return 0;
}


/**
 * Checks the  chip id
 * Returns 0 on passing
 * Verify pointers before
 */
static int set_chipID_args(cJSON *json, blower_update *b){
	int ret = 1;
	char str_chipid[60] = "None";
	char *flag = flag_timout;


	cJSON *chipID_json = cJSON_GetObjectItemCaseSensitive(json, "chipID");
	if (chipID_json == NULL ) {
		// delete the object
		cJSON_AddStringToObject(json, "chipID", str_chipid);
	}


	ret = get_chipid(b->devid, &(b->chipid));
	if (ret == 1 ) {
		sprintf(str_chipid, "%u", b->chipid);
		ret = 0;
		flag = flag_none;

	}
	update_flag(json, flag);

    cJSON_ReplaceItemInObjectCaseSensitive(json, "chipID", cJSON_CreateString(str_chipid));

	printf("set_chipID_args:   Updating chipID:%s\n", str_chipid);

	return ret;
}



/**
 * Checks the  offset
 * Returns 0 on passing
 * Verify pointers before
 */
static int set_offset_args(cJSON *json, blower_update *b){
	int ret = 1;
	char *flag = flag_timout;

	cJSON *offset_json = cJSON_GetObjectItemCaseSensitive(json, "offset_json");
	if (offset_json == NULL ) {
		// delete the object
		cJSON_AddNumberToObject(json, "offset", 2222);
	}

	ret = get_raw_pressure(b->devid, &(b->offset));
	if (ret == 1 ) {
		ret = 0;
		flag = flag_none;
	}
	update_flag(json, flag);

    cJSON_ReplaceItemInObjectCaseSensitive(json, "offset", cJSON_CreateNumber(b->offset));

	printf("set_offset_args:   Updating offset:%d\n", b->offset);

	return ret;
}
/**
 * Checks the device id chip id and uuid
 * Returns 0 on passing
 * Verify pointers before
 */
static int set_check_id_args(cJSON *json, blower_update *b){

	int ret = 0;

	if (b->cmd == CHECK_ID) {
		ret += set_chipID_args(json, b);
		ret += set_offset_args(json, b);
		ret += get_uuid_args(json, b);
	}
	else if (b->cmd == CHECK_ID_UUID) {
		ret += set_chipID_args(json, b);
		ret += set_offset_args(json, b);
		ret += set_uuid_args(json, b);
	}
	else if (b->cmd == CHECK_ID_ALL) {
		ret += set_check_connected_devices(json);
	}

	blowerinfo.valid = 1;
	blowerinfo.is_updated = 1;

	if (ret !=0) {
		blowerinfo.valid = 0;
		blowerinfo.is_updated = 1;

	}

	return 0;
}

/**
 * Assigns the uuid to the board
 * Returns 0 on passing 1 on Fail
 * Verify pointers before
 */
static int set_uuid_args(cJSON *json, blower_update *b){

	int ret;

	cJSON *uuid_json = cJSON_GetObjectItemCaseSensitive(json, "uuid");
	if (uuid_json == NULL ) {
		// delete the object
		cJSON_AddNumberToObject(json, "uuid", 0);
		return 1;

	}
	else if (!cJSON_IsNumber(uuid_json)) {
		printf("Warning: Expected uuid as a number.\n");
		return 1;
	}
	b->uuid = uuid_json->valueint;
	ret = set_uuid(b->devid, b->uuid);
    cJSON_ReplaceItemInObjectCaseSensitive(json, "uuid", cJSON_CreateNumber(0));

	printf("set_uuid_args:   Updating uuid:%u\n", (unsigned) b->uuid);


	return (ret==1)?0:1;
}

/**
 * Runs the calibration sequence
 * Returns 0 on passing 1 on Fail
 * Verify pointers before
 */
static int set_run_cal_args(cJSON *json, blower_update *b){
	int ret = 1;
	char *flag = flag_timout;

	cJSON *cal_json = cJSON_GetObjectItemCaseSensitive(json, "cal_success");
	if (cal_json == NULL ) {
		// delete the object
		cJSON_AddNumberToObject(json, "cal_success", 0);
		return 1;

	}
	ret = run_caibration(b->devid);
	if (ret == 1){
		cJSON_ReplaceItemInObjectCaseSensitive(json, "cal_success", cJSON_CreateNumber(0));
		ret = 0;
		printf("cal_success:  Passed  :%d\n", ret);
	}
	else if (ret == 0){
		printf("cal_success:  Failed  :%d\n", ret);
		flag =flag_transact;
	}
	else{
		printf("cal_success:  Failed  :%d\n", ret);
		flag =flag_cmd_unsuccessful;
	}
	update_flag(json, flag);
	return ret;
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

//
//static int set_offset_args(cJSON *json, blower_update *b){
//
//	int ret;
//	char *flag = flag_timout;
//
//
//	cJSON *offset_json = cJSON_GetObjectItemCaseSensitive(json, "offset");
//	if (offset_json == NULL ) {
//		// delete the object
//		if (set_offset_args)
//		cJSON_AddNumberToObject(json, "offset", 0);
//	}
//	cJSON_ReplaceItemInObjectCaseSensitive(json, "offset", cJSON_CreateNumber(b->offset));
//	ret = get_off(b->devid, &b->uuid);
//	printf("   Updating offset:%u\n", (unsigned) b->offset);
//	cJSON_ReplaceItemInObjectCaseSensitive(json, "offset", cJSON_CreateNumber(b->offset));
//
//	return (ret==1)?0:1;
//}

/******************************************************
 * CMD Line Callback Functions
 */


static int consl_run_cal_test(int argc, char **argv){
	printf("Error: Not Implemented.\n");
	// Acting as set uuid for know
		int ret = 1;
		cJSON *json = NULL;

		// Check that we can load the argument into a json object
		json = verify_args(argc, argv);
		printf("consl_run_cal_test |%s\n", argv[1]);

		if (!json) {
			return 1; // Return an error
		}
		blowerinfo.cmd = CHECK_ID;
		blowerinfo.devid = 0;
		blowerinfo.offset = 0;
		blowerinfo.valid =0;
		blowerinfo.is_updated =0;





		// Parse the ID arg first should we check the ID?
		if (parse_id_args(json, &blowerinfo) == 0){
			printf("Parsed id Arg %d. ", blowerinfo.devid);
			ret += set_chipID_args(json, &blowerinfo);
			ret += set_offset_args(json, &blowerinfo);
			ret += set_uuid_args(json, &blowerinfo);
			blowerinfo.cmd = RUN_CAL;



			if (ret == 0 && set_run_cal_args(json, &blowerinfo) == 0) {

				printf("set_cal_args Args %d. \n", blowerinfo.offset);
				blowerinfo.valid = 1;
				blowerinfo.is_updated = 1;
			}


		}
		print_response(json);
		cJSON_Delete(json);
	return 0;
}

static int consl_uuid(int argc, char **argv){
	// Acting as set uuid for know
	int ret = 1;
	cJSON *json = NULL;

	// Check that we can load the argument into a json object
	json = verify_args(argc, argv);
	printf("consl_check_id |%s\n", argv[1]);

	if (!json) {
		return 1; // Return an error
	}

	blowerinfo.devid = 0;
	blowerinfo.uuid = 0;
	blowerinfo.valid =0;
	blowerinfo.is_updated =0;

//	printf("UUID Options parse argv |%s|%s||%s|  result:%d:%d \n",
//			uuid_get_str, uuid_set_str, argv[0],
//			strcmp(argv[0],uuid_get_str ), strcmp(argv[0],uuid_set_str));


	// Parse the ID arg first should we check the ID?
	if (parse_id_args(json, &blowerinfo) == 0){
		printf("Parsed id Arg %d. ", blowerinfo.devid);

		if (parse_uuid_args(json, &blowerinfo) == 0) {
			printf("Parsed uuid Args %s. \n", argv[0]);

		}

		if (blowerinfo.cmd == UUID_SET) {
			// Parse args; Set UUID; Read UUID
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


		if (blowerinfo.cmd == UUID_GET) {
			printf("Running UUID get\n");
			if (get_uuid_args(json, &blowerinfo) == 0) {
				printf("consl_uuid get uuid  JSON.\n");
				blowerinfo.valid = 1;
				blowerinfo.is_updated = 1;
				ret = 0;
			}
		}
	}


	printf("Uuid- %d. \n", blowerinfo.uuid);

	print_response(json);
	cJSON_Delete(json);

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
 * resp_suc: {"cmd":"check_version", "id": 1,  "hw_version":0xELE107, "sw_ver_maj":10,
 * 		"sw_ver_minor":9, "sw_version":"F-10.9", "blower_type":1, "sensor_type":2, "flag":"None", "msg_id":435}
 * resp_timeout: {"cmd": "check_version", "id": 1, "msg_id":435, "flag":"timeout"}
 */
static int consl_get_dev_info(int argc, char **argv)
{
	cJSON *json = NULL;
	reset_blower_info(&blowerinfo);

	// Check that we can load the argument into a json object
	json = verify_args(argc, argv);
	printf("consl_check_id |%s\n", argv[1]);

	if (!json) {
		return 1; // Return an error
	}


	int ret = parse_blower_version_args(json, &blowerinfo);
	printf("consl_get_dev_info |parsed version args =%d\n", ret);

	if (ret== 0) {
		blowerinfo.valid = 1;
		ret = set_blower_version_args(json, &blowerinfo);
		if (ret != 0) {
			blowerinfo.valid = 0;
		}
	}

	blowerinfo.is_updated = 1;
	print_response(json);
	cJSON_Delete(json);
	print_blower_update(&blowerinfo);

    return 0;
}



/**
 * Expected Json request
 * Requires: arg = {"id": 255}
 * Optional: arg = {"id":255,"chipID":"12345678"}
 * response sent through printf()
 * resp_suc: {"cmd": "check_id", "id": 1,  "offset":-2, "chipID":"12345678", "flag":"None"}
 * resp_timeout: {"cmd": "check_id", "id": 1, "msg_id":435, "flag":"timeout"}
 */
static int consl_check_id(int argc, char **argv)
{	cJSON *json = NULL;

	// Check that we can load the argument into a json object
	json = verify_args(argc, argv);
	printf("consl_check_id |%s\n", argv[1]);

	if (!json) {
		return 1; // Return an error
	}
	reset_blower_info(&blowerinfo);

	print_blower_update(&blowerinfo);
	if(parse_check_id_args(json, &blowerinfo) != 0){
		print_blower_update(&blowerinfo);

		print_response(json);
		return 1;
	}
	print_blower_update(&blowerinfo);


	printf("Setting ChipID Args.\n");

	if(set_check_id_args(json, &blowerinfo) != 0){
		print_response(json);
		return 1;
	}

	print_response(json);
	cJSON_Delete(json);

    return 0;
}


//Using the cJson library with c language. can you fill in the rest of the function to create the json object. The template code is:




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
	cJSON_Delete(json);
    return 0;
}



