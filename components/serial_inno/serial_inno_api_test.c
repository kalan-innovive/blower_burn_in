/*
 * innno_serial_test_api.c
 *
 *  Created on: Jan 10, 2024
 *      Author: ekalan
 */


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "serial_inno_test.h"
#include "serial_inno.h"
#include <string.h>


#include "msg16.h"
#include "rack_device.h"
#include "esp_log.h"
#include "esp_err.h"

static const char* tag = "ser_inno_api_test";

static esp_err_t test_get_blower_type();
static esp_err_t test_get_valve_position();
static esp_err_t test_get_raw_pressure();

static esp_err_t test_get_caibration();
static esp_err_t test_run_caibration();
static esp_err_t test_get_target_pressure();
static esp_err_t test_get_fan_rpm();
static esp_err_t test_set_target_pressure();

esp_err_t serial_inno_blower_api_tests(void){
	esp_err_t ret;

	vTaskDelay(1000 / portTICK_PERIOD_MS);

	ret = test_get_blower_type();
	ret = test_get_valve_position();
	ret = test_get_raw_pressure();
	ret = test_get_caibration();
	ret = test_get_target_pressure();
	ret = test_get_fan_rpm();
	ret = test_run_caibration();

	ret = test_set_target_pressure();


	return ret;
}

static esp_err_t test_get_blower_type() {
	int ret;
	unsigned val;
	ret = get_blower_type(DEV_SUPA, &val);
	if (ret == 1){
		ESP_LOGI(tag, "Passed test_get_blower_type  %d type %d", ret, val);
		if (val == FAN){
			return ESP_OK;
		}
		else if (val == VALVE) {
			return ESP_OK;
		}
		else if (val == CONTROL) {
			return ESP_OK;
		}
		else if (val == UKNOWN_TYPE) {
			return ESP_OK;
		}
	}
	ESP_LOGE(tag, "Failed test_get_blower_type %d ", ret);
	return ESP_ERR_INVALID_RESPONSE;
}


static esp_err_t test_get_valve_position() {
	int ret;
	unsigned val;
	ret = get_valve_position(DEV_SUPA, &val);
	if (ret == 1){
		ESP_LOGI(tag, "Passed test_get_valve_position test  %d type %d", ret, val);
		return ESP_OK;

	}
	ESP_LOGE(tag, "Failed test_get_valve_position %d ", ret);
	return ESP_ERR_INVALID_RESPONSE;
}


static esp_err_t test_get_raw_pressure() {
	int ret;
	int val;
	ret = get_raw_pressure(DEV_SUPA, &val);
	if (ret == 1){
		ESP_LOGI(tag, "Passed get_raw_pressure test ret:%d val %d", ret, val);
		return ESP_OK;

	}
	ESP_LOGE(tag, "Failed blower type test %d ", ret);
	return ESP_ERR_INVALID_RESPONSE;
}


static esp_err_t test_get_caibration() {
	int ret;
	int val;
	ret = get_caibration(DEV_SUPA, &val);
	if (ret == 1){
		ESP_LOGI(tag, "Passed get_caibration test ret:%d val %d", ret, val);
		return ESP_OK;

	}
	ESP_LOGE(tag, "Failed get_caibration test %d ", ret);
	return ESP_ERR_INVALID_RESPONSE;
}


static esp_err_t test_run_caibration() {
	int ret;
	ret = run_caibration(DEV_SUPA);
	if (ret == 1){
		ESP_LOGI(tag, "Passed run_caibration test ret:%d", ret);
		return ESP_OK;

	}
	ESP_LOGE(tag, "Failed run_caibration test %d ", ret);
	return ESP_ERR_INVALID_RESPONSE;
}


static esp_err_t test_get_target_pressure() {
	int ret;
	unsigned val;
	ret = get_target_pressure(DEV_SUPA, &val);
	if (ret == 1){
		ESP_LOGI(tag, "Passed get_target_pressure test ret:%d val %d", ret, val);
		return ESP_OK;
	}
	ESP_LOGE(tag, "Failed get_target_pressure test %d ", ret);
	return ESP_ERR_INVALID_RESPONSE;
}



static esp_err_t test_get_fan_rpm() {
	int ret;
	unsigned val;
	ret = get_fan_rpm(DEV_SUPA, &val);
	if (ret == 1){
		ESP_LOGI(tag, "Passed get_fan_rpm test ret:%d val %d", ret, val);
		return ESP_OK;
	}
	ESP_LOGE(tag, "Failed get_fan_rpm test %d ", ret);
	return ESP_ERR_INVALID_RESPONSE;
}


static esp_err_t test_set_target_pressure() {
	int ret;
	unsigned val = 300;
	ret = set_target_pressure(DEV_SUPA, val);
	if (ret == 1){
		ESP_LOGI(tag, "Passed set_target_pressure test ret:%d val %d", ret, val);
		return ESP_OK;
	}
	ESP_LOGE(tag, "Failed set_target_pressure test %d ", ret);
	return ESP_ERR_INVALID_RESPONSE;
}


//
//
//static esp_err_t test_set_target_pressure() {
//	int ret;
//	unsigned val;
//	ret = get_fan_rpm(DEV_SUPA, &val);
//	if (ret == 1){
//		ESP_LOGI(tag, "Passed set_target_pressure test ret:%d val %d", ret, val);
//	}
//	ESP_LOGE(tag, "Failed set_target_pressure test %d ", ret);
//	return ESP_ERR_INVALID_RESPONSE;
//}
//
typedef enum {
	cal_init,
	cal_stopping_fan,
	cal_wait,
	cal_running,
	cal_complete,
	cal_success,
	cal_failed
}calibration_state_t;

typedef struct {
	dev_id id;
	dev_id contr_id;
	TickType_t start;
	TickType_t stop;
	calibration_state_t state;
	int cal_val;
}calibration_conf_t;


void valve_calibration_task(void *arg){
	calibration_conf_t *v_conf = (calibration_conf_t*)  arg;

}

void valve_function_test(void *arg){
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	ESP_LOGI(tag, "___________ Starting Valve blower burn in test ___________");

	while (true) {
		// Stage one Retreive Blower information

		unsigned blower_type;
		int ret;
		ret = get_blower_type(DEV_SUPA, &blower_type);
		if (blower_type != VALVE){
			ESP_LOGE(tag, "Blower type expected Valve ret:%d Type %d", ret, blower_type);
		}
		get_blower_type(DEV_SUPB, &blower_type);
		if (blower_type != VALVE){
			ESP_LOGE(tag, "Blower type expected Control ret:%d Type %d", ret, blower_type);
		}


		unsigned cur_press_v = 0;
		unsigned cur_press_c = 0;
		unsigned rpm_v = 0;
		unsigned rpm_c = 0;
		unsigned pwm = 100;

		//Ramp the fan blower up to verify it is connected
		for (int i=0; i<5; i++){
			int ret_v, ret_c;
			// Set both blowers to 100%
			ret_v = set_pwm(DEV_SUPA, 100);
			ret_c = set_pwm(DEV_SUPB, RESET_PWM_CMD);

			if (ret_v != 1 || ret_c != 1){
				ESP_LOGE(tag, "Error setting PWM  ret valve:%d ret_control %d", ret_v, ret_c);
			}
			ESP_LOGI(tag, "set_pwm %d ret valve:%d ret_control %d", pwm, ret_v, ret_c);


			vTaskDelay(3000 / portTICK_PERIOD_MS);

			// Set both blowers to 100%
			ret_v = set_pwm(DEV_SUPA, 0);
			ret_c = set_pwm(DEV_SUPB, RESET_PWM_CMD);

			if (ret_v != 1 || ret_c != 1){
				ESP_LOGE(tag, "Error setting PWM  ret valve:%d ret_control %d", ret_v, ret_c);
			}
			ESP_LOGI(tag, "set_pwm %d ret valve:%d ret_control %d", pwm, ret_v, ret_c);


			vTaskDelay(3000 / portTICK_PERIOD_MS);

			// Set both blowers to 100%
			ret_v = set_pwm(DEV_SUPA, 25);
			ret_c = set_pwm(DEV_SUPB, RESET_PWM_CMD);

			if (ret_v != 1 || ret_c != 1){
				ESP_LOGE(tag, "Error setting PWM  ret valve:%d ret_control %d", ret_v, ret_c);
			}
			ESP_LOGI(tag, "set_pwm %d ret valve:%d ret_control %d", pwm, ret_v, ret_c);


			vTaskDelay(3000 / portTICK_PERIOD_MS);

			// Set both blowers to 100%
			ret_v = set_pwm(DEV_SUPA, 0);
			ret_c = set_pwm(DEV_SUPB, RESET_PWM_CMD);

			if (ret_v != 1 || ret_c != 1){
				ESP_LOGE(tag, "Error setting PWM  ret valve:%d ret_control %d", ret_v, ret_c);
			}
			ESP_LOGI(tag, "set_pwm %d ret valve:%d ret_control %d", pwm, ret_v, ret_c);

		}



		//Ramp the fan blower up to verify it is connected
		for (int i=0; i<20; i++){
			int ret_v, ret_c;
			// Set both blowers to 100%
			ret_v = set_pwm(DEV_SUPA, pwm);
			ret_c = set_pwm(DEV_SUPB, pwm);

			if (ret_v != 1 || ret_c != 1){
				ESP_LOGE(tag, "Error setting PWM  ret valve:%d ret_control %d", ret_v, ret_c);
			}
			ESP_LOGI(tag, "set_pwm %d ret valve:%d ret_control %d", pwm, ret_v, ret_c);


			vTaskDelay(3000 / portTICK_PERIOD_MS);

			// Read the current pressure on both devices should be greater than 500
			ret_v = get_current_pressure(DEV_SUPA, &cur_press_v);
			ret_c = get_current_pressure(DEV_SUPB, &cur_press_c);
			ESP_LOGI(tag, "Current pressure  valve:%d mInH2O ret_control %d mInH2O",  cur_press_v, cur_press_c);
			// Break the loop if both pressures reach needed value
			if (cur_press_v >= 500 && cur_press_c >= 500) {
				vTaskDelay(0);

				ESP_LOGI(tag, "Stopping PWM Mode" );
				ret_v = set_pwm(DEV_SUPA, RESET_PWM_CMD);
				ret_c = set_pwm(DEV_SUPB, RESET_PWM_CMD);

				ESP_LOGI(tag, "set_pwm %d ret valve:%d ret_control %d", pwm, ret_v, ret_c);
				if (ret_v != 1 || ret_c != 1){
					ESP_LOGE(tag, "Error setting PWM  ret valve:%d ret_control %d", ret_v, ret_c);
				}
				ESP_LOGI(tag, "Setting the target pressure to 0" );

				vTaskDelay(500 / portTICK_PERIOD_MS);
				ret_v = set_target_pressure(DEV_SUPA, 0);
				ret_c = set_target_pressure(DEV_SUPB, 0);
				if (ret_v != 1 || ret_c != 1){
					ESP_LOGE(tag, "Error setting Target Pressure  ret_valve:%d ret_control %d", ret_v, ret_c);
				} else {
					ESP_LOGI(tag, "set_target_pressure 0 after the %d itteration ", i);
					i = 25;
				}
			}

		}
		ESP_LOGI(tag, "___________ Passed Setup test___________");

		unsigned targ_press_v = 700;
		unsigned targ_press_c = 700;
		unsigned pwm_v = 700;
		unsigned pwm_c = 700;


		vTaskDelay(5000 / portTICK_PERIOD_MS);

		//Check that the fan is off and run calibration
		int raw_pressure_c = 0;
		int raw_pressure_v = 0;
		int cal_c = 0;
		int cal_v = 0;
		int ret_v, ret_c;

		for (int i=0; i<20; i++){



			vTaskDelay(1000 / portTICK_PERIOD_MS);

			// Set the setpoint to 0
			ret_v = set_target_pressure(DEV_SUPA, 0);
			ret_c = set_target_pressure(DEV_SUPB, 0);
			if (ret_v != 1 || ret_c != 1){
				ESP_LOGE(tag, "Error setting Target Pressure  ret valve:%d ret_control %d", ret_v, ret_c);
				continue;
			}

			// Set the setpoint to 0
			ret_v = get_target_pressure(DEV_SUPA, &targ_press_v);
			ret_c = get_target_pressure(DEV_SUPB, &targ_press_c);
			if (ret_v != 1 || ret_c != 1){
				ESP_LOGE(tag, "Error Getting Target Pressure  ret valve:%d ret_control %d", ret_v, ret_c);
				continue;
			}
			ESP_LOGD(tag, "Target Pressure valve:%d control %d", targ_press_v, targ_press_c);

			ret_v = get_pwm(DEV_SUPA, &pwm_v);
			ret_c = get_pwm(DEV_SUPB, &pwm_c);
			if (ret_v != 1 || ret_c != 1){
				ESP_LOGE(tag, "Error getting PWM  ret valve:%d ret_control %d", ret_v, ret_c);
			}
			ESP_LOGD(tag, "get PWM valve:%d ret_control %d",  pwm_v, pwm_c);



			// Set both blowers to closed position
			ret_v = get_fan_rpm(DEV_SUPA, &rpm_v);
			ret_c = get_fan_rpm(DEV_SUPB, &rpm_c);

			if (ret_v != 1 || ret_c != 1){
				ESP_LOGE(tag, "Error setting Getting RPM  ret valve:%d ret_control %d", ret_v, ret_c);
				rpm_c = 0xffff;
				continue;
			}
			if (rpm_c >0){
				ESP_LOGW(tag, "FAN is still spinning %dRPM ret valve:%d ret_control %d",rpm_c,  ret_v, ret_c);
				continue;
			}



			vTaskDelay(5000 / portTICK_PERIOD_MS);

			// Run the calibration
			ret_v = run_caibration(DEV_SUPA);
			ret_c = run_caibration(DEV_SUPB);
			if (ret_v != 1 || ret_c != 1){
				ESP_LOGE(tag, "Error running calibration  ret valve:%d ret_control %d", ret_v, ret_c);
				continue;
			}

			// Get the calibration
			ret_v = get_caibration(DEV_SUPA, &cal_v);
			ret_c = get_caibration(DEV_SUPB, &cal_c);
			if (ret_v != 1 || ret_c != 1){
				ESP_LOGE(tag, "Error retrieving calibration  ret valve:%d ret_control %d", ret_v, ret_c);
				continue;
			}

			// Get the raw pressure
			ret_v = get_raw_pressure(DEV_SUPA, &raw_pressure_v);
			ret_c = get_raw_pressure(DEV_SUPB, &raw_pressure_c);
			if (ret_v != 1 || ret_c != 1){
				ESP_LOGE(tag, "Error retrieving raw pressure  ret valve:%d ret_control %d", ret_v, ret_c);
				continue;
			}

			ESP_LOGD(tag, "Raw Pressure valve:%d control %d", raw_pressure_v, raw_pressure_c);
			// Break out if calibration is the same as raw
			if (raw_pressure_v == cal_v  ) {
				i = 25;
			}
		}
		ESP_LOGI(tag, "___________ Passed Calibration test___________");





		vTaskDelay(1000 / portTICK_PERIOD_MS);

		targ_press_v = 0;
		targ_press_c = 700;


		// Ramp up the Control Blower to check the blowby at min
		for (int i=0; i<100; i++){
			// Set the set_point on control blower and close the valve blower
			ret_v = set_target_pressure(DEV_SUPA, targ_press_v);
			ret_c = set_target_pressure(DEV_SUPB, targ_press_c);
			if (ret_v != 1 || ret_c != 1){
				ESP_LOGE(tag, "Error setting Target Pressure  ret valve:%d ret_control %d", ret_v, ret_c);
				continue;
			}
			ESP_LOGD(tag, "Set Target pressure valve:%d control %d", targ_press_v, targ_press_c);

			vTaskDelay(5000 / portTICK_PERIOD_MS);

			// Read the current pressure on both devices
			ret_v = get_current_pressure(DEV_SUPA, &cur_press_v);
			ret_c = get_current_pressure(DEV_SUPB, &cur_press_c);
			ESP_LOGD(tag, "Current pressure  valve:%d mInH2O control %d mInH2O",  cur_press_v, cur_press_c);

			// Break the loop if both pressures reach needed value
			if (cur_press_v <= 40 && cur_press_c >= targ_press_c- 50) {
				ret_v = set_target_pressure(DEV_SUPA, 0);
				ret_c = set_target_pressure(DEV_SUPB, 0);
				if (ret_v != 1 || ret_c != 1){
					ESP_LOGE(tag, "Error setting Target Pressure  ret_valve:%d ret_control %d", ret_v, ret_c);
					continue;
				} else {
					ESP_LOGD(tag, "set_target_pressure 0 after the %d iteration ", i);
					i = 1000;
				}
			}
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
		ESP_LOGI(tag, "___________ Passed Blow-by low test___________");


		targ_press_v = 0;
		targ_press_c = 1800;
		// Ramp up the Control Blower to check the blowby at min
		for (int i=0; i<100; i++){

			// Set the set_point on control blower and close the valve blower
			ret_v = set_target_pressure(DEV_SUPA, targ_press_v);
			ret_c = set_target_pressure(DEV_SUPB, targ_press_c);
			if (ret_v != 1 || ret_c != 1){
				ESP_LOGE(tag, "Error setting Target Pressure  ret valve:%d ret_control %d", ret_v, ret_c);
				continue;
			}
			ESP_LOGI(tag, "Set Target pressure valve:%d control %d", targ_press_v, targ_press_c);

			vTaskDelay(5000 / portTICK_PERIOD_MS);

			// Read the current pressure on both devices
			ret_v = get_current_pressure(DEV_SUPA, &cur_press_v);
			ret_c = get_current_pressure(DEV_SUPB, &cur_press_c);
			ESP_LOGD(tag, "Current pressure  valve:%d mInH2O control %d mInH2O",  cur_press_v, cur_press_c);
			// Break the loop if both pressures reach needed value
			if (cur_press_v <= 40 && cur_press_c >= targ_press_c- 50) {
				ret_v = set_target_pressure(DEV_SUPA, 0);
				ret_c = set_target_pressure(DEV_SUPB, 0);
				if (ret_v != 1 || ret_c != 1){
					ESP_LOGE(tag, "Error setting Target Pressure  ret_valve:%d ret_control %d", ret_v, ret_c);
					continue;
				} else {
					ESP_LOGD(tag, "set_target_pressure 0 after the %d iteration ", i);
					i = 1000;
				}
			}
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
		ESP_LOGI(tag, "___________ Passed Blow-by high test___________");

		targ_press_v = 0;
		targ_press_c = 700;
		ESP_LOGI(tag, "___________ Starting Min supply  low test___________");
		// Set the control blower to Lowest HVAC setting
		for (int i=0; i<100; i++){

			// Set the set_point on control blower and close the valve blower
			ret_v = set_target_pressure(DEV_SUPA, targ_press_v);
			ret_c = set_target_pressure(DEV_SUPB, targ_press_c);
			if (ret_v != 1 || ret_c != 1){
				ESP_LOGE(tag, "Error setting Target Pressure  ret valve:%d ret_control %d", ret_v, ret_c);
				continue;
			}
			ESP_LOGI(tag, "Set Target pressure valve:%d control %d", targ_press_v, targ_press_c);

			vTaskDelay(5000 / portTICK_PERIOD_MS);

			// Read the current pressure on both devices
			ret_v = get_current_pressure(DEV_SUPA, &cur_press_v);
			ret_c = get_current_pressure(DEV_SUPB, &cur_press_c);
			if (ret_v != 1 || ret_c != 1){
				ESP_LOGE(tag, "Error Getting Current Pressure ret valve:%d ret_control %d", ret_v, ret_c);
				continue;
			}
			ESP_LOGD(tag, "Current pressure  valve:%d mInH2O control %d mInH2O",  cur_press_v, cur_press_c);
			// Check if the control pressure has reached the set point +- 10%
			if (cur_press_c >  targ_press_c*.9 && cur_press_c <  targ_press_c*1.1) {
				// Set the taget pressure of v to lowest setpoint
				if (targ_press_v == 0 || cur_press_v < 30 ){
					targ_press_v = 160;
					ret_v = set_target_pressure(DEV_SUPA, targ_press_v);
					ret_c = set_target_pressure(DEV_SUPB, targ_press_c);

					if (ret_v != 1 || ret_c != 1){
						ESP_LOGE(tag, "Error setting Target Pressure  ret_valve:%d ret_control %d", ret_v, ret_c);
						continue;
					} else {
						ESP_LOGD(tag, "Updated target pressure on valve %d after the %d iteration ",targ_press_v , i);
						i = 1000;
					}
				}
			}

			vTaskDelay(1000 / portTICK_PERIOD_MS);
			// Setpoint reached state
			// Check the state in a loop everysecond and record the values looking for consecutive values of 60
			for (int i=0; i<120; i++){
				// Read the current pressure on both devices
				ret_v = get_current_pressure(DEV_SUPA, &cur_press_v);
				ret_c = get_current_pressure(DEV_SUPB, &cur_press_c);
				if (ret_v != 1 || ret_c != 1){
					ESP_LOGE(tag, "Error Getting Current Pressure ret valve:%d ret_control %d", ret_v, ret_c);
					continue;
				}
				ESP_LOGD(tag, "Current pressure  valve:%d mInH2O control %d mInH2O",  cur_press_v, cur_press_c);
				// Check if the control pressure has reached the set point +- 10%
				if (cur_press_c >  targ_press_c*.9 && cur_press_c <  targ_press_c*1.1) {
					// Set the taget pressure of v to lowest setpoint
					if (targ_press_v == 0 || cur_press_v < 30 ){
						targ_press_v = 160;
						ret_v = set_target_pressure(DEV_SUPA, targ_press_v);
						ret_c = set_target_pressure(DEV_SUPB, targ_press_c);

						if (ret_v != 1 || ret_c != 1){
							ESP_LOGE(tag, "Error setting Target Pressure  ret_valve:%d ret_control %d", ret_v, ret_c);
							continue;
						} else {
							ESP_LOGD(tag, "Updated target pressure on valve %d after the %d iteration ",targ_press_v, i);
							i = 1000;
						}
					}
				}
			}
		}
		ESP_LOGI(tag, "___________ Passed Blow-by high test___________");

	}
	ESP_LOGI(tag, "___________ End Valve blower burn in test___________");



}





















