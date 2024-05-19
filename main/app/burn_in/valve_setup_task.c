//
// Created by Erik Klan on 1/22/24.
//

#include "valve_setup_task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "serial_inno.h"

#include "burnin_types.h"

static const char *tag = "VSetup";
TickType_t t;

void valve_calibration_task(void *arg) {
    calibration_conf_t *v_conf = (calibration_conf_t *) arg;

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
    log_calibration_conf_t(v_conf);

    for (int i = 0;; i++) {
        xTaskGetTickCount();
        vTaskDelay(100 / portTICK_PERIOD_MS);
        xTaskGetTickCount();
        // Check the if we have timed out
        if (v_conf->stop <= xTaskGetTickCount()) {
            ESP_LOGE(tag, "Calibration Error task timed out %d on device %d", (int) v_conf->stop, dev_v);
            v_conf->state = cal_failed;
            v_conf->burnin_state = burnin_test_failed;
            break;
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
        v_conf->state = cal_init;
        v_conf->iterations += 1;

        // Set the setpoint to 0
        ret_v = set_target_pressure(dev_v, 0);
        ret_c = set_target_pressure(dev_c, 0);
        if (ret_v != 1 || ret_c != 1) {
            ESP_LOGE(tag, "Error setting Target Pressure  ret valve:%d ret_control %d", ret_v, ret_c);
            continue;
        }

        // Set the setpoint to 0
        ret_v = get_target_pressure(dev_v, &targ_press_v);
        ret_c = get_target_pressure(dev_c, &targ_press_c);
        if (ret_v != 1 || ret_c != 1) {
            ESP_LOGE(tag, "Error Getting Target Pressure  ret valve:%d ret_control %d", ret_v, ret_c);
            continue;
        }
        ESP_LOGD(tag, "Target Pressure valve:%d control %d", targ_press_v, targ_press_c);

        ret_v = get_pwm(dev_v, &pwm_v);
        ret_c = get_pwm(dev_c, &pwm_c);
        if (ret_v != 1 || ret_c != 1) {
            ESP_LOGE(tag, "Error getting PWM  ret valve:%d ret_control %d", ret_v, ret_c);
        }
        ESP_LOGD(tag, "get PWM valve:%d ret_control %d", pwm_v, pwm_c);

        v_conf->state = cal_wait;

        // Set both blowers to closed position
        ret_v = get_fan_rpm(dev_v, &rpm_v);
        ret_c = get_fan_rpm(dev_c, &rpm_c);

        if (ret_v != 1 || ret_c != 1) {
            ESP_LOGE(tag, "Error setting Getting RPM  ret valve:%d ret_control %d", ret_v, ret_c);
            rpm_c = 0xffff;
            continue;
        }
        if (rpm_c > 0) {
            ESP_LOGW(tag, "FAN is still spinning %dRPM ret valve:%d ret_control %d", rpm_c, ret_v, ret_c);
            continue;
        }

        v_conf->state = cal_running;

        vTaskDelay(5000 / portTICK_PERIOD_MS);

        // Run the calibration
        ret_v = run_caibration(dev_v);
        ret_c = run_caibration(dev_c);
        if (ret_v != 1 || ret_c != 1) {
            ESP_LOGE(tag, "Error running calibration  ret valve:%d ret_control %d", ret_v, ret_c);
            continue;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // Get the calibration
        ret_v = get_caibration(dev_v, &cal_v);
        ret_c = get_caibration(dev_c, &cal_c);
        if (ret_v != 1 || ret_c != 1) {
            ESP_LOGE(tag, "Error retrieving calibration  ret valve:%d ret_control %d", ret_v, ret_c);
            continue;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);


        // Get the raw pressure
        ret_v = get_raw_pressure(dev_v, &raw_pressure_v);
        ret_c = get_raw_pressure(dev_c, &raw_pressure_c);
        if (ret_v != 1 || ret_c != 1) {
            ESP_LOGE(tag, "Error retrieving raw pressure  ret valve:%d ret_control %d", ret_v, ret_c);
            continue;
        }


        ESP_LOGD(tag, "Raw Pressure valve:%d control %d", raw_pressure_v, raw_pressure_c);
        // Break out if calibration is the same as raw
        if (raw_pressure_v == cal_v) {
            v_conf->state = cal_success;
            v_conf->burnin_state = burnin_test_passed;
            v_conf->cal_val = cal_v;
            ESP_LOGI(tag, "___________ Passed Calibration test___________");
            ESP_LOGV(tag, "Calibration time %d", xTaskGetTickCount() - v_conf->start);
            break;
        } else {
            v_conf->state = cal_failed;
            v_conf->burnin_state = burnin_test_failed;
        }
    }
    log_calibration_conf_t(v_conf);

    vTaskDelete(NULL);

}


/**
 *
 * @param arg
 */
void valve_blowby_task(void *arg) {
    blowby_conf_t *v_conf = (blowby_conf_t *) arg;

    v_conf->start = xTaskGetTickCount();
    v_conf->stop = xTaskGetTickCount() + v_conf->time_out;
    dev_id dev_v = v_conf->id;
    dev_id dev_c = v_conf->contr_id;


    unsigned targ_press_v = 700;
    unsigned targ_press_c = 700;
    int ret_v, ret_c;
    unsigned cur_press_v = 0;
    unsigned cur_press_c = 0;
    unsigned valv_position = 0;

    int i = 0;

    v_conf->state = blowby_init;


    for (; i >= 0; i++) {
        // Loop delay time
        vTaskDelay(v_conf->loop_time / portTICK_PERIOD_MS);

        // Check the if we have timed out
        if (v_conf->stop < xTaskGetTickCount()) {
            ESP_LOGE(tag, "Calibration Error task timed out %d on device %d", (int) v_conf->stop, dev_v);
            v_conf->state = blowby_valve_failed;
            break;
        }
//        v_conf->iterations = i;
        // switch between states of the test
//        ret_v = get_current_pressure(dev_v, &cur_press_v);
//        ret_c = get_current_pressure(dev_c, &cur_press_c);
//        if (ret_v != 1 || ret_c != 1) {
//            ESP_LOGE(tag, "%d, Error retrieving raw pressure  ret valve:%d ret_control %d", i, ret_v, ret_c);
//            continue;
//        }
        // Only increment when transactions completes
        v_conf->iterations++;

        switch (v_conf->state) {

            case blowby_init:
                // set the setpoint to 0
                ret_v = set_target_pressure(dev_v, 0);
                ret_c = set_target_pressure(dev_c, v_conf->control_pressure);
                if (ret_v != 1 || ret_c != 1) {
                    ESP_LOGE(tag, "%d, Error Setting target pressure  ret valve:%d ret_control %d", i, ret_v, ret_c);
                    break;
                }
                v_conf->state = blowby_set_control;
                break;
            case blowby_set_control:
                ret_c = get_target_pressure(dev_c, &targ_press_c);
                ret_v = get_target_pressure(dev_v, &targ_press_v);
                if (ret_v == 1 && ret_c == 1 && targ_press_c >= v_conf->control_pressure && targ_press_v == 0) {
                    v_conf->state = blowby_wait_contr;
                } else {
                    v_conf->state = blowby_init;
                }
                break;
            case blowby_wait_contr:
                // Wait for the control pressure to reach the set point
                ret_c = get_current_pressure(dev_c, &cur_press_c);
                if (ret_c == 1 && cur_press_c >= v_conf->control_pressure) {

                    v_conf->state = blowby_contr_set;
                }
                break;
            case blowby_contr_set:
                // Send the valve command to closed position verify that it has reached position
                ret_v = get_target_pressure(dev_v, &targ_press_v);
                if (ret_v == 1 && targ_press_v == 0) {
                    // Valve position is actual location of valve
                    v_conf->state = blowby_valve_set;
                } else {
                    v_conf->state = blowby_init;
                }
                break;
            case blowby_valve_set:
                // Reset the counter
                v_conf->rec_index = 0;
                ret_v = get_valve_position(dev_v, &valv_position);
                if (ret_v == 1 && valv_position <= v_conf->control_pressure) {
                    v_conf->state = blowby_valve_wait;
                    v_conf->rec_index = 0;
                }
                break;
            case blowby_valve_wait:
                // record the blowby pressure once the
                ret_v = get_current_pressure(dev_v, &cur_press_v);

                if (ret_v == 1) {

                    ++v_conf->records->index;
                    ++v_conf->rec_index;
                    unsigned index = v_conf->records->index;
                    v_conf->records->records[index] = (int) cur_press_v;
                    v_conf->records->status[index] = (int) (cur_press_v > v_conf->max_blowby) ? 1 : 0;

                    v_conf->blow_by_val = (v_conf->blow_by_val > v_conf->records->records[index]) ? v_conf->blow_by_val
                                                                                                  : v_conf->records->records[index];
                }
                if (v_conf->rec_index >= 10) {
                    // Check if any of the values are out of range
                    v_conf->state = (v_conf->blow_by_val < v_conf->max_blowby) ? blowby_valve_passed
                                                                               : blowby_valve_failed;

                }
                break;
            case blowby_valve_passed:
                // verify that all the records status are 0
                for (int j = 0; j < v_conf->rec_index; j++) {
                    ESP_LOGD(tag, "%d  Record pressure %d, Status %d", j, v_conf->records->records[j],
                             v_conf->records->status[j]);
                    if (v_conf->records->status[j] == 1) {
                        v_conf->state = blowby_valve_failed;
                    }

                }
                i = -10;
                break;
            case blowby_valve_failed:
                i = -10;
                break;
        }
//        if (v_conf->state == blowby_valve_passed || v_conf->state == blowby_valve_failed) {
//            break;
//        }
        ESP_LOGI(tag, "%d , Elapsed time %d:  %s: Valve:%d, Control %d", i, (xTaskGetTickCount() - v_conf->start),
                 blowby_state_str[v_conf->state],
                 cur_press_v, cur_press_c);

    }

    vTaskDelete(NULL);

}
