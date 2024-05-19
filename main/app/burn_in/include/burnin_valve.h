/*
 * burnin_valve.h
 *
 *  Created on: Jan 10, 2024
 *      Author: ekalan
 */

#ifndef MAIN_APP_BURNIN_VALVE_H_
#define MAIN_APP_BURNIN_VALVE_H_

#include "register.h"
#include "burnin_types.h"
#include "stdbool.h"


typedef enum {
    prog_idle,
    prog_init, // Set tasks to init when using or idle when not using
    prog_start,
    prog_running,
    prog_stop,

} burnin_progress_t;

//static const char *burnin_progress_repr[] = {"prog_idle", "prog_init", "prog_start", "prog_running", "prog_stop"};


typedef enum {
    task_calibration,
    task_positioning,
    task_blowby,
    task_ach,
} runner_processes_t;

//static const char *runner_processes_repr[] = {"task_calibration", "task_positioning", "task_blowby", "task_ach"};


typedef enum {
    sideA,
    sideB,
    sideNone
} side_t;

// Const char for side_t
//static const char *side_repr[] = {"sideA", "sideB", "sideNone"};

typedef struct side_conf {
    int index;
    side_t side;
    device_valve_blower_t *dev[2];
} side_dev_conf_t;

typedef struct {
    void *config_handle;               /*? pointer to the current config handle*/
    TaskHandle_t *task_handle;         /*? pointer to the current task handle*/
    burnin_progress_t task_prog;       /*? state that monitors which type of task is run.  */
    task_result_t task_result;         /*? result of the current task from the task handle*/
    const char *task_name;                   /*? name of the task*/
    side_dev_conf_t side_conf;          /*? pointer to the side configuration*/
    burnin_state_t state;                                /**< state of the overal burnin  */
} burnin_task_runner_handle_t;

/**
 * @brief Burnin Handler
 *
 */
typedef struct {
    burnin_type_t type;                           /**< type of burnin  */
    unsigned start_time;                     /**< start time of the burnin */
    unsigned stop_time;                    /**< stop time of the burnin */
    int num_iterations;                    /**< number of iterations in the main loop*/
    bool side_A;                         /**< side A of the burnin */
    bool side_B;                         /**< side B of the burnin */
    int b_index[4];                        /**< number of blowers */
    burnin_task_runner_handle_t *runner_handle[2];         /**< list of task runner handls to set a burnin task too */
    burnin_progress_t progress;                         /**< progress of the burnin */
    device_valve_blower_t *blower_handle[4];         /**< list of blower handlers */
} blower_burnin_handle_t;

/**
 * @briefDefault burnin configuration for valve blower
 * @param *b_handle : pointer to the burnin handler
 * @return : 0 on success, -1 on failure
 */
int default_valve_burnin_config(blower_burnin_handle_t *b_handle);

void valve_burnin_task(void *arg);

#endif /* MAIN_APP_BURNIN_VALVE_H_ */
