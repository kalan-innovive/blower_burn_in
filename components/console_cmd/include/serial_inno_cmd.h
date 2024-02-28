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

extern blower_update blowerinfo;

#endif /* COMPONENTS_INNO_CONNECT_INCLUDE_SERIAL_INNO_CMD_H_ */
