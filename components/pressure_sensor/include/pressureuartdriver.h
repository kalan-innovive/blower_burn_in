/*
 * pressureuartdriver.h
 *
 *  Created on: Aug 15, 2022
 *      Author: ekalan
 */

#ifndef MAIN_PRESSUREUARTDRIVER_H_
#define MAIN_PRESSUREUARTDRIVER_H_

//#define EX_UART_NUM UART_NUM_0
#define PATTERN_CHR_NUM    (1)

//#include "freertos/queue.h"

void rx_task(void *arg);

void tx_task(void *arg);

void init_uart(void);

void init_uart_w_queue(void);

int sendUARTData(const char *data);

void uart_event_task(void *pvParameters);

void tx_task_spi_test(void *arg);


#endif /* MAIN_PRESSUREUARTDRIVER_H_ */
