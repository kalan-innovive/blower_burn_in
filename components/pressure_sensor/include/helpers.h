/*
 * helpers.h
 *
 *  Created on: Aug 15, 2022
 *      Author: ekalan
 */
//
//#ifndef MAIN_HELPERS_H_
//#define MAIN_HELPERS_H_


#pragma once

#include <stdint.h>
//#include <stdarg.h>
#include "sdkconfig.h"
#include "esp_rom_sys.h"
#include "hal/cpu_hal.h" // for cpu_hal_get_cycle_count()


#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h" // will be removed in idf v5.0
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/ets_sys.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Function which returns timestamp to be used in log output
 *
 * This function is used in expansion of ESP_LOGx macros.
 * In the 2nd stage bootloader, and at early application startup stage
 * this function uses CPU cycle counter as time source. Later when
 * FreeRTOS scheduler start running, it switches to FreeRTOS tick count.
 *
 * For now, we ignore millisecond counter overflow.
 *
 * @return timestamp, in milliseconds
 */
uint32_t ms_timestamp(void);
uint32_t us_timestamp(void);

uint32_t spin_us(int i);

//#define configASSERT ( x )     if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ )

#ifdef __cplusplus
}
#endif
//#endif /* MAIN_HELPERS_H_ */
