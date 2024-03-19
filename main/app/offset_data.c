/*
 * offset_data.c
 *
 *  Created on: May 17, 2023
 *      Author: ekalan
 */

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "esp_log.h"

#include "offset_data.h"

int len_chip_array = 0;
// define the array
//ChipData chipArray[10] = { { .chipID = 83114623, .vas_offset = -60, .qc_offset =
//		-54 }, { .chipID = 258452932, .vas_offset = 70, .qc_offset = 65 }, {
//		.chipID = 4192804164, .vas_offset = -47, .qc_offset = -50 }, { .chipID =
//		404168636, .vas_offset = 9, .qc_offset = 10 }, { .chipID = 318243897,
//		.vas_offset = -54, .qc_offset = -57 }, { .chipID = 6, .vas_offset = 0,
//		.qc_offset = 0 }, { .chipID = 7, .vas_offset = 0, .qc_offset = 0 }, {
//		.chipID = 8, .vas_offset = 0, .qc_offset = 0 }, { .chipID = 9,
//		.vas_offset = 0, .qc_offset = 0 }, { .chipID = 10, .vas_offset = 0,
//		.qc_offset = 0 }, };

ChipData chipArray[10];
const char *TAG = "offset_data";

void init_chipArrray() {
	for (int i = 0; i < LEN_CHIPDATA_ARRAY; i++) {
		chipArray[i].chipID = 0;
		init_circular_array(&chipArray[i].offset_array);
	}
}

/**
 * @brief
 *  - Checks if the chipData is in the array and updates the data if it is
 *  - if not in data adds chip data to array
 *
 *  @param ChipData *cd- data to update or insert
 *  @returns
 *   - index of the chip_array
 *   - -1 if not able to add
 */
int addChipData(ChipData *cd) {
	int index = chip_data_index(cd->chipID);
	// get a pointer to the correct chipdata object
	ChipData *data = getChipData(cd->chipID);

	ESP_LOGD(TAG, "[%s,%d]\tAdding Chip Data to :%p ", __FUNCTION__, __LINE__,
			(void* ) data);

	// Assign the correct data to the pointer if not found in list
	if (data == NULL) {

		data = &chipArray[len_chip_array];
		index = ++len_chip_array;
		ESP_LOGD(TAG, "[%s,%d]\t Updated Chip array %d at:%p", __FUNCTION__,
				__LINE__, index, (void* ) data);

	}
	memcpy(data, cd, sizeof(ChipData));

	return index;
}

int chip_data_index(unsigned int chipID) {
	for (int i = 0; i < LEN_CHIPDATA_ARRAY; i++) {
		if (chipArray[i].chipID == chipID) {
			return i;
		}
	}
	return -1;
}

ChipData* getChipData(unsigned int chipID) {
	ESP_LOGD(TAG, "[%s,%d] Retrieving :for chipid:%u",
			__FUNCTION__, __LINE__, chipID);
	for (int i = 0; i < LEN_CHIPDATA_ARRAY; i++) {
		ESP_LOGD(TAG, "[%s,%d] Chip Data to :%p  for chipid:%u", __FUNCTION__,
				__LINE__,
				(void* )&chipArray[i], chipArray[i].chipID);
		if (chipArray[i].chipID == chipID) {
			ESP_LOGD(TAG, "[%s,%d] Adding Chip Data to :%p  for chipid:%u",
					__FUNCTION__, __LINE__,
					(void* )&chipArray[i], chipID);
			return &chipArray[i];

		}
	}
	return NULL;
}

int offset_min(ChipData *cd) {
	int min_tmp = get_min(&cd->offset_array);
	min_tmp = (min_tmp < cd->qc_offset) ? min_tmp : cd->qc_offset;
	min_tmp = (min_tmp < cd->vas_offset) ? min_tmp : cd->vas_offset;
	return min_tmp;
}

int offset_max(ChipData *cd) {
	int max_tmp = get_max(&cd->offset_array);
	max_tmp = (max_tmp > cd->qc_offset) ? max_tmp : cd->qc_offset;
	max_tmp = (max_tmp > cd->vas_offset) ? max_tmp : cd->vas_offset;
	return max_tmp;
}

int offset_range(ChipData *cd) {
	int min_tmp = offset_min(cd);
	int max_tmp = offset_min(cd);
	int range = max_tmp - min_tmp;
	return range;
}

int offset_min_last_n(ChipData *cd, int last_n) {
	int min_tmp = get_min_last_n(&cd->offset_array, last_n);
	min_tmp = (min_tmp < cd->qc_offset) ? min_tmp : cd->qc_offset;
	min_tmp = (min_tmp < cd->vas_offset) ? min_tmp : cd->vas_offset;
	return min_tmp;
}

int offset_max_last_n(ChipData *cd, int last_n) {
	int max_tmp = get_max(&cd->offset_array);
	max_tmp = (max_tmp > cd->qc_offset) ? max_tmp : cd->qc_offset;
	max_tmp = (max_tmp > cd->vas_offset) ? max_tmp : cd->vas_offset;
	return max_tmp;
}

int offset_range_last_n(ChipData *cd, int last_n) {
	int min_tmp = offset_min(cd);
	int max_tmp = offset_min(cd);
	int range = max_tmp - min_tmp;
	return range;
}

int addOffset(unsigned int chipID, int offset) {
	for (int i = 0; i < LEN_CHIPDATA_ARRAY; i++) {
		if (chipArray[i].chipID == chipID) {
			return add(&chipArray[i].offset_array, offset);
		}
	}
	return 0;
}

int getCurrOffset(ChipData *cd) {
	if (cd != NULL) {
		int index = cd->offset_array.head - 1;
		CircularArray *ca = &cd->offset_array;
		return get(ca, index);

	}
	return 0xffff;
}

void init_circular_array(CircularArray *ca) {
	ca->head = 0;
	ca->count = 0;
}

int add(CircularArray *ca, int value) {
	ca->data[ca->head] = value;
	ca->head = (ca->head + 1) % SIZE_OFFSET;
	if (ca->count < SIZE_OFFSET) {
		ca->count++;
//		printf("Circular Array index %d, added value: %d, New count %d\n", ca->head - 1, ca->data[ca->head - 1], ca->count);
	}
	// Return the new length of the array
	return ca->count;
}

int get(CircularArray *ca, int index) {
	if (index >= ca->count) {
//        printf("Index %d out of bounds, Circular array count: %d\n", index, ca->count);
		return -1;  // using -1 as error value
	}
	return ca->data[(ca->head + SIZE_OFFSET - ca->count + index) % SIZE_OFFSET];
}

int get_min_last_n(ChipData *cd, int last_n) {
	int min = INT_MAX;
	CircularArray *ca = &cd->offset_array;

	int start_i = (last_n >= ca->count) ? 0 : ca->count - last_n;
	for (int i = start_i; i < ca->count; i++) {
		int value = get(ca, i);
		if (value < min) {
			min = value;
		}
	}
	int min_tmp = min;

	if (cd->vas_offset != DEF_OFFSET_VAL) {
		min = (cd->vas_offset > min) ? min : cd->vas_offset;
	}
	if (cd->qc_offset != DEF_OFFSET_VAL) {
		min = (cd->qc_offset > min) ? min : cd->qc_offset;
	}


	ESP_LOGW(TAG,
			"[%s,%d] last N: %d, Min circular array :%d, Min ChipData %d",
			__FUNCTION__,
			__LINE__, last_n, min_tmp, min);

	return min;

}

int get_max_last_n(ChipData *cd, int last_n) {
	int max = INT_MIN;
	CircularArray *ca = &cd->offset_array;

	int start_i = (last_n >= ca->count) ? 0 : ca->count - last_n;

	for (int i = start_i; i < ca->count; i++) {
		int value = get(ca, i);
		if (value > max) {
			max = value;
		}
	}
	int max_tmp = max;

	if (cd->vas_offset != DEF_OFFSET_VAL) {
		max = (cd->vas_offset < max) ? max : cd->vas_offset;

	}
	if (cd->qc_offset != DEF_OFFSET_VAL) {
		max = (cd->qc_offset < max) ? max : cd->qc_offset;

	}

	ESP_LOGW(TAG,
			"[%s,%d] last N: %d, Max circular array :%d, Max ChipData: %d",
			__FUNCTION__,
			__LINE__, last_n, max_tmp, max);
	return max;
}

int get_min(CircularArray *ca) {
	int min = INT_MIN;
	for (int i = 0; i < ca->count; i++) {
		int value = get(ca, i);
		if (value < min) {
			min = value;
		}
	}
	return min;
}

int get_max(CircularArray *ca) {
	int max = INT_MAX;
	for (int i = 0; i < ca->count; i++) {
		int value = get(ca, i);
		if (value > max) {
			max = value;
		}
	}
	return max;
}

int copy_array(CircularArray *ca, int *new_array, int n) {
	if (n > SIZE_OFFSET) {
//        printf("Error: Can't copy more than %d values\n", SIZE_OFFSET);
		return -1;
	}

	int values_copied = 0;

	for (int i = 0; i < n; i++) {
		int value = get(ca, i);
		if (value == -1) {
			new_array[i] = 0xFFFFF;
//            printf("%d. copied default value %d\n", i, new_array[i]);
		} else {
			new_array[i] = value;
//            printf("%d. copied value %d\n", i, new_array[i]);
			values_copied++;
		}
	}

	return values_copied;
}

int insert_array(CircularArray *ca, int *new_array, int n) {
	ESP_LOGD(TAG, "[%s,%d]\t insert array:%p,  %d", __FUNCTION__,
			__LINE__, (void* )new_array, n);
	if (n > SIZE_OFFSET) {
		ESP_LOGW(TAG, "[%s,%d]\tArray length out of bounds len:%d",
				__FUNCTION__, __LINE__, n);
//        printf("Error: Can't copy more than %d values\n", SIZE_OFFSET);
		return -1;
	}
	init_circular_array(ca);

	int values_copied = 0;

	for (int i = 0; i < n; i++) {
		values_copied = add(ca, new_array[i]);
		ESP_LOGD(TAG, "[%s,%d]\t %d. Added array value:%d ", __FUNCTION__,
				__LINE__, values_copied, new_array[i]);
	}

	return values_copied;
}

