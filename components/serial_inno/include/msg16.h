/*
 * msg16.h
 *
 *  Created on: Apr 19, 2023
 *      Author: ekalan
 */

#ifndef COMPONENTS_SERIAL_INNO_INCLUDE_MSG16_H_
#define COMPONENTS_SERIAL_INNO_INCLUDE_MSG16_H_
#include <stdint.h>
#include <stdlib.h>

#define READ_REQ 0x20
#define WRITE_REQ 0x21
#define READ_RESP 0xa0
#define WRITE_RESP 0xa1

#define REG_HWTYPE_RETURN 0xE107
#define NUM_NV_STORE 14
#define RECORD_VERSION 4

#define FLAG 0x07e
#define ESC 0x07d
#define CHK_LEN 2
#define READ_PAYLOAD_LEN 3
#define READ_RESP_PAYLOAD_LEN 2
#define READ_WRITE_LEN 3
#define GET_PAYLOAD_LEN(frame_len) (frame_len-5)
#define GET_FRAME_LEN(payload_len) (payload_len+5)
#define CALIBRATE_CMD 0x5678
#define RESET_PWM_CMD 0xffff
#ifdef MAIN

#define global(var,val) var = val

#else

#define global(var,val) extern var

#endif
// pre-release gets 0xFF for version
// last released version was 8
#define SW_VERSION 8
#define HW_VERSION 107  // FIXME - should have a board rev included here

typedef enum {
	PRESSURE_SENSOR_TYPE_MPXV7002 = 0, 		/** Voltage regulated pressure sensor*/
	PRESSURE_SENSOR_TYPE_MS4515DS3BS002 = 1,	/** TE Pressure Sensor 2 In H2O*/
	PRESSURE_SENSOR_TYPE_HSCDRRD002NDSA3 =2,	/** Honeywell Pressure Sensor 2 In H2O*/
	PRESSURE_SENSOR_TYPE_HSCDRRD005NDSA3 = 3,	/** Honeywell Pressure Sensor 5 In H2O*/
	PRESSURE_SENSOR_TYPE_UNKNOWN = 0xFFFF,	/** Pressure Sensor is undefined will default to app sensor*/
}pressure_type;

typedef enum {
	DEV_SUPA = 0x01,
	DEV_EXHA = 0x03,
	DEV_SUPB = 0x21,
	DEV_EXB = 0x23,
	DEV_UNDEF = 0xff,
	DEV_RACK = 0x11,
} dev_id;


typedef enum{
	FAN = 0x00,
	VALVE = 0x01,
	CONTROL = 0xFE,
	UKNOWN_TYPE = 0xFFFF
} blower_t;


typedef enum {
	BLOWER_TYPE_IDX = 0,
	CAL_1 = 1,
	CAL_2,
	CAL_3,
	CAL_4,
	CAL_5,

	KI_IDX = 6,
	KP_IDX = 7,
	CONT_FREQ_IDX = 8,
	SAMP_FREQ_IDX = 9,
	NUM_SAMPLE_IDX = 10,
	UUID_IDX = 11

}NVStoreIndex;

typedef enum {
	REG_STATUS,               //0
	REG_CURR_PRESS,           //1
	REG_TARG_PRESS,           //2
	REG_RPM_A,                //3
	REG_RPM_B,                //4
	REG_CMD,                  //5
	REG_CALIBRATE,            //6
	REG_DIAGIDfan,            //7
	REG_SERVO_DEG,            //8
	REG_HC_DETECT,            //9
	REG_RAW_PRESS, 			//10 signed pressure in 0.001 Inch H2O prior to calibration (signed)
	REG_RAW_SAMPLE,			//11 averaged ADC or pressure sample in native output
	REG_RAW_SAMPLE_MIN,		//12 minimum native sample over last averaging period
	REG_RAW_SAMPLE_MAX,		//13 maximum native sample over last averaging period
	REG_CURR_PWM,			//14 current pwm value [0-100]
	REG_CURR_FANS,			//15 current number of enabled fanse
	REG_FAIL_PWM,			//16 pwm value [0-100] to use on pressure sensor fail
	REG_FAIL_FANS,			//17 number of fans to use on pressure sensor fail
	REG_CURRENT_TEMP,				//18 units are 0.1C (signed)
	REG_PRESSURE_SENSOR_TYPE,//19 0 = MPXV7002, 1 = MS4515DS3BS002, 0xffff = unknown
	REG_PWM_TEST_MODE,			//20 0xffff = normal operation, 0-100 PWM value, others ?
	REG_VALVE_POS,			//21 0xffff = normal operation, 0-1000 PWM value, others ?
	REG_VALVE_POS_MAX,			//22 0xffff = normal operation, 0-1000 PWM value, others ?
	REG_VALVE_POS_MIN,			//22 0xffff = normal operation, 0-1000 PWM value, others ?

	REG_HWTYPE = 0x1000,        //0x1000
	REG_VERSION,              //0x1001
	REG_INVOKE_BOOTLOADERH,   //0x1002
	REG_INVOKE_BOOTLOADERL,   //0x1003
	REG_CHIPIDH,              //0x1004
	REG_CHIPIDL,              //0x1005
	REG_MODBUS_ADDR,          //0x1006
	REG_BLOWER_TYPE,    	  //0x1007 0 == fan, 1 == valve- airflow related registers
	REG_BLOWER_UUID1,    	  //0x1008 0 == fan, 1 == valve- airflow related registers

	REG_NV_STORE_LOW = 0xA000,          //0xA000
	REG_NV_STORE_HIGH = (REG_NV_STORE_LOW + NUM_NV_STORE - 1),
	REG_BLOWER_UUID2 = REG_NV_STORE_LOW + UUID_IDX // Set for record version 3 and 4 comparability

} blower_reg_addr;

typedef enum {
//    REG_CHIPIDH=0x1004,
//    REG_CHIPIDL=0x1005,
//    REG_MODBUS_ADDR=0x1006,
	INFO_REVISION = 0xA000,
	INFO_RACKTYPE = 0xA001,
	INFO_ROWS = 0xA002,
	INFO_COLUMNS = 0xA003,
	INFO_SIDES = 0xA004,
	INFO_CHECKSUM = 0xA005,
	INFO_RACKREV = 0xA006,

} rack_reg_addr;

typedef struct {
	/* Type:READ_REQ, READ_RESP, WRITE_REQ, WRITE_REP*/
	uint16_t type;
	/* Type: one of dev_id values*/
	uint16_t dev_id;
	/* Address of register considered the base address for multi read and write*/
	uint16_t addr;
	/* Length:
	 * READ_REQ= Number of consecutive registers requested stored in payload
	 * READ_RESP: Number of registers in response starting at base address
	 * WRITE_REQ : Number consecutive registers to write stored in payload
	 * WRITE_RESP: Number of consecutive registers in payload
	 */
	uint16_t len;
	/* Array holder of values set to Null if not using */
	uint16_t payload[128];
} __attribute__((packed)) msg16_t;

size_t pack_msg16(const msg16_t *msg16, uint8_t *packed_msg,
		size_t *packed_msg_size);
//void send_msg16(msg16_t* msg16, char* response, size_t response_size);
size_t unpack_msg16(uint8_t *packed_msg, size_t packed_msg_size,
		msg16_t *msg16);

#endif /* COMPONENTS_SERIAL_INNO_INCLUDE_MSG16_H_ */
