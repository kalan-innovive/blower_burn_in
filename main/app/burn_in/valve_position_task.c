//
// Created by Erik Klan on 1/23/24.
//



#ifdef UNIT_TESTING

#include "burnin_types.h"

#else

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#endif

#include "valve_position_task.h"
#include "burnin_types.h"
#include "msg16_types.h"

#ifdef UNIT_TESTING

#include "unit_tests/serial_inno.h"


#else

#include "serial_inno.h"

#endif

// Task tags are camelcased
const static char *tag = "ValvePosTask";

valve_positioning_conf_t *create_positioning_conf() {
    valve_positioning_conf_t *p = (valve_positioning_conf_t *) malloc(sizeof(valve_positioning_conf_t));
    return p;
}


void destruct_positioning_conf(valve_positioning_conf_t *p) {
    if (p == NULL) {
        return;
    }
    free(p);
}

void valve_position_task(void *pvparam) {
    // set the parameters to the proper struct
    valve_positioning_conf_t *pth = (valve_positioning_conf_t *) pvparam;
    // pointer to the struct record
    pwm_record_t *p_rec = pth->pwm_record;
    // Pointer to the device handles
    device_valve_blower_t *vdev = pth->vdev;
    device_valve_blower_t *cdev = pth->cdev;
    // set the start time to now
    pth->start = esp_timer_get_time();
    // Add the start time and the loop time to get the stop time
    pth->stop = pth->start + (pth->time_out);
    // Log all of the parameters to debug
    ESP_LOGD(tag, "    Valve Position Task Started*****");
    ESP_LOGD(tag, "    Valve Device: %s", blower_id_str[vdev->type]);
    ESP_LOGD(tag, "    Contr Device: %s", blower_id_str[cdev->type]);
    ESP_LOGD(tag, "    End timeout: %d", (int) pth->stop);
    ESP_LOGD(tag, "    Loop Time: %d", (int) pth->loop_time);

    int ret_v, ret_c;
    int pos_maintain = 0;
    int contr_pwm_index = 0;
    // Create an index and array for the currnt pwm record
    int cur_pwm_val = 0;
    int pwm_val_index = 0;
    // Holds pressure value for valve blower
    unsigned cur_press_v = 0;
    unsigned target_pres_v = 0;

    int pwm_val[5] = {0, 25, 50, 75, 100};
    reg_record_t *pwm_reg[5] = {
            p_rec->pwm0,
            p_rec->pwm25,
            p_rec->pwm50,
            p_rec->pwm75,
            p_rec->pwm100
    };
    reg_record_t *cur_record = pwm_reg[0];


    while (pth->stop > esp_timer_get_time()) {
        // Set the task time to the loop time and wiat for it to
        // Update required registers
//        update_device_blower_register(vdev);
//        update_device_blower_register(cdev);
        switch (pth->state) {
            case position_init:
                // set current pressure to 0
                ret_v = set_target_pressure(vdev->type, 0);
                if (ret_v != 1) {
                    break;
                }
                // check that they have been set
                ret_v = get_target_pressure(vdev->type, &target_pres_v);
                vdev->target_press.curr_value_status = (ret_v) ? REG_VALID : REG_NOT_UPDATED;
                vdev->target_press.val = (int) target_pres_v;
                vdev->target_press.last_updated = esp_timer_get_time();
                if (vdev->target_press.curr_value_status != REG_VALID) {
                    pth->state = position_set_control;
                }

                break;
            case position_set_control:
                ret_c = set_pwm(cdev->type, p_rec->control_pwm[contr_pwm_index]);
                if (ret_c == 1) {
                    pth->state = position_set_pwm;
                }
                break;
            case position_set_pwm:
                // set the pwm value for the valve
                ret_c = set_pwm(vdev->type, cur_pwm_val);
                if (ret_c == 1) {
                    pth->state = position_wait;
                }
                break;
            case position_wait:
                cur_press_v = 0;
                ret_v = get_current_pressure(vdev->type, &cur_press_v);
                vdev->target_press.curr_value_status = (ret_v) ? REG_VALID : REG_NOT_UPDATED;
                vdev->target_press.last_updated = esp_timer_get_time();
                if (ret_v) {
                    // Record the pressure in the records

                    cur_record->records[cur_record->index] = (int) cur_press_v;
                    cur_record->status[cur_record->index] = 1;
                    cur_record->index = (cur_record->index + 1) % cur_record->len;
                }
                if (pos_maintain++ > 10) {
                    // change the state to position passed

                    pth->state = position_passed;
                }

                break;
            case position_passed:
                // increment the record index for the valve index
                pwm_val_index = (pwm_val_index + 1) % 5;
                // set the current record to the next index
                cur_record = pwm_reg[pwm_val_index];
                // set the current pwm value to the next index 0,25,50,75,100
                cur_pwm_val = pwm_val[pwm_val_index];
                // Once the valve blowers have iterated through all 5 of the values increment the control pwm index
                if (pwm_val_index == 0) {
                    contr_pwm_index = (contr_pwm_index + 1) % p_rec->control_pwm_len;
                }
                // restart the cycle
                pth->state = position_init;

                break;
            case position_failed:
                //TODO: implement checking the records for the failed state
                break;
        }

        vTaskDelay(pth->loop_time / portTICK_PERIOD_MS);
    }
    pth->result_state = burnin_test_passed;
    pth->state = position_passed;
    vTaskDelete(NULL);

}
