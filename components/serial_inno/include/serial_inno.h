#ifndef COMPONENTS_SERIAL_INNO_INCLUDE_SERIAL_INNO_H_
#define COMPONENTS_SERIAL_INNO_INCLUDE_SERIAL_INNO_H_
#include <stdint.h>
#include "msg16.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#define TRANSMIT_WAIT_TIME 30 // Time in ticks to wait for transaction to be writtten
#define TRANSACT_TIMEOUT 50 // Time in ticks to wait for transaction to be writtten

#define UART_SERIAL_INNO 1
#define USING_CONTROL_LINE_RTS 1

void setup_driver(void);

/** Inno Serial Modbus tasks*/
void uart_rx_task(void *pvParameters);
int uart_tx_task(uint8_t *buf, size_t len);
/*
 * Transact functions
 * @Return the error code or the number of byteswritten
 *
 */
int transact_read(const msg16_t *request, msg16_t *response,
		TickType_t timeout);
int transact_write(const msg16_t *request, msg16_t *response,
		TickType_t timeout);
int clear_uart_rx_queue(void);

/** Current Pressure Register*/
int get_current_pressure(int devid, unsigned *val);
/** Blower Type Register*/
int get_blower_type(int devid, unsigned *blower_type);
int set_blower_type(int devid, int val);
/** Valve Position Register*/
int get_valve_position(int devid, unsigned *val);
/** Raw Pressure Register*/
int get_raw_pressure(int devid, int *val);
/** Calibration Register*/
int get_caibration(int devid, int *val);
int run_caibration(int devid);
/** Target Pressure Register*/
int get_target_pressure(int devid, unsigned *val);
int set_target_pressure(int devid, unsigned val);
/** Fan RPM Register*/
int get_fan_rpm(int devid, unsigned *val);
/** PWM Registers Register*/
int get_pwm(int devid, unsigned *val);
int set_pwm(int devid, unsigned val);
/** Device ID Register*/
int check_dev_id(int devid);
/** Chip ID Register*/
int get_chipid(int devid, unsigned *chipid);

#endif /* COMPONENTS_SERIAL_INNO_INCLUDE_SERIAL_INNO_TEST_H_ */
