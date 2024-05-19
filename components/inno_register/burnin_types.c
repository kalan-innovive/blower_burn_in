//
// Created by Erik Klan on 1/22/24.
//
#include "include/burnin_types.h"
#include <string.h>


reg_record_t *create_record(int len) {
    reg_record_t *record = (reg_record_t *) malloc(sizeof(reg_record_t));
    record->len = len;
    record->records = (int *) malloc(sizeof(int) * len);
    record->status = (int *) malloc(sizeof(int) * len);
    record->index = -1;
    return record;
}

void destruct_record(reg_record_t *rec) {
    free(rec->records);
    free(rec->status);
    free(rec);

}

const static char *tag = "bi_types";

void log_calibration_conf_t(calibration_conf_t *p) {
    if (p == NULL) {
        ESP_LOGE(tag, "%d:%s Cannot log null pointer", __LINE__, __FUNCTION__);
    }
    ESP_LOGD(tag, "Calibration Config");
    ESP_LOGD(tag, "Valve Blower Dev: %s", (p->vdev != NULL) ? get_dev_repr_full(p->vdev->type) : "NULL");
    ESP_LOGD(tag, "Contr Blower Dev: %s", (p->cdev != NULL) ? get_dev_repr_full(p->cdev->type) : "NULL");
    ESP_LOGD(tag, "Contr Blower ID: %u", p->id);
    ESP_LOGD(tag, "Contr Blower ID: %u", p->contr_id);
    ESP_LOGD(tag, "Start Time: %u", (unsigned) (p->start));
    ESP_LOGD(tag, "Stop Time: %u", (unsigned) p->stop);
    ESP_LOGD(tag, "Timeout: %u", (unsigned) p->time_out);
    ESP_LOGD(tag, "Calibration State: %s", calibration_state_str[p->state]);
    ESP_LOGD(tag, "Task State: %s", task_result_str[p->burnin_state]);
    ESP_LOGD(tag, "Max Press: %d", p->max_press);
    ESP_LOGD(tag, "Iterations: %d", p->iterations);
    ESP_LOGD(tag, "Calibration Value: %d", p->cal_val);


}

calibration_conf_t *create_calibration_config() {
    // Create the pointer for the calibration config
    calibration_conf_t *p = (calibration_conf_t *) malloc(sizeof(calibration_conf_t));
    p->id = DEV_UNDEF;
    p->contr_id = DEV_UNDEF;
    p->start = 0;
    p->stop = 0;
    p->time_out = CAL_CONF_TIMEOUT;
    p->state = cal_init;
    p->max_press = CAL_CONF_MAX_PRESS;
    p->iterations = 0;
    p->cal_val = CAL_CONF_DEFAULT_VAL;

    return p;
}

void destruct_calibration_config(calibration_conf_t *c) {
    if (c != NULL) {
        free(c);
        c = NULL;
    }
}


/**
 * @param num_samples : Number of samples for the record to hold
 * @param contr : array of pwm values for control to cycle through
 * @param contr_len : len of array
 * @return pwm_record_t* : Returns pointer to the created record
 */
pwm_record_t *create_pwm_record_t(int num_samples, const int contr[], size_t contr_len) {
    // create space for struct in momory
    pwm_record_t *p = (pwm_record_t *) malloc(sizeof(pwm_record_t));
    p->control_pwm = (int *) malloc(sizeof(int) * contr_len);
    // copy over the contr array
    memcpy(p->control_pwm, contr, sizeof(int) * contr_len);
    p->control_pwm_len = (int) contr_len;
    p->pwm0 = create_record(num_samples);
    p->pwm0_index = 0;
    p->pwm25 = create_record(num_samples);
    p->pwm25_index = 0;
    p->pwm50 = create_record(num_samples);
    p->pwm50_index = 0;
    p->pwm75 = create_record(num_samples);
    p->pwm75_index = 0;
    p->pwm100 = create_record(num_samples);
    p->pwm100_index = 0;
    return p;
}

void destruct_pwm_record_t(pwm_record_t *p) {
    free(p->control_pwm);
    destruct_record(p->pwm0);
    destruct_record(p->pwm25);
    destruct_record(p->pwm50);
    destruct_record(p->pwm75);
    destruct_record(p->pwm100);
    free(p);
}

ach_conf_t *create_ach_conf(const setpoint_map_t *map) {
    ach_conf_t *p = (ach_conf_t *) malloc(sizeof(ach_conf_t));
    p->id = DEV_UNDEF;
    p->contr_id = DEV_UNDEF;
    p->start = 0;
    p->stop = 0;
    p->time_out = ACH_CONF_DEFAULT_TIMEOUT;
    p->loop_time = ACH_CONF_DEFAULT_LOOP_TIME;
    p->control_pressure = map->hvac_press;
    p->control_pwm = 80;
    p->max_press_val = (unsigned) (map->hvac_press / .9);
    p->min_press_val = (unsigned) (map->hvac_press / 1.1);
    p->rec_len = ACH_CONF_DEFAULT_REC_LEN;
    p->ach_state = ach_unknown;
    p->records = create_record(p->rec_len);
    p->rec_index = 0;
    p->rec_warn_index = 0;
    p->state = cal_init;
    p->time_state = cal_init;
    p->iterations = 0;

    return p;

}

void destruct_ach_conf(ach_conf_t *c) {
    if (c != NULL) {
        if (c->records != NULL) {
            destruct_record(c->records);
        }
        free(c);
        c = NULL;
    }
}

blowby_conf_t *create_blowby_conf() {
    blowby_conf_t *p = (blowby_conf_t *) malloc(sizeof(blowby_conf_t));
    p->id = DEV_UNDEF;
    p->contr_id = DEV_UNDEF;
    p->start = 0;
    p->stop = 0;
    p->time_out = BLOWBY_CONF_DEF_TIMEOUT;
    p->control_pressure = BLOWBY_CONF_DEF_CONTROL_PRESS;
    p->control_pwm = BLOWBY_CONF_DEF_CONTROL_PWM;
    p->max_blowby = BLOWBY_CONF_DEF_MAX_BLOWBY;
    p->rec_index = -1;
    p->records = create_record(BLOWBY_CONF_DEF_REC_LEN);
    p->max_press_val = 0;
    p->state = blowby_init;
    p->iterations = 0;
    p->blow_by_val = 0;
    p->loop_time = BLOWBY_CONF_DEF_LOOP_TIME;
    return p;
}

void destruct_blowby_conf(blowby_conf_t *c) {
    if (c != NULL) {
        if (c->records != NULL) {
            destruct_record(c->records);
        }
        free(c);
        c = NULL;
    }
}
