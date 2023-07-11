/*
 * server_helper.c
 *
 *  Created on: Jul 5, 2023
 *      Author: ekalan
 */

#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include "cJSON.h"

unsigned int to_valuint(const cJSON *json)
{
	if (json->type == cJSON_Number)
	{
		uint32_t number = (uint32_t)
				json->valuedouble;
		if (number <= UINT_MAX)
		{
			return (unsigned int) number;
		}
	}

	// If the input is not a valid 32-bit unsigned integer, you can handle the error here.
	printf("Invalid input or number out of range!\n");
	return 0; // Default value or error handling
}

