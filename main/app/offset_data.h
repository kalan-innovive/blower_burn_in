/*
 * offset_data.h
 *
 *  Created on: May 17, 2023
 *      Author: ekalan
 */

#ifndef MAIN_APP_OFFSET_DATA_H_
#define MAIN_APP_OFFSET_DATA_H_

#define SIZE_OFFSET 10
#define LEN_CHIPDATA_ARRAY 10

#ifndef DEF_OFFSET_VAL
#define DEF_OFFSET 0xFFFE
#else
#define DEF_OFFSET DEF_OFFSET_VAL
#endif

// define the CircularArray struct
typedef struct {
	int data[SIZE_OFFSET];
	int head;
	int count;
} CircularArray;

// define the ChipData struct
typedef struct {
	unsigned int chipID;
	int vas_offset;
	int qc_offset;
	CircularArray offset_array;
} ChipData;

void init_chipArrray(void);

int add(CircularArray *ca, int value);
void init_circular_array(CircularArray *ca);
int addChipData(ChipData *cd);
int getCurrOffset(ChipData *cd);
int offset_min(ChipData *cd);
int offset_max(ChipData *cd);
int get(CircularArray *ca, int index);
int get_max(CircularArray *ca);
int get_min(CircularArray *ca);

ChipData* getChipData(unsigned int chipID);
int chip_data_index(unsigned int chipID);
int addOffset(unsigned int chipID, int offset);
int copy_array(CircularArray *ca, int *new_array, int n);
int insert_array(CircularArray *ca, int *new_array, int n);
int get_max_last_n(ChipData *cd, int last_n);
int get_min_last_n(ChipData *cd, int last_n);
int offset_range(ChipData *cd);
int offset_min_last_n(ChipData *cd, int last_n);
int offset_max_last_n(ChipData *cd, int last_n);
int offset_range_last_n(ChipData *cd, int last_n);

#endif /* MAIN_APP_OFFSET_DATA_H_ */
