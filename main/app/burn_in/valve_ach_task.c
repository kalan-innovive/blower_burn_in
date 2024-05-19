//
// Created by Erik Klan on 1/22/24.
//


#ifdef UNIT_TESTING

#include "burnin_types.h"

#else

#include "esp_log.h"

#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "valve_ach_task.h"
#include "burnin_types.h"
#include "serial_inno.h"


static const char *tag = "ach_task";

/**
 * @brief Burn in ACH test
 *  - Sets the control fan to a pressure value and waits until it is reached
 *   - Sets the valve to a pressure value and records the data once a second
 *   - Every second the valve data is checked to see if it is within the min max pressure setting
 *    - If the value is reached for at least 10 consecutive iterations the passed flag is set
 *    - if the value is not reached for 60 seconds the failed flag is set and the test fails and returns
 *   - After the end time is reached the test is stopped and ends the task
 * @param arg
 */
void valve_ach_task(void *arg) {
    ach_conf_t *v_conf = (ach_conf_t *) arg;

    v_conf->start = xTaskGetTickCount();
    v_conf->stop = xTaskGetTickCount() + v_conf->time_out;
    dev_id dev_v = v_conf->id;
    dev_id dev_c = v_conf->contr_id;
    unsigned targ_press_v = 0;
    unsigned targ_press_c = 0;
    int ret_v, ret_c;
    unsigned cur_press_v = 0;
    unsigned cur_press_c = 0;
    int reg_status = 0;    /**used to flag any value not in the setpoint range */
    int ach_iter = 0;
    int ach_reach = 0;
    int ach_warn = 0;
    int ach_contr = 0;
    int cur_ach_state = 0;
    int prev_ach_state = 0;
    int i = 0;

    ESP_LOGI(tag, " Init: Starting ACH test for %d blower", dev_v);
    ESP_LOGD(tag, "       Start time: %d", (int) v_conf->start);
    ESP_LOGD(tag, "       Stop time: %d", (int) v_conf->stop);
    ESP_LOGD(tag, "       Timeout: %d", (int) v_conf->time_out);
    ESP_LOGD(tag, "       Control Pressure: %d mInH2O", v_conf->control_pressure);
    ESP_LOGD(tag, "       Valve Set-Point: %d mInH2O", v_conf->control_pwm);
    v_conf->time_state = cal_started;
    v_conf->state = burnin_test_init;


    // Start the loop
    while (v_conf->stop > xTaskGetTickCount()) {
        i++;
        v_conf->iterations = i;
        vTaskDelay(v_conf->loop_time / portTICK_PERIOD_MS);

        // Retrieve current pressure
        // Get the current check point is not over the pressure max value and record at least 10 values
        ret_v = get_current_pressure(dev_v, &cur_press_v);
        ret_c = get_current_pressure(dev_c, &cur_press_c);
        if (ret_v != 1 || ret_c != 1) {
            ESP_LOGE(tag, "Error getting current pressure  ret valve:%d ret_control %d", ret_v, ret_c);
            continue;
        }
        vTaskDelay(0);
        // Read the setpoints
        ret_v = get_target_pressure(dev_v, &targ_press_v);
        ret_c = get_target_pressure(dev_c, &targ_press_c);
        if (ret_v != 1 || ret_c != 1) {
            ESP_LOGE(tag, "Error Getting Target Pressure  ret valve:%d ret_control %d", ret_v, ret_c);
            continue;
        }
        vTaskDelay(0);
        // only start logging once the control pressure is reached
        if (v_conf->ach_state >= ach_set) {
            ach_iter += 1;
            v_conf->records->records[ach_iter] = (int) cur_press_v;
            reg_status = (cur_press_v > v_conf->min_press_val &&
                          cur_press_v < v_conf->max_press_val) ? ach_reached : ach_warning;
            // increment the warning value and the ach_reached value
            ach_warn += (reg_status == ach_warning) ? 1 : 0;
            ach_reach += (reg_status == ach_reached) ? 1 : 0;
            ach_contr++;
        }


        /**
         * @brief ACH State Machine
         * - ach_unknown : Initialize the ach test resseting the control perameters
         * - ach_init : Initialize the ach test setting the target pressure to 0 on valve and the control to the control pressure
         * - ach_setup : Waits until control pressure is reached
         * - ach_setup_complete : Verify that the control pressure is maintained for 10 cycles
         * - ach_set : Set the valve setpoint to the desired pressure
         * - ach_reached : valve setpoint is reached and the control pressure is maintained
         * - ach_maintain : checks that the valve setpoint is maintained
         * - ach_warning : Set if the valve setpoint is not reached in 60 seconds or maintainde setpoint for 10 seconds
         * - ach_passed : the time is reached and the valve setpoint is maintained
         * - ach_failed : Time is reached and the warning state is set
         */
        switch (v_conf->ach_state) {
            case ach_unknown:
                v_conf->ach_state = ach_init;
                break;

            case ach_init:
                /** Initialize the ach test set the valve to 0 and the control to the control pressure
                *  ach_init-> ach_setup : on successfull transmission
                */
                ret_v = set_target_pressure(dev_v, 0);
                ret_c = set_target_pressure(dev_c, v_conf->control_pressure);
                if (ret_v != 1 || ret_c != 1 || (v_conf->control_pressure == targ_press_c && targ_press_v == 0)) {
                    break;
                }
                v_conf->ach_state = ach_setup;
                break;
            case ach_setup:
                /** Waits until control pressure is reached
                 *  ach_setup-> ach_setup_complete : Control blower reaches the control pressure
                 */
                if (cur_press_c >= v_conf->control_pressure) {
                    // Reset the ach counters
                    ach_contr = 0;
                    v_conf->ach_state = ach_setup_complete;
                }
                break;
            case ach_setup_complete:
                /** Verify that the control pressure is maintained for 10 cycles
                 *  ach_setup_complete-> ach_set : maintain the control pressure for 10 cycles
                 */
                ach_contr += (cur_press_c >= (v_conf->control_pressure * .9)) ? 1 : 0;
                if (ach_contr > 10) {
                    v_conf->ach_state = ach_set;
                    ach_reach = 0;
                    ach_warn = 0;
                }
                break;
            case ach_set:
                /**
                 * Set the valve setpoint to the desired pressure
                 * ach_set-> ach_reached : on successfull transmission
                 * ach_set-> ach_warning : if the control pressure is not maintained for 60 cycles
                 * ach_set-> ach_setup_complete : if the valve setpoint set
                 */
                if (targ_press_v != v_conf->valve_setpoint) {
                    set_target_pressure(dev_v, v_conf->valve_setpoint);
                    v_conf->ach_state = ach_setup_complete;
                } else if (targ_press_c > v_conf->min_press_val && targ_press_c < v_conf->max_press_val) {
                    v_conf->ach_state = ach_reached;
                } else if (ach_warn > 60) {
                    v_conf->ach_state = ach_warning;
                }
//                v_conf->state = burnin_test_running;
                break;
            case ach_reached:
                /**
                 * valve setpoint is reached and the control pressure is maintained
                 * ach_reached-> ach_maintain : on successfull transmission
                 * ach_reached-> ach_warning : if the control pressure is not maintained for 10 cycles
                 */
                // If we do not reach the pressure in 60 seconds go to warning state
                if (ach_warn > 10) {
                    v_conf->ach_state = ach_warning;
                } else if (ach_reach > 10) {
                    v_conf->ach_state = ach_maintain;
                    ach_warn = 0;
                }
                break;
            case ach_maintain:
                /**
                 * checks that the valve setpoint is maintained +- 10% of the setpoint
                 * ach_maintain-> ach_passed : on successfull timeot
                 * ach_maintain-> ach_warning : if the control pressure is not maintained for 10 cycles
                 */
                // If we have a warn state go back to ach_reached
                if (reg_status == ach_warning) {
                    v_conf->ach_state = ach_reached;
                    // reset the ach_reach counter
                    ach_reach = 0;
                }
                break;
            case ach_warning:
                /**
                 * error_state for warnings on the valve
                 * ach_warning-> ach_failed : timeout
                 */
                v_conf->state = cal_failed;
                if (reg_status == ach_reached) {
                    v_conf->ach_state = ach_reached;
                }
                break;

            case ach_passed:
                // Only implemented after the timeout
                break;
            case ach_failed:
                // Only implemented after the timeout
                break;
            default:
                ESP_LOGE(tag, "Unknown state");
                break;
        }
        // Log the current state previous state control pressure and valve pressure in json format
        ESP_LOGD(tag, "{loop:%d, state:{cur:%s,cur:%s},sp{valve:%d,contr:%d},press:{}},count{warn%d,reach:%d}",
                 i, ach_state_str[v_conf->ach_state], ach_state_str[prev_ach_state], targ_press_v, targ_press_c,
                 cur_press_v, cur_press_c, ach_warn, ach_reach);
        cur_ach_state = v_conf->ach_state;
        if (cur_ach_state != prev_ach_state) {
            ESP_LOGI(tag, "%d, Running:%s -> &s State change", i, ach_state_str[prev_ach_state],
                     ach_state_str[cur_ach_state]);
            prev_ach_state = cur_ach_state;
        }
    }
    // Loop timed out
    ESP_LOGI(tag, "    Valve ACH Test Finished with state %s", ach_state_str[v_conf->ach_state]);
    ESP_LOGD(tag, "        Valve Pressure: %d", cur_press_v);
    ESP_LOGD(tag, "        Maintain count: %d", ach_reach);
    ESP_LOGD(tag, "        Warning count: %d", ach_warn);
    ESP_LOGD(tag, "        Loop itteration count: %d", v_conf->iterations);
    ESP_LOGD(tag, "        ACH loops completed: %d", ach_contr);
    if (v_conf->ach_state < ach_set) {
        v_conf->state = burnin_test_failed;
    } else if (v_conf->ach_state == ach_warning) {
        v_conf->state = burnin_test_failed;
        v_conf->ach_state = ach_failed;
    } else if (v_conf->ach_state == ach_reached || v_conf->ach_state == ach_maintain) {
        v_conf->state = burnin_test_passed;
        v_conf->ach_state = ach_passed;
    }
    ESP_LOGD(tag, "        Returning Test State: %s", task_result_str[v_conf->state]);
    ESP_LOGD(tag, "        Returning ACH State: %s", ach_state_str[v_conf->ach_state]);
    vTaskDelay(1000 / portTICK_PERIOD_MS); // wait for the last message to be sent

    vTaskDelete(NULL);

}
