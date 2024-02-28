/*
 * spi_mod.c
 *
 *  Created on: Aug 8, 2022
 *      Author: ekalan
 */
#include "driver/spi_master.h"
#include "pins.h"
#include "spi_mod.h"
#include "esp_log.h"
#include "string.h"
#include "helpers.h"
#include "cjson.h"

static const char *TAG = "spi";
static int driver_set = 0;


int is_spi_set(){
	return driver_set;
}

/* SPI Config */
spi_device_handle_t spi_init(void) {
	spi_device_handle_t spi;
	esp_err_t ret;
	spi_bus_config_t buscfg = {
			.miso_io_num = PIN_NUM_MISO,
			.mosi_io_num = -1,
			.sclk_io_num = PIN_NUM_CLK,
			.quadwp_io_num = -1,
			.quadhd_io_num = -1,
			.max_transfer_sz = 16,
	};

	//Initialize the SPI bus
	ret = spi_bus_initialize(SPI3_HOST, &buscfg, DMA_CHAN);
	ESP_ERROR_CHECK(ret);

	spi_device_interface_config_t devCfg = {
			.mode = 0,
			.clock_speed_hz = 35 * 1000,
			.spics_io_num = PIN_NUM_CS,
			.queue_size = 3,
	};
	ESP_LOGI(TAG, "SPI Port Pin");
	ESP_LOGI(TAG, "MISO   : %d", PIN_NUM_MISO);
	ESP_LOGI(TAG, "CLK    : %d", PIN_NUM_CLK);
	ESP_LOGI(TAG, "CSel   : %d", PIN_NUM_CS);



	ret = spi_bus_add_device(SPI3_HOST, &devCfg, &spi);
	ESP_ERROR_CHECK(ret);
	driver_set = 1;

	return spi;
}

p_result_handle_t init_results(uint32_t num) {

	p_reading_msg *msgs = NULL;
//	p_result_handle_t p_handle = NULL;
	p_results *res;
	res = malloc(sizeof(p_results));
	//Allocate memory
	msgs = malloc(num * sizeof(p_reading_msg));

	res->num_reads = num;
	res->valid_reads = 0;
	res->min = DEFAULT_MIN;
	res->max = READING_ERROR_VALUE;
	res->avg = READING_ERROR_VALUE;
	res->start = 0;
	res->stop = 0;
	res->elapsed = 0;
	res->r_msgs = msgs;

	p_result_handle_t p_handle = res;

	return p_handle;
}

void set_results(p_result_handle_t res) {

	float tmp = 0;

	for (int i = 0; i < res->num_reads; i++) {
		p_reading_msg *p = &res->r_msgs[i];
		if (p->val <= -2222) {
			continue;
		}
//		res->start = (res->start == 0) ? p->time : res->start;
//		res->stop = p->time;

		res->min = (p->val < res->min) ? p->val : res->min;
		res->max = (p->val > res->max) ? p->val : res->max;

		tmp += p->val;
		res->valid_reads++;
//		ESP_LOGI(TAG, "%d\tTime: %d, Val:%.2f\n", i, res->r_msgs[i].time,
//				res->r_msgs[i].val);

	}
	// Compute the elapsed time in milli seconds
	res->elapsed = res->stop - res->start;
	// Set the average to either error value or computed average
	res->avg =
			(res->num_reads != 0) ?
									tmp / (res->valid_reads * 1.0) :
									READING_ERROR_VALUE;
}

void collect_samples_s(p_result_handle_t res, spi_device_handle_t spi) {
	uint32_t data;

	spi_transaction_t tM = {
			.tx_buffer = NULL,
			.rx_buffer = &data,
			.length = 16,
			.rxlength = 16,
	};
	ESP_LOGI(TAG, "Acquiring SPI Port\n");
	spi_device_acquire_bus(spi, portMAX_DELAY);
	ESP_LOGI(TAG, "Transmitting read\n");
	spi_device_transmit(spi, &tM);
	res->start = us_timestamp();
	for (int i = 0; i < res->num_reads; i++) {
		spi_device_transmit(spi, &tM);

		uint16_t r = (int16_t) SPI_SWAP_DATA_RX(data, 16);

		uint16_t tmp = r & 0x03FFF;

//		float val = (4000 / (0.9 * 0x4000)) * (tmp - 0x02000);
	    static int32_t press_max = -2000;
	    static int32_t press_min = 2000;
		float val = ((tmp - 0x0666) * (press_max - (press_min)) / (0x3999 - 0x0666)) + (press_min);

		if (res == 0) {
			ESP_LOGE(TAG, "Sensor is not connected\n");

		}
		uint32_t t_stamp = us_timestamp() - res->start;
		res->r_msgs[i].time = t_stamp;
		res->r_msgs[i].val = val;

//		ESP_LOGI(TAG, "%p\tTime: %d, Val:%.2f\n",
//				(void* ) &res->r_msgs[i],
//				t_stamp,
//				val);
	}

	spi_device_release_bus(spi);
	res->stop = us_timestamp();
	ESP_LOGI(TAG, "Releasing bus\n");

}

void collect_samples(p_result_handle_t res, spi_device_handle_t spi) {
	uint32_t data;

	spi_transaction_t tM = {
			.tx_buffer = NULL,
			.rx_buffer = &data,
			.length = 16,
			.rxlength = 16,
	};
	ESP_LOGI(TAG, "Acquiring SPI Port\n");
	res->start = us_timestamp();

	for (int i = 0; i < res->num_reads; i++) {
		spi_device_acquire_bus(spi, portMAX_DELAY);
//		ESP_LOGI(TAG, "Transmitting read\n");
		spi_device_transmit(spi, &tM);
		spi_device_transmit(spi, &tM);
		spi_device_release_bus(spi);

		uint16_t r = (int16_t) SPI_SWAP_DATA_RX(data, 16);

		uint16_t tmp = r & 0x03FFF;

		float val = (4000 / (0.9 * 0x4000)) * (tmp - 0x02000);

		if (res == 0) {
			ESP_LOGE(TAG, "Sensor is not connected\n");

		}
		uint32_t t_stamp = us_timestamp() - res->start;
		res->r_msgs[i].time = t_stamp;
		res->r_msgs[i].val = val;

		ESP_LOGD(TAG, "Value:%.2f,   Raw Output: 0x:%X", val, tmp);


//		ESP_LOGI(TAG, "%p\tTime: %d, Val:%.2f\n",
//				(void* ) &res->r_msgs[i],
//				t_stamp,
//				val);
	}

	res->stop = us_timestamp();

}

void free_p_results(p_result_handle_t p) {
	free(p->r_msgs);
	free(p);
}

void create_json_resp(p_result_handle_t p, int req_id) {
	ESP_LOGI(TAG, "Serialize.....");
	cJSON *root;
	root = cJSON_CreateObject();

	float* mih2o = (float*)pvPortMalloc(p->num_reads * sizeof(float));
	int* usec = (int*)pvPortMalloc(p->num_reads * sizeof(int));


	for (int i = 0; i< p->num_reads; i++){
		p_reading_msg *r = p->r_msgs;
		if (r->val == READING_ERROR_VALUE || r->val< DEFAULT_MIN ){
			continue;
		}
		mih2o[i] = r->val;
		usec[i] = r->time;
	}

	cJSON *floatArray;
	floatArray = cJSON_CreateFloatArray(mih2o, p->num_reads);
	cJSON_AddItemToObject(root, "mInH2O", floatArray);



	// Free string by caller
	char *res_json_string = cJSON_Print(root);
	ESP_LOGI(TAG, "my_json_string\n%s",res_json_string);
	cJSON_Delete(root);

	// Buffers returned by cJSON_Print must be freed by the caller.
	// Please use the proper API (cJSON_free) rather than directly calling stdlib free.
	cJSON_free(res_json_string);

	cJSON *root2;
	root2 = cJSON_CreateObject();
	cJSON *intArray;
	intArray = cJSON_CreateIntArray(usec, p->num_reads);
	cJSON_AddItemToObject(intArray, "usec", intArray);

	char *my_json_string2 = cJSON_Print(root2);
	ESP_LOGI(TAG, "my_json_string\n%s",my_json_string2);
	cJSON_Delete(root2);

	// Buffers returned by cJSON_Print must be freed by the caller.
	// Please use the proper API (cJSON_free) rather than directly calling stdlib free.
	cJSON_free(my_json_string2);
	vPortFree(mih2o);
	vPortFree(usec);
}

void print_result_handle(p_result_handle_t p) {
	ESP_LOGI("P Result handle", "Print result handle:\n"
			"\t number reads: %lu \n"
			"\t valid_reads : %lu \n"
			"\t min:          %.1f \n"
			"\t max:          %.1f \n"
			"\t avg:          %.1f \n"
			"\t start/ stop:  %lu : %lu \n"
			"\t elapsed:      %lu \n",
			p->num_reads,
			p->valid_reads,
			p->min,
			p->max,
			p->avg,
			(uint32_t)p->start,
			(uint32_t)p->stop,
			(uint32_t)p->elapsed
			);
}

int json_list(p_result_handle_t res, char *buf) {
	int buf_size = res->num_reads * 15 + 25;
//	char *valstr = malloc(buf_size);
	char valstr[buf_size];
	char timestr[buf_size];

	int pos_v = 0;
	int pos_t = 0;

	const char *info_str = ""
//			"\"Results\": { "
					"{\"num_reads\" : %d,";
	int pos_b = 0;
	pos_b += sprintf(buf, info_str, res->valid_reads);

	int n = res->num_reads;
	p_reading_msg *p = res->r_msgs;
	pos_v += sprintf(&valstr[pos_v], "\"mInH2O\" :[");
	pos_t += sprintf(&timestr[pos_t], "\"usec\" :[");

	for (int k = 0; k < n; k++, p++) {
		pos_v += sprintf(&valstr[pos_v], "%.2f,", p->val);
		pos_t += sprintf(&timestr[pos_t], "%d,", (int)p->time);
	}
	pos_v += sprintf(&valstr[pos_v - 1], "]");
	pos_t += sprintf(&timestr[pos_t - 1], "]");

	ESP_LOGI("JSON", "Val  Array is now: %s length %zu \n", valstr,
			strlen(valstr));
	ESP_LOGI("JSON", "Time Array is now: %s length %zu\n", timestr,
			strlen(timestr));

	pos_b += sprintf(&buf[pos_b], " %s,%s } \n", valstr, timestr);

	ESP_LOGI("JSON", "Msg:\n %s \n\tlength: %zu\n", buf,
			strlen(buf));

//	free(valstr);

	return 1;
}
