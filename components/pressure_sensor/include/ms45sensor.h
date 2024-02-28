/*
 * ms45sensor.h
 *
 *  Created on: Aug 28, 2022
 *      Author: ekalan
 */

#ifndef MAIN_MS45SENSOR_H_
#define MAIN_MS45SENSOR_H_

void MS45_sensor_task(void *pvParams);

//#include "driver/spi_master.h"
//#include "pressureuartdriver.h"
//#include "spi_mod.h"

//int json_list(p_result_handle_t res, char *buf);
char* read_MS45_sensor_jsn(int num_reads);
int MS45_sensor_read(char* buf);

#endif /* MAIN_MS45SENSOR_H_ */
