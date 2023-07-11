/*
 * burn_in.h
 *
 *  Created on: May 1, 2023
 *      Author: ekalan
 */

#ifndef MAIN_APP_BURN_IN_H_
#define MAIN_APP_BURN_IN_H_

#define UPDATE_UI()

// If defined the app has a shorter cycle time and monitors the heap
#define TESTING_APP_BURNIN 0

#ifdef TESTING_APP_BURNIN

#define APP_LOOP_RATE_MS 1000

// Start delay to give communication time to sync
#define APP_START_DELAY_MS 5000

// The amount the ui is updated. must be a factor of loop rate to gicve an integer value
#define APP_CAL_UPDATE_TIME_MS 5000

// The amount the ui is updated
#define UPDATE_UI_COUNT APP_CAL_UPDATE_TIME_MS/APP_LOOP_RATE_MS

// Loop time that defines how often the main app loop is checked
#define LOOP_TIME  1000

#else

#define APP_LOOP_RATE_MS 1000

// Start delay to give communication time to sync
#define APP_START_DELAY_MS 4000

// The amount the ui is updated
#define APP_CAL_UPDATE_TIME_MS 5000

// Loop time that defines how often the main app loop is checked
#define LOOP_TIME  1000

#endif //TESTING_APP_BURNIN

void burn_in_task(void *pvParameter);

#endif /* MAIN_APP_BURN_IN_H_ */
