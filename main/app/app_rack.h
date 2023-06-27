/*
 * app_rack.h
 *
 *  Created on: Apr 19, 2023
 *      Author: ekalan
 */

#pragma once

#include "driver/gpio.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t app_rack_init(gpio_num_t u_tx, gpio_num_t u_rx);
#ifdef __cplusplus
}
#endif
