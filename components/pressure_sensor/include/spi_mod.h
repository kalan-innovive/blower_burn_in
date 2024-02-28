/*
 * spi_mod.h
 *
 *  Created on: Aug 8, 2022
 *      Author: ekalan
 */

#ifndef MAIN_SPI_MOD_H_
#define MAIN_SPI_MOD_H_

#include "driver/spi_master.h"

#define DMA_CHAN  SPI_DMA_CH_AUTO

spi_device_handle_t spi_init(void);

typedef struct {
	uint32_t time;
	float val;
} p_reading_msg;

typedef p_reading_msg *p_reading_msg_array;

#define READING_ERROR_VALUE 0xFFFF
#define DEFAULT_MIN -2222
typedef struct {
	uint32_t num_reads;
	uint32_t valid_reads;
	float min;
	float max;
	float avg;
	uint32_t start;
	uint32_t stop;
	uint32_t elapsed;
	p_reading_msg *r_msgs;
} p_results;

typedef p_results *p_result_handle_t; ///< Handle for a p sensor readings

p_result_handle_t init_results(uint32_t num);
void set_results(p_result_handle_t res);
void collect_samples_s(p_result_handle_t res, spi_device_handle_t spi);
void collect_samples(p_result_handle_t res, spi_device_handle_t spi);
void free_p_results(p_result_handle_t res);
void print_result_handle(p_result_handle_t res);
int json_list(p_result_handle_t res, char *buf);

int is_spi_set(void);


#endif /* MAIN_SPI_MOD_H_ */
