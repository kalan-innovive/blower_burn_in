/*
 * burnin_valve.c
 *
 *  Created on: Jan 10, 2024
 *      Author: ekalan
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
//#include "json_parser.h"
#include "burnin_valve.h"
#include "valve_position_task.h"
#include "valve_ach_task.h"
#include "valve_setup_task.h"

const static char *tag = "VBurnin";

// Const char for side_t
static const char *side_repr[] = {"sideA", "sideB", "sideNone"};
static const char *runner_processes_repr[] = {"task_calibration", "task_positioning", "task_blowby", "task_ach"};
static const char *burnin_progress_repr[] = {"prog_idle", "prog_init", "prog_start", "prog_running", "prog_stop"};


/**
 * @brief Default valve burnin configuration
 * @return : 0 on success, -1 on failure
 */
int default_valve_burnin_config(blower_burnin_handle_t *b_handle) {
    // Check if the burnin handle is valid
    if (b_handle == NULL) {
    	ESP_LOGE(__FUNCTION__, "%d Handle is null", __LINE__);
        return -1;
    }
    // Create the default settings
    b_handle->type = VALVE;
    b_handle->start_time = 0;
    b_handle->stop_time = 0;
    b_handle->num_iterations = 0;
    b_handle->side_A = true;
    b_handle->side_B = true;
    b_handle->b_index[0] = 1;
    b_handle->b_index[1] = 3;
    b_handle->b_index[2] = 0x21;
    b_handle->b_index[3] = 0x23;
    for (int i=0;i<4;i++){
    	b_handle->blower_handle[i] = create_device_handle();
    }




    // set all the blower handles

//    b_handle->runner_handle = (burnin_task_runner_handle_t*) malloc(sizeof(burnin_task_runner_handle_t) * 2);
    b_handle->progress = prog_init;
    ESP_LOGI(__FUNCTION__, "%d Set up initial blower_burnin_handle_t", __LINE__);
    return 0;
}


static int init_burnin_task_runner_handle(burnin_task_runner_handle_t *r, runner_processes_t task, side_t
side) {
    ESP_LOGI(__FUNCTION__, "%d Resetting task handle %s", __LINE__, runner_processes_repr[task]);
    if (r == NULL) {
        ESP_LOGE(__FUNCTION__, "%d Passed handle NULL", __LINE__);
        return -1;
    }
    // initialize the fields that are the same for all tasks
    r->task_result = burnin_test_init;
    // Check if the config handle has been created

    switch (task) {

        case task_calibration:
            r->config_handle = create_calibration_config();
            r->task_name = (side == sideA) ? "Calibration TaskA" : "Calibration TaskB";
            break;
        case task_positioning:
            r->config_handle = create_positioning_conf();
            r->task_name = (side == sideA) ? "Position TaskA" : "Position TaskB";

            break;
        case task_blowby:
            r->config_handle = create_blowby_conf();
            r->task_name = (side == sideA) ? "Calibration TaskA" : "Calibration TaskB";

            break;
        case task_ach:
            r->config_handle = create_ach_conf(&ach_map.setpoints[0]);
            r->task_name = (side == sideA) ? "Calibration TaskA" : "Calibration TaskB";
            break;
    }
    return 0;
}

static int destruct_burnin_task_runner_handle(burnin_task_runner_handle_t *r, runner_processes_t task) {
    ESP_LOGI(__FUNCTION__, "%d Resetting task handle %s", __LINE__, runner_processes_repr[task]);
    if (r == NULL) {
        ESP_LOGE(__FUNCTION__, "%d Passed handle NULL", __LINE__);
        return -1;
    }
    if (r->config_handle == NULL) {
        ESP_LOGW(__FUNCTION__, "%d Config handle set to NULL ", __LINE__);
        return 0;
    }
    switch (task) {
        case task_calibration:
            destruct_calibration_config((calibration_conf_t *) r->config_handle);
            break;
        case task_positioning:
            destruct_positioning_conf((valve_positioning_conf_t *) r->config_handle);
            break;
        case task_blowby:
            destruct_blowby_conf((blowby_conf_t *) r->config_handle);
            break;
        case task_ach:
            destruct_ach_conf((ach_conf_t *) r->config_handle);
            break;
    }
    return 0;
}

//
//void static inline update_runner_devices(conf_handle_t *h, device_valve_blower_t* vdev, device_valve_blower_t* cdev) {
//    h->
////    h->cdev = cdev;
////    h->id = vdev->type;
////    h->contr_id = cdev->type;
//}

/**
 * @brief Default valve burnin configuration
 * @param arg : pointer to the valve_burnin handle
 * Created by main burnin task handle.
 * Runs the valve burn in tests and reports the results
 * Enter critical section and set the state to finished to inturrupt the task
 * Task does not timeout instead continues to run until the state is set to finished
 */
void valve_burnin_task(void *arg) {
    // create a burn in handle
	ESP_LOGI(tag, "____Valve Burn in Task Started______________");
	vTaskDelay(100 / portTICK_PERIOD_MS);

    blower_burnin_handle_t *b = (blower_burnin_handle_t *) arg;

    calibration_conf_t *cal_conf;

    // Set the start time to now
    b->start_time = esp_timer_get_time();
    b->num_iterations = 0;

    // Create the ach configs
//    init_burnin_task_runner_handle(b->runner_handle[0],task_calibration, Side)


    side_dev_conf_t s_A = {0, sideA, {NULL, NULL}};
    side_dev_conf_t s_B = {1, sideB, {NULL, NULL}};
    burnin_task_runner_handle_t runner_one;
    burnin_task_runner_handle_t runner_two;
    b->runner_handle[0] = &runner_one;
    b->runner_handle[1] = &runner_two;

    side_t handle_index = sideA;
//    ESP_LOGI(tag, "____Valve Burn in Task Started______________");
    ESP_LOGI(tag, "    Valve Burn in State %s", burnin_progress_repr[b->progress]);
    ESP_LOGI(tag, "    Type: %d", (int)b->type);
    ESP_LOGI(tag, "    Start_time: %d", (int)b->start_time);
    ESP_LOGI(tag, "    Stop time: %d", (int)b->stop_time);
    ESP_LOGI(tag, "    SideA : %d", (int)b->side_A);
    ESP_LOGI(tag, "    SideB : %d", (int)b->side_B);
    ESP_LOGI(tag, "    Runner [0] : %p", (void*)b->runner_handle[0]);
    ESP_LOGI(tag, "    Runner [1] : %p", (void*)b->runner_handle[1]);
    ESP_LOGI(tag, "    Blower [0] : %p", (void*)b->blower_handle[0]);
    ESP_LOGI(tag, "    Blower [1] : %p", (void*)b->blower_handle[1]);
    ESP_LOGI(tag, "    Blower [2] : %p", (void*)b->blower_handle[2]);
    ESP_LOGI(tag, "    Blower [3] : %p", (void*)b->blower_handle[3]);
    ESP_LOGI(tag, "    Progress : %s", burnin_progress_repr[b->progress]);
    vTaskDelay(300 / portTICK_PERIOD_MS);


    // task waits for the state to be init before starting
    while (b->progress != prog_stop) {

        handle_index = (handle_index == sideA) ? sideB : sideA;
        burnin_task_runner_handle_t *task = b->runner_handle[handle_index];
        b->num_iterations += 1;
        ESP_LOGI(tag, "%d Itterateion running side %d ", b->num_iterations, task->state);
        vTaskDelay(1000 / portTICK_PERIOD_MS);


        switch (task->state) {
            case burnin_init:
                /**
                 * Sets up burnin with b_index for the blower handles
                 */
                if (task->task_prog == prog_idle) {
                    ESP_LOGI(tag, "Setting up side A task");
                    b->progress = prog_init;
                    // check if the first and 3 device are valve and blower
                    if (b->side_A && task->side_conf.side == sideA) {

                        task->side_conf.dev[0] = b->blower_handle[0];
                        task->side_conf.dev[1] = b->blower_handle[2];
                        task->task_prog = burnin_calibrate;

                    } else if (b->side_B && task->side_conf.side == sideB) {

                        task->side_conf.dev[0] = b->blower_handle[1];
                        task->side_conf.dev[1] = b->blower_handle[3];
                        task->task_prog = burnin_calibrate;
                    } else {
                        // Dont run again
                        task->state = burnin_finished;
                    }
//                    char* side_conf = side_repr[task->side_conf.side];
//                    char* task_side =  blower_id_str[task->side_conf.dev[1]->type];
//                    ESP_LOGI(tag, "%d:    burnin_init configuring %s  for burn in Dev1= %s Dev2= %d",
//                             __LINE__, side_repr[task->side_conf.side], blower_id_str[task->side_conf
//                            .dev[0]->type], task->side_conf.dev[1]->type);
                }
                break;
            case burnin_calibrate:
                //check the current state of the calibration task
                if (task->task_prog == prog_idle) {

                    // Delete the old task
                    destruct_burnin_task_runner_handle(task, task_calibration);
                    // configure the new task
                    init_burnin_task_runner_handle(task, task_calibration, task->side_conf.side);

                    task->task_prog = prog_init;
                } else if (task->task_prog == prog_init) {
                    cal_conf = (calibration_conf_t *) &task->config_handle;
                    cal_conf->id = task->side_conf.dev[0]->type;
                    cal_conf->contr_id = task->side_conf.dev[1]->type;
                    task->task_prog = prog_start;
                    ESP_LOGI(tag, "Line%d:state%s     Task:%s Progress_state:%s\t result_state:%s", __LINE__,
                             burnin_state_repr[task->state], task->task_name, burnin_progress_repr[task->task_prog],
                             task_result_str[task->task_result]);

                } else if (task->task_prog == prog_start) {
                    xTaskCreate(valve_calibration_task, task->task_name, 4096, task->config_handle, 6,
                                task->task_handle);
                    // Check if we have started the task
                    if (task->task_handle != NULL) {
                        task->task_prog = prog_running;
                        task->task_result = burnin_test_init;
                    } else {
                        task->task_prog = prog_stop;
                        task->task_result = burnin_test_Error;
                        task->state = burnin_finished;
                    }
                    ESP_LOGI(tag, "Line%d:state%s     Task:%s Progress_state:%s\t result_state:%s", __LINE__,
                             burnin_state_repr[task->state], task->task_name, burnin_progress_repr[task->task_prog],
                             task_result_str[task->task_result]);
                } else if (task->task_prog == prog_running) {
                    cal_conf = (calibration_conf_t *) &task->config_handle;

                    task->task_result = cal_conf->burnin_state;

                    if (task->task_result == burnin_test_failed ||
                        task->task_result == burnin_test_passed) {
                        // copy the results and set the progress state to finished
                        task->task_prog = prog_stop;
                        // TODO: update the screen with the results
                        break;
                    }
                    task->state = burnin_finished;
                    ESP_LOGI(tag, "Line%d:state%s     Task:%s Progress_state:%s\t result_state:%s", __LINE__,
                             burnin_state_repr[task->state], task->task_name, burnin_progress_repr[task->task_prog],
                             task_result_str[task->task_result]);

                }
                break;

            default:
                break;
        }
        if (b->runner_handle[0]->state == burnin_finished && b->runner_handle[1]->state == burnin_finished) {
            b->progress = prog_stop;
            ESP_LOGI(tag, "Line%d:state%s     Task:%s Progress_state:%s\t Task:%s Progress_state:%s\t", __LINE__,
                     burnin_progress_repr[b->progress], b->runner_handle[0]->task_name,
                     burnin_progress_repr[b->runner_handle[0]->state],
                     b->runner_handle[1]->task_name, burnin_progress_repr[b->runner_handle[1]->state]);
        }
//        ESP_LOGI(tag, "Line%d:state%s     Task:%s Progress_state:%s\t Task:%s Progress_state:%s\t", __LINE__,
//                 burnin_progress_repr[b->progress], b->runner_handle[0]->task_name,
//                 burnin_progress_repr[b->runner_handle[0]->state],
//                 b->runner_handle[1]->task_name, burnin_progress_repr[b->runner_handle[1]->state]);

        ESP_LOGI(tag, "Line%d:state%d     Task Progress_state:%d\t  Progress_state:%d\t", __LINE__,
        		b->progress,b->runner_handle[0]->state,b->runner_handle[1]->state);

    }
    b->stop_time = esp_timer_get_time();
    ESP_LOGI(tag, "____Valve Burn in Task Stopped______________");


    vTaskDelete(NULL);


}



