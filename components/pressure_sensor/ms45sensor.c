/*
 * ms45sensor.c
 *
 *  Created on: Aug 28, 2022
 *      Author: ekalan
 */

/*
 * Include the generic headers required for the FreeRTOS port being used.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "ms45sensor.h"
#include "spi_mod.h"
#include "pressureuartdriver.h"

static const char *TAG = "MS45";
static 	spi_device_handle_t spi_h;

// Set the queue handle
extern QueueHandle_t read_spi_queue;

void MS45_sensor_task(void *pvParams)
{

	// Init the spi bus and handle
	spi_device_handle_t spi = spi_init();
	p_result_handle_t res;

	// Move to the header after debug
	uint32_t MAX_NUM_READS = 1024;

	// Holds the number of reads expected
	uint32_t num_reads = 50;
	// Used for the buffer to send uart message should be at least the
	//    size of the num_reads_expected * 20
	char *buf = malloc(MAX_NUM_READS * 20 + 50);

	for (;;) {
//		 Wait for read statement from uart
		if (xQueueReceive(read_spi_queue, (void*) &num_reads,
				(TickType_t) 10000 / portTICK_PERIOD_MS)) {
			ESP_LOGI(TAG, "spi[%d] event:", (int)num_reads);

			if (num_reads > MAX_NUM_READS) {
				ESP_LOGI(TAG,
						"Request for reads (%lu) is greater than max reads(%lu)",
						num_reads, MAX_NUM_READS);
				continue;
			} else if (num_reads == 0) {
				ESP_LOGI(TAG, "Request for reads (%lu) is zero", num_reads);
				continue;
			} else {
				res = init_results(num_reads);
				collect_samples(res, spi);
				set_results(res);
				json_list(res, buf);
				sendUARTData(buf);
				print_result_handle(res);
			}
		}
	}
	free(buf);
}

int MS45_sensor_read(char* buf)
{

	// Init the spi bus and handle
	if (!is_spi_set()) {
			spi_h = spi_init();

		}
	p_result_handle_t res;


	// Move to the header after debug
	uint32_t MAX_NUM_READS = 150;

	// Holds the number of reads expected
	uint32_t num_reads = 10;
//	// Used for the buffer to send uart message should be at least the
//	//    size of the num_reads_expected * 20
//	*buf = malloc(MAX_NUM_READS * 20 + 50);

	if (num_reads > MAX_NUM_READS) {
		ESP_LOGI(TAG,
				"Request for reads (%lu) is greater than max reads(%lu)",
				num_reads, MAX_NUM_READS);
	} else if (num_reads == 0) {
		ESP_LOGI(TAG, "Request for reads (%lu) is zero", num_reads);
	} else {
		res = init_results(num_reads);
		collect_samples(res, spi_h);
		set_results(res);
		json_list(res, buf);
		return 1;

	}
	return 0;


}
/*
 * read MS45 sensor
 * Param: number of reads
 * Param: jsn_result buffer to place the jason message into
 * Format {valid_reads:n,mInH2O:[n1,n2,...], usec[n1,n2,...]}
 *      valid_reads=int Max value 1024
 *      mInH20: List of floats: min -2000 max 2000 no read -2222.2
 *      usec: List of Unsigned ints; All values are micro seconds since the read was initiated
 * Note: buff is an empty pointer that is initialized it is your responsibility to free it and not overflow the heap
 */
char* read_MS45_sensor_jsn(int num_reads){
	if (!is_spi_set()) {
		spi_h = spi_init();

	}
	uint32_t MAX_NUM_READS = 1024;
	char *buf = malloc(MAX_NUM_READS * 20 + 50);
	p_result_handle_t *res = (p_result_handle_t*) malloc(sizeof(p_result_handle_t));

	res = init_results(num_reads);
	collect_samples(res, spi_h);
	set_results(res);
	json_list(res, buf);
	return  buf;


}
