/*
 * serialinno_api.c
 *
 *  Created on: Jan 9, 2024
 *      Author: ekalan
 */


#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
//#include "app_event_handler.h"
#include "esp_event.h"

#include "include/msg16.h"
#include "serial_inno.h"
//#include "serial_inno_api_test.h"

#include <stdio.h>

#include "driver/gpio.h"
#include "sdkconfig.h"

static const char *tag = "inno_api";

/*
 * Creates a read transaction to get the current pressure
 * @Param: int devid, unsigned *val
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int get_current_pressure(int devid, unsigned *val) {
	int ret = 0;
	TickType_t timeout = 30 / portTICK_PERIOD_MS;
	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid,
			.addr = REG_CURR_PRESS, .len = 1 };

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0xffff;

	unsigned tmp = 0;

	ret = transact_read(&msg_req, &msg_resp, timeout);
	if (ret < 1) {
		ESP_LOGW(tag, "current_pressure Transact Error: %d", ret);
		return ret;

	}
	tmp = (uint16_t) msg_resp.payload[0];
	ESP_LOGI(tag, "Returning current_pressure %d mInH2O", (uint16_t ) msg_resp.payload[0]);

	*val = tmp;

	return ret;
}

/*
 * Creates a read transaction to get the blower_type from
 * @Param: int devid, unsigned *blower_type
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int get_blower_type(int devid, unsigned *blower_type) {
	int ret = 0;
	TickType_t timeout = 12;

//	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid, .addr = REG_BLOWER_TYPE,
//				.len = 1 };

	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid, .addr = REG_RPM_B,
				.len = 1 };

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0;


	clear_uart_rx_queue();

	ret = transact_read(&msg_req, &msg_resp, timeout);

	if (ret < 1) {
		ESP_LOGW(tag, "get_blower_type Transact Error: %d", ret);
		return ret;
	}

	unsigned temp  = msg_resp.payload[0] ;
	if (temp == FAN){
		ESP_LOGI(tag, "Returning blowerType  0x%04x, Fan", temp);
	}
	else if (temp == VALVE){
		ESP_LOGI(tag, "Returning blowerType  0x%04x, VALVE", temp);
	}
	else if (temp == CONTROL){
			ESP_LOGI(tag, "Returning blowerType  0x%04x, CONTROL", temp);
		}
	else if (temp == UKNOWN_TYPE){
		ESP_LOGI(tag, "Returning blowerType  0x%04x, UKNOWN", temp);

	}
	*blower_type = (uint16_t) msg_resp.payload[0];
	return ret;
}

/*
 * Creates a Write transaction to set blower type
 * @Param: int val,
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -2 invalid blower type
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int set_blower_type(int devid, int val) {
	int ret = 0;
	TickType_t timeout = 100 / portTICK_PERIOD_MS;

	if (val == FAN){
			ESP_LOGI(tag, "Setting blowerType  0x%04x, Fan", val);
	}
	else if (val == VALVE){
		ESP_LOGI(tag, "Setting blowerType  0x%04x, VALVE", val);
	}
	else if (val == CONTROL){
			ESP_LOGI(tag, "Setting blowerType  0x%04x, CONTROL", val);
		}
	else {
		ESP_LOGI(tag, "Setting blowerType  0x%04x, UKNOWN", val);
		return -2;

	}

	msg16_t msg_req = { .type = WRITE_REQ, .dev_id = devid,
			.addr = REG_BLOWER_TYPE, .len = 1, .payload[0]= val };

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0xFFFF;


	ret = transact_write(&msg_req, &msg_resp, timeout);
	if (ret < 1) {
		ESP_LOGW(tag, " set_blower_type Transact Error: %d", ret);

	}
	ESP_LOGI(tag, "Set Blower Type value to: %d", (int16_t ) val);

	return ret;
}

/** Get the pressure sensor type*/
int set_sensor_type(int devid, unsigned val){
	int ret = 0;
	TickType_t timeout = 100 / portTICK_PERIOD_MS;



	if (val == PRESSURE_SENSOR_TYPE_MPXV7002){
			ESP_LOGI(tag, "Setting blowerType  0x%04x, Fan", val);
	}
	else if (val == PRESSURE_SENSOR_TYPE_MS4515DS3BS002){
		ESP_LOGI(tag, "Setting blowerType  0x%04x, VALVE", val);
	}
	else if (val == PRESSURE_SENSOR_TYPE_HSCDRRD002NDSA3){
			ESP_LOGI(tag, "Setting blowerType  0x%04x, CONTROL", val);
		}
	else if (val == PRESSURE_SENSOR_TYPE_HSCDRRD005NDSA3){
			ESP_LOGI(tag, "Setting blowerType  0x%04x, CONTROL", val);
		}
	else if (val == PRESSURE_SENSOR_TYPE_UNKNOWN){
		ESP_LOGI(tag, "Not valid sensorType  0x%04x, PRESSURE_SENSOR_TYPE_UNKNOWN", val);
		return -2;
	}
	else {
		ESP_LOGI(tag, "Abborting Type  0x%04x, UKNOWN", val);
		return -2;

	}

	uint8_t val_h, val_l;
	val_l = (val & 0x00FF);
	val_h = ~(val & 0x00FF);
	uint16_t s_msg = (val_h<<8) +val_l;


	msg16_t msg_req = { .type = WRITE_REQ, .dev_id = devid,
				.addr = REG_PRESSURE_SENSOR_TYPE, .len = 1, .payload[0]= s_msg };


	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0xFFFF;


	ret = transact_write(&msg_req, &msg_resp, timeout);
	if (ret < 1) {
		ESP_LOGW(tag, " set_blower_type Transact Error: %d", ret);

	}
	ESP_LOGI(tag, "Set Blower Type value to: %d", (int ) s_msg);

	return ret;
}


int get_sensor_type(int devid, unsigned *val) {
	int ret = 0;
	TickType_t timeout = 12;

	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid, .addr = REG_PRESSURE_SENSOR_TYPE,
				.len = 1 };

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0;


	clear_uart_rx_queue();

	ret = transact_read(&msg_req, &msg_resp, timeout);

	if (ret < 1) {
		ESP_LOGW(tag, "get_blower_type Transact Error: %d", ret);
		return ret;
	}

	unsigned temp  = msg_resp.payload[0] ;
	if (temp == PRESSURE_SENSOR_TYPE_MPXV7002){
			ESP_LOGI(tag, "Setting blowerType  0x%04x, Fan", temp);
	}
	else if (temp == PRESSURE_SENSOR_TYPE_MS4515DS3BS002){
		ESP_LOGI(tag, "Setting blowerType  0x%04x, VALVE", temp);
	}
	else if (temp == PRESSURE_SENSOR_TYPE_HSCDRRD002NDSA3){
			ESP_LOGI(tag, "Setting blowerType  0x%04x, CONTROL", temp);
		}
	else if (temp == PRESSURE_SENSOR_TYPE_HSCDRRD005NDSA3){
			ESP_LOGI(tag, "Setting blowerType  0x%04x, CONTROL", temp);
		}
	else if (temp == PRESSURE_SENSOR_TYPE_UNKNOWN){
		ESP_LOGI(tag, "Not valid sensorType  0x%04x, PRESSURE_SENSOR_TYPE_UNKNOWN", temp);
		return -2;
	}
	else {
		ESP_LOGI(tag, "Aborting Type  0x%04x, UKNOWN", temp);
		return -2;

	}
	*val = (unsigned) temp;
	return ret;
}

/*
 * Creates a read transaction to get the valve type
 * @Param: int devid, unsigned *blower_type
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int get_valve_position(int devid, unsigned *val) {
	int ret = 0;
	TickType_t timeout = 3;

	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid, .addr = REG_SERVO_DEG,
				.len = 1 };

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0;


	ret = transact_read(&msg_req, &msg_resp, timeout);
	unsigned tmp;
	if (ret < 1) {
		ESP_LOGW(tag, "get_valve_position Transact Error: %d", ret);
		return ret;
	}

	tmp  = msg_resp.payload[0] ;
	ESP_LOGI(tag, "Returning Valve position  %d mInH2O", (int16_t ) msg_resp.payload[0]);

	*val = (uint16_t) msg_resp.payload[0];
	return ret;
}



/*
 * Creates a read transaction to get the raw pressure in miH2O
 * @Param: int devid, unsigned *val
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int get_raw_pressure(int devid, int *val) {
	int ret = 0;
	TickType_t timeout = 30 / portTICK_PERIOD_MS;
	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid,
			.addr = REG_RAW_PRESS, .len = 1 };

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0xFFFF;


	int16_t tmp = 0;

	ret = transact_read(&msg_req, &msg_resp, timeout);
	if (ret < 1) {
		ESP_LOGW(tag, " get_raw_pressure Transact Error: %d", ret);
		return ret;

	}
	tmp = (int16_t) msg_resp.payload[0];
	ESP_LOGI(tag, "Returning Raw Pressure  %d mInH2O", (int16_t ) msg_resp.payload[0]);

	*val = (int16_t) tmp;

	return ret;
}

/*
 * Creates a read transaction to get the calibration pressure
 * @Param: int devid, unsigned *raw_pressure
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int get_caibration(int devid, int *val) {
	int ret = 0;
	TickType_t timeout = 30 / portTICK_PERIOD_MS;
	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid,
			.addr = REG_RAW_PRESS, .len = 1 };

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0xffff;

	int16_t tmp = 0;

	ret = transact_read(&msg_req, &msg_resp, timeout);
	if (ret < 1) {
		ESP_LOGW(tag, "get_valve_position Transact Error: %d", ret);
		return ret;
	}

	tmp = (int16_t) msg_resp.payload[0];
	ESP_LOGI(tag, "Returning Calibration value  %d mInH2O", (int16_t ) msg_resp.payload[0]);

	*val = tmp;

	return ret;
}

/*
 * Creates a Write transaction to run calibration on blower
 * @Param: int devid,
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int run_caibration(int devid) {
	int ret = 0;
	TickType_t timeout = 1000 / portTICK_PERIOD_MS;
	msg16_t msg_req = { .type = WRITE_REQ, .dev_id = devid,
			.addr = REG_CALIBRATE, .len = 1, .payload[0]= CALIBRATE_CMD };

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0xFFFF;
	// TODO add sleep before and after verify pressure is off

	ret = transact_write(&msg_req, &msg_resp, timeout);
	if (ret < 1) {
		ESP_LOGW(tag, "run_caibration Transact Error: %d", ret);
		return ret;
	}

	ESP_LOGI(tag, "Set Calibration Pressure Success %d", (int16_t ) msg_resp.payload[0]);

	return ret;
}

/*
 * Creates a read transaction to get the current target pressure
 * @Param: int devid, unsigned *val
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int get_target_pressure(int devid, unsigned *val) {
	int ret = 0;
	TickType_t timeout = 30 / portTICK_PERIOD_MS;
	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid,
			.addr = REG_RAW_PRESS, .len = 1 };

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0xffff;

	unsigned tmp = 0;

	ret = transact_read(&msg_req, &msg_resp, timeout);
	if (ret < 1) {
		ESP_LOGW(tag, "get_target_pressure Transact Error: %d", ret);

	}
	tmp = (uint16_t) msg_resp.payload[0];
	ESP_LOGI(tag, "Returning target pressure %d",  msg_resp.payload[0]);

	*val = tmp;

	return ret;
}

/*
 * Creates a Write transaction to set target pressure in mInH2O
 * @Param: int devid, unsigned val 0-2000
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int set_target_pressure(int devid, unsigned val) {
	int ret = 0;
//	ret = set_pwm(devid, RESET_PWM_CMD);
//	if (ret < 1) {
//		ESP_LOGW(tag, "target_pressure Transact Error could not send PWM off : %d", ret);
//		return ret;
//
//	}
//	vTaskDelay(0);

	TickType_t timeout = 100 / portTICK_PERIOD_MS;
	msg16_t msg_req = { .type = WRITE_REQ, .dev_id = devid,
			.addr = REG_TARG_PRESS, .len = 1, .payload[0]= val };

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0xffff;

	// TODO: verify the proper setpoint constraints and return

	ret = transact_write(&msg_req, &msg_resp, timeout);
	if (ret < 1) {
		ESP_LOGW(tag, "target_pressure Transact Error: %d", ret);
		return ret;

	}
	ESP_LOGI(tag, "target_pressure value to: %d", (int16_t ) val);

	return ret;
}


/*
 * Creates a read transaction to get the current fan speed
 * @Param: int devid, unsigned *val
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int get_fan_rpm(int devid, unsigned *val) {
	int ret = 0;
	TickType_t timeout = 30 / portTICK_PERIOD_MS;
	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid,
			.addr = REG_RPM_A, .len = 1 };

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0xffff;

	unsigned tmp = 0;

	ret = transact_read(&msg_req, &msg_resp, timeout);
	if (ret < 1) {
		ESP_LOGW(tag, "transact_read Transact Error: %d", ret);
		return ret;

	}
	tmp = (uint16_t) msg_resp.payload[0];
	tmp = (tmp==0) ? 0: tmp/2 * 60;
	ESP_LOGI(tag, "Returning Fan speed: %d RPM", tmp);

	*val = tmp;

	return ret;
}


/*
 * Creates a read transaction to get the current pwm of control system
 * 0 = min pwm control
 * 100 = max pwm_control
 * @Param: int devid, unsigned *val
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int get_pwm(int devid, unsigned *val) {
	int ret = 0;
	TickType_t timeout = 30 / portTICK_PERIOD_MS;
	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid,
			.addr = REG_CURR_PWM, .len = 1 };

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0xffff;

	unsigned tmp = 0;

	ret = transact_read(&msg_req, &msg_resp, timeout);
	if (ret < 1) {
		ESP_LOGW(tag, "get_pwm Transact Error: %d", ret);
		return ret;

	}
	tmp = (uint16_t) msg_resp.payload[0];
	tmp = tmp/2 * 60;
	ESP_LOGI(tag, " get_pwm Returning pwm: %d ", tmp);

	*val = tmp;

	return ret;
}


/*
 * Creates a Write transaction to set pwm testmode
 * @Param: int devid, unsigned val in miH2O
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int set_pwm(int devid, unsigned val) {
	int ret = 0;
	TickType_t timeout = 100 / portTICK_PERIOD_MS;
	msg16_t msg_req = { .type = WRITE_REQ, .dev_id = devid,
			.addr = REG_PWM_TEST_MODE, .len = 1, .payload[0]= (uint16_t)(val & 0xffff) };

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0xffff;

	ret = transact_write(&msg_req, &msg_resp, timeout);
	if (ret < 1) {
		ESP_LOGW(tag, "set_pwm Transact Error: %d", ret);

	}
	ESP_LOGI(tag, "Set set_pwm set %d ", (int) msg_resp.payload[0]);

	return ret;
}

/*
 * Creates a read transaction to get the current pwm of control system
 * 0 = min pwm control
 * 100 = max pwm_control
 * @Param: int devid, unsigned *val
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int get_uuid(int devid, unsigned *val)
{
	int ret = 0;
	TickType_t timeout = 30 / portTICK_PERIOD_MS;
	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid,
			.addr = REG_BLOWER_UUID2, .len = 2};

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0xffff;

	unsigned tmp = 0;
	ESP_LOGW(tag, "get_uuid Transact : %d", msg_req.addr);
	ret = transact_read(&msg_req, &msg_resp, timeout);
	if (ret < 1) {
		ESP_LOGW(tag, "get_uuid Transact Error: %d", ret);

	}
	uint16_t  tmp_h, tmp_l;
	tmp_h = (msg_resp.payload[0] & 0xFFFF);
	tmp_l = (msg_resp.payload[1] & 0xFFFF);
	tmp = (tmp_h << 16) |tmp_l;
	ESP_LOGI(tag, " get_uuid high: 0x%04X low:0x%04X", tmp_h, tmp_l);
	ESP_LOGI(tag, " get_uuid Returning uuid: 0x%08X ", tmp);

	*val = tmp;

	return ret;
}
/*
 * Creates a Write transaction to set uuid in nv store
 * @Param: int devid, unsigned val uuid
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int set_uuid(int devid, unsigned val) {
	int ret = 0;
	unsigned tmp;
	uint16_t  tmp_h, tmp_l;
	ESP_LOGI(tag, " set_uuid: 0x%08X ", val);
	tmp_h = (val >>16) & 0x0000FFFF;
	tmp_l = (val & 0x0000FFFF);
	tmp = (tmp_h << 16) | tmp_l;
	ESP_LOGI(tag, " set_uuid high: 0x%04X low:0x%04X ", tmp_h, tmp_l);
	ESP_LOGI(tag, " set_uuid Setting uuid: 0x%08X ", tmp);

	TickType_t timeout = 100 / portTICK_PERIOD_MS;
	msg16_t msg_req = { .type = WRITE_REQ, .dev_id = devid,
			.addr = REG_BLOWER_UUID2, .len = 2 };
	msg_req.payload[0] = tmp_h;
	msg_req.payload[1] = tmp_l;
	ESP_LOGI(tag, " set_uuid high: 0x%04X low:0x%04X ", msg_req.payload[0], msg_req.payload[1]);


	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0xffff;
	msg_resp.payload[1] = 0xffff;
	ESP_LOGI(tag, "set_uuid Transact: %d, addr:%d", val, (int)msg_req.addr);

	ret = transact_write(&msg_req, &msg_resp, timeout);
	if (ret < 1) {
		ESP_LOGW(tag, "set_uuid Transact Error: %d", ret);

	}
	ESP_LOGI(tag, "Set set_uuid set %u : %u ", tmp, val);

	return ret;
}



/*
 * Creates a read transaction for modbus id check, used to check if a modbus id is connected
 * @Param: int devid to check
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */
int check_dev_id(int devid) {
	ESP_LOGD(tag, "%s Checking Modbus id : %d", __FUNCTION__, devid);
	int ret = 0;
	TickType_t timeout = 30 / portTICK_PERIOD_MS;
	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid,
			.addr = REG_MODBUS_ADDR, .len = 1 };

	// Set up the msg set the msg to default and msg_val to 0 as we expect response 0xffff
	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0x00;


	ret = transact_read(&msg_req, &msg_resp, timeout);
	if (ret == 1) {
		ESP_LOGD(tag, "Modbus id found for id 0x%x resp code: %d", devid, ret);

	} else if (ret == 0) {
		ESP_LOGD(tag, "Did not receive response from id 0x%x resp code: %d",
				devid, ret);

	} else if (ret == -1) {
		ESP_LOGE(tag,
				"Error sending message not receive response from id 0x%x resp code: %d",
				devid, ret);

	}
	return ret;
}

/*
 * Creates a read transaction to get the chipID
 * @Param: int devid, unsigned *chipid
 * Stores chip id in pointer adress provided
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */


int get_chipid(int devid, unsigned *chipid) {
	int ret = 0;
	TickType_t timeout = 30 / portTICK_PERIOD_MS;

	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid, .addr = REG_CHIPIDH,
				.len = 2 };

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0;
	msg_resp.payload[1] = 0;

	unsigned high, low, c_id;

//	clear_uart_rx_queue();

	//Run transaction verify that the response was received without error
	ret = transact_read(&msg_req, &msg_resp, timeout);
	// Why ret
	if (ret < 1) {
		ESP_LOGW(tag, "get_chipid Transact Error: %d", ret);
		return ret;

	}
	if (msg_resp.len != 2) {
		ESP_LOGW(tag, "Chip ID Transact Error %d: %d", ret, msg_resp.len);
		return -1;

	}
	high = (msg_resp.payload[0] & 0xffff) << 16;
	low = (msg_resp.payload[1] & 0xffff);
	c_id = high + low;
	ESP_LOGI(tag, "Returning ChipID High 0x%04x Low 0x%04x  ChipID:%u", msg_resp.payload[0],msg_resp.payload[1], c_id);

	*chipid = c_id;

	return ret;

}


/*
 * Creates a read transaction to get the Hardware Version of the Device
 * @Param: int devid, unsigned *chipid
 * @Return:  1 if successful, 0 or -1 if failed to get transaction
 *  Error: -1 on invalid response
 *  Error: 0 on if response timed out dev unavailable
 */


int get_hw_version(int devid, unsigned *val) {
	int ret = 0;
	TickType_t timeout = 30 / portTICK_PERIOD_MS;

	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid, .addr = REG_HWTYPE,
				.len = 1 };

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0;

	//Run transaction verify that the response was received without error
	ret = transact_read(&msg_req, &msg_resp, timeout);
	// Why ret
	if (ret < 1) {
		ESP_LOGW(tag, "get_hw_version Transact Error: %d", ret);
		return ret;

	}
	if (msg_resp.len != 1) {
		ESP_LOGW(tag, "get_hw_version Transact Error %d: %d", ret, msg_resp.len);
		return -1;
	}

	ESP_LOGI(tag, "Returning Hardware Version 0x%04x ", msg_resp.payload[0]);

	*val = msg_resp.payload[0];

	return ret;

}

int get_sw_version(int devid, unsigned *major, unsigned *minor) {
	int ret = 0;
	TickType_t timeout = 30 / portTICK_PERIOD_MS;

	msg16_t msg_req = { .type = READ_REQ, .dev_id = devid, .addr = REG_VERSION,
				.len = 1 };

	msg16_t msg_resp;
	msg_resp.len = 0;
	msg_resp.payload[0] = 0;

	//Run transaction verify that the response was received without error
	ret = transact_read(&msg_req, &msg_resp, timeout);
	// Why ret
	if (ret < 1) {
		ESP_LOGW(tag, "get_sw_version Transact Error: %d", ret);
		return ret;

	}
	if (msg_resp.len != 1) {
		ESP_LOGW(tag, "get_sw_version Transact Error %d: %d", ret, msg_resp.len);
		return -1;
	}
	unsigned temp = msg_resp.payload[0];
	temp = (temp && 0x00FF);

	ESP_LOGI(tag, "Returning Software Version 0x%04x ", temp);

	*major = temp;
	*minor = 0;

	return ret;

}
