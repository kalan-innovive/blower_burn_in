/*
 * sensor_cmd.c
 *
 *  Created on: Apr 7, 2023
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
//#include "esp_flash.h"
//#include "driver/rtc_io.h"
//#include "driver/uart.h"
#include "argtable3/argtable3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "cmd_system.h"
#include "sdkconfig.h"
#include "ms45sensor.h"


// Prototypes
static int sensor_read(int argc, char **argv);
const char* TAG = "SensorCmd";



static struct {
    struct arg_int *num_reads;
    struct arg_end *end;
} set_args;


static void register_read_cmd(void){
	ESP_LOGD(TAG, "Setting up sensor read command");

	set_args.num_reads = arg_int0(NULL, "num_reads", "<n>", "requested number of reads");
	// sensor_read --num_reads=10
	// cmd {sensor_read:10}

	set_args.end = arg_end(2);

	const esp_console_cmd_t cmd = {
	        .command = "sensor_read",
	        .help = "Sensor Read Cmd arguments number of reads",
	        .hint = NULL,
	        .func = &sensor_read,
			.argtable = &set_args,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static void register_alt_read_cmd(void){
	ESP_LOGD(TAG, "Setting up sensor read oammand");

	set_args.num_reads = arg_int0(NULL, "num_reads", "<n>", "requested number of reads");
	// sensor_read --num_reads=10
	// cmd {sensor_read:10}

	set_args.end = arg_end(2);

	const esp_console_cmd_t cmd = {
	        .command = "ensor_read",
	        .help = "Sensor Read Cmd arguments number of reads",
	        .hint = NULL,
	        .func = &sensor_read,
			.argtable = &set_args,
	    };
	    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

void register_sensor(void){
	ESP_LOGD(TAG, "Setting up sensor read command");
	register_read_cmd();
	register_alt_read_cmd();
}



static int sensor_read(int argc, char **argv)
{
	if (argc >= 1){
		for (int i=0;i<argc;i++){
			ESP_LOGI(TAG, "sensor_read %d: %s", i, argv[i]);

		}
	}
	int nerrors = arg_parse(argc, argv, (void **) &set_args);
	if (nerrors != 0) {
		arg_print_errors(stderr, set_args.end, argv[0]);
		return 1;
	}

	if (set_args.num_reads->count == 0){
		set_args.num_reads->ival[0] = 10;
	}

	const int n_reads= set_args.num_reads->ival[0];


	ESP_LOGI(TAG, "sensor_read num_reads:%d", n_reads);
	char* buf;
	buf = malloc(200 * 20 + 50);

	int res = MS45_sensor_read(buf);
	if (res) {
		printf("%s\n", buf);

	}
	free(buf);

//	printf("{\"valid_reads\":5, \"mInH2O\":[34.1,35.4,33.2,36.4,31],\"usec\":[1,2,3,4,5]}\n");

    return 0;

}
