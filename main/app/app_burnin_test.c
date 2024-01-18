/*
 * app_valve_burnin_test.c
 *
 *  Created on: Jan 10, 2024
 *      Author: ekalan
 */

//
//#include "app_burnin.h"
//
//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
//#include "freertos/queue.h"
//
//#include "serial_inno_test.h"
//#include "serial_inno.h"
//#include <string.h>
//
//
//#include "msg16.h"
//#include "rack_device.h"
//#include "esp_log.h"
//#include "esp_err.h"
//
//
//
//void valve_function_test(void *arg){
//	vTaskDelay(1000 / portTICK_PERIOD_MS);
//	while (true) {
//		// set the fan blower to off
//		unsigned blower_type;
//		get_blower_type(DEV_SUPA, &blower_type);
//		get_blower_type(DEV_SUPB, &blower_type);
//		unsigned targ_press = 300;
//
//		for (int i=0; i<100; i++){
//			unsigned tmp;
//			set_target_pressure(DEV_SUPB, targ_press);
//			set_target_pressure(DEV_SUPA, 0);
//			int ret = get_target_pressure(DEV_SUPB, &tmp);
//			if (tmp >= 270 && tmp<=320) {
//				set_target_pressure(DEV_SUPB, 0);
//				break;
//			}
//			vTaskDelay(1000 / portTICK_PERIOD_MS);
//
//		}
//		vTaskDelay(10000 / portTICK_PERIOD_MS);
//
//		for (int i=0; i<100; i++){
//			unsigned tmp;
//			set_target_pressure(DEV_SUPB, 0);
//			int ret = get_raw_pressure(DEV_SUPB, &tmp);
//			if (tmp >= 30 ) {
//				set_target_pressure(DEV_SUPA, 0);
//				break;
//			}
//			vTaskDelay(1000 / portTICK_PERIOD_MS);
//
//		}
//		for (int i=0; i<100; i++){
//				unsigned tmp;
//				set_target_pressure(DEV_SUPB, 1000);
//				int ret = get_raw_pressure(DEV_SUPB, &tmp);
//				if (tmp >= 30 ) {
//					set_target_pressure(DEV_SUPA, 350);
//				}
//				vTaskDelay(1000 / portTICK_PERIOD_MS);
//
//			}
//		set_target_pressure(DEV_SUPA, 0);
//		vTaskDelay(100000 / portTICK_PERIOD_MS);
//		for (int i=0; i<100; i++){
//			unsigned tmp;
//			set_target_pressure(DEV_SUPB, 1500);
//			int ret = get_raw_pressure(DEV_SUPB, &tmp);
//			if (tmp >= 1000 ) {
//				set_target_pressure(DEV_SUPA, 750);
//			}
//			vTaskDelay(1000 / portTICK_PERIOD_MS);
//
//		}
//
//	}
//
//
//}
