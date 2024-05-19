//
// Created by Erik Klan on 1/23/24.
//

#ifndef BLOWER_BURN_IN_VALVE_POSITION_TASK_H
#define BLOWER_BURN_IN_VALVE_POSITION_TASK_H


#include "burnin_types.h"

#define POS_TASK_RECORD_LEN 100
#define PWM_VAL_LIST_LEN  5
#define VALVE_POS_TASK_TIMEOUT 30
#define VALVE_POS_TASK_LOOPTIME 200

const static int PWM_VAL_LIST[] = {30, 50, 70, 20, 60};

valve_positioning_conf_t *create_positioning_conf();

void destruct_positioning_conf(valve_positioning_conf_t *p);

void valve_position_task(void *pvparam);


#endif //BLOWER_BURN_IN_VALVE_POSITION_TASK_H
