/*
 * pins.h
 *
 *  Created on: Aug 8, 2022
 *      Author: ekalan
 */

#ifndef MAIN_PINS_H_
#define MAIN_PINS_H_

#define DEV_BOX_ESP32_S3
//#define DEV_KIT_WROV_SPI0
#define MS45_SENSOR 1
#define RS485_COM 0
#include "hal/gpio_types.h"

#if CONFIG_IDF_TARGET_ESP32S2
#ifdef DEV_KIT_WROV_SPI0
// Brown  | IO26 | pin  | GPIO26  | Camera | SS   | CS0
// Orange | IO35 | pin  | GPIO35  | Camera | SCK  | SCLK
// Green  | IO39 | pin  | GPIO39  | Camera | MISO | SDO
// NA     | IO34 | pin  | GPIO34  | Camera | MOSI | SDA

#define PIN_NUM_CS   26
#define PIN_NUM_CLK  23  // working option 20
#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 22 // Trying 36 after 34 failed
#endif
#endif
#if CONFIG_IDF_TARGET_ESP32S3
#include "driver/gpio.h"
// Brown  | IO10 | pin  | GPIO10  | Camera | SS   | CS0
// Orange | IO12 | pin  | GPIO12  | Camera | SCK  | SCLK
// Green  | IO13 | pin  | GPIO13  | Camera | MISO | SDO
// NA     | IO11 | pin  | GPIO11  | Camera | MOSI | SDA

#define PIN_NUM_CS   10
#define PIN_NUM_CLK  12  // working option 20
#define PIN_NUM_MISO 13
#define PIN_NUM_MOSI 11 // Trying 36 after 34 failed

// Blue   | IO34 | pin  | GPIO34  | Rear | tx
// White  | IO12 | pin  | GPIO44  | Rear | rx

#define PIN_NUM_TX_  19
#define PIN_NUM_RX_  20


#endif




#if (RS485_COM & MS45_SENSOR)
// Uart Controller to interface with innoRack modbus over half duplex RS485
#define UART_RS485 UART_NUM_2
#define TXD_PIN_RS485 GPIO_NUM_17
#define RXD_PIN_RS485 GPIO_NUM_16

// Uart Controller to interface with computer uart->serial connector

#define UART_SERIAL UART_NUM_1
#define TXD_PIN_SER GPIO_NUM_4
#define RXD_PIN_SER GPIO_NUM_5

#elif MS45_SENSOR

// Uart Controller to interface with innoRack modbus over half duplex RS485
#define UART_RS485 UART_NUM_2
#define TXD_PIN_RS485 GPIO_NUM_17
#define RXD_PIN_RS485 GPIO_NUM_16

// Uart Controller to interface with computer uart->serial connector

#define UART_SERIAL UART_NUM_0
#define TXD_PIN_SER PIN_NUM_TX_
#define RXD_PIN_SER PIN_NUM_RX_

#else

// Uart Controller to interface with innoRack modbus over half duplex RS485
#define UART_RS485 UART_NUM_1
#define TXD_PIN_RS485 GPIO_NUM_4
#define RXD_PIN_RS485 GPIO_NUM_5

// Uart Controller to interface with computer uart->serial connector

#define UART_SERIAL UART_NUM_1
#define TXD_PIN_SER GPIO_NUM_4
#define RXD_PIN_SER GPIO_NUM_5

#endif

#endif /* MAIN_PINS_H_ */
