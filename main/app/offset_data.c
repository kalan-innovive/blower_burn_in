/*
 * offset_data.c
 *
 *  Created on: May 17, 2023
 *      Author: ekalan
 */

#include <stdio.h>
#include <limits.h>
//#include <std>

#include "offset_data.h"

int len_chip_array = 3;
// define the array
ChipData chipArray[10] = { { .chipID = 83114623, .vas_offset = -60, .qc_offset =
		-54 }, { .chipID = 258452932, .vas_offset = 70, .qc_offset = 65 }, {
		.chipID = 4192804164, .vas_offset = -47, .qc_offset = -50 }, { .chipID =
		404168636, .vas_offset = 9, .qc_offset = 10 }, { .chipID = 318243897,
		.vas_offset = -54, .qc_offset = -57 }, { .chipID = 6, .vas_offset = 0,
		.qc_offset = 0 }, { .chipID = 7, .vas_offset = 0, .qc_offset = 0 }, {
		.chipID = 8, .vas_offset = 0, .qc_offset = 0 }, { .chipID = 9,
		.vas_offset = 0, .qc_offset = 0 }, { .chipID = 10, .vas_offset = 0,
		.qc_offset = 0 }, };

void init_chipArrray() {
	for (int i = 0; i < LEN_CHIPDATA_ARRAY; i++) {
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
	int index = -1;
	// get a pointer to the correct chipdata object
	ChipData *data = getChipData(cd->chipID);

	// Assign the correct data to the pointer if not found in list
	if (data == NULL) {
		data = &chipArray[len_chip_array];
		index = ++len_chip_array;
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
	for (int i = 0; i < LEN_CHIPDATA_ARRAY; i++) {
		if (chipArray[i].chipID == chipID) {
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

int get_min(CircularArray *ca) {
	int min = INT_MAX;
	for (int i = 0; i < ca->count; i++) {
		int value = get(ca, i);
		if (value < min) {
			min = value;
		}
	}
	return min;
}

int get_max(CircularArray *ca) {
	int max = INT_MIN;
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

