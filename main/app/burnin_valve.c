/*
 * burnin_valve.c
 *
 *  Created on: Jan 10, 2024
 *      Author: ekalan
 */

#ifndef MAIN_APP_BURNIN_VALVE_C_
#define MAIN_APP_BURNIN_VALVE_C_

#include <stdio.h>
//#include "esp_heap_caps.h"

#include "string.h"
#include <time.h>
#include "stdlib.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_check.h"
#include "esp_err.h"
#include "bsp_board.h"
#include "bsp/esp-bsp.h"


#include "serial_inno.h"

#include "app_event_handler.h"
#include "gui/ui_main.h"
#include "mqtt_handler.h"
#include "burnin_valve.h"

static const char *tag = "V_BI_Task";

//extern TaskHandle_t burn_in_valve_handle;


void valve_calibration_task(void *arg){
	calibration_conf_t *v_conf = (calibration_conf_t*)  arg;

	v_conf->start = xTaskGetTickCount();
	v_conf->stop = xTaskGetTickCount() + v_conf->time_out;
	dev_id dev_v = v_conf->id;
	dev_id dev_c = v_conf->contr_id;


	unsigned rpm_v = 0;
	unsigned rpm_c = 0;
	unsigned targ_press_v = 700;
	unsigned targ_press_c = 700;
	unsigned pwm_v = 700;
	unsigned pwm_c = 700;
	int raw_pressure_c = 0;
	int raw_pressure_v = 0;
	int cal_c = 0;
	int cal_v = 0;
	int ret_v, ret_c;

	for (int i=0;; i++){
		// Check the if we have timed out
		if (v_conf->stop > xTaskGetTickCount() ) {
			ESP_LOGE(tag, "Calibration Error task timed out %d on device %d", (int)v_conf->stop, dev_v);
			v_conf->state = cal_failed;
			break;
		}

		vTaskDelay(1000 / portTICK_PERIOD_MS);
		v_conf->state = cal_init;
		v_conf->itterations +=1;

		// Set the setpoint to 0
		ret_v = set_target_pressure(dev_v, 0);
		ret_c = set_target_pressure(dev_c, 0);
		if (ret_v != 1 || ret_c != 1){
			ESP_LOGE(tag, "Error setting Target Pressure  ret valve:%d ret_control %d", ret_v, ret_c);
			continue;
		}

		// Set the setpoint to 0
		ret_v = get_target_pressure(dev_v, &targ_press_v);
		ret_c = get_target_pressure(dev_c, &targ_press_c);
		if (ret_v != 1 || ret_c != 1){
			ESP_LOGE(tag, "Error Getting Target Pressure  ret valve:%d ret_control %d", ret_v, ret_c);
			continue;
		}
		ESP_LOGD(tag, "Target Pressure valve:%d control %d", targ_press_v, targ_press_c);

		ret_v = get_pwm(dev_v, &pwm_v);
		ret_c = get_pwm(dev_c, &pwm_c);
		if (ret_v != 1 || ret_c != 1){
			ESP_LOGE(tag, "Error getting PWM  ret valve:%d ret_control %d", ret_v, ret_c);
		}
		ESP_LOGD(tag, "get PWM valve:%d ret_control %d",  pwm_v, pwm_c);



		// Set both blowers to closed position
		ret_v = get_fan_rpm(dev_v, &rpm_v);
		ret_c = get_fan_rpm(dev_c, &rpm_c);

		if (ret_v != 1 || ret_c != 1){
			ESP_LOGE(tag, "Error setting Getting RPM  ret valve:%d ret_control %d", ret_v, ret_c);
			rpm_c = 0xffff;
			continue;
		}
		if (rpm_c >0){
			ESP_LOGW(tag, "FAN is still spinning %dRPM ret valve:%d ret_control %d",rpm_c,  ret_v, ret_c);
			continue;
		}

		v_conf->state = cal_running;

		vTaskDelay(5000 / portTICK_PERIOD_MS);

		// Run the calibration
		ret_v = run_caibration(dev_v);
		ret_c = run_caibration(dev_c);
		if (ret_v != 1 || ret_c != 1){
			ESP_LOGE(tag, "Error running calibration  ret valve:%d ret_control %d", ret_v, ret_c);
			continue;
		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);

		// Get the calibration
		ret_v = get_caibration(dev_v, &cal_v);
		ret_c = get_caibration(dev_c, &cal_c);
		if (ret_v != 1 || ret_c != 1){
			ESP_LOGE(tag, "Error retrieving calibration  ret valve:%d ret_control %d", ret_v, ret_c);
			continue;
		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);


		// Get the raw pressure
		ret_v = get_raw_pressure(dev_v, &raw_pressure_v);
		ret_c = get_raw_pressure(dev_c, &raw_pressure_c);
		if (ret_v != 1 || ret_c != 1){
			ESP_LOGE(tag, "Error retrieving raw pressure  ret valve:%d ret_control %d", ret_v, ret_c);
			continue;
		}


		ESP_LOGD(tag, "Raw Pressure valve:%d control %d", raw_pressure_v, raw_pressure_c);
		// Break out if calibration is the same as raw
		if (raw_pressure_v == cal_v  ) {
			v_conf->state = cal_success;
			v_conf->cal_val = cal_v;
			ESP_LOGI(tag, "___________ Passed Calibration test___________");
			break;
		}
	}
	vTaskDelete(NULL);

}






void valve_blowby_task(void *arg){
	blowby_conf_t *v_conf = (blowby_conf_t*)  arg;

	v_conf->start = xTaskGetTickCount ();
	v_conf->stop = xTaskGetTickCount() + v_conf->time_out;
	dev_id dev_v = v_conf->id;
	dev_id dev_c = v_conf->contr_id;


	unsigned rpm_v = 0;
	unsigned rpm_c = 0;
	unsigned targ_press_v = 700;
	unsigned targ_press_c = 700;
	unsigned pwm_v = 700;
	unsigned pwm_c = 700;
	int raw_pressure_c = 0;
	int raw_pressure_v = 0;
	int cal_c = 0;
	int cal_v = 0;
	int ret_v, ret_c;
	unsigned cur_press_v = 0;
	unsigned cur_press_c = 0;

	for (int i=0;; i++){
		// Check the if we have timed out
		if (v_conf->stop > xTaskGetTickCount() ) {
			ESP_LOGE(tag, "Calibration Error task timed out %d on device %d", (int)v_conf->stop, dev_v);
			v_conf->state = cal_failed;
			break;
		}

		vTaskDelay(1000 / portTICK_PERIOD_MS);
		v_conf->state = cal_init;
		v_conf->iterations +=1;

		// Set the setpoint to 0
		ret_v = set_target_pressure(dev_v, 0);
		ret_c = set_target_pressure(dev_c, v_conf->control_pressure);
		if (ret_v != 1 || ret_c != 1){
			ESP_LOGE(tag, "Error setting Target Pressure  ret valve:%d ret_control %d", ret_v, ret_c);
			continue;
		}

		// Set the setpoint to 0
		ret_v = get_target_pressure(dev_v, &targ_press_v);
		ret_c = get_target_pressure(dev_c, &targ_press_c);
		if (ret_v != 1 || ret_c != 1){
			ESP_LOGE(tag, "Error Getting Target Pressure  ret valve:%d ret_control %d", ret_v, ret_c);
			continue;
		}
		ESP_LOGD(tag, "Target Pressure valve:%d control %d", targ_press_v, targ_press_c);

		ret_c = get_pwm(dev_c, &pwm_c);
		if ( ret_c != 1){
			ESP_LOGE(tag, "Error getting PWM  ret_control %d", ret_c);
		}
		ESP_LOGD(tag, "get PWM ret_control %d",  pwm_c);

		vTaskDelay(8000 / portTICK_PERIOD_MS);


		v_conf->state = cal_running;
		ret_v = get_current_pressure(dev_v, &cur_press_v);
		ret_c = get_current_pressure(dev_c, &cur_press_c);
		ESP_LOGD(tag, "Current pressure  valve:%d mInH2O control %d mInH2O",  cur_press_v, cur_press_c);

		// Break the loop if both pressures reach needed value
		if (cur_press_v <= v_conf->max_blowby && cur_press_c >= v_conf->control_pressure) {
			ret_v = set_target_pressure(dev_v, 0);
			ret_c = set_target_pressure(dev_c, 0);

			if (ret_v != 1 || ret_c != 1){
				ESP_LOGE(tag, "Error setting Target Pressure  ret_valve:%d ret_control %d", ret_v, ret_c);
				continue;
			} else if (v_conf->state == cal_running){
				ESP_LOGD(tag, "set_target_pressure 0 after the %d iteration ", i);
				v_conf->state = cal_complete;
				ESP_LOGI(tag, "___________ Passed blowby test___________");

			}
		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);



		ESP_LOGD(tag, "Blowby pressure valve:%d control %d", raw_pressure_v, raw_pressure_c);
		// Break out if calibration is the same as raw
		if (raw_pressure_v == cal_v  ) {
			v_conf->state = cal_success;
			v_conf->blow_by_val = cal_v;
			ESP_LOGI(tag, "___________ Passed Blow-by test___________");
			break;
		}
	}
	vTaskDelete(NULL);

}


#endif /* MAIN_APP_BURNIN_VALVE_C_ */
