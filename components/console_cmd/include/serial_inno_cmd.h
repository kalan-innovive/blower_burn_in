/*
 * serial_inno_cmd.h
 *
 *  Created on: Feb 26, 2024
 *      Author: ekalan
 */

#ifndef COMPONENTS_INNO_CONNECT_INCLUDE_SERIAL_INNO_CMD_H_
#define COMPONENTS_INNO_CONNECT_INCLUDE_SERIAL_INNO_CMD_H_

void register_serialinno(void);

typedef struct{
	int devid;
	unsigned chipid;
	unsigned uuid;
	int offset;
	int is_updated;
	int valid;

}blower_update;


typedef struct{
	int devid;
	int hw_ver;
	int sw_ver_maj;
	int sw_ver_min;
	int p_sensor_type;
	int blower_type;
	int is_updated;
	int valid;
}blower_version;

#define DEFAULT_BLOWER_VER_VALUES {0,0,0,0,0,0,0,0}

extern blower_update blowerinfo;

#endif /* COMPONENTS_INNO_CONNECT_INCLUDE_SERIAL_INNO_CMD_H_ */
