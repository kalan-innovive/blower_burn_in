#ifndef COMPONENTS_SERIAL_INNO_INCLUDE_SERIAL_INNO_H_
#define COMPONENTS_SERIAL_INNO_INCLUDE_SERIAL_INNO_H_
#include <stdint.h>
#include "msg16.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#define TRANSMIT_WAIT_TIME 30 // Time in ticks to wait for transaction to be writtten
#define TRANSACT_TIMEOUT 50 // Time in ticks to wait for transaction to be writtten

#define UART_SERIAL_INNO 1
void uart_rx_task(void *pvParameters);

/*
 * Transmit bytes over the network
 * @Return the error code or the number of byteswritten
 *
 */
int uart_tx_task(uint8_t *buf, size_t len);
void setup_driver(void);
int clear_uart_rx_queue(void);
int transact_read(const msg16_t *request, msg16_t *response,
		TickType_t timeout);
int get_chipid(int devid, unsigned *chipid);
int get_raw_pressure(int devid, int *raw_pressure);
int check_dev_id(int devid);

#endif /* COMPONENTS_SERIAL_INNO_INCLUDE_SERIAL_INNO_TEST_H_ */
