/*
 * helpers.c
 *
 *  Created on: Aug 15, 2022
 *      Author: ekalan
 */
#include "hal/cpu_hal.h" // for cpu_hal_get_cycle_count()
#include <rom/ets_sys.h>
#include <stdint.h>
//#include <time.h>
//#include <sys/time.h>
//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
//#include "freertos/semphr.h"
#include "esp_compiler.h"
//#include "esp_log.h"
//#include "esp_log_private.h"

uint32_t ms_timestamp(void)
{
	{
#if CONFIG_IDF_TARGET_ESP32
		/* ESP32 ROM stores separate clock rate values for each CPU, but we want the PRO CPU value always */
		extern uint32_t g_ticks_per_us_pro;
		return cpu_hal_get_cycle_count() / (g_ticks_per_us_pro * 1000);
#else
	    return cpu_hal_get_cycle_count() / (ets_get_cpu_frequency() * 1000);
	#endif
	}
}

uint32_t us_timestamp(void)
{
	{
#if CONFIG_IDF_TARGET_ESP32
		/* ESP32 ROM stores separate clock rate values for each CPU, but we want the PRO CPU value always */
		extern uint32_t g_ticks_per_us_pro;
		return cpu_hal_get_cycle_count() / (g_ticks_per_us_pro);
#else
	    return cpu_hal_get_cycle_count() / (ets_get_cpu_frequency());
	#endif
	}
}

void spin_us(uint32_t i) {
	uint32_t start_timer = us_timestamp() + i;
//	if (i > 10000) {
//		return;
//	}
	while (us_timestamp() < start_timer)
		;
}
